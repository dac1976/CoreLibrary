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

#include "Asio/TcpConnection.h"
#include <iterator>
#include <iterator>
#include <algorithm>
#include <boost/bind.hpp>
#include "Asio/TcpConnections.h"

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
    using async_send_t = std::function<void(defs::char_buffer_t const&, size_t)>;

public:
    AsyncSendCallableObj()                            = default;
    ~AsyncSendCallableObj()                           = default;
    AsyncSendCallableObj(AsyncSendCallableObj const&) = default;
    AsyncSendCallableObj(AsyncSendCallableObj&&)      = default;
    AsyncSendCallableObj& operator=(AsyncSendCallableObj const&) = default;
    AsyncSendCallableObj& operator=(AsyncSendCallableObj&&) = default;

    AsyncSendCallableObj(std::shared_ptr<defs::char_buffer_t> const& messageBufPtr,
                         size_t poolIndex, async_send_t const& sendFn)
        : m_messageBufPtr(messageBufPtr)
        , m_poolIndex(poolIndex)
        , m_asyncSendFn(sendFn)
    {
    }

    void operator()() const
    {
        if (m_asyncSendFn && m_messageBufPtr && !m_messageBufPtr->empty())
        {
            m_asyncSendFn(*m_messageBufPtr, m_poolIndex);
        }
    }

private:
    std::shared_ptr<defs::char_buffer_t> m_messageBufPtr;
    size_t                               m_poolIndex{0};
    async_send_t                         m_asyncSendFn;
};

// ****************************************************************************
// 'class TcpConnection' definition
// ****************************************************************************
TcpConnection::TcpConnection(boost_iocontext_t& ioContext, TcpConnections& connections,
                             size_t                                  minAmountToRead,
                             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                             defs::message_received_handler_t const& messageReceivedHandler,
                             eSendOption sendOption, size_t maxAllowedUnsentAsyncMessages,
                             size_t sendPoolMsgSize)
    : m_closing{false}
    , m_strand{ioContext}
    , m_connections(connections)
    , m_minAmountToRead{minAmountToRead}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_sendOption{sendOption}
    , m_maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages)
    , m_socket{ioContext}
{
    InitialiseMsgPool(maxAllowedUnsentAsyncMessages, sendPoolMsgSize);

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
        boost_tcp_t::endpoint tcpEndPoint(boost_address_t::from_string(endPoint.first),
                                          endPoint.second);

        boost::system::error_code connectError;
        m_socket.async_connect(
            tcpEndPoint,
            boost::bind(&TcpConnection::ConnectHandler, shared_from_this(), _1, connectError));

        // Async connect event signalled within time limit.
        if (m_connectEvent.WaitForTime(MAX_TCP_CONNECT_TIMEOUT))
        {
            if (connectError)
            {
                m_socket.close();
                return false;
            }
        }
        // Async connect timed out.
        else
        {
            m_socket.close();
            return false;
        }

        boost_tcp_t::no_delay option(m_sendOption == eSendOption::nagleOff);
        m_socket.set_option(option);

        StartAsyncRead();
    }
    catch (...)
    {
        return false;
    }

    return true;
}

void TcpConnection::CloseConnection()
{
    if (!m_socket.is_open())
    {
        return;
    }

    SetClosing(true);
    boost_asio::post(m_strand, boost::bind(&TcpConnection::ProcessCloseSocket, shared_from_this()));
    m_closedEvent.Wait();
}

void TcpConnection::SetClosing(bool closing)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_closing = closing;
}

bool TcpConnection::IsClosing() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_closing;
}

void TcpConnection::ProcessCloseSocket()
{
    try
    {
        m_socket.shutdown(m_socket.shutdown_both);
        m_socket.close();
    }
    catch (...)
    {
        // Consume error...do nothing.
    }

    m_closedEvent.Signal();
}

void TcpConnection::DestroySelf()
{
    if (!IsClosing())
    {
        m_connections.Remove(shared_from_this());
    }
}

void TcpConnection::StartAsyncRead()
{
    m_connections.Add(shared_from_this());

    boost_asio::post(
        m_strand,
        boost::bind(&TcpConnection::AsyncReadFromSocket, shared_from_this(), m_minAmountToRead));
}

