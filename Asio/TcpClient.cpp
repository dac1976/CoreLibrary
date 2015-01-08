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
 * \file TcpClient.cpp
 * \brief File containing TCP client class definition.
 */

#include "TcpClient.hpp"
#include "TcpConnection.hpp"

namespace core_lib {
namespace asio {
namespace tcp {

TcpClient::TcpClient(boost_ioservice_t& ioService
                     , const defs::connection_t& server
					 , const size_t minAmountToRead
                     , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
                     , const defs::message_received_handler_t& messageReceivedHandler
					 , const eSendOption sendOption)
	: m_ioService(ioService), m_server{server}
	, m_minAmountToRead{minAmountToRead}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_sendOption{sendOption}
{
	CreateConnection();
}

TcpClient::TcpClient(const defs::connection_t& server
					 , const size_t minAmountToRead
                     , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
                     , const defs::message_received_handler_t& messageReceivedHandler
					 , const eSendOption sendOption)
	: m_ioThreadGroup{new IoServiceThreadGroup(2)}
	, m_ioService(m_ioThreadGroup->IoService())
	, m_server{server}, m_minAmountToRead{minAmountToRead}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_sendOption{sendOption}
{
	CreateConnection();
}

TcpClient::~TcpClient()
{
	CloseConnection();
}

auto TcpClient::ServerConnection() const -> defs::connection_t
{
	return m_server;
}

auto TcpClient::GetClientDetailsForServer() const -> defs::connection_t
{
	return m_serverConnection.GetLocalEndForRemoteEnd(m_server);
}

void TcpClient::CloseConnection()
{
	m_serverConnection.CloseConnections();
}

void TcpClient::SendMessageToServerAsync(const defs::char_buffer_t& message)
{
	CheckAndCreateConnection();
	m_serverConnection.SendMessageAsync(m_server, message);
}

bool TcpClient::SendMessageToServerSync(const defs::char_buffer_t& message)
{
	CheckAndCreateConnection();
	return m_serverConnection.SendMessageSync(m_server, message);
}

void TcpClient::CreateConnection()
{
	try
	{
		auto connection = std::make_shared<TcpConnection>(m_ioService
														  , m_serverConnection
														  , m_minAmountToRead
														  , m_checkBytesLeftToRead
														  , m_messageReceivedHandler
														  , m_sendOption);
        connection->Connect(boost_tcp_t::endpoint(boost_address_t::from_string(m_server.first)
												, m_server.second));
	}
	catch(boost::system::system_error& )
	{
		//NOTE: We catch here because if this fails in constructor
		// we want our TcpClient to stay viable as calling
		// TcpClient::SendMessageToServer* later will attempt
		// to reconnect. Only catch boost::system::system_error
		// exceptions as any other ones are a problem we should
		// definitely still propogate.
	}
}

void TcpClient::CheckAndCreateConnection()
{
	if (m_serverConnection.Empty())
	{
		CreateConnection();
	}
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
