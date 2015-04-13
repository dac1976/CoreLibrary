
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

#ifndef TCPTYPEDSERVER
#define TCPTYPEDSERVER

#include "TcpServer.hpp"
#include "MessageUtils.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

template<typename MsgBldr>
class TcpTypedServer final
{
public:
	TcpTypedServer(boost_ioservice_t& ioService
				   , const uint16_t listenPort
				   , const size_t minAmountToRead
				   , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				   , const defs::message_received_handler_t& messageReceivedHandler
				   , const MsgBldr& messageBuilder
				   , const eSendOption sendOption = eSendOption::nagleOn)
		: m_messageBuilder{messageBuilder}
		, m_tcpServer{ioService, listenPort, minAmountToRead
					  , checkBytesLeftToRead, messageReceivedHandler
					  , sendOption}
	{
	}

	TcpTypedServer(const uint16_t listenPort
				   , const size_t minAmountToRead
				   , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				   , const defs::message_received_handler_t& messageReceivedHandler
				   , const MsgBldr& messageBuilder
				   , const eSendOption sendOption = eSendOption::nagleOn)
		: m_messageBuilder{messageBuilder}
		, m_tcpServer{listenPort, minAmountToRead
					  , checkBytesLeftToRead, messageReceivedHandler
					  , sendOption}
	{
	}

	~TcpTypedServer() = default;
	TcpTypedServer(const TcpTypedServer& ) = delete;
	TcpTypedServer& operator=(const TcpTypedServer& ) = delete;

	auto GetServerDetailsForClient(const defs::connection_t& client)
		const -> defs::connection_t
	{
		return m_tcpServer.GetServerDetailsForClient(client);
	}

	uint16_t ListenPort() const
	{
		return m_tcpServer.ListenPort();
	}

	void CloseAcceptor()
	{
		m_tcpServer.CloseAcceptor();
	}

	void OpenAcceptor()
	{
		m_tcpServer.OpenAcceptor();
	}

	void SendMessageToClientAsync(const defs::connection_t& client, const uint32_t messageId
								  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(client, messageId, responseAddress);
		m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
	}

	bool SendMessageToClientSync(const defs::connection_t& client, const uint32_t messageId
								 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(client, messageId, responseAddress);
		return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
	}

	void SendMessageToAllClients(const uint32_t messageId
								 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(defs::NULL_CONNECTION, messageId, responseAddress);
		m_tcpServer.SendMessageToAllClients(messageBuffer);
	}

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToClientAsync(const T& message
                                  , const defs::connection_t& client, const uint32_t messageId
								  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        auto messageBuffer = BuildMessage<T, A>(message, client, messageId, responseAddress);
		m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
	}

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToClientSync(const T& message
                                 , const defs::connection_t& client, const uint32_t messageId
								 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        auto messageBuffer = BuildMessage<T, A>(message, client, messageId, responseAddress);
		return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
	}

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToAllClients(const T& message
                                 , const uint32_t messageId
								 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        auto messageBuffer = BuildMessage<T, A>(message, defs::NULL_CONNECTION, messageId
										  , responseAddress);
		m_tcpServer.SendMessageToAllClients(messageBuffer);
	}

private:
	const MsgBldr& m_messageBuilder;
	TcpServer m_tcpServer;

	auto BuildMessage(const defs::connection_t& client, const uint32_t messageId
					  , const defs::connection_t& responseAddress) const -> defs::char_buffer_t
	{
		auto responseConn = (responseAddress == defs::NULL_CONNECTION)
							? GetServerDetailsForClient(client)
							: responseAddress;
        return m_messageBuilder(messageId, responseConn);
	}

    template<typename T, typename A>
    auto BuildMessage(const T& message
                      , const defs::connection_t& client, const uint32_t messageId
					  , const defs::connection_t& responseAddress) const -> defs::char_buffer_t
	{
		auto responseConn = (responseAddress == defs::NULL_CONNECTION)
							? GetServerDetailsForClient(client)
							: responseAddress;
        return m_messageBuilder<T,A>(message, messageId, responseConn);
	}
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT
