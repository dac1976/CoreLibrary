// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
// Contact <duncan.crutchley+corelibrary@gmail.com>
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
 * \file TcpConnection.cpp
 * \brief File containing TCP connections class definition.
 */ 
 
#include "TcpConnections.hpp" 
#include "TcpConnection.hpp" 
#include <utility>

namespace core_lib {
namespace tcp_conn{

// ****************************************************************************
// 'class TcpConnections' definition
// ****************************************************************************

void TcpConnections::Add(asio_defs::tcp_conn_ptr connection)
{	
	std::lock_guard<std::mutex> lock{m_mutex};
    m_connections.insert(std::make_pair(connection->Socket().remote_endpoint()
                                        , connection));
}

void TcpConnections::Remove(asio_defs::tcp_conn_ptr connection) 
{ 
	std::lock_guard<std::mutex> lock{m_mutex};
    m_connections.erase(connection->Socket().remote_endpoint());
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

size_t TcpConnections::Size() const
{
	std::lock_guard<std::mutex> lock{m_mutex};
	return m_connections.size();
}

void TcpConnections::SendMessageAsync(const boost_tcp::endpoint& targetEndPoint
					                  , const asio_defs::char_vector& message)
{
	std::lock_guard<std::mutex> lock{m_mutex};	
    tcp_conn_map::iterator connIt{m_connections.find(targetEndPoint)};

    if (connIt != m_connections.end())
    {
        connIt->second->SendMessageAsync(message);
    }
}
				   
bool TcpConnections::SendMessageSync(const boost_tcp::endpoint& targetEndPoint
					                 , const asio_defs::char_vector& message)
{
	std::lock_guard<std::mutex> lock{m_mutex};	
    tcp_conn_map::iterator connIt{m_connections.find(targetEndPoint)};
    return connIt == m_connections.end()
           ? false
           : connIt->second->SendMessageSync(message);
}

void TcpConnections::SendMessageToAll(const asio_defs::char_vector& message)
{
	std::lock_guard<std::mutex> lock{m_mutex};
	
	for (auto& connection : m_connections)
	{
        connection.second->SendMessageAsync(message);
	}
}

bool TcpConnections::GetLocalEndPointForRemoteEndPoint(const boost_tcp::endpoint& remoteEndPoint
	                                                   , boost_tcp::endpoint& localEndPoint) const
{
	std::lock_guard<std::mutex> lock{m_mutex};
    tcp_conn_map::const_iterator connIt{m_connections.find(remoteEndPoint)};

    if (connIt == m_connections.end())
    {
        return false;
    }
    else
    {
        localEndPoint = connIt->second->Socket().local_endpoint();
        return true;
    }
}

} // namespace tcp_conn
} // namespace core_lib
 
 
