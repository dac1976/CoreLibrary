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
    /*! \brief Move constructor - deleted. */
    TcpConnections(TcpConnections&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    TcpConnections& operator=(TcpConnections&&) = delete;
    /*!
     * \brief Add a connection.
     * \param[in] connection - Shared pointer to connection object.
     */
    void Add(defs::tcp_conn_ptr_t const& connection);
    /*!
     * \brief Remove a connection.
     * \param[in] connection - Shared pointer to connection object.
     */
    void Remove(defs::tcp_conn_ptr_t const& connection);
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
     * \return True if sent successfully, false otherwise.
     */
    bool SendMessageAsync(const defs::connection_t&  target,
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
     * \return True if sent successfully, false otherwise.
     */
    bool SendMessageToAll(const defs::char_buffer_t& message) const;
    /*!
     * \brief Get the connection details for one of the remote connections.
     * \param[in] remoteEnd - Remote end's connection details.
     * \return Connection details for remote end.
     *
     * Throws std::invalid_argument if remoteEnd is not valid.
     */
    defs::connection_t GetLocalEndForRemoteEnd(const defs::connection_t& remoteEnd) const;
    /*!
     * \brief Get number of unsent async messages.
     * \param[in] target - Target connection details.
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages(const defs::connection_t& target) const;

    /*!
     * \brief Tells if a given client is currently connected ot the server
     * \param[in] target - Target connection details.
     * \return true if connected, false if not
     */
    bool IsConnected(const defs::connection_t& client) const;

private:
    /*! \brief Access mutex for thread safety. */
    mutable std::mutex m_mutex;
    /*! \brief Typedef to our connection map type. */
    using tcp_conn_map = std::map<defs::connection_t, defs::tcp_conn_ptr_t>;
    /*! \brief The connections map. */
    tcp_conn_map m_connections{};
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTIONS
