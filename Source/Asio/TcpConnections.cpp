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
#include <sstream>
#include <boost/exception/all.hpp>
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

void TcpConnections::SetOnCloseCallback(defs::on_close_t const& onClose)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_onClose = onClose;
}

void TcpConnections::Add(defs::connection_t const& endpoint, const defs::tcp_conn_ptr_t& connection)
{
    if (connection)
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_connections[endpoint] = connection;
    }
}

void TcpConnections::Remove(const defs::tcp_conn_ptr_t& connection)
{
    if (!connection)
    {
        return;
    }

    defs::on_close_t   onClose;
    defs::connection_t key = defs::NULL_CONNECTION;

    {
        std::lock_guard<std::mutex> lock{m_mutex};

        // Compare raw pointer identity to avoid any shared_ptr aliasing surprises
        TcpConnection* const c = connection.get();

        // Builder 10.1-safe: no std::find_if, no generic lambda
        for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
        {
            if (it->second.get() == c)
            {
                key = it->first;         // copy key *before* erase
                m_connections.erase(it); // erase invalidates only 'it'
                onClose = m_onClose;     // copy callback under lock
                break;
            }
        }
    }

    // Invoke callback without holding the mutex
    if (onClose && (key != defs::NULL_CONNECTION))
    {
        onClose(key);
    }
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
    std::vector<defs::tcp_conn_ptr_t> snapshot;

    {
        std::lock_guard<std::mutex> lock{m_mutex};
        snapshot.reserve(m_connections.size());
        for (auto const& kv : m_connections)
        {
            snapshot.push_back(kv.second);
        }
    }

    // Close outside the mutex. Each connection should call Remove() once.
    for (auto const& c : snapshot)
    {
        c->CloseConnection(); // blocking
    }
}

bool TcpConnections::SendMessageAsync(const defs::connection_t&  target,
                                      const defs::char_buffer_t& message) const
{
    defs::tcp_conn_ptr_t conn;

    // Reduce mutex scope: find and copy the shared_ptr, then unlock.
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        auto                        it = m_connections.find(target);

        if (it == m_connections.end())
        {
            return false;
        }

        conn = it->second;
    }

    return conn->SendMessageAsync(message);
}

bool TcpConnections::SendMessageSync(const defs::connection_t&  target,
                                     const defs::char_buffer_t& message) const
{
    defs::tcp_conn_ptr_t conn;

    // Reduce mutex scope: find and copy the shared_ptr, then unlock.
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        auto                        it = m_connections.find(target);

        if (it == m_connections.end())
        {
            return false;
        }

        conn = it->second;
    }

    return conn->SendMessageSync(message);
}

void TcpConnections::SendMessageToAll(const defs::char_buffer_t& message) const
{
    std::vector<defs::tcp_conn_ptr_t> snapshot;

    // Reduce mutex scope.
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        snapshot.reserve(m_connections.size());

        for (auto const& kv : m_connections)
        {
            snapshot.push_back(kv.second);
        }
    }

    for (auto const& c : snapshot)
    {
        c->SendMessageAsync(message);
    }
}

auto TcpConnections::GetLocalEndForRemoteEnd(const defs::connection_t& remoteEnd) const
    -> defs::connection_t
{
    defs::tcp_conn_ptr_t conn;

    // Reduce mutex scope: grab the connection pointer then release lock.
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        auto                        it = m_connections.find(remoteEnd);

        if (it == m_connections.end())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("unknown connection"));
        }

        conn = it->second;
    }

    boost_sys::error_code ec;
    auto                  lep = conn->Socket().local_endpoint(ec);

    if (ec)
    {
        std::ostringstream ssErr;
        ssErr << "local_endpoint failed: " << ec.message();
        BOOST_THROW_EXCEPTION(std::runtime_error(ssErr.str()));
    }

    return std::make_pair(lep.address().to_string(), lep.port());
}

size_t TcpConnections::NumberOfUnsentAsyncMessages(const defs::connection_t& target) const
{
    defs::tcp_conn_ptr_t conn;

    // Reduce mutex scope
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        auto                        it = m_connections.find(target);
        if (it == m_connections.end())
        {
            return 0;
        }
        conn = it->second;
    }

    return conn->NumberOfUnsentAsyncMessages();
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
