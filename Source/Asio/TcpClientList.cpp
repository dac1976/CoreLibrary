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
 * \file TcpClientList.cpp
 * \brief File containing TCP client list class definition.
 */
#include "Asio/TcpClientList.h"
#include "Asio/TcpClient.h"

namespace core_lib
{
namespace asio
{
namespace tcp
{

TcpClientList::TcpClientList(boost_ioservice_t& ioService, size_t minAmountToRead,
                             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                             defs::message_received_handler_t const& messageReceivedHandler,
                             eSendOption                             sendOption)
    : m_ioServicePtr(&ioService)
    , m_minAmountToRead(minAmountToRead)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_sendOption(sendOption)
{
}

TcpClientList::TcpClientList(size_t                                  minAmountToRead,
                             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                             defs::message_received_handler_t const& messageReceivedHandler,
                             eSendOption                             sendOption)
    : m_minAmountToRead(minAmountToRead)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_sendOption(sendOption)
{
}

TcpClientList::~TcpClientList()
{
    CloseConnections();
}

auto TcpClientList::ServerConnection(defs::connection_t const& clientConn) const
    -> defs::connection_t
{
    defs::connection_t server{defs::NULL_CONNECTION};

    for (auto& client : m_clientMap)
    {
        if (clientConn == client.second->GetClientDetailsForServer())
        {
            server = client.second->ServerConnection();
            break;
        }
    }

    return server;
}

bool TcpClientList::Connected(defs::connection_t const& server) const
{
    bool connected = false;
    auto clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        connected = clientPtr->Connected();
    }

    return connected;
}

auto TcpClientList::GetClientDetailsForServer(defs::connection_t const& server) const
    -> defs::connection_t
{
    defs::connection_t client{defs::NULL_CONNECTION};
    auto               clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        client = clientPtr->GetClientDetailsForServer();
    }

    return client;
}

void TcpClientList::CloseConnection(defs::connection_t const& server)
{
    auto clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        clientPtr->CloseConnection();
    }
}

void TcpClientList::CloseConnections()
{
    for (auto& client : m_clientMap)
    {
        client.second->CloseConnection();
    }
}

void TcpClientList::SendMessageToServerAsync(defs::connection_t const&  server,
                                             defs::char_buffer_t const& message)
{
    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr)
    {
        clientPtr->SendMessageToServerAsync(message);
    }
}

bool TcpClientList::SendMessageToServerSync(defs::connection_t const&  server,
                                            defs::char_buffer_t const& message)
{
    bool success   = false;
    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr)
    {
        success = clientPtr->SendMessageToServerSync(message);
    }

    return success;
}

auto TcpClientList::CreateTcpClient(defs::connection_t const& server) -> client_ptr_t
{
    client_ptr_t clientPtr;

    if (m_ioServicePtr)
    {
        clientPtr = std::make_shared<TcpClient>(*m_ioServicePtr,
                                                server,
                                                m_minAmountToRead,
                                                m_checkBytesLeftToRead,
                                                m_messageReceivedHandler,
                                                m_sendOption);
    }
    else
    {
        clientPtr = std::make_shared<TcpClient>(server,
                                                m_minAmountToRead,
                                                m_checkBytesLeftToRead,
                                                m_messageReceivedHandler,
                                                m_sendOption);
    }

    m_clientMap[server] = clientPtr;
    return clientPtr;
}

auto TcpClientList::FindTcpClient(defs::connection_t const& server) const -> client_ptr_t
{
    auto         clientItr = m_clientMap.find(server);
    client_ptr_t clientPtr;

    if (clientItr != m_clientMap.end())
    {
        clientPtr = clientItr->second;
    }

    return clientPtr;
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
