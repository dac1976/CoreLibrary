// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License and GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// and GNU Lesser General Public License along with this program. If
// not, see <http://www.gnu.org/licenses/>.

/*!
 * \file TcpConnection.cpp
 * \brief File containing TCP connection class definition.
 */

#include "TcpConnection.h"
#include "TcpConnections.h"
// #include <iterator>
#include <algorithm>
#include <limits>
// #include <stdexcept>
#include <boost/bind.hpp>
#if defined(CORELIB_SOCKET_DEBUG)
#include <boost/exception/all.hpp>
#include "DebugLogging.h"
#else
#include <boost/throw_exception.hpp>
#endif

namespace core_lib
{
namespace asio
{
namespace tcp
{

// Special "lightweight" object to pass to IO service to remove any
// unintended deep vector copies from occurring when sending async
// messages.
class AsyncSendCallableObj final
{
    using free_pool_msg_t = std::function<void(size_t, bool)>;

public:
#if defined(USE_DEFAULT_CONSTRUCTOR_)
    AsyncSendCallableObj(){};
#else
    AsyncSendCallableObj()                                  = default;
#endif
    ~AsyncSendCallableObj()                                      = default;
    AsyncSendCallableObj(AsyncSendCallableObj const&)            = default;
    AsyncSendCallableObj& operator=(AsyncSendCallableObj const&) = default;
#if defined(USE_EXPLICIT_MOVE_)
    AsyncSendCallableObj(AsyncSendCallableObj&& x)
    {
        *this = std::move(x);
    }

    AsyncSendCallableObj& operator=(AsyncSendCallableObj&& x)
    {
        std::swap(m_messageBufPtr, x.m_messageBufPtr);
        std::swap(m_poolIndex, x.m_poolIndex);
        std::swap(m_freePoolMsg, x.m_freePoolMsg);
        return *this;
    }
#else
    AsyncSendCallableObj(AsyncSendCallableObj&&)            = default;
    AsyncSendCallableObj& operator=(AsyncSendCallableObj&&) = default;
#endif

    AsyncSendCallableObj(std::shared_ptr<defs::char_buffer_t> const& msgPtr, size_t poolIndex,
                         free_pool_msg_t const& freePoolMsg)
        : m_messageBufPtr(msgPtr)
        , m_poolIndex(poolIndex)
        , m_freePoolMsg(freePoolMsg)
    {
    }

    void operator()(const boost::system::error_code& error,
                    CORELIB_ARG_MAYBE_UNUSED std::size_t bytes_transferred) const
    {
        bool destroySelf = false;

        if (error)
        {
#if defined(CORELIB_SOCKET_DEBUG)
            DEBUG_MESSAGE_EX_ERROR("Error detected in async_write completion handler, error: "
                                   << error.message()
                                   << ", bytes transferred = " << bytes_transferred
                                   << ", size of message sent = " << m_messageBufPtr->size()
                                   << ", will safely destroy self.");
#else
            CORELIB_UNUSED_ARG(bytes_transferred)
#endif
            destroySelf = true;
        }

        if (m_freePoolMsg)
        {
            m_freePoolMsg(m_poolIndex, destroySelf);
        }
    }

private:
    std::shared_ptr<defs::char_buffer_t> m_messageBufPtr;
    size_t                               m_poolIndex{0};
    free_pool_msg_t                      m_freePoolMsg;
};

// ****************************************************************************
// 'class TcpConnection' definition
// ****************************************************************************
TcpConnection::TcpConnection(asio_compat::io_service_t&                 ioService,
                             std::shared_ptr<TcpConnections> const&     connections,
                             defs::check_bytes_left_to_read_t const&    checkBytesLeftToRead,
                             defs::message_received_handler_t const&    messageReceivedHandler,
                             TcpConnSettings const&                     settings,
                             defs::message_received_handler_ex_t const& messageReceivedHandlerEx,
                             defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx)
    : m_closing{false}
    , m_strand(asio_compat::make_strand(ioService))
    , m_connections{connections}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_checkBytesLeftToReadEx{checkBytesLeftToReadEx}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_messageReceivedHandlerEx{messageReceivedHandlerEx}
    , m_settings{settings}
    , m_connectEvent(eNotifyType::signalOneThread, eResetCondition::manualReset,
                     eIntialCondition::notSignalled)
    , m_socket{ioService}
{
    InitialiseMsgPool();

#if defined(CORELIB_SOCKET_DEBUG)
    DEBUG_MESSAGE_EX_DEBUG(
        "Reserving memory for receive and send buffers as: " << DEFAULT_RESERVED_SIZE << " bytes");
#endif

    m_receiveBuffer.reserve(DEFAULT_RESERVED_SIZE);
    m_messageBuffer.reserve(DEFAULT_RESERVED_SIZE);
}

// [DEPRECATED]
TcpConnection::TcpConnection(asio_compat::io_service_t&              ioService,
                             std::shared_ptr<TcpConnections> const&  connections,
                             size_t                                  minAmountToRead,
                             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                             defs::message_received_handler_t const& messageReceivedHandler,
                             eSendOption sendOption, size_t maxAllowedUnsentAsyncMessages,
                             size_t sendPoolMsgSize)
    : m_closing{false}
    , m_strand{asio_compat::make_strand(ioService)}
    , m_connections{connections}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_connectEvent(eNotifyType::signalOneThread, eResetCondition::manualReset,
                     eIntialCondition::notSignalled)
    , m_socket{ioService}
{
    m_settings.minAmountToRead               = minAmountToRead;
    m_settings.sendOption                    = sendOption;
    m_settings.maxAllowedUnsentAsyncMessages = maxAllowedUnsentAsyncMessages;
    m_settings.sendPoolMsgSize               = sendPoolMsgSize;

    InitialiseMsgPool();

#if defined(CORELIB_SOCKET_DEBUG)
    DEBUG_MESSAGE_EX_DEBUG(
        "Reserving memory for receive and send buffers as: " << DEFAULT_RESERVED_SIZE << " bytes");
#endif

    m_receiveBuffer.reserve(DEFAULT_RESERVED_SIZE);
    m_messageBuffer.reserve(DEFAULT_RESERVED_SIZE);
}

boost_tcp_t::socket& TcpConnection::Socket()
{
    return m_socket;
}

const boost_tcp_t::socket& TcpConnection::Socket() const
{
    return m_socket;
}

bool TcpConnection::Connect(const defs::connection_t& endPoint)
{
    try
    {
#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_INFO("Connecting TCP socket endpoint to: " << endPoint.first << ":"
                                                                    << endPoint.second);
#endif

        boost_tcp_t::endpoint tcpEndPoint(asio_compat::make_address(endPoint.first),
                                          endPoint.second);

        // Error object must not go out of scope before handler called.
        auto connectError = std::make_shared<boost::system::error_code>();

        // Don't want any previously timed out calls to WaitForTime to fire by
        // mistake when trying to setup a new call to WaitForTime.
        m_connectEvent.Reset();

        auto connectionId = NextConnectionId();
        SetCurrentConnectionId(connectionId);

        m_socket.async_connect(tcpEndPoint,
                               asio_compat::wrap(m_strand,
                                                 boost::bind(&TcpConnection::ConnectHandler,
                                                             shared_from_this(),
                                                             _1,
                                                             connectError,
                                                             connectionId)));

        // Async connect event signalled within time limit.
        if (m_connectEvent.WaitForTime(m_settings.maxTcpConnectTimeout))
        {
            if (*connectError)
            {
#if defined(CORELIB_SOCKET_DEBUG)
                DEBUG_MESSAGE_EX_ERROR("Async connect reported an error: "
                                       << connectError->message() << ", for: " << endPoint.first
                                       << ":" << endPoint.second);
#endif
                boost_sys::error_code ec;
                m_socket.close(ec);

                return false;
            }
        }
        // Async connect timed out.
        else
        {
#if defined(CORELIB_SOCKET_DEBUG)
            DEBUG_MESSAGE_EX_ERROR("Async connect timeout, for: " << endPoint.first << ":"
                                                                  << endPoint.second);
#endif
            // Make sure async_connect gets cancelled, just in case close() isn't enough.
            boost_sys::error_code ec;
            m_socket.cancel(ec);
            m_socket.close(ec);

            return false;
        }

        boost_tcp_t::no_delay nagleOption(m_settings.sendOption == eSendOption::nagleOff);
        m_socket.set_option(nagleOption);

        boost::asio::socket_base::keep_alive option(m_settings.keepAliveOption ==
                                                    eKeepAliveOption::on);
        m_socket.set_option(option);

        if (m_settings.recvBufferSize > 0)
        {
            boost::asio::socket_base::receive_buffer_size recvBufOption(
                static_cast<int>(m_settings.recvBufferSize));
            m_socket.set_option(recvBufOption);
        }

        if (m_settings.sendBufferSize > 0)
        {
            boost::asio::socket_base::send_buffer_size sendBufOption(
                static_cast<int>(m_settings.sendBufferSize));
            m_socket.set_option(sendBufOption);
        }

#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_INFO("Calling StartAsyncRead for first time after connection for: "
                              << endPoint.first << ":" << endPoint.second);
#endif

        StartAsyncRead(endPoint);
    }
    catch (...)
    {
#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Exception caught while connecting socket: "
                               << boost::current_exception_diagnostic_information()
                               << ", for: " << endPoint.first << ":" << endPoint.second);
#endif
        return false;
    }

    return true;
}

void TcpConnection::CloseConnection()
{
#if defined(CORELIB_SOCKET_DEBUG)
    DEBUG_MESSAGE_EX_INFO("Calling CloseConnection for: " << m_endPoint.first << ":"
                                                          << m_endPoint.second);
#endif
    bool postClose = false;

    {
        std::lock_guard<std::mutex> lock{m_mutex};

        // If we already initiated close, just wait for completion below.
        if (!m_closing)
        {
            m_closing = true;
        }

        if (!m_socket.is_open())
        {
            // Ensure the container gets cleaned even if the socket is already closed.
            DestroySelf();          // idempotent via m_removed
            m_closedEvent.Signal(); // preserve the blocking contract
            return;
        }

        postClose = true;
    }

    if (postClose)
    {
        asio_compat::post(m_strand,
                          boost::bind(&TcpConnection::ProcessCloseSocket, shared_from_this()));
    }

    m_closedEvent.Wait();
}

bool TcpConnection::IsClosing() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_closing;
}

void TcpConnection::ProcessCloseSocket()
{
    boost_sys::error_code ec;
    m_socket.shutdown(boost_tcp_t::socket::shutdown_both, ec);
    m_socket.close(ec);

    // We ARE on strand here
    bool expected = false;

    if (m_removed.compare_exchange_strong(expected, true))
    {
        DestroySelfOnStrand();
    }

    m_closedEvent.Signal();
}

void TcpConnection::DestroySelf()
{
    bool expected = false;

    if (!m_removed.compare_exchange_strong(expected, true))
    {
        return;
    }

    // If strand is dead / io_service stopped, posting may never run.
    // But in practice, during normal lifetime, this is correct.
    asio_compat::post(m_strand,
                      boost::bind(&TcpConnection::DestroySelfOnStrand, shared_from_this()));
}

void TcpConnection::DestroySelfOnStrand()
{
    // Lock weak_ptr safely
    auto connections = m_connections.lock();

    if (!connections)
    {
        return;
    }

    connections->Remove(shared_from_this());
}

