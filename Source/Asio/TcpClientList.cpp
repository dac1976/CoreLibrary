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

TcpClientList::TcpClientList(boost_iocontext_t& ioContext, size_t minAmountToRead,
                             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                             defs::message_received_handler_t const& messageReceivedHandler,
                             eSendOption sendOption, size_t maxAllowedUnsentAsyncMessages)
    : m_ioContextPtr(&ioContext)
    , m_minAmountToRead(minAmountToRead)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_sendOption(sendOption)
    , m_maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages)
{
}

TcpClientList::TcpClientList(size_t                                  minAmountToRead,
                             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                             defs::message_received_handler_t const& messageReceivedHandler,
                             eSendOption sendOption, size_t maxAllowedUnsentAsyncMessages)
    : m_minAmountToRead(minAmountToRead)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_sendOption(sendOption)
    , m_maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages)
{
}

TcpClientList::~TcpClientList()
{
    CloseConnections();
}

auto TcpClientList::ServerConnection(defs::connection_t const& clientConn) const
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

bool TcpClientList::Connected(defs::connection_t const& server) const
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

auto TcpClientList::GetClientDetailsForServer(defs::connection_t const& server) const
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

void TcpClientList::CloseConnection(defs::connection_t const& server) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto clientPtr = FindTcpClient(server);

    if (clientPtr)
    {
        clientPtr->CloseConnection();
    }
}

void TcpClientList::CloseConnections() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& client : m_clientMap)
    {
        client.second->CloseConnection();
    }
}

void TcpClientList::ClearConnections()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_clientMap.clear();
}

bool TcpClientList::SendMessageToServerAsync(defs::connection_t const&  server,
                                             defs::char_buffer_t const& message)
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

bool TcpClientList::SendMessageToServerSync(defs::connection_t const&  server,
                                            defs::char_buffer_t const& message)
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

auto TcpClientList::CreateTcpClient(defs::connection_t const& server) -> client_ptr_t
{
    client_ptr_t clientPtr;

    if (m_ioContextPtr)
    {
        clientPtr = std::make_shared<TcpClient>(*m_ioContextPtr,
                                                server,
                                                m_minAmountToRead,
                                                m_checkBytesLeftToRead,
                                                m_messageReceivedHandler,
                                                m_sendOption,
                                                m_maxAllowedUnsentAsyncMessages);
    }
    else
    {
        clientPtr = std::make_shared<TcpClient>(server,
                                                m_minAmountToRead,
                                                m_checkBytesLeftToRead,
                                                m_messageReceivedHandler,
                                                m_sendOption,
                                                m_maxAllowedUnsentAsyncMessages);
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
    std::lock_guard<std::mutex> lock(m_mutex);

    m_clientMap.clear();
}

auto TcpClientList::GetServerList() const -> std::vector<defs::connection_t>
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<defs::connection_t> serverDetailsList;

    for (auto const& clientItr : m_clientMap)
    {
        serverDetailsList.emplace_back(clientItr.first);
    }

    return serverDetailsList;
}

size_t TcpClientList::NumberOfUnsentAsyncMessages(const defs::connection_t& server) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

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
