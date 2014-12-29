
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
 * \file TcpTypedServer.hpp
 * \brief File containing TCP typed server class declaration.
 */

#ifndef TCPTYPEDSERVER_HPP
#define TCPTYPEDSERVER_HPP

#include "Asio/TcpServer.hpp"
#include "Asio/MessageUtils.hpp"
#include <iterator>
#include <algorithm>
#include <cstring>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class TcpTypedServer final
{
public:
	TcpTypedServer(boost_ioservice& ioService
				   , const unsigned short listenPort
				   , const defs::message_dispatcher& messageDispatcher
				   , const eSendOption sendOption = eSendOption::nagleOn
				   , const std::string& magicString = defs::DEFAULT_MAGIC_STRING);

	TcpTypedServer(const unsigned short listenPort
				   , const defs::message_dispatcher& messageDispatcher
				   , const eSendOption sendOption = eSendOption::nagleOn
				   , const std::string& magicString = defs::DEFAULT_MAGIC_STRING);

	~TcpTypedServer() = default;
	TcpTypedServer(const TcpTypedServer& ) = delete;
	TcpTypedServer& operator=(const TcpTypedServer& ) = delete;

	auto GetServerDetailsForClient(const defs::connection& client) const
			 -> defs::connection;

	unsigned short ListenPort() const;

	void CloseAcceptor();

	void OpenAcceptor();

	void SendMessageToClientAsync(const defs::connection& client, const uint32_t messageId
								  , const defs::eArchiveType archive = defs::eArchiveType::portableBinary
								  , const defs::connection& responseAddress = defs::NULL_CONNECTION);

	bool SendMessageToClientSync(const defs::connection& client, const uint32_t messageId
								 , const defs::eArchiveType archive = defs::eArchiveType::portableBinary
								 , const defs::connection& responseAddress = defs::NULL_CONNECTION);

	void SendMessageToAllClients(const uint32_t messageId
								 , const defs::eArchiveType archive = defs::eArchiveType::portableBinary
								 , const defs::connection& responseAddress = defs::NULL_CONNECTION);

	template<typename T>
	void SendMessageToClientAsync(const T& message, const defs::connection& client, const uint32_t messageId
								  , const defs::eArchiveType archive = defs::eArchiveType::portableBinary
								  , const defs::connection& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(message, client, messageId, responseAddress, archive);
		m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
	}

	template<typename T>
	bool SendMessageToClientSync(const T& message, const defs::connection& client, const uint32_t messageId
								 , const defs::eArchiveType archive = defs::eArchiveType::portableBinary
								 , const defs::connection& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(message, client, messageId, responseAddress, archive);
		return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
	}

	template<typename T>
	void SendMessageToAllClients(const T& message, const uint32_t messageId
								 , const defs::eArchiveType archive = defs::eArchiveType::portableBinary
								 , const defs::connection& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(message, defs::NULL_CONNECTION, messageId
										  , responseAddress, archive);
		m_tcpServer.SendMessageToAllClients(messageBuffer);
	}

private:
	messages::MessageHandler m_messageHandler;
	TcpServer m_tcpServer;

	auto BuildMessage(const defs::connection& client, const uint32_t messageId
								, const defs::connection& responseAddress
								, const defs::eArchiveType archive) const -> defs::char_buffer;
	template<typename T>
	auto BuildMessage(T&& message, const defs::connection& client, const uint32_t messageId
					  , const defs::connection& responseAddress
					  , const defs::eArchiveType archive) const -> defs::char_buffer
	{
		auto responseConn = (responseAddress == defs::NULL_CONNECTION)
							? GetServerDetailsForClient(client)
							: responseAddress;
		return messages::BuildMessageBuffer(m_messageHandler.MagicString(), message, messageId
											, responseConn, archive);
	}
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT_HPP
