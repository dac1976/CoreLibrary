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
    asio_compat::io_service_t& ioService,
    defs::default_message_dispatcher_t const& messageDispatcher, 
	SimpleTcpSettings const& settings)
    : m_ioServicePtr(&ioService)
    , m_messageDispatcher(messageDispatcher)
    , m_settings(settings)
{
}

SimpleTcpClientList::SimpleTcpClientList(
    defs::default_message_dispatcher_t const& messageDispatcher, 
	SimpleTcpSettings const& settings)
    : m_messageDispatcher(messageDispatcher)
    , m_settings(settings)
{
}

SimpleTcpClientList::~SimpleTcpClientList()
{
    CloseConnections();
}

auto SimpleTcpClientList::ServerConnection(defs::connection_t const& clientConn) const
    -> defs::connection_t
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

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
    bool connected = false;
    auto clientPtr = FindTcpClient(server);

    if (clientPtr != nullptr)
    {
        connected = clientPtr->Connected();
    }

    return connected;
}

auto SimpleTcpClientList::GetClientDetailsForServer(defs::connection_t const& server) const
    -> defs::connection_t
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    defs::connection_t client{defs::NULL_CONNECTION};
    auto               clientPtr = FindTcpClient(server);

    if (clientPtr != nullptr)
    {
        client = clientPtr->GetClientDetailsForServer();
    }

    return client;
}

void SimpleTcpClientList::CloseConnection(defs::connection_t const& server)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    auto clientPtr = FindTcpClient(server);

    if (clientPtr != nullptr)
    {
        clientPtr->CloseConnection();

        m_clientMap.erase(server);
    }
}

void SimpleTcpClientList::CloseConnections()
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    for (auto& client : m_clientMap)
    {
        client.second->CloseConnection();
    }

    m_clientMap.clear();
}

bool SimpleTcpClientList::CreateConnectionToServer(defs::connection_t const& server)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    auto clientPtr = FindTcpClient(server);

    if (!clientPtr)
    {
        clientPtr = CreateTcpClient(server);
    }

    return static_cast<bool>(clientPtr);
}

bool SimpleTcpClientList::SendMessageToServerAsync(defs::connection_t const& server,
                                          int32_t messageId,
                                          defs::connection_t const& responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

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
                                         int32_t messageId,
                                         defs::connection_t const& responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

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

bool SimpleTcpClientList::SendMessageToServerAsync(defs::connection_t const& server,
                                          const defs::char_buffer_t& message,
                                          int32_t messageId,
                                          defs::connection_t const& responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

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

bool SimpleTcpClientList::SendMessageToServerSync(defs::connection_t const& server,
                                         const defs::char_buffer_t& message,
                                         int32_t messageId,
                                         defs::connection_t const& responseAddress)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

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

bool SimpleTcpClientList::SendMessageToServerAsync(defs::connection_t const& server,
                                          const defs::char_buffer_t& message)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

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

bool SimpleTcpClientList::SendMessageToServerSync(defs::connection_t const& server,
                                         const defs::char_buffer_t& message)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

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

    if (m_ioServicePtr != nullptr)
    {
        clientPtr = std::make_shared<SimpleTcpClient>(
            *m_ioServicePtr, server, m_messageDispatcher, m_settings);
    }
    else
    {
        clientPtr = std::make_shared<SimpleTcpClient>(server, m_messageDispatcher, m_settings);
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

void SimpleTcpClientList::ClearList()
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    m_clientMap.clear();
}

void SimpleTcpClientList::GetServerList(std::list<defs::connection_t>& serverDetailsList) const
{
    serverDetailsList.clear();

    std::lock_guard<std::mutex> lock(m_mapMutex);

    for (auto const& clientItr : m_clientMap)
    {
        serverDetailsList.push_back(clientItr.first);
    }
}

auto SimpleTcpClientList::GetServerList() const -> std::vector<defs::connection_t>
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    std::vector<defs::connection_t> serverDetailsList;

    for (auto const& clientItr : m_clientMap)
    {
        serverDetailsList.emplace_back(clientItr.first);
    }

    return serverDetailsList;
}

size_t SimpleTcpClientList::NumberOfUnsentAsyncMessages(const defs::connection_t& server) const
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    auto clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        return clientPtr->NumberOfUnsentAsyncMessages();
    }

    return 0;
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
