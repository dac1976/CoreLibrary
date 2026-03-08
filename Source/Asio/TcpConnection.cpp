// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
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
#include "Asio/TcpConnections.h"
#include <algorithm>
#include <limits>
#include <boost/bind.hpp>
#if defined(USE_SOCKET_DEBUG)
#include <boost/exception/all.hpp>
#include "DebugLog/DebugLogging.h"
#endif

namespace core_lib
{
namespace asio
{
namespace tcp
{

// ****************************************************************************
// 'class TcpConnection' definition
// ****************************************************************************
TcpConnection::TcpConnection(asio_compat::io_service_t&                    ioService,
                             const std::shared_ptr<TcpConnections>&     connections,
                             const defs::check_bytes_left_to_read_t&    checkBytesLeftToRead,
                             const defs::message_received_handler_t&    messageReceivedHandler,
                             const TcpConnSettings&                  settings,
                             const defs::message_received_handler_ex_t& messageReceivedHandlerEx,
                             const defs::check_bytes_left_to_read_ex_t& checkBytesLeftToReadEx)
    : m_closing{false}
    , m_strand(asio_compat::make_strand(ioService))
    , m_connections{connections}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_checkBytesLeftToReadEx{checkBytesLeftToReadEx}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_messageReceivedHandlerEx{messageReceivedHandlerEx}
    , m_settings{settings}
    , m_connectEvent(threads::eNotifyType::signalOneThread, threads::eResetCondition::manualReset,
                  threads::eIntialCondition::notSignalled)
    , m_socket{ioService}
{
    InitialiseMsgPool();

#if defined(USE_SOCKET_DEBUG)
    DEBUG_MESSAGE_EX_DEBUG(
        "Reserving memory for receive and send buffers as: " << DEFAULT_RESERVED_SIZE << " bytes");
#endif

    m_messageBuffer.reserve(DEFAULT_LARGE_RESERVED_SIZE);
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
#if defined(USE_SOCKET_DEBUG)
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
#if defined(USE_SOCKET_DEBUG)
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
#if defined(USE_SOCKET_DEBUG)
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

#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_INFO("Calling StartAsyncRead for first time after connection for: "
                              << endPoint.first << ":" << endPoint.second);
#endif

        StartAsyncRead(endPoint);
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
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
#if defined(USE_SOCKET_DEBUG)
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

void TcpConnection::StartAsyncRead(const defs::connection_t& endPoint)
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
    // Wrap in strand just to be safe in case of multiple threads running the IO service.
    boost_asio::async_read(m_socket,
                           boost_asio::buffer(m_receiveBuffer.data(), amountToRead),
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
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in ReadComplete, error: "
                               << error.message() << ", will safely self-destruct, for: "
                               << m_endPoint.first << ":" << m_endPoint.second);
#endif
        DestroySelf();
        return;
    }

    if (bytesReceived != bytesExpected)
    {
#if defined(USE_SOCKET_DEBUG)
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
                numBytes    = m_settings.minAmountToRead;
                clearMsgBuf = true;
            }
            else
            {
                // We do not want to ever reallocate the m_receiveBuffer beyond its initial
                // size, as this would cause a significant performance hit. We will always
                // read in up to DEFAULT_SMALL_RESERVED_SIZE chunks.
                numBytes = std::min(numBytes, static_cast<size_t>(DEFAULT_SMALL_RESERVED_SIZE));
            }
        }
        catch (...)
        {
#if defined(USE_SOCKET_DEBUG)
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
        if (m_messageBuffer.capacity() > DEFAULT_LARGE_RESERVED_SIZE)
        {
            defs::char_buffer_t tmp;
            tmp.reserve(DEFAULT_LARGE_RESERVED_SIZE);
            m_messageBuffer.swap(tmp);
        }
        else
        {
            m_messageBuffer.clear();
        }
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

bool TcpConnection::SendMessageAsync(defs::char_buf_cspan_t message)
{
    // Copying overload: accepts if we can reserve a slot.
    // We *prepare* the pool/dynamic backing store before posting to the strand to avoid
    // per-send allocations when the pool is enabled.
    // "true" means accepted/queued for sending by this connection (not that it reached the wire).
    if (IsClosing())
    {
        return false;
    }

	PendingWrite w;

	if (!AcquirePendingWrite(message, w))
	{
		return false;
	}

    try
    {
        asio_compat::post(m_strand, EnqueuePreparedSendHandler(shared_from_this(), std::move(w)));

        return true;
    }
    catch (...)
    {
        // Extremely rare (post throwing), but keep reservation accurate.
        if (w.kind == PendingWrite::eKind::pool)
        {
            ReleasePoolIndex(w.poolIndex);
        }

        m_numUnsentAsyncMessages.fetch_sub(1, std::memory_order_acq_rel);

#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in SendMessageAsync(post), error: "
                               << boost::current_exception_diagnostic_information()
                               << ", for: " << m_endPoint.first << ":" << m_endPoint.second);
#endif
        return false;
    }
}

bool TcpConnection::SendMessageSync(defs::char_buf_cspan_t message)
{
	if (IsClosing())
    {
        return false;
    }

    size_t bytesSent;

    try
    {
        bytesSent = boost_asio::write(m_socket, boost_asio::buffer(message.data(), message.size()));
    }
    catch (...)
    {
        bytesSent = 0;
    }

    bool success = (bytesSent == message.size());

    if (!success)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in SendMessageSync, bytesSent != message.size(), will safely "
                               "self-destruct, for: "
                               << m_endPoint.first << ":" << m_endPoint.second);
#endif
        DestroySelf();
    }

    return success;
}

