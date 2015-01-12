
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2015 Duncan Crutchley
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
 * \file SimpleTcpClient.hpp
 * \brief File containing the simple TCP client class declaration.
 */

#ifndef SIMPLETCPCLIENT_HPP
#define SIMPLETCPCLIENT_HPP

#include "TcpTypedClient.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class SimpleTcpClient final
{
public:
    SimpleTcpClient(boost_ioservice_t& ioService
                   , const defs::connection_t& server
                   , const defs::default_message_dispatcher_t& messageDispatcher
                   , const eSendOption sendOption = eSendOption::nagleOn);

    SimpleTcpClient(const defs::connection_t& server
                   , const defs::default_message_dispatcher_t& messageDispatcher
                   , const eSendOption sendOption = eSendOption::nagleOn);

    ~SimpleTcpClient() = default;
    SimpleTcpClient(const SimpleTcpClient& ) = delete;
    SimpleTcpClient& operator=(const SimpleTcpClient& ) = delete;

    auto ServerConnection() const -> defs::connection_t;

    auto GetClientDetailsForServer() const -> defs::connection_t;

    void CloseConnection();

    void SendMessageToServerAsync(const uint32_t messageId
                                  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION);

    bool SendMessageToServerSync(const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION);

	template<typename T>
    void SendMessageToServerAsync(T&& message
                                  , const uint32_t messageId
                                  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        m_tcpTypedClient.SendMessageToServerAsync(message, messageId, responseAddress);
	}

	template<typename T>
    bool SendMessageToServerSync(T&& message
                                 , const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        return m_tcpTypedClient.SendMessageToServerAsync(message, messageId, responseAddress);
	}

private:
    messages::MessageBuilder m_messageBuilder;
    messages::MessageHandler m_messageHandler;
    TcpTypedClient<messages::MessageBuilder> m_tcpTypedClient;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // SIMPLETCPCLIENT_HPP