void TcpConnection::StartAsyncRead(defs::connection_t const& endPoint)
{
    // Reduce scope of mutex to store endpoint
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_endPoint = endPoint;
    }

    auto self = shared_from_this();

    auto connections = self->m_connections.lock();

    if (!connections)
    {
        // Server already destroyed → nothing to remove
        return;
    }

    connections->Add(self->m_endPoint, self);

    asio_compat::post(
        m_strand,
        boost::bind(&TcpConnection::AsyncReadFromSocket, self, self->m_settings.minAmountToRead));
}

void TcpConnection::AsyncReadFromSocket(size_t amountToRead)
{
    m_receiveBuffer.resize(amountToRead);

    // Wrap in strand just to be safe in case of multiple threads running the IO service.
    boost_asio::async_read(m_socket,
                           boost_asio::buffer(m_receiveBuffer),
                           asio_compat::wrap(m_strand,
                                             boost::bind(&TcpConnection::ReadComplete,
                                                         shared_from_this(),
                                                         boost_placeholders::error,
                                                         boost_placeholders::bytes_transferred,
                                                         amountToRead)));
}

void TcpConnection::ReadComplete(const boost_sys::error_code& error, size_t bytesReceived,
                                 size_t bytesExpected)
{
    size_t numBytes    = 0;
    bool   clearMsgBuf = false;

    if (error)
    {
#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in ReadComplete, error: "
                               << error.message() << ", will safely self-destruct, for: "
                               << m_endPoint.first << ":" << m_endPoint.second);
#endif
        DestroySelf();
        return;
    }

    if (bytesReceived != bytesExpected)
    {
#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_WARNING("Warning in ReadComplete, bytesReceived != bytesExpected, will "
                                 "reset read buffers and try to read again, for: "
                                 << m_endPoint.first << ":" << m_endPoint.second);
#endif
        numBytes    = m_settings.minAmountToRead;
        clearMsgBuf = true;
    }
    else
    {
        try
        {
            // The buffer copying below has been optimised for performance
            // after evaluating various possible mechanisms and is a few
            // orders of magnitude faster than the original iterator plus
            // back_inserter solution.
            auto const currentSize = m_messageBuffer.size();
            m_messageBuffer.resize(currentSize + bytesReceived);
            auto dataWritePos = m_messageBuffer.data() + currentSize;
            std::copy(m_receiveBuffer.data(), m_receiveBuffer.data() + bytesReceived, dataWritePos);

            if (m_checkBytesLeftToReadEx)
            {
                numBytes =
                    m_checkBytesLeftToReadEx(m_messageBuffer,
                                             m_socket.remote_endpoint().address().to_string(),
                                             m_socket.remote_endpoint().port());
            }
            else
            {
                if (m_checkBytesLeftToRead)
                {
                    numBytes = m_checkBytesLeftToRead(m_messageBuffer);
                }
            }

            if (numBytes == 0)
            {
                // Ideally only one of m_messageReceivedHandler or m_messageReceivedHandlerEx should
                // be defined at any one time.

                if (m_messageReceivedHandlerEx)
                {
                    m_messageReceivedHandlerEx(m_messageBuffer,
                                               m_socket.remote_endpoint().address().to_string(),
                                               m_socket.remote_endpoint().port());
                }
                else
                {
                    if (m_messageReceivedHandler)
                    {
                        m_messageReceivedHandler(m_messageBuffer);
                    }
                }

                numBytes    = m_settings.minAmountToRead;
                clearMsgBuf = true;
            }
            else if (std::numeric_limits<size_t>::max() == numBytes)
            {
                // We have a problem.
                clearMsgBuf = true;
            }
        }
        catch (...)
        {
#if defined(CORELIB_SOCKET_DEBUG)
            DEBUG_MESSAGE_EX_ERROR("Error in ReadComplete, error: "
                                   << boost::current_exception_diagnostic_information()
                                   << ", for: " << m_endPoint.first << ":" << m_endPoint.second);
#endif
            numBytes    = m_settings.minAmountToRead;
            clearMsgBuf = true;
        }
    }

    if (clearMsgBuf)
    {
        m_messageBuffer.clear();
    }

    if (numBytes > 0)
    {
        if (!IsClosing())
        {
            // We dispatch instead of post as we are already with a strand protected posted
            // branch of execution, so dispatch will be faster and still safe.
            asio_compat::dispatch(
                m_strand,
                boost::bind(&TcpConnection::AsyncReadFromSocket, shared_from_this(), numBytes));
        }
        // else: closing -> don't schedule further reads
    }
}