// -----------------------------------------------------------------------------
// High-performance async write pipeline (strand-only)
// -----------------------------------------------------------------------------

bool TcpConnection::TryAcquirePoolIndex(size_t& idx)
{
    std::lock_guard<std::mutex> lock{m_poolMutex};

    if (m_availablePoolIndices.empty())
    {
        return false;
    }

    idx = m_availablePoolIndices.back();
    m_availablePoolIndices.pop_back();
    return true;
}

void TcpConnection::ReleasePoolIndex(size_t idx)
{
    std::lock_guard<std::mutex> lock{m_poolMutex};
    m_availablePoolIndices.push_back(idx);
}

// Uses move semantics hence pass by value
void TcpConnection::EnqueuePreparedSendOnStrand(PendingWrite w)
{
    if (IsClosing())
    {
        // If we already took a pool slot, return it.
        if (w.kind == PendingWrite::eKind::pool)
        {
            ReleasePoolIndex(w.poolIndex);
        }
        DecrementUnsentAsyncCounterOnStrand();
        return;
    }

    m_pendingWrites.push_back(std::move(w));
    StartNextWriteOnStrand();
}

void TcpConnection::StartNextWriteOnStrand()
{
    if (m_writeInProgress)
    {
        return;
    }

    if (m_pendingWrites.empty())
    {
        return;
    }

    m_writeInProgress = true;
    DoAsyncWriteOnStrand(m_pendingWrites.front());
}

void TcpConnection::DoAsyncWriteOnStrand(const PendingWrite& w)
{
    if (w.kind == PendingWrite::eKind::pool)
    {
        auto& block = m_msgPool[w.poolIndex];
        boost_asio::async_write(
            m_socket,
            boost_asio::buffer(block.data(), w.len),
            asio_compat::wrap(
                m_strand,
                boost::bind(&TcpConnection::WriteCompleteOnStrand,
                            shared_from_this(),
                            _1,
                            _2)));
    }
    else
    {
        boost_asio::async_write(
            m_socket,
            boost_asio::buffer(w.dyn->data(), w.len),
            asio_compat::wrap(
                m_strand,
                boost::bind(&TcpConnection::WriteCompleteOnStrand, shared_from_this(), _1, _2)));
    }
}

void TcpConnection::WriteCompleteOnStrand(const boost_sys::error_code& error,
                                          CORELIB_ARG_MAYBE_UNUSED size_t  bytesTransferred)
{
    CORELIB_UNUSED_ARG(bytesTransferred)

    if (m_pendingWrites.empty())
    {
        // Should not happen, but keep state consistent.
        m_writeInProgress = false;
        return;
    }

    // Release resources for the completed write.
    ReleasePendingWriteOnStrand(m_pendingWrites.front());
    m_pendingWrites.pop_front();

    m_writeInProgress = false;

    if (error)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error detected in async_write completion handler, error: "
                               << error.message() << ", will safely destroy self, for: "
                               << m_endPoint.first << ":" << m_endPoint.second);
#endif
        // Drain anything still queued so counters/pool slots don't leak.
        while (!m_pendingWrites.empty())
        {
            ReleasePendingWriteOnStrand(m_pendingWrites.front());
            m_pendingWrites.pop_front();
        }

        DestroySelf();
        return;
    }

    StartNextWriteOnStrand();
}

void TcpConnection::ReleasePendingWriteOnStrand(const PendingWrite& w)
{
    // Return pool index if pool-backed.
    if (w.kind == PendingWrite::eKind::pool)
    {
        ReleasePoolIndex(w.poolIndex);
    }

    DecrementUnsentAsyncCounterOnStrand();
}

size_t TcpConnection::NumberOfUnsentAsyncMessages() const
{
    return m_numUnsentAsyncMessages.load(std::memory_order_acquire);
}

