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
 * \file TcpServer.cpp
 * \brief File containing TCP server class definition.
 */

#include "Asio/TcpServer.h"
#include <stdexcept>
#include <boost/bind.hpp>
#include "Asio/TcpConnection.h"

namespace core_lib
{
namespace asio
{
namespace tcp
{

// ****************************************************************************
// 'class TcpServer' definition
// ****************************************************************************
TcpServer::TcpServer(boost_iocontext_t& ioContext, uint16_t listenPort, size_t minAmountToRead,
                     const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                     const defs::message_received_handler_t& messageReceivedHandler,
                     eSendOption sendOption, size_t maxAllowedUnsentAsyncMessages,
                     size_t sendPoolMsgSize)
    : m_ioContext(ioContext)
    , m_strand{ioContext}
    , m_listenPort{listenPort}
    , m_minAmountToRead{minAmountToRead}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_sendOption{sendOption}
    , m_maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages)
    , m_sendPoolMsgSize(sendPoolMsgSize)
{
    OpenAcceptor();
}

TcpServer::TcpServer(uint16_t listenPort, size_t minAmountToRead,
                     const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                     const defs::message_received_handler_t& messageReceivedHandler,
                     eSendOption sendOption, size_t maxAllowedUnsentAsyncMessages,
                     size_t sendPoolMsgSize)
    : m_ioThreadGroup{new IoContextThreadGroup(2)}
    , m_ioContext(m_ioThreadGroup->IoContext())
    , m_strand{m_ioThreadGroup->IoContext()}
    , m_listenPort{listenPort}
    , m_minAmountToRead{minAmountToRead}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_sendOption{sendOption}
    , m_maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages)
    , m_sendPoolMsgSize(sendPoolMsgSize)
{
    OpenAcceptor();
}

TcpServer::~TcpServer()
{
    CloseAcceptor();
}

auto TcpServer::GetServerDetailsForClient(const defs::connection_t& client) const
    -> defs::connection_t
{
    return client == defs::NULL_CONNECTION ? std::make_pair("0.0.0.0", m_listenPort)
                                           : m_clientConnections.GetLocalEndForRemoteEnd(client);
}

uint16_t TcpServer::ListenPort() const
{
    return m_listenPort;
}

size_t TcpServer::NumberOfClients() const
{
    return m_clientConnections.Size();
}

void TcpServer::CloseAcceptor()
{
    if (m_acceptor && m_acceptor->is_open())
    {
        boost_asio::post(m_strand, boost::bind(&TcpServer::ProcessCloseAcceptor, this));
        m_closedEvent.Wait();
    }

    m_clientConnections.CloseConnections();
}

void TcpServer::OpenAcceptor()
{
    if (!m_acceptor || !m_acceptor->is_open())
    {
        m_acceptor.reset();
        m_acceptor = std::make_unique<boost_tcp_acceptor_t>(
            m_ioContext, boost_tcp_t::endpoint(boost_tcp_t::v4(), m_listenPort));

        boost_asio::post(m_strand, boost::bind(&TcpServer::AcceptConnection, this));
    }
}

bool TcpServer::SendMessageToClientAsync(const defs::connection_t&  client,
                                         const defs::char_buffer_t& message) const
{
    return m_clientConnections.SendMessageAsync(client, message);
}

bool TcpServer::SendMessageToClientSync(const defs::connection_t&  client,
                                        const defs::char_buffer_t& message) const
{
    return m_clientConnections.SendMessageSync(client, message);
}

bool TcpServer::SendMessageToAllClients(const defs::char_buffer_t& message) const
{
    return m_clientConnections.SendMessageToAll(message);
}

size_t TcpServer::NumberOfUnsentAsyncMessages(const defs::connection_t& client) const
{
    return m_clientConnections.NumberOfUnsentAsyncMessages(client);
}

bool TcpServer::IsConnected(const defs::connection_t& client) const
{
    return m_clientConnections.IsConnected(client);
}

void TcpServer::AcceptConnection()
{
    auto connection = std::make_shared<TcpConnection>(m_ioContext,
                                                      m_clientConnections,
                                                      m_minAmountToRead,
                                                      m_checkBytesLeftToRead,
                                                      m_messageReceivedHandler,
                                                      m_sendOption,
                                                      m_maxAllowedUnsentAsyncMessages,
                                                      m_sendPoolMsgSize);
    m_acceptor->async_accept(
        connection->Socket(),
        boost::asio::bind_executor(
            m_strand,
            boost::bind(&TcpServer::AcceptHandler, this, connection, boost_placeholders::error)));
}

void TcpServer::AcceptHandler(defs::tcp_conn_ptr_t connection, const boost_sys::error_code& error)
{
    if (!error)
    {
        connection->StartAsyncRead();
    }

    if (error != boost_asio::error::operation_aborted)
    {
        boost_asio::post(m_strand, boost::bind(&TcpServer::AcceptConnection, this));
    }
}

void TcpServer::ProcessCloseAcceptor()
{
    m_acceptor->close();
    m_closedEvent.Signal();
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
