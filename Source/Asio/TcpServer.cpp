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
 * \file TcpServer.cpp
 * \brief File containing TCP server class definition.
 */
#include "Asio/TcpServer.h"
#include "Asio/TcpConnection.h"
#if defined(USE_SOCKET_DEBUG)
#include "DebugLogging.h"
#endif
#include <boost/bind.hpp>

namespace core_lib
{
namespace asio
{
namespace tcp
{

// ****************************************************************************
// 'class TcpServer' definition
// ****************************************************************************
TcpServer::TcpServer(asio_compat::io_service_t& ioService,
                 uint16_t listenPort,
                 defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                 defs::message_received_handler_t const& messageReceivedHandler,
                 TcpConnSettings const& settings,
                 defs::message_received_handler_ex_t const& messageReceivedHandlerEx,
                 defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx)
    : m_ioService(ioService)
    , m_strand(asio_compat::make_strand(ioService))
    , m_listenPort{listenPort}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_checkBytesLeftToReadEx{checkBytesLeftToReadEx}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_messageReceivedHandlerEx{messageReceivedHandlerEx}
    , m_settings{settings}
    , m_clientConnections(std::make_shared<TcpConnections>())
{
    OpenAcceptor();
}

// When using an internal IO service we'll only use 2 threads, which for regular usage
// of the socket will be good enough for sending and receiving. For better threading
// control use an external IO service thread group.
TcpServer::TcpServer(uint16_t listenPort,
                 defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                 defs::message_received_handler_t const& messageReceivedHandler,
                 TcpConnSettings const& settings,
                 defs::message_received_handler_ex_t const& messageReceivedHandlerEx,
                 defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx)
    : m_ioThreadGroup{new IoContextThreadGroup(2)}
    , m_ioService(m_ioThreadGroup->IoService())
    , m_strand{asio_compat::make_strand(m_ioThreadGroup->IoService())}
    , m_listenPort{listenPort}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_checkBytesLeftToReadEx{checkBytesLeftToReadEx}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_messageReceivedHandlerEx{messageReceivedHandlerEx}
    , m_settings{settings}
    , m_clientConnections(std::make_shared<TcpConnections>())
{
    OpenAcceptor();
}

TcpServer::~TcpServer()
{
    CloseAcceptor();
}

void TcpServer::SetOnCloseCallback(defs::on_close_t const& onClose)
{
    m_clientConnections->SetOnCloseCallback(onClose);
}

auto TcpServer::GetServerDetailsForClient(const defs::connection_t& client) const
    -> defs::connection_t
{
    return client == defs::NULL_CONNECTION ? std::make_pair("0.0.0.0", m_listenPort)
                                     : m_clientConnections->GetLocalEndForRemoteEnd(client);
}

uint16_t TcpServer::ListenPort() const
{
    return m_listenPort;
}

size_t TcpServer::NumberOfClients() const
{
    return m_clientConnections->Size();
}

void TcpServer::CloseAcceptor()
{
    // Close acceptor on the strand to serialize with AcceptConnection/AcceptHandler
    if (m_acceptor && m_acceptor->is_open())
    {
        // Dispatch for maximum safety, in case CloseAcceptor is called via work
        // posted to an io_service thread from elsewhere.
        asio_compat::dispatch(m_strand, boost::bind(&TcpServer::ProcessCloseAcceptor, this));

        m_closedEvent.Wait();
    }

    // Close all client sockets after acceptor shutdown is sequenced
    m_clientConnections->CloseConnections();
}

void TcpServer::OpenAcceptor()
{
    if (!m_acceptor || !m_acceptor->is_open())
    {
        m_acceptor.reset(new boost_tcp_acceptor_t(
            m_ioService, boost_tcp_t::endpoint(boost_tcp_t::v4(), m_listenPort)));

        asio_compat::post(m_strand, boost::bind(&TcpServer::AcceptConnection, this));
    }
}

bool TcpServer::SendMessageToClientAsync(const defs::connection_t&  client,
                                        defs::char_buf_cspan_t message) const
{
    return m_clientConnections->SendMessageAsync(client, message);
}

bool TcpServer::SendMessageToClientSync(const defs::connection_t&  client,
                                        defs::char_buf_cspan_t message) const
{
    return m_clientConnections->SendMessageSync(client, message);
}

void TcpServer::SendMessageToAllClients(defs::char_buf_cspan_t message) const
{
    m_clientConnections->SendMessageToAll(message);
}

size_t TcpServer::NumberOfUnsentAsyncMessages(const defs::connection_t& client) const
{
    return m_clientConnections->NumberOfUnsentAsyncMessages(client);
}

bool TcpServer::IsConnected(const defs::connection_t& client) const
{
    return m_clientConnections->IsConnected(client);
}

void TcpServer::AcceptConnection()
{
    auto connection = std::make_shared<TcpConnection>(m_ioService,
                                                      m_clientConnections,
                                                      m_checkBytesLeftToRead,
                                                      m_messageReceivedHandler,
                                                      m_settings,
                                                      m_messageReceivedHandlerEx,
                                                      m_checkBytesLeftToReadEx);
    m_acceptor->async_accept(
        connection->Socket(),
        asio_compat::wrap(
            m_strand,
            boost::bind(&TcpServer::AcceptHandler, this, connection, boost_placeholders::error)));
}

void TcpServer::AcceptHandler(const defs::tcp_conn_ptr_t&  connection,
                              const boost_sys::error_code& error)
{
    // If accept succeeded, start async reads on the new connection
    if (!error)
    {
        boost_sys::error_code ec;
        auto                  rep = connection->Socket().remote_endpoint(ec);
        if (!ec)
        {
            auto endpoint = std::make_pair(rep.address().to_string(), rep.port());
            connection->StartAsyncRead(endpoint);
        }
        else
        {
#if defined(USE_SOCKET_DEBUG)
            DEBUG_MESSAGE_EX_ERROR("Failed to get remote endpoint in AccepHandler.");
#endif
            return;
        }
    }

    // Continue accepting unless we were explicitly aborted during shutdown
    if (error != boost_asio::error::operation_aborted)
    {
        // We dispatch instead of post as we are already with a strand protected posted
        // branch of execution, so dispatch will be faster and still safe.
        asio_compat::dispatch(m_strand, boost::bind(&TcpServer::AcceptConnection, this));
    }
}

void TcpServer::ProcessCloseAcceptor()
{
    if (m_acceptor && m_acceptor->is_open())
    {
        boost_sys::error_code ec;
        m_acceptor->close(ec); // avoid exceptions
    }

    m_closedEvent.Signal();
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
