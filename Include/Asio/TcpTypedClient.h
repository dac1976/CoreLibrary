
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
 * \file TcpTypedClient.h
 * \brief File containing TCP typed client class declaration.
 */

#ifndef TCPTYPEDCLIENT
#define TCPTYPEDCLIENT

#include "TcpClient.h"
#include "MessageUtils.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

template<typename MsgBldr>
class TcpTypedClient final
{
public:
	TcpTypedClient(boost_ioservice_t& ioService
				   , const defs::connection_t& server
				   , const size_t minAmountToRead
				   , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				   , const defs::message_received_handler_t& messageReceivedHandler
				   , const MsgBldr& messageBuilder
				   , const eSendOption sendOption = eSendOption::nagleOn)
		: m_messageBuilder{messageBuilder}
		, m_tcpClient{ioService, server, minAmountToRead
					  , checkBytesLeftToRead, messageReceivedHandler
					  , sendOption}
	{
	}

	TcpTypedClient(const defs::connection_t& server
				   , const size_t minAmountToRead
				   , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				   , const defs::message_received_handler_t& messageReceivedHandler
				   , const MsgBldr& messageBuilder
				   , const eSendOption sendOption = eSendOption::nagleOn)
		: m_messageBuilder{messageBuilder}
		, m_tcpClient{server, minAmountToRead
					  , checkBytesLeftToRead, messageReceivedHandler
					  , sendOption}
	{
	}

	~TcpTypedClient() = default;
	TcpTypedClient(const TcpTypedClient& ) = delete;
	TcpTypedClient& operator=(const TcpTypedClient& ) = delete;

	auto ServerConnection() const -> defs::connection_t
	{
		return m_tcpClient.ServerConnection();
	}

	auto GetClientDetailsForServer() const -> defs::connection_t
	{
		return m_tcpClient.GetClientDetailsForServer();
	}

	void CloseConnection()
	{
		m_tcpClient.CloseConnection();
	}

	void SendMessageToServerAsync(const uint32_t messageId
								  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(messageId, responseAddress);
		m_tcpClient.SendMessageToServerAsync(messageBuffer);
	}

	bool SendMessageToServerSync(const uint32_t messageId
								 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
		auto messageBuffer = BuildMessage(messageId, responseAddress);
		return m_tcpClient.SendMessageToServerSync(messageBuffer);
	}

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToServerAsync(const T& message
                                  , const uint32_t messageId
								  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        auto messageBuffer = BuildMessage<T, A>(message, messageId, responseAddress);
		m_tcpClient.SendMessageToServerAsync(messageBuffer);
	}

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerSync(const T& message
                                 , const uint32_t messageId
								 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        auto messageBuffer = BuildMessage<T, A>(message, messageId, responseAddress);
		return m_tcpClient.SendMessageToServerSync(messageBuffer);
	}

private:
	const MsgBldr& m_messageBuilder;
	TcpClient m_tcpClient;

	auto BuildMessage(const uint32_t messageId, const defs::connection_t& responseAddress) const
		-> defs::char_buffer_t
	{
		auto responseConn = (responseAddress == defs::NULL_CONNECTION)
							? GetClientDetailsForServer()
							: responseAddress;
        return m_messageBuilder.Build(messageId, responseConn);
	}

    template<typename T, typename A>
    auto BuildMessage(const T& message
                      , const uint32_t messageId
					  , const defs::connection_t& responseAddress) const
		-> defs::char_buffer_t
	{
		auto responseConn = (responseAddress == defs::NULL_CONNECTION)
							? GetClientDetailsForServer()
							: responseAddress;
        return m_messageBuilder.template Build<T,A>(message, messageId, responseConn);
	}
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT
