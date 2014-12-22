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
 * \file TcpTypedServer.cpp
 * \brief File containing TCP typed server class definition.
 */

#include "../TcpTypedServer.hpp"

namespace core_lib {
namespace asio {
namespace tcp {

TcpTypedServer::TcpTypedServer(boost_ioservice& ioService
			   , const unsigned short listenPort
			   , const defs::message_dispatcher& messageDispatcher
			   , const eSendOption sendOption)
	: m_messageHandler(messageDispatcher)
	, m_tcpServer(ioService, listenPort, sizeof(defs::MessageHeader)
				  , std::bind(&messages::MessageHandler::CheckBytesLeftToRead, std::placeholders::_1)
				  , std::bind(&messages::MessageHandler::MessageReceivedHandler
							  , &m_messageHandler, std::placeholders::_1)
				  , sendOption)
{
}

TcpTypedServer::TcpTypedServer(const unsigned short listenPort
			   , const defs::message_dispatcher& messageDispatcher
			   , const eSendOption sendOption)
	: m_messageHandler(messageDispatcher)
	, m_tcpServer(listenPort, sizeof(defs::MessageHeader)
				  , std::bind(&messages::MessageHandler::CheckBytesLeftToRead, std::placeholders::_1)
				  , std::bind(&messages::MessageHandler::MessageReceivedHandler
							  , &m_messageHandler, std::placeholders::_1)
				  , sendOption)
{
}

auto TcpTypedServer::GetServerDetailsForClient(const defs::connection& client) const -> defs::connection
{
	return m_tcpServer.GetServerDetailsForClient(client);
}

unsigned short TcpTypedServer::ListenPort() const
{
	return m_tcpServer.ListenPort();
}

void TcpTypedServer::CloseAcceptor()
{
	m_tcpServer.CloseAcceptor();
}

void TcpTypedServer::OpenAcceptor()
{
	m_tcpServer.OpenAcceptor();
}

void TcpTypedServer::SendMessageToClientAsync(const defs::connection& client, const uint32_t messageId
											  , const defs::eArchiveType archive
											  , const defs::connection& responseAddress)
{
	auto messageBuffer = BuildMessageHeaderOnly(client, messageId, responseAddress, archive);
	m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
}

bool TcpTypedServer::SendMessageToClientSync(const defs::connection& client, const uint32_t messageId
											 , const defs::eArchiveType archive
											 , const defs::connection& responseAddress)
{
	auto messageBuffer = BuildMessageHeaderOnly(client, messageId, responseAddress, archive);
	return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
}

void TcpTypedServer::SendMessageToAllClients(const uint32_t messageId, const defs::eArchiveType archive
											 , const defs::connection& responseAddress)
{
	auto messageBuffer = BuildMessageHeaderOnly(defs::NULL_CONNECTION, messageId, responseAddress
												, archive);
	m_tcpServer.SendMessageToAllClients(messageBuffer);
}

auto TcpTypedServer::BuildMessageHeaderOnly(const defs::connection& client, const uint32_t messageId
											, const defs::connection& responseAddress
											, const defs::eArchiveType archive) const
		 -> defs::char_buffer
{
	auto responseConn = (responseAddress == defs::NULL_CONNECTION)
						? GetServerDetailsForClient(client)
						: responseAddress;
	return messages::BuildMessageBufferHeaderOnly(messageId, responseConn, archive);
}


} // namespace tcp
} // namespace asio
} // namespace core_lib
