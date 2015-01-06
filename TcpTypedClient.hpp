
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
 * \file TcpTypedClient.hpp
 * \brief File containing TCP typed client class declaration.
 */

#ifndef TCPTYPEDCLIENT_HPP
#define TCPTYPEDCLIENT_HPP

#include "Asio/TcpClient.hpp"
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

class TcpTypedClient final
{
public:
	TcpTypedClient(boost_ioservice& ioService
			  , const defs::connection& server
			  , const defs::message_dispatcher& messageDispatcher
			  , const eSendOption sendOption = eSendOption::nagleOn
			  , const std::string& magicString = defs::DEFAULT_MAGIC_STRING);

	TcpTypedClient(const defs::connection& server
			  , const defs::message_dispatcher& messageHandler
			  , const eSendOption sendOption = eSendOption::nagleOn
			  , const std::string& magicString = defs::DEFAULT_MAGIC_STRING);

	~TcpTypedClient() = default;
	TcpTypedClient(const TcpTypedClient& ) = delete;
	TcpTypedClient& operator=(const TcpTypedClient& ) = delete;

	auto ServerConnection() const -> defs::connection;

	auto GetClientDetailsForServer() const -> defs::connection;

	void CloseConnection();

	void SendMessageToServerAsync(const uint32_t messageId
                                  , const defs::connection& responseAddress = defs::NULL_CONNECTION
                                  , const defs::eArchiveType archive = defs::eArchiveType::portableBinary);

	bool SendMessageToServerSync(const uint32_t messageId
                                 , const defs::connection& responseAddress = defs::NULL_CONNECTION
                                 , const defs::eArchiveType archive = defs::eArchiveType::portableBinary);

	template<typename T>
	void SendMessageToServerAsync(const T& message, const uint32_t messageId
                                  , const defs::connection& responseAddress = defs::NULL_CONNECTION
                                  , const defs::eArchiveType archive = defs::eArchiveType::portableBinary)
	{
		auto messageBuffer = BuildMessage(message, messageId, responseAddress, archive);
		m_tcpClient.SendMessageToServerAsync(messageBuffer);
	}

	template<typename T>
	bool SendMessageToServerSync(const T& message, const uint32_t messageId
                                 , const defs::connection& responseAddress = defs::NULL_CONNECTION
                                 , const defs::eArchiveType archive = defs::eArchiveType::portableBinary)
	{
		auto messageBuffer = BuildMessage(message, messageId, responseAddress, archive);
		return m_tcpClient.SendMessageToServerSync(messageBuffer);
	}

private:
	messages::MessageHandler m_messageHandler;
	TcpClient m_tcpClient;

	auto BuildMessage(const uint32_t messageId, const defs::connection& responseAddress
					  , const defs::eArchiveType archive) const -> defs::char_buffer;

	template<typename T>
    auto BuildMessage(const T& message, const uint32_t messageId
                      , const defs::connection& responseAddress
					  , const defs::eArchiveType archive) const -> defs::char_buffer
	{
		auto responseConn = (responseAddress == defs::NULL_CONNECTION)
							? GetClientDetailsForServer()
							: responseAddress;
		return messages::BuildMessageBuffer(m_messageHandler.MagicString(), message, messageId
											, responseConn, archive);
	}
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT_HPP
