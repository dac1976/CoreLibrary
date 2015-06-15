
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
 * \file SimpleTcpServer.h
 * \brief File containing the simple TCP server class declaration.
 */

#ifndef SIMPLETCPSERVER
#define SIMPLETCPSERVER

#include "TcpTypedServer.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class SimpleTcpServer final
{
public:
    SimpleTcpServer(boost_ioservice_t& ioService
                    , const uint16_t listenPort
                    , const defs::default_message_dispatcher_t& messageDispatcher
                    , const eSendOption sendOption = eSendOption::nagleOn);

    SimpleTcpServer(const uint16_t listenPort
                   , const defs::default_message_dispatcher_t& messageDispatcher
                   , const eSendOption sendOption = eSendOption::nagleOn);

    ~SimpleTcpServer() = default;
    SimpleTcpServer(const SimpleTcpServer& ) = delete;
    SimpleTcpServer& operator=(const SimpleTcpServer& ) = delete;

    auto GetServerDetailsForClient(const defs::connection_t& client)
        const -> defs::connection_t;

    uint16_t ListenPort() const;

    void CloseAcceptor();

    void OpenAcceptor();

    void SendMessageToClientAsync(const defs::connection_t& client
                                  , const uint32_t messageId
                                  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;

    bool SendMessageToClientSync(const defs::connection_t& client
                                 , const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;

    void SendMessageToAllClients(const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToClientAsync(const T& message
                                  , const defs::connection_t& client, const uint32_t messageId
                                  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        m_tcpTypedServer.SendMessageToClientAsync<T, A>(message, client, messageId, responseAddress);
	}

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToClientSync(const T& message
                                 , const defs::connection_t& client, const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        return m_tcpTypedServer.SendMessageToClientSync<T, A>(message, client, messageId, responseAddress);
	}

    template<typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToAllClients(const T& message
                                 , const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        m_tcpTypedServer.SendMessageToAllClients<T, A>(message, messageId, responseAddress);
	}

private:
    messages::MessageBuilder m_messageBuilder{};
    messages::MessageHandler m_messageHandler;
    TcpTypedServer<messages::MessageBuilder> m_tcpTypedServer;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // SIMPLETCPSERVER