void TcpConnection::AsyncReadFromSocket(size_t amountToRead)
{
    m_receiveBuffer.resize(amountToRead);
    // We do not need a strand here as a connection object
    // can only read serially from one, so for one connection
    // object it cannot be doing more than one async read
    // at a time in multiple threads.
    boost_asio::async_read(
        m_socket,
        boost_asio::buffer(m_receiveBuffer),
        boost::asio::bind_executor(m_strand,
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
        DestroySelf();
    }
    else if (bytesReceived != bytesExpected)
    {
        numBytes    = m_minAmountToRead;
        clearMsgBuf = true;
    }
    else
    {
        try
        {
            std::copy(m_receiveBuffer.begin(),
                      m_receiveBuffer.end(),
                      std::back_inserter(m_messageBuffer));

            numBytes = m_checkBytesLeftToRead(m_messageBuffer);

            if (numBytes == 0)
            {
                m_messageReceivedHandler(m_messageBuffer);
                numBytes    = m_minAmountToRead;
                clearMsgBuf = true;
            }
        }
        catch (...)
        {
            numBytes    = m_minAmountToRead;
            clearMsgBuf = true;
        }
    }

    if (clearMsgBuf)
    {
        m_messageBuffer.clear();
    }

    if (numBytes > 0)
    {
        boost_asio::post(
            m_strand,
            boost::bind(&TcpConnection::AsyncReadFromSocket, shared_from_this(), numBytes));
    }
}

bool TcpConnection::SendMessageAsync(const defs::char_buffer_t& message)
{
    try
    {
        std::pair<msg_ptr_t, size_t> msgItem;

        if (GetNewMessgeObject(msgItem, message))
        {
            AsyncSendCallableObj callableObj(
                msgItem.first,
                msgItem.second,
                boost::bind(&TcpConnection::AsyncWriteToSocket, shared_from_this(), _1, _2));

            m_strand.post(callableObj);
            return true;
        }
    }
    catch (...)
    {
        // Consume error...do nothing.
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
        DestroySelf();
    }

    return success;
}

size_t TcpConnection::NumberOfUnsentAsyncMessages() const
{
    std::lock_guard<std::mutex> lock{m_asyncPoolMutex};
    return m_numUnsentAsyncMessages;
}

void TcpConnection::AsyncWriteToSocket(defs::char_buffer_t const& message, size_t poolIndex)
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

    DecrementUnsentAsyncCounter(poolIndex);

    if (bytesSent != message.size())
    {
        DestroySelf();
    }
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

void TcpConnection::InitialiseMsgPool(size_t memPoolMsgCount, size_t defaultMsgSize)
{
    m_availablePoolIndices.clear();

    if ((0 == memPoolMsgCount) || (0 == defaultMsgSize))
    {
        m_msgPool.clear();
        return;
    }

    m_msgPool.resize(memPoolMsgCount);
    size_t index = 0;

    auto generateMsg = [defaultMsgSize, &index, this]() {
        auto msg = std::make_shared<defs::char_buffer_t>();

        if (defaultMsgSize > 0)
        {
            msg->reserve(defaultMsgSize);
        }

        m_availablePoolIndices.push_back(index++);

        return msg;
    };

    std::generate(m_msgPool.begin(), m_msgPool.end(), generateMsg);
}

bool TcpConnection::GetNewMessgeObject(std::pair<msg_ptr_t, size_t>& msgItem,
                                       defs::char_buffer_t const&    sourceMsg)
{
    if (m_msgPool.empty())
    {
        // Reduce mutex scope.
        {
            std::lock_guard<std::mutex> lock{m_asyncPoolMutex};

            if (m_numUnsentAsyncMessages == m_maxAllowedUnsentAsyncMessages)
            {
                return false;
            }

            ++m_numUnsentAsyncMessages;
        }

        // Create new dynamic message and initialise it with the source message.
        msgItem.second = 0;
        msgItem.first  = std::make_shared<defs::char_buffer_t>(sourceMsg);
    }
    else
    {
        // Reduce mutex scope.
        {
            std::lock_guard<std::mutex> lock{m_asyncPoolMutex};

            if (m_availablePoolIndices.empty())
            {
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

void TcpConnection::ConnectHandler(boost::system::error_code const& error,
                                   boost::system::error_code&       errCodeOut)
{
    errCodeOut = error;
    m_connectEvent.Signal();
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
