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
 * \file TcpClient.cpp
 * \brief File containing TCP client class definition.
 */

#include "Asio/TcpClient.h"
#include "Asio/TcpConnection.h"

namespace core_lib
{
namespace asio
{
namespace tcp
{

// ****************************************************************************
// 'class TcpClient' definition
// ****************************************************************************
TcpClient::TcpClient(boost_iocontext_t& ioContext, const defs::connection_t& server,
                     size_t                                  minAmountToRead,
                     const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                     const defs::message_received_handler_t& messageReceivedHandler,
                     eSendOption                             sendOption, 
					 size_t maxAllowedUnsentAsyncMessages)
    : m_ioContext(ioContext)
    , m_server{server}
    , m_minAmountToRead{minAmountToRead}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_sendOption{sendOption}
	, m_maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages)
{
    CreateConnection();
}

TcpClient::TcpClient(const defs::connection_t& server, size_t minAmountToRead,
                     const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                     const defs::message_received_handler_t& messageReceivedHandler,
                     eSendOption                             sendOption, 
					 size_t maxAllowedUnsentAsyncMessages)
    : m_ioThreadGroup{new IoContextThreadGroup(2)}
    // 2 threads we can send/receive to/from the server
    , m_ioContext(m_ioThreadGroup->IoContext())
    , m_server{server}
    , m_minAmountToRead{minAmountToRead}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_sendOption{sendOption}
	, m_maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages)
{
    CreateConnection();
}

TcpClient::~TcpClient()
{
    CloseConnection();
}

auto TcpClient::ServerConnection() const -> defs::connection_t
{
    return m_server;
}

bool TcpClient::Connected() const
{
    return !m_serverConnection.Empty();
}

auto TcpClient::GetClientDetailsForServer() const -> defs::connection_t
{
    return m_serverConnection.GetLocalEndForRemoteEnd(m_server);
}

void TcpClient::CloseConnection()
{
    m_serverConnection.CloseConnections();
}

void TcpClient::SendMessageToServerAsync(const defs::char_buffer_t& message)
{
    if (CheckAndCreateConnection())
    {
        m_serverConnection.SendMessageAsync(m_server, message);
    }
}

bool TcpClient::SendMessageToServerSync(const defs::char_buffer_t& message)
{
    if (CheckAndCreateConnection())
    {
        return m_serverConnection.SendMessageSync(m_server, message);
    }

    return false;
}

size_t TcpClient::NumberOfUnsentAsyncMessages() const
{
    return m_serverConnection.NumberOfUnsentAsyncMessages(m_server);
}

void TcpClient::CreateConnection()
{
    try
    {
        auto connection = std::make_shared<TcpConnection>(m_ioContext,
                                                          m_serverConnection,
                                                          m_minAmountToRead,
                                                          m_checkBytesLeftToRead,
                                                          m_messageReceivedHandler,
                                                          m_sendOption,
														  m_maxAllowedUnsentAsyncMessages);
        connection->Connect(m_server);
    }
    catch (...)
    {
        // NOTE: We catch here because if this fails in constructor
        // we want our TcpClient to stay viable as calling
        // TcpClient::SendMessageToServer* later will attempt
        // to reconnect. Only catch boost::system::system_error
        // exceptions as any other ones are a problem we should
        // definitely still propagate.
    }
}

bool TcpClient::CheckAndCreateConnection()
{
    if (m_serverConnection.Empty())
    {
        CreateConnection();
    }

    return !m_serverConnection.Empty();
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
