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
 * \file TcpConnections.hpp
 * \brief File containing TCP connections class declaration.
 */

#ifndef TCPCONNECTIONS_H
#define TCPCONNECTIONS_H

#include "AsioDefines.hpp"
#include <map>
#include <mutex>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class TcpConnection;

class TcpConnections final
{
public:
	TcpConnections() = default;

	~TcpConnections() = default;

	TcpConnections(const TcpConnections& ) = delete;

	const TcpConnections& operator=(const TcpConnections& ) = delete;

	void Add(defs::tcp_conn_ptr Connection);

	void Remove(defs::tcp_conn_ptr Connection);

	void CloseConnections();

	size_t Size() const;

	void SendMessageAsync(const defs::connection_address& target                     
						  , const defs::char_buffer& message);

	bool SendMessageSync(const defs::connection_address& target  
						 , const defs::char_buffer& message);

	void SendMessageToAll(const defs::char_buffer& message);

	bool GetLocalEndForRemoteEnd(const defs::connection_address& remoteEnd 
								 , defs::connection_address& localEnd) const;

private:
	mutable std::mutex m_mutex;
	typedef std::map<defs::connection_address, defs::tcp_conn_ptr> tcp_conn_map;
	tcp_conn_map m_connections;
};


} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTIONS_H
