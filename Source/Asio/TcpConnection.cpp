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
#include <algorithm>
#include <boost/bind.hpp>
#include "Asio/TcpConnections.h"

namespace core_lib
{
namespace asio
{
namespace tcp
{

// ****************************************************************************
// 'class TcpConnection' definition
// ****************************************************************************
TcpConnection::TcpConnection(boost_ioservice_t& ioService, TcpConnections& connections,
                             const size_t                            minAmountToRead,
                             const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                             const defs::message_received_handler_t& messageReceivedHandler,
                             const eSendOption                       sendOption)
    : m_closing{false}
    , m_ioService(ioService)
    , m_strand{ioService}
    , m_socket{ioService}
    , m_connections(connections)
    , m_minAmountToRead{minAmountToRead}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_sendOption{sendOption}
    , m_sendSuccess{false}
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
    m_ioService.post(boost::bind(&TcpConnection::ProcessCloseSocket, shared_from_this()));
    m_closedEvent.Wait();
}

void TcpConnection::SetClosing(const bool closing)
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
    m_socket.close();
    m_sendEvent.Signal();
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
    AsyncReadFromSocket(m_minAmountToRead);
}

void TcpConnection::AsyncReadFromSocket(const size_t amountToRead)
{
    m_receiveBuffer.resize(amountToRead);
    // We do not need a strand here as a connection object
    // can only read serially from one, so for one connection
    // object it cannot be doing more than one async read
    // at a time in multiple threads.
    boost_asio::async_read(m_socket,
                           boost_asio::buffer(m_receiveBuffer),
                           boost::bind(&TcpConnection::ReadComplete,
                                       shared_from_this(),
                                       boost_placeholders::error,
                                       boost_placeholders::bytes_transferred,
                                       amountToRead));
}

void TcpConnection::ReadComplete(const boost_sys::error_code& error, const size_t bytesReceived,
                                 const size_t bytesExpected)
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
        catch (const std::exception& /*e*/)
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
        AsyncReadFromSocket(numBytes);
    }
}

void TcpConnection::SendMessageAsync(const defs::char_buffer_t& message)
{
    // Wrap in a strand to make sure we don't get weird issues
    // with the send event signalling and waiting. As we're
    // sending async in this case so we could get another
    // call to this method before the original async write
    // has completed.
    m_ioService.post(m_strand.wrap(
        boost::bind(&TcpConnection::AsyncWriteToSocket, shared_from_this(), message, false)));
}

bool TcpConnection::SendMessageSync(const defs::char_buffer_t& message)
{
    SyncWriteToSocket(message, true);
    return m_sendSuccess;
}

void TcpConnection::AsyncWriteToSocket(defs::char_buffer_t message, const bool setSuccessFlag)
{
    SyncWriteToSocket(message, setSuccessFlag);
}

void TcpConnection::SyncWriteToSocket(const defs::char_buffer_t& message, const bool setSuccessFlag)
{
    boost_asio::async_write(m_socket,
                            boost_asio::buffer(message),
                            boost::bind(&TcpConnection::WriteComplete,
                                        shared_from_this(),
                                        boost_placeholders::error,
                                        boost_placeholders::bytes_transferred,
                                        setSuccessFlag));
    // Wait here until WriteComplete signals, this makes sure the
    // message vector remains viable.
    m_sendEvent.Wait();
}

void TcpConnection::WriteComplete(const boost_sys::error_code& error, const std::size_t bytesSent,
                                  const bool setSuccessFlag)
{
    if (setSuccessFlag)
    {
        m_sendSuccess = !error && (bytesSent > 0);
    }

    m_sendEvent.Signal();

    if (error)
    {
        DestroySelf();
    }
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