void TcpConnection::DecrementUnsentAsyncCounterOnStrand()
{
    // Strand-only decrement paired with the reservation in SendMessageAsync.
    // Use fetch_sub to remain safe even if mis-called.
    m_numUnsentAsyncMessages.fetch_sub(1, std::memory_order_acq_rel);
}

void TcpConnection::InitialiseMsgPool()
{
    m_availablePoolIndices.clear();

    if (0 == m_settings.maxAllowedUnsentAsyncMessages)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_DEBUG(
            "Async sending message pool NOT being used because memPoolMsgCount = "
            << m_settings.maxAllowedUnsentAsyncMessages << ", for: " << m_endPoint.first << ":"
            << m_endPoint.second);
#endif
        m_msgPool.clear();
        return;
    }

#if defined(USE_SOCKET_DEBUG)
    DEBUG_MESSAGE_EX_DEBUG("Async sending message pool will be used with memPoolMsgCount = "
                           << m_settings.maxAllowedUnsentAsyncMessages
                           << " and defaultMsgSize = " << m_settings.sendPoolMsgSize
                           << ", for: " << m_endPoint.first << ":" << m_endPoint.second);
#endif

    // If sendPoolMsgSize == 0 then we disable pool buffers and fall back to dynamic allocations.
    // If sendPoolMsgSize > 0 we create fixed-size pool blocks (size == sendPoolMsgSize) so they
    // never resize during sends. Messages larger than sendPoolMsgSize will automatically fall back
    // to dynamic sends.

    m_msgPool.clear();
    m_availablePoolIndices.clear();

    if (m_settings.sendPoolMsgSize == 0)
    {
        return;
    }

    const size_t poolCount = m_settings.maxAllowedUnsentAsyncMessages;
    const size_t poolSize  = m_settings.sendPoolMsgSize;

    m_msgPool.resize(poolCount);

    for (auto& b : m_msgPool)
    {
        b.resize(poolSize);
    }

    m_availablePoolIndices.reserve(poolCount);

    for (size_t i = 0; i < poolCount; ++i)
    {
        m_availablePoolIndices.push_back(i);
    }
}

void TcpConnection::ConnectHandler(const boost::system::error_code&                  errorIn,
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

bool TcpConnection::AcquirePendingWrite(defs::char_buf_cspan_t message, PendingWrite& w)
{
	 const size_t maxAllowed = m_settings.maxAllowedUnsentAsyncMessages;

    if (0 == maxAllowed)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_DEBUG("Cannot send async message, max allowed unsent async messages is 0, for: "
                                 << m_endPoint.first << ":" << m_endPoint.second);
#endif
        return false;
    }

    const size_t prev = m_numUnsentAsyncMessages.fetch_add(1, std::memory_order_acq_rel);

    if (prev >= maxAllowed)
    {
        m_numUnsentAsyncMessages.fetch_sub(1, std::memory_order_acq_rel);

#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_WARNING("Cannot send async message, currently at unsent async "
                                 "message count limit, for: "
                                 << m_endPoint.first << ":" << m_endPoint.second);
#endif
        return false;
    }

    w.kind = PendingWrite::eKind::dynamic;

    // Prefer pool when enabled AND message fits.
    const bool   poolEnabled = !m_msgPool.empty();
    const size_t poolCap     = m_settings.sendPoolMsgSize;

    if (poolEnabled && (poolCap > 0) && (message.size() <= poolCap))
    {
        size_t idx = 0;

		if (TryAcquirePoolIndex(idx))
        {
            // We exclusively own this pool slot now; safe to write off-strand.
            auto& block = m_msgPool[idx];

            // Pool blocks are fixed-size (size == poolCap). Do NOT resize.
            if (block.size() < poolCap)
            {
                // Defensive: should not happen after InitialiseMsgPool().
                block.resize(poolCap);
            }

            if (!message.empty())
            {
                std::memcpy(block.data(), message.data(), message.size());
            }

            w.kind      = PendingWrite::eKind::pool;
            w.poolIndex = idx;
            w.len       = message.size();
        }
    }

    if (w.kind == PendingWrite::eKind::dynamic)
    {
        try
        {
            w.dyn = std::make_shared<defs::char_buffer_t>(message.begin(), message.end()); // copy
            w.len = w.dyn->size();
        }
        catch (...)
        {
            // Allocation failure: keep reservation accurate.
            m_numUnsentAsyncMessages.fetch_sub(1, std::memory_order_acq_rel);

#if defined(USE_SOCKET_DEBUG)
            DEBUG_MESSAGE_EX_ERROR("Error allocating dynamic async send buffer, error: "
                                   << boost::current_exception_diagnostic_information()
                                   << ", for: " << m_endPoint.first << ":" << m_endPoint.second);
#endif
            return false;
        }
    }

	return true;
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
