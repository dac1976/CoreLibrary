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
 * \file SimpleTcpClientList.cpp
 * \brief File containing simple TCP client list class definition.
 */
#include "Asio/SimpleTcpClientList.h"

namespace core_lib
{
namespace asio
{
namespace tcp
{

SimpleTcpClientList::SimpleTcpClientList(
    boost_iocontext_t& ioContext, defs::default_message_dispatcher_t const& messageDispatcher,
    eSendOption sendOption)
    : m_ioContextPtr(&ioContext)
    , m_messageDispatcher(messageDispatcher)
    , m_sendOption(sendOption)
{
}

SimpleTcpClientList::SimpleTcpClientList(
    defs::default_message_dispatcher_t const& messageDispatcher, eSendOption sendOption)
    : m_messageDispatcher(messageDispatcher)
    , m_sendOption(sendOption)
{
}

SimpleTcpClientList::~SimpleTcpClientList()
{
    CloseConnections();
}

auto SimpleTcpClientList::ServerConnection(defs::connection_t const& clientConn) const
    -> defs::connection_t
{
    std::lock_guard<std::mutex> lock(m_mutex);

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

bool SimpleTcpClientList::Connected(defs::connection_t const& server) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    bool connected = false;
    auto clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        connected = clientPtr->Connected();
    }

    return connected;
}

auto SimpleTcpClientList::GetClientDetailsForServer(defs::connection_t const& server) const
    -> defs::connection_t
{
    std::lock_guard<std::mutex> lock(m_mutex);

    defs::connection_t client{defs::NULL_CONNECTION};
    auto               clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        client = clientPtr->GetClientDetailsForServer();
    }

    return client;
}

void SimpleTcpClientList::CloseConnection(defs::connection_t const& server) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        clientPtr->CloseConnection();
    }
}

void SimpleTcpClientList::CloseConnections() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& client : m_clientMap)
    {
        client.second->CloseConnection();
    }
}

void SimpleTcpClientList::ClearConnections()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_clientMap.clear();
}

bool SimpleTcpClientList::SendMessageToServerAsync(defs::connection_t const& server,
                                                   int32_t                   messageId,
                                                   defs::connection_t const& responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr)
    {
        return clientPtr->SendMessageToServerAsync(messageId, responseAddress);
    }
	
	return false;
}

bool SimpleTcpClientList::SendMessageToServerSync(defs::connection_t const& server,
                                                  int32_t                   messageId,
                                                  defs::connection_t const& responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    bool success   = false;
    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr)
    {
        success = clientPtr->SendMessageToServerSync(messageId, responseAddress);
    }

    return success;
}

bool SimpleTcpClientList::SendMessageToServerAsync(defs::connection_t const&  server,
                                                   const defs::char_buffer_t& message,
                                                   int32_t                    messageId,
                                                   defs::connection_t const&  responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr)
    {
        return clientPtr->SendMessageToServerAsync(message, messageId, responseAddress);
    }
	
	return false;
}

bool SimpleTcpClientList::SendMessageToServerSync(defs::connection_t const&  server,
                                                  const defs::char_buffer_t& message,
                                                  int32_t                    messageId,
                                                  defs::connection_t const&  responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    bool success   = false;
    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr)
    {
        success = clientPtr->SendMessageToServerSync(message, messageId, responseAddress);
    }

    return success;
}

bool SimpleTcpClientList::SendMessageToServerAsync(defs::connection_t const&  server,
                                                   const defs::char_buffer_t& message)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr)
    {
        return clientPtr->SendMessageToServerAsync(message);
    }
	
	return false;
}

bool SimpleTcpClientList::SendMessageToServerSync(defs::connection_t const&  server,
                                                  const defs::char_buffer_t& message)
{
    std::lock_guard<std::mutex> lock(m_mutex);

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

auto SimpleTcpClientList::CreateTcpClient(defs::connection_t const& server) -> client_ptr_t
{
    client_ptr_t clientPtr;

    if (m_ioContextPtr)
    {
        clientPtr = std::make_shared<SimpleTcpClient>(
            *m_ioContextPtr, server, m_messageDispatcher, m_sendOption);
    }
    else
    {
        clientPtr = std::make_shared<SimpleTcpClient>(server, m_messageDispatcher, m_sendOption);
    }

    m_clientMap[server] = clientPtr;
    return clientPtr;
}

auto SimpleTcpClientList::FindTcpClient(defs::connection_t const& server) const -> client_ptr_t
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
