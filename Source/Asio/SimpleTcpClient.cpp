
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
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
SimpleTcpClient::SimpleTcpClient(asio_compat::io_service_t& ioService,
                           const defs::connection_t& server,
                           const defs::default_message_dispatcher_t& messageDispatcher,
                           SimpleTcpSettings const& settings)
    : m_messageHandler{messageDispatcher,
                       defs::DEFAULT_MAGIC_STRING,
                       settings.memPoolMsgCount,
                       settings.recvPoolMsgSize}
    , m_tcpTypedClient{ioService,
                       server,
                       std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                                 std::placeholders::_1),
                       std::bind(&messages::MessageHandler::MessageReceivedHandler,
                                 &m_messageHandler, std::placeholders::_1),
                       m_messageBuilder,
                       settings.connSettings,
                       defs::message_received_handler_ex_t(),
                       defs::check_bytes_left_to_read_ex_t()}
{
}

SimpleTcpClient::SimpleTcpClient(const defs::connection_t& server,
                           const defs::default_message_dispatcher_t& messageDispatcher,
                           SimpleTcpSettings const& settings)
    : m_messageHandler{messageDispatcher,
                       defs::DEFAULT_MAGIC_STRING,
                       settings.memPoolMsgCount,
                       settings.recvPoolMsgSize}
    , m_tcpTypedClient{server,
                       std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                                 std::placeholders::_1),
                       std::bind(&messages::MessageHandler::MessageReceivedHandler,
                                 &m_messageHandler, std::placeholders::_1),
                       m_messageBuilder,
                       settings.connSettings,
                       defs::message_received_handler_ex_t(),
                       defs::check_bytes_left_to_read_ex_t()}
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

void SimpleTcpClient::Reconnect(defs::connection_t const& server, TcpConnSettings const& settings)
{
    m_tcpTypedClient.Reconnect(server, settings);
}

bool SimpleTcpClient::SendMessageToServerAsync(int32_t messageId,
                                       const defs::connection_t& responseAddress)
{
    return m_tcpTypedClient.SendMessageToServerAsync(messageId, responseAddress);
}

bool SimpleTcpClient::SendMessageToServerSync(int32_t messageId,
                                      const defs::connection_t& responseAddress)
{
    return m_tcpTypedClient.SendMessageToServerSync(messageId, responseAddress);
}

bool SimpleTcpClient::SendMessageToServerAsync(int32_t messageId,
                                       defs::char_buf_cspan_t message,
                                       const defs::connection_t& responseAddress,
                                       defs::eArchiveType        archiveType)
{
    return m_tcpTypedClient.SendMessageToServerAsync(messageId, message, responseAddress, archiveType);
}

bool SimpleTcpClient::SendMessageToServerSync(int32_t messageId,
                                      defs::char_buf_cspan_t message,
                                      const defs::connection_t& responseAddress,
                                      defs::eArchiveType        archiveType)
{
    return m_tcpTypedClient.SendMessageToServerSync(messageId, message, responseAddress, archiveType);
}

bool SimpleTcpClient::SendMessageToServerAsync(defs::char_buf_cspan_t message)
{
    return m_tcpTypedClient.SendMessageToServerAsync(message);
}

bool SimpleTcpClient::SendMessageToServerSync(defs::char_buf_cspan_t message)
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
