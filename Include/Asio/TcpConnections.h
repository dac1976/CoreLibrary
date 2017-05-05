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
 * \file TcpConnections.h
 * \brief File containing TCP connections class declaration.
 */

#ifndef TCPCONNECTIONS
#define TCPCONNECTIONS

#include "AsioDefines.h"
#include "Exceptions/CustomException.h"
#include <map>
#include <mutex>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{

/*!
 * \brief Unknown connection exception.
 *
 * This exception class is intended to be thrown when an unknown
 * connection is specified as an argument.
 */
class CORE_LIBRARY_DLL_SHARED_API xUnknownConnectionError : public exceptions::xCustomException
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
    /*! \brief Copy constructor. */
    xUnknownConnectionError(const xUnknownConnectionError&) = default;
    /*! \brief Copy assignment operator. */
    xUnknownConnectionError& operator=(const xUnknownConnectionError&) = default;
};

/*! \brief Forward declaration of TCP connection class. */
class TcpConnection;

/*!
 * \brief TCP connections class to manage the TcpConnection objects.
 *
 * This class is the one of the fundamental building blocks for the other TCP networking classes.
 */
class CORE_LIBRARY_DLL_SHARED_API TcpConnections final
{
public:
    /*! \brief Default constructor. */
    TcpConnections() = default;
    /*! \brief Default desctructor. */
    ~TcpConnections() = default;
    /*! \brief Copy constructor - deleted. */
    TcpConnections(const TcpConnections&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    TcpConnections& operator=(const TcpConnections&) = delete;
    /*!
     * \brief Add a connection.
     * \param[in] connection - Shared pointer to connection object.
     */
    void Add(defs::tcp_conn_ptr_t connection);
    /*!
     * \brief Remove a connection.
     * \param[in] connection - Shared pointer to connection object.
     */
    void Remove(defs::tcp_conn_ptr_t connection);
    /*!
     * \brief Get the number of connections.
     * \return Number of connections.
     */
    size_t Size() const;
    /*!
     * \brief Is the connection map empty?
     * \return True if empty, false otherwise.
     */
    bool Empty() const;
    /*! \brief Close all connections. */
    void CloseConnections();
    /*!
     * \brief Send an asynchronous message.
     * \param[in] target - Target connection details.
     * \param[in] message - Message buffer to send.
     */
    void SendMessageAsync(const defs::connection_t&  target,
                          const defs::char_buffer_t& message) const;
    /*!
     * \brief Send a synchronous message.
     * \param[in] target - Target connection details.
     * \param[in] message - Message buffer to send.
     * \return True if sent successfully, false otherwise.
     */
    bool SendMessageSync(const defs::connection_t&  target,
                         const defs::char_buffer_t& message) const;
    /*!
     * \brief Send an asynchronous message to all connections.
     * \param[in] message - Message buffer to send.
     */
    void SendMessageToAll(const defs::char_buffer_t& message) const;
    /*!
     * \brief Get the connection details for one of the remote connections.
     * \param[in] remoteEnd - Remote end's connection details.
     * \return Connection details for remote end.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
    defs::connection_t GetLocalEndForRemoteEnd(const defs::connection_t& remoteEnd) const;

private:
    /*! \brief Access mutex for thread safety. */
    mutable std::mutex m_mutex;
    /*! \brief Typedef to our connection map type. */
    typedef std::map<defs::connection_t, defs::tcp_conn_ptr_t> tcp_conn_map;
    /*! \brief The connections map. */
    tcp_conn_map m_connections;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTIONS
