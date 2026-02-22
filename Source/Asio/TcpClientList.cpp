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

TcpClientList::TcpClientList(asio_compat::io_service_t& ioService,
                        defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                        defs::message_received_handler_t const& messageReceivedHandler,
                        TcpConnSettings const& settings,
                        defs::message_received_handler_ex_t const& messageReceivedHandlerEx,
                        defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx)
    : m_ioServicePtr(&ioService)
    , m_settings(settings)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_checkBytesLeftToReadEx(checkBytesLeftToReadEx)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_messageReceivedHandlerEx(messageReceivedHandlerEx)
{
}

TcpClientList::TcpClientList(defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
					    defs::message_received_handler_t const& messageReceivedHandler,
					    TcpConnSettings const& settings,
					    defs::message_received_handler_ex_t const& messageReceivedHandlerEx,
					    defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx)
    : m_settings(settings)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_checkBytesLeftToReadEx(checkBytesLeftToReadEx)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_messageReceivedHandlerEx(messageReceivedHandlerEx)
{
}

TcpClientList::~TcpClientList()
{
    CloseConnections();
}

auto TcpClientList::ServerConnection(defs::connection_t const& clientConn) const
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

bool TcpClientList::Connected(defs::connection_t const& server) const
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    bool connected = false;
    auto clientPtr = FindTcpClient(server);

    if (clientPtr != nullptr)
    {
        connected = clientPtr->Connected();
    }

    return connected;
}

auto TcpClientList::GetClientDetailsForServer(defs::connection_t const& server) const
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

void TcpClientList::CloseConnection(defs::connection_t const& server)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    auto clientPtr = FindTcpClient(server);

    if (clientPtr != nullptr)
    {
        clientPtr->CloseConnection();

        m_clientMap.erase(server);
    }
}

void TcpClientList::CloseConnections()
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    for (auto& client : m_clientMap)
    {
        client.second->CloseConnection();
    }

    m_clientMap.clear();
}

bool TcpClientList::CreateConnectionToServer(defs::connection_t const& server)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    auto clientPtr = FindTcpClient(server);

    if (clientPtr == nullptr)
    {
        clientPtr = CreateTcpClient(server);
    }

    return static_cast<bool>(clientPtr);
}

bool TcpClientList::SendMessageToServerAsync(defs::connection_t const&  server,
                                             defs::char_buffer_t const& message)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    auto clientPtr = FindTcpClient(server);

    if (clientPtr == nullptr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr != nullptr)
    {
        return clientPtr->SendMessageToServerAsync(message);
    }

    return false;
}

bool TcpClientList::SendMessageToServerSync(defs::connection_t const&  server,
                                            defs::char_buffer_t const& message)
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    bool success   = false;
    auto clientPtr = FindTcpClient(server);

    if (clientPtr == nullptr)
    {
        clientPtr = CreateTcpClient(server);
    }

    if (clientPtr != nullptr)
    {
        success = clientPtr->SendMessageToServerSync(message);
    }

    return success;
}

auto TcpClientList::CreateTcpClient(defs::connection_t const& server) -> client_ptr_t
{
    client_ptr_t clientPtr;

    if (m_ioServicePtr != nullptr)
    {
        clientPtr = std::make_shared<TcpClient>(*m_ioServicePtr,
                                                server,
                                                m_checkBytesLeftToRead,
                                                m_messageReceivedHandler,
                                                m_settings,
                                                m_messageReceivedHandlerEx,
                                                m_checkBytesLeftToReadEx);
    }
    else
    {
        clientPtr = std::make_shared<TcpClient>(server,
                                                m_checkBytesLeftToRead,
                                                m_messageReceivedHandler,
                                                m_settings,
                                                m_messageReceivedHandlerEx,
                                                m_checkBytesLeftToReadEx);
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

void TcpClientList::ClearList()
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    m_clientMap.clear();
}

void TcpClientList::GetServerList(std::list<defs::connection_t>& serverDetailsList) const
{
    serverDetailsList.clear();

    std::lock_guard<std::mutex> lock(m_mapMutex);

    for (auto const& clientItr : m_clientMap)
    {
        serverDetailsList.push_back(clientItr.first);
    }
}

auto TcpClientList::GetServerList() const -> std::vector<defs::connection_t>
{
    std::lock_guard<std::mutex> lock(m_mapMutex);

    std::vector<defs::connection_t> serverDetailsList;

    for (auto const& clientItr : m_clientMap)
    {
        serverDetailsList.emplace_back(clientItr.first);
    }

    return serverDetailsList;
}

size_t TcpClientList::NumberOfUnsentAsyncMessages(const defs::connection_t& server) const
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
