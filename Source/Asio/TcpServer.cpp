// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
// Contact <duncan.crutchley+corelibrary@gmail.com>
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

#include "../../Include/Asio/TcpServer.h"
#include "../../Include/Asio/TcpConnection.h"
#include "boost/bind.hpp"

namespace core_lib {
namespace asio {
namespace tcp {

// ****************************************************************************
// 'class TcpServer' definition
// ****************************************************************************
TcpServer::TcpServer(boost_ioservice_t& ioService
					 , const uint16_t listenPort
					 , const size_t minAmountToRead
					 , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
					 , const defs::message_received_handler_t& messageReceivedHandler
					 , const eSendOption sendOption)
	: m_ioService(ioService)
	, m_listenPort{listenPort}
	, m_minAmountToRead{minAmountToRead}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_sendOption{sendOption}
{
	OpenAcceptor();
}

TcpServer::TcpServer(const uint16_t listenPort
					 , const size_t minAmountToRead
					 , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
					 , const defs::message_received_handler_t& messageReceivedHandler
					 , const eSendOption sendOption)
	: m_ioThreadGroup{new IoServiceThreadGroup(std::thread::hardware_concurrency())} // Num logical cores threads as we can send/receive to/from multiple clients
	, m_ioService(m_ioThreadGroup->IoService())
	, m_listenPort{listenPort}
	, m_minAmountToRead{minAmountToRead}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_sendOption{sendOption}
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
	return client == defs::NULL_CONNECTION
			? std::make_pair("0.0.0.0", m_listenPort)
			: m_clientConnections.GetLocalEndForRemoteEnd(client);
}

uint16_t TcpServer::ListenPort() const
{
	return m_listenPort;
}

void TcpServer::CloseAcceptor()
{
	if (m_acceptor && m_acceptor->is_open())
	{
		m_ioService.post(boost::bind(&TcpServer::ProcessCloseAcceptor, this));
		m_closedEvent.Wait();
	}

	m_clientConnections.CloseConnections();
}

void TcpServer::OpenAcceptor()
{
	if (!m_acceptor || !m_acceptor->is_open())
	{
		m_acceptor.reset(new boost_tcp_acceptor_t(m_ioService,
												boost_tcp_t::endpoint(boost_tcp_t::v4()
																	, m_listenPort)));
		AcceptConnection();
	}
}

void TcpServer::SendMessageToClientAsync(const defs::connection_t& client
										 , const defs::char_buffer_t& message)
{
	m_clientConnections.SendMessageAsync(client, message);
}

bool TcpServer::SendMessageToClientSync(const defs::connection_t& client
										, const defs::char_buffer_t& message)
{
	return m_clientConnections.SendMessageSync(client, message);
}

void TcpServer::SendMessageToAllClients(const defs::char_buffer_t& message)
{
	m_clientConnections.SendMessageToAll(message);
}

void TcpServer::AcceptConnection()
{
	auto connection = std::make_shared<TcpConnection>(m_ioService
													  , m_clientConnections
													  , m_minAmountToRead
													  , m_checkBytesLeftToRead
													  , m_messageReceivedHandler
													  , m_sendOption);
	m_acceptor->async_accept(connection->Socket(),
							 boost::bind(&TcpServer::AcceptHandler
										 , this
										 , connection
										 , boost_placeholders::error));
}

void TcpServer::AcceptHandler(defs::tcp_conn_ptr_t connection
							  , const boost_sys::error_code& error)
{
	if (!error)
	{
		connection->StartAsyncRead();
	}

	if(error != boost_asio::error::operation_aborted)
	{
		AcceptConnection();
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
