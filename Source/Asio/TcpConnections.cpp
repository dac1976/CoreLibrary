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
 * \file TcpConnections.cpp
 * \brief File containing TCP connections class definition.
 */

#include "Asio/TcpConnections.h"
#include <stdexcept>
#include <boost/throw_exception.hpp>
#include "Asio/TcpConnection.h"

namespace core_lib
{
namespace asio
{
namespace tcp
{

// ****************************************************************************
// 'class TcpConnections' definition
// ****************************************************************************

void TcpConnections::Add(defs::tcp_conn_ptr_t const& connection)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_connections.emplace(
        std::make_pair(connection->Socket().remote_endpoint().address().to_string(),
                       connection->Socket().remote_endpoint().port()),
        connection);
}

void TcpConnections::Remove(defs::tcp_conn_ptr_t const& connection)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_connections.erase(std::make_pair(connection->Socket().remote_endpoint().address().to_string(),
                                       connection->Socket().remote_endpoint().port()));
}

size_t TcpConnections::Size() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_connections.size();
}

bool TcpConnections::Empty() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_connections.empty();
}

void TcpConnections::CloseConnections()
{
    std::lock_guard<std::mutex> lock{m_mutex};

    for (auto& connection : m_connections)
    {
        connection.second->CloseConnection();
    }

    m_connections.clear();
}

void TcpConnections::SendMessageAsync(const defs::connection_t&  target,
                                      const defs::char_buffer_t& message) const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    auto                        connIt = m_connections.find(target);

    if (connIt != m_connections.end())
    {
        connIt->second->SendMessageAsync(message);
    }
}

bool TcpConnections::SendMessageSync(const defs::connection_t&  target,
                                     const defs::char_buffer_t& message) const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    auto                        connIt = m_connections.find(target);
    return connIt == m_connections.end() ? false : connIt->second->SendMessageSync(message);
}

void TcpConnections::SendMessageToAll(const defs::char_buffer_t& message) const
{
    std::lock_guard<std::mutex> lock{m_mutex};

    for (auto& connection : m_connections)
    {
        connection.second->SendMessageAsync(message);
    }
}

auto TcpConnections::GetLocalEndForRemoteEnd(const defs::connection_t& remoteEnd) const
    -> defs::connection_t
{
    std::lock_guard<std::mutex> lock{m_mutex};
    defs::connection_t          localEnd;
    auto                        connIt = m_connections.find(remoteEnd);

    if (connIt == m_connections.end())
    {
        BOOST_THROW_EXCEPTION(std::invalid_argument("unknown connection"));
    }
    else
    {
        localEnd = std::make_pair(connIt->second->Socket().local_endpoint().address().to_string(),
                                  connIt->second->Socket().local_endpoint().port());
    }

    return localEnd;
}

size_t TcpConnections::NumberOfUnsentAsyncMessages(const defs::connection_t& target) const
{
	std::lock_guard<std::mutex> lock{m_mutex};
	auto                        connIt = m_connections.find(target);
	return connIt == m_connections.end() ? 0 : connIt->second->NumberOfUnsentAsyncMessages();
}

bool TcpConnections::IsConnected(const defs::connection_t& client) const
{
    std::lock_guard<std::mutex> lock{m_mutex};
	auto                        connIt = m_connections.find(client);
	return (connIt != m_connections.end());
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
