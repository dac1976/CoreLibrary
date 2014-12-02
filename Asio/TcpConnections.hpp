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
#include <boost/shared_ptr.hpp>
#include <set>
#include <string>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The tcp_conn namespace. */
namespace tcp_conn{

class TcpConnection;

class TcpConnections final
{
public:	
	TcpConnections();
	
	~TcpConnections() = default;
	
	void Add(asio_defs::tcp_conn_ptr Connection);
	
	void Remove(asio_defs::tcp_conn_ptr Connection);
	
	void CloseConnections();
	
	size_t NumberOfConnections() const;
	
	void SendMessageAsync(const std::string& targetIp
					      , unsigned short targetPort
						  , const asio_defs::char_vector& message);
					   
	bool SendMessageSync(const std::string& targetIp
					      , unsigned short targetPort
						  , const asio_defs::char_vector& message);

    void SendMessageToAll(const asio_defs::char_vector& message);

	std::string GetLocalIPForRemoteConnection(const std::string& targetIp, 
											  unsigned short targetPort) const;
	
	unsigned short GetLocalPortForRemoteConnection(const std::string& targetIp, 
												   unsigned short targetPort) const;

private:
	mutable boost::mutex m_mutex;
	typedef std::set<asio_defs::tcp_conn_ptr> tcp_conn_set;
	tcp_conn_set m_connections;	
};


} // namespace tcp_conn
} // namespace core_lib

#endif // TCPCONNECTIONS_H