void TcpConnection::DoAsyncWrite(msg_ptr_t const& msgPtr, size_t poolIndex)
{
    AsyncSendCallableObj callableObj(
        msgPtr,
        poolIndex,
        boost::bind(
            &TcpConnection::DecrementUnsentAsyncCounterCallback, shared_from_this(), _1, _2));

    boost_asio::async_write(
        m_socket, boost_asio::buffer(*msgPtr), asio_compat::wrap(m_strand, callableObj));
}

bool TcpConnection::SendMessageAsync(const defs::char_buffer_t& message)
{
    try
    {
        std::pair<msg_ptr_t, size_t> msgItem;

        if (!GetNewMessgeObject(msgItem, message))
        {
            return false;
        }

        asio_compat::post(
            m_strand,
            boost::bind(
                &TcpConnection::DoAsyncWrite, shared_from_this(), msgItem.first, msgItem.second));

        return true;
    }
    catch (...)
    {
#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in SendMessageAsync, error: "
                               << boost::current_exception_diagnostic_information()
                               << ", for: " << m_endPoint.first << ":" << m_endPoint.second);
#endif
    }

    return false;
}

bool TcpConnection::SendMessageSync(const defs::char_buffer_t& message)
{
    size_t bytesSent;

    try
    {
        bytesSent = boost_asio::write(m_socket, boost_asio::buffer(message));
    }
    catch (...)
    {
        bytesSent = 0;
    }

    bool success = (bytesSent == message.size());

    if (!success)
    {
#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in SendMessageSync, bytesSent != message.size(), will safely "
                               "self-destruct, for: "
                               << m_endPoint.first << ":" << m_endPoint.second);
#endif
        DestroySelf();
    }

    return success;
}

size_t TcpConnection::NumberOfUnsentAsyncMessages() const
{
    std::lock_guard<std::mutex> lock{m_asyncPoolMutex};
    return m_numUnsentAsyncMessages;
}

void TcpConnection::DecrementUnsentAsyncCounter(size_t messagePoolIndex)
{
    std::lock_guard<std::mutex> lock{m_asyncPoolMutex};

    if (m_numUnsentAsyncMessages > 0)
    {
        --m_numUnsentAsyncMessages;
    }

    // If we are using a message pool
    // put this index back onto the queue
    // of available pool indices.
    if (!m_msgPool.empty())
    {
        m_availablePoolIndices.push_back(messagePoolIndex);
    }
}

void TcpConnection::DecrementUnsentAsyncCounterCallback(size_t messagePoolIndex, bool destroySelf)
{
    DecrementUnsentAsyncCounter(messagePoolIndex);

    if (destroySelf)
    {
        DestroySelf();
    }
}

void TcpConnection::InitialiseMsgPool()
{
    m_availablePoolIndices.clear();

    if (0 == m_settings.maxAllowedUnsentAsyncMessages)
    {
#if defined(CORELIB_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_DEBUG(
            "Async sending message pool NOT being used because memPoolMsgCount = "
            << m_settings.maxAllowedUnsentAsyncMessages << ", for: " << m_endPoint.first << ":"
            << m_endPoint.second);
#endif
        m_msgPool.clear();
        return;
    }

#if defined(CORELIB_SOCKET_DEBUG)
    DEBUG_MESSAGE_EX_DEBUG("Async sending message pool will be used with memPoolMsgCount = "
                           << m_settings.maxAllowedUnsentAsyncMessages
                           << " and defaultMsgSize = " << m_settings.sendPoolMsgSize
                           << ", for: " << m_endPoint.first << ":" << m_endPoint.second);
