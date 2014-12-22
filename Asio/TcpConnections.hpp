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
#include "../Exceptions/CustomException.hpp"
#include <map>
#include <mutex>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

/*!
 * \brief Unknown connection exception.
 *
 * This exception class is intended to be thrown when an unknown
 * connection is specified as an argument.
 */
class xUnknownConnectionError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xUnknownConnectionError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xUnknownConnectionError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xUnknownConnectionError();
};

class TcpConnection;

class TcpConnections final
{
public:
	TcpConnections() = default;

	~TcpConnections() = default;

	TcpConnections(const TcpConnections& ) = delete;

    TcpConnections& operator=(const TcpConnections& ) = delete;

	void Add(defs::tcp_conn_ptr Connection);

	void Remove(defs::tcp_conn_ptr Connection);

	size_t Size() const;
	
	bool Empty() const;

    void CloseConnections();

    void SendMessageAsync(const defs::connection& target
						  , const defs::char_buffer& message);

    bool SendMessageSync(const defs::connection& target
						 , const defs::char_buffer& message);

	void SendMessageToAll(const defs::char_buffer& message);

    // Throws xUnknownConnectionError is remoteEnd is not valid.
    auto GetLocalEndForRemoteEnd(const defs::connection& remoteEnd) const
             -> defs::connection;

private:
	mutable std::mutex m_mutex;
    typedef std::map<defs::connection, defs::tcp_conn_ptr> tcp_conn_map;
	tcp_conn_map m_connections;
};


} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTIONS_H
