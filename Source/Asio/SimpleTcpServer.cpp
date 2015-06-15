
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
 * \file SimpleTcpServer.cpp
 * \brief File containing the simple TCP server class definition.
 */

#include "../../Include/Asio/SimpleTcpServer.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

// ****************************************************************************
// 'class SimpleTcpServer' definition
// ****************************************************************************
SimpleTcpServer::SimpleTcpServer(boost_ioservice_t& ioService
                , const uint16_t listenPort
                , const defs::default_message_dispatcher_t& messageDispatcher
                , const eSendOption sendOption)
    : m_messageHandler{messageDispatcher, defs::DEFAULT_MAGIC_STRING}
    , m_tcpTypedServer{ioService, listenPort, sizeof(defs::MessageHeader)
                       , std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler, std::placeholders::_1)
                       , std::bind(&messages::MessageHandler::MessageReceivedHandler, &m_messageHandler, std::placeholders::_1)
                       , m_messageBuilder, sendOption}
{

}

SimpleTcpServer::SimpleTcpServer(const uint16_t listenPort
               , const defs::default_message_dispatcher_t& messageDispatcher
               , const eSendOption sendOption)
    : m_messageHandler{messageDispatcher, defs::DEFAULT_MAGIC_STRING}
    , m_tcpTypedServer{listenPort, sizeof(defs::MessageHeader)
                       , std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler, std::placeholders::_1)
                       , std::bind(&messages::MessageHandler::MessageReceivedHandler, &m_messageHandler, std::placeholders::_1)
                       , m_messageBuilder, sendOption}
{
}

auto SimpleTcpServer::GetServerDetailsForClient(const defs::connection_t& client)
    const -> defs::connection_t
{
    return m_tcpTypedServer.GetServerDetailsForClient(client);
}

uint16_t SimpleTcpServer::ListenPort() const
{
    return m_tcpTypedServer.ListenPort();
}

void SimpleTcpServer::CloseAcceptor()
{
    m_tcpTypedServer.CloseAcceptor();
}

void SimpleTcpServer::OpenAcceptor()
{
    m_tcpTypedServer.OpenAcceptor();
}

void SimpleTcpServer::SendMessageToClientAsync(const defs::connection_t& client, const uint32_t messageId
                                               , const defs::connection_t& responseAddress) const
{
    m_tcpTypedServer.SendMessageToClientAsync(client, messageId, responseAddress);
}

bool SimpleTcpServer::SendMessageToClientSync(const defs::connection_t& client, const uint32_t messageId
                                              , const defs::connection_t& responseAddress) const
{
    return m_tcpTypedServer.SendMessageToClientSync(client, messageId, responseAddress);
}

void SimpleTcpServer::SendMessageToAllClients(const uint32_t messageId
                                              , const defs::connection_t& responseAddress) const
{
    m_tcpTypedServer.SendMessageToAllClients(messageId, responseAddress);
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