#endif

    // If we get here then we have a non-zero value if maxAllowedUnsentAsyncMessages
    // but we do not create a message pool if the message size for the pool messages
    // is set to 0. In this case we use dynamic allocations for messages and monitor
    // how may unsent/pending async messages we have against maxAllowedUnsentAsyncMessages
    // using a counter.
    if (m_settings.sendPoolMsgSize > 0)
    {
        m_msgPool.resize(m_settings.maxAllowedUnsentAsyncMessages);
        size_t index = 0;

        auto generateMsg = [&index, this]()
        {
            auto msg = std::make_shared<defs::char_buffer_t>();

            msg->reserve(m_settings.sendPoolMsgSize);

            m_availablePoolIndices.push_back(index++);

            return msg;
        };

        std::generate(m_msgPool.begin(), m_msgPool.end(), generateMsg);
    }
    else
    {
        m_msgPool.clear();
    }
}

bool TcpConnection::GetNewMessgeObject(std::pair<msg_ptr_t, size_t>& msgItem,
                                       defs::char_buffer_t const&    sourceMsg) const
{
    // Using dynamic messages.
    if (m_msgPool.empty())
    {
        // Reduce mutex scope.
        {
            std::lock_guard<std::mutex> lock{m_asyncPoolMutex};

            if (m_numUnsentAsyncMessages == m_settings.maxAllowedUnsentAsyncMessages)
            {
#if defined(CORELIB_SOCKET_DEBUG)
                DEBUG_MESSAGE_EX_WARNING(
                    "Cannot send async dynamic message, currently at unsent async "
                    "message count limit, for: "
                    << m_endPoint.first << ":" << m_endPoint.second);
#endif
                return false;
            }

            ++m_numUnsentAsyncMessages;
        }

        // Create new dynamic message and initialise it with the source message.
        msgItem.second = 0;
        msgItem.first  = std::make_shared<defs::char_buffer_t>(sourceMsg);
    }
    // Using pool messages.
    else
    {
        // Reduce mutex scope.
        {
            std::lock_guard<std::mutex> lock{m_asyncPoolMutex};

            if (m_availablePoolIndices.empty())
            {
#if defined(CORELIB_SOCKET_DEBUG)
                DEBUG_MESSAGE_EX_WARNING(
                    "Cannot send async pool message, no pool messages currently available, for: "
                    << m_endPoint.first << ":" << m_endPoint.second);
#endif
                // No message pool slots available at the moment.
                return false;
            }

            ++m_numUnsentAsyncMessages;

            // Get message index to use.
            msgItem.second = m_availablePoolIndices.front();

            // Remove index from queue of usable pool indices.
            m_availablePoolIndices.pop_front();

            // Get pointer to usable message buffer.
            msgItem.first = m_msgPool[msgItem.second];
        }

        // Initialise pool message with copy of source message.
        msgItem.first->assign(sourceMsg.begin(), sourceMsg.end());
    }

    return true;
}

void TcpConnection::ConnectHandler(boost::system::error_code const&                  errorIn,
                                   std::shared_ptr<boost::system::error_code> const& errorOut,
                                   size_t connectionCounter) NO_EXCEPT_
{
    if (errorOut)
    {
        *errorOut = errorIn;
    }

    if (CurrentConnectionId() == connectionCounter)
    {
        m_connectEvent.Signal();
    }
}

size_t TcpConnection::NextConnectionId() NO_EXCEPT_
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto                        id = ++m_nextConnectionId;
    return id;
}

void TcpConnection::SetCurrentConnectionId(size_t currentConnectionId) NO_EXCEPT_
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentConnectionId = currentConnectionId;
}

size_t TcpConnection::CurrentConnectionId() const NO_EXCEPT_
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentConnectionId;
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
