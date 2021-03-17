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
    using async_send_t = std::function<void(defs::char_buffer_t const&)>;

public:
    AsyncSendCallableObj()                            = default;
    ~AsyncSendCallableObj()                           = default;
    AsyncSendCallableObj(AsyncSendCallableObj const&) = default;
    AsyncSendCallableObj(AsyncSendCallableObj&&)      = default;
    AsyncSendCallableObj& operator=(AsyncSendCallableObj const&) = default;
    AsyncSendCallableObj& operator=(AsyncSendCallableObj&&) = default;

    AsyncSendCallableObj(defs::char_buffer_t const& message, async_send_t const& sendFn)
        : messageBufPtr(std::make_shared<defs::char_buffer_t>(message))
        , asyncSendFn(sendFn)
    {
    }

    void operator()() const
    {
        if (asyncSendFn && messageBufPtr && !messageBufPtr->empty())
        {
            asyncSendFn(*messageBufPtr);
        }
    }

private:
    std::shared_ptr<defs::char_buffer_t> messageBufPtr;
    async_send_t                         asyncSendFn;
};

// ****************************************************************************
// 'class TcpConnection' definition
// ****************************************************************************
TcpConnection::TcpConnection(boost_iocontext_t& ioContext, TcpConnections& connections,
                             size_t                                  minAmountToRead,
                             const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                             const defs::message_received_handler_t& messageReceivedHandler,
                             eSendOption                             sendOption, 
							 size_t maxAllowedUnsentAsyncMessages)
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

void TcpConnection::Connect(const defs::connection_t& endPoint)
{
    boost_tcp_t::endpoint tcpEndPoint(boost_address_t::from_string(endPoint.first),
                                      endPoint.second);

    m_socket.connect(tcpEndPoint);
    boost_tcp_t::no_delay option(m_sendOption == eSendOption::nagleOff);
    m_socket.set_option(option);

    StartAsyncRead();
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
	if (IncrementUnsentAsyncCounter())
    {
		 AsyncSendCallableObj callableObj(
            message, boost::bind(&TcpConnection::AsyncWriteToSocket, shared_from_this(), message));

        // Wrap in a strand to make sure we don't get weird issues
        // with the send event signalling and waiting. As we're
        // sending async in this case so we could get another
        // call to this method before the original async write
        // has completed.
        m_strand.post(callableObj);
	    return true;
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
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_numUnsentAsyncMessages;
}

void TcpConnection::AsyncWriteToSocket(defs::char_buffer_t const& message)
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
	
	DecrementUnsentAsyncCounter();

    if (bytesSent != message.size())
    {
        DestroySelf();
    }
}

bool TcpConnection::IncrementUnsentAsyncCounter()
{
    std::lock_guard<std::mutex> lock{m_mutex};

    if (m_numUnsentAsyncMessages < m_maxAllowedUnsentAsyncMessages)
    {
        ++m_numUnsentAsyncMessages;
        return true;
    }

    return false;
}

void TcpConnection::DecrementUnsentAsyncCounter()
{
    std::lock_guard<std::mutex> lock{m_mutex};

    if (m_numUnsentAsyncMessages > 0)
    {
        --m_numUnsentAsyncMessages;
    }
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
