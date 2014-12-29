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
 * \file TcpTypedClient.cpp
 * \brief File containing TCP typed client class definition.
 */

#include "../TcpTypedClient.hpp"

namespace core_lib {
namespace asio {
namespace tcp {

TcpTypedClient::TcpTypedClient(boost_ioservice& ioService
							   , const defs::connection& server
							   , const defs::message_dispatcher& messageDispatcher
							   , const eSendOption sendOption
							   , const std::string& magicString)
	: m_messageHandler{messageDispatcher, magicString}
	, m_tcpClient{ioService, server, sizeof(defs::MessageHeader)
				  , std::bind(&messages::MessageHandler::CheckBytesLeftToRead
							  , &m_messageHandler, std::placeholders::_1)
				  , std::bind(&messages::MessageHandler::MessageReceivedHandler
							  , &m_messageHandler, std::placeholders::_1)
				  , sendOption}
{
}

TcpTypedClient::TcpTypedClient(const defs::connection& server
							   , const defs::message_dispatcher& messageDispatcher
							   , const eSendOption sendOption
							   , const std::string& magicString)
	: m_messageHandler{messageDispatcher, magicString}
	, m_tcpClient{server, sizeof(defs::MessageHeader)
				  , std::bind(&messages::MessageHandler::CheckBytesLeftToRead
							  , &m_messageHandler, std::placeholders::_1)
				  , std::bind(&messages::MessageHandler::MessageReceivedHandler
							  , &m_messageHandler, std::placeholders::_1)
				  , sendOption}
{
}

auto TcpTypedClient::ServerConnection() const -> defs::connection
{
	return m_tcpClient.ServerConnection();
}

auto TcpTypedClient::GetClientDetailsForServer() const -> defs::connection
{
	return m_tcpClient.GetClientDetailsForServer();
}

void TcpTypedClient::CloseConnection()
{
	m_tcpClient.CloseConnection();
}

void TcpTypedClient::SendMessageToServerAsync(const uint32_t messageId, const defs::eArchiveType archive
											  , const defs::connection& responseAddress)
{
	auto messageBuffer = BuildMessage(messageId, responseAddress, archive);
	m_tcpClient.SendMessageToServerAsync(messageBuffer);
}

bool TcpTypedClient::SendMessageToServerSync(const uint32_t messageId, const defs::eArchiveType archive
											 , const defs::connection& responseAddress)
{
	auto messageBuffer = BuildMessage(messageId, responseAddress, archive);
	return m_tcpClient.SendMessageToServerSync(messageBuffer);
}

auto TcpTypedClient::BuildMessage(const uint32_t messageId
								  , const defs::connection& responseAddress
								  , const defs::eArchiveType archive) const
		 -> defs::char_buffer
{
	auto responseConn = (responseAddress == defs::NULL_CONNECTION)
						? GetClientDetailsForServer()
						: responseAddress;
	return messages::BuildMessageBuffer(m_messageHandler.MagicString(), messageId, responseConn, archive);
}


} // namespace tcp
} // namespace asio
} // namespace core_lib
