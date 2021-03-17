
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
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
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{

// ****************************************************************************
// 'class SimpleTcpClient' definition
// ****************************************************************************
SimpleTcpClient::SimpleTcpClient(boost_iocontext_t& ioContext, const defs::connection_t& server,
                                 const defs::default_message_dispatcher_t& messageDispatcher,
                                 eSendOption                               sendOption,
								 size_t maxAllowedUnsentAsyncMessages, size_t memPoolMsgCount)
    : m_messageHandler{messageDispatcher, defs::DEFAULT_MAGIC_STRING, memPoolMsgCount}
    , m_tcpTypedClient{ioContext,
                       server,
                       sizeof(defs::MessageHeader),
                       std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                                 std::placeholders::_1),
                       std::bind(&messages::MessageHandler::MessageReceivedHandler,
                                 &m_messageHandler, std::placeholders::_1),
                       m_messageBuilder,
                       sendOption,
					   maxAllowedUnsentAsyncMessages}
{
}

SimpleTcpClient::SimpleTcpClient(const defs::connection_t&                 server,
                                 const defs::default_message_dispatcher_t& messageDispatcher,
                                 eSendOption                               sendOption,
								 size_t maxAllowedUnsentAsyncMessages, size_t memPoolMsgCount)
    : m_messageHandler{messageDispatcher, defs::DEFAULT_MAGIC_STRING, memPoolMsgCount}
    , m_tcpTypedClient{server,
                       sizeof(defs::MessageHeader),
                       std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                                 std::placeholders::_1),
                       std::bind(&messages::MessageHandler::MessageReceivedHandler,
                                 &m_messageHandler, std::placeholders::_1),
                       m_messageBuilder,
                       sendOption,
					   maxAllowedUnsentAsyncMessages}
{
}

auto SimpleTcpClient::ServerConnection() const -> defs::connection_t
{
    return m_tcpTypedClient.ServerConnection();
}

bool SimpleTcpClient::Connected() const
{
    return m_tcpTypedClient.Connected();
}

auto SimpleTcpClient::GetClientDetailsForServer() -> defs::connection_t
{
    return m_tcpTypedClient.GetClientDetailsForServer();
}

void SimpleTcpClient::CloseConnection()
{
    m_tcpTypedClient.CloseConnection();
}

bool SimpleTcpClient::SendMessageToServerAsync(int32_t                   messageId,
                                               const defs::connection_t& responseAddress)
{
    return m_tcpTypedClient.SendMessageToServerAsync(messageId, responseAddress);
}

bool SimpleTcpClient::SendMessageToServerSync(int32_t                   messageId,
                                              const defs::connection_t& responseAddress)
{
    return m_tcpTypedClient.SendMessageToServerSync(messageId, responseAddress);
}

bool SimpleTcpClient::SendMessageToServerAsync(const defs::char_buffer_t& message,
                                               int32_t                    messageId,

                                               const defs::connection_t& responseAddress)
{
    return m_tcpTypedClient.SendMessageToServerAsync(message, messageId, responseAddress);
}

bool SimpleTcpClient::SendMessageToServerSync(const defs::char_buffer_t& message, int32_t messageId,
                                              const defs::connection_t& responseAddress)
{
    return m_tcpTypedClient.SendMessageToServerSync(message, messageId, responseAddress);
}

bool SimpleTcpClient::SendMessageToServerAsync(const defs::char_buffer_t& message)
{
    return m_tcpTypedClient.SendMessageToServerAsync(message);
}

bool SimpleTcpClient::SendMessageToServerSync(const defs::char_buffer_t& message)
{
    return m_tcpTypedClient.SendMessageToServerSync(message);
}

size_t SimpleTcpClient::NumberOfUnsentAsyncMessages() const
{
    return m_tcpTypedClient.NumberOfUnsentAsyncMessages();
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
