
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014,2015 Duncan Crutchley
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
 * \file SimpleTcpClient.cpp
 * \brief File containing the simple TCP client class definition.
 */

#include "Asio/SimpleTcpClient.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

// ****************************************************************************
// 'class SimpleTcpClient' definition
// ****************************************************************************
SimpleTcpClient::SimpleTcpClient(boost_ioservice_t& ioService
               , const defs::connection_t& server
               , const defs::default_message_dispatcher_t& messageDispatcher
               , const eSendOption sendOption)
    : m_messageHandler{messageDispatcher, defs::DEFAULT_MAGIC_STRING}
    , m_tcpTypedClient{ioService, server, sizeof(defs::MessageHeader)
                       , std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler, std::placeholders::_1)
                       , std::bind(&messages::MessageHandler::MessageReceivedHandler, &m_messageHandler, std::placeholders::_1)
                       , m_messageBuilder, sendOption}
{
}

SimpleTcpClient::SimpleTcpClient(const defs::connection_t& server
               , const defs::default_message_dispatcher_t& messageDispatcher
               , const eSendOption sendOption)
    : m_messageHandler{messageDispatcher, defs::DEFAULT_MAGIC_STRING}
    , m_tcpTypedClient{server, sizeof(defs::MessageHeader)
                       , std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler, std::placeholders::_1)
                       , std::bind(&messages::MessageHandler::MessageReceivedHandler, &m_messageHandler, std::placeholders::_1)
                       , m_messageBuilder, sendOption}
{
}

auto SimpleTcpClient::ServerConnection() const -> defs::connection_t
{
    return m_tcpTypedClient.ServerConnection();
}

auto SimpleTcpClient::GetClientDetailsForServer() const -> defs::connection_t
{
    return m_tcpTypedClient.GetClientDetailsForServer();
}

void SimpleTcpClient::CloseConnection()
{
    m_tcpTypedClient.CloseConnection();
}

void SimpleTcpClient::SendMessageToServerAsync(const uint32_t messageId
                              , const defs::connection_t& responseAddress)
{
    m_tcpTypedClient.SendMessageToServerAsync(messageId, responseAddress);
}

bool SimpleTcpClient::SendMessageToServerSync(const uint32_t messageId
                             , const defs::connection_t& responseAddress)
{
    return m_tcpTypedClient.SendMessageToServerSync(messageId, responseAddress);
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
