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
 * \file TcpClientList.h
 * \brief File containing TCP client list class declaration.
 */
#ifndef TCPCLIENTLIST_H
#define TCPCLIENTLIST_H

#include <map>
#include <mutex>
#include "AsioDefines.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{

/*! \brief Forward declaration of TcpClient. */
class TcpClient;

/*! \brief A class implementing a collection of bi-directional TCP clients. */
class CORE_LIBRARY_DLL_SHARED_API TcpClientList final
{
    using client_ptr_t = std::shared_ptr<TcpClient>;
    using client_map_t = std::map<defs::connection_t, client_ptr_t>;

public:
    /*! \brief Default constructor - deleted. */
    TcpClientList() = delete;
    /*! \brief Deleted copy constructor. */
    TcpClientList(const TcpClientList&) = delete;
    /*! \brief Deleted copy assignment operator. */
    TcpClientList& operator=(const TcpClientList&) = delete;
    /*! \brief Deleted move constructor. */
    TcpClientList(TcpClientList&&) = delete;
    /*! \brief Deleted move assignment operator. */
    TcpClientList& operator=(TcpClientList&&) = delete;

    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     *            computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] settings - structure containing connection options
     * and behavioural settings.
     * \param[in] messageReceivedHandlerEx - Special
     * callback for when socket is used for special use cases where the message handler needs the
     * endpoint details passed to it. If this is defined then you ideally would set
     * messageReceivedHandler = {}.
     * \param[in] checkBytesLeftToReadEx - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message. Extended to take endpoint
     * details.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    TcpClientList(asio_compat::io_service_t&                 ioService,
                  defs::check_bytes_left_to_read_t const&    checkBytesLeftToRead,
                  defs::message_received_handler_t const&    messageReceivedHandler,
                  TcpConnSettings const&                     settings                 = {},
                  defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
                  defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx   = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     *            computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] settings - structure containing connection options
     * and behavioural settings.
     * \param[in] messageReceivedHandlerEx - Special
     * callback for when socket is used for special use cases where the message handler needs the
     * endpoint details passed to it. If this is defined then you ideally would set
     * messageReceivedHandler = {}.
     * \param[in] checkBytesLeftToReadEx - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message. Extended to take endpoint
     * details.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own 2 threads. For simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommended.
     */
    TcpClientList(defs::check_bytes_left_to_read_t const&    checkBytesLeftToRead,
                  defs::message_received_handler_t const&    messageReceivedHandler,
                  TcpConnSettings const&                     settings                 = {},
                  defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
                  defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx   = {});
    /*! \brief Default destructor. */
    ~TcpClientList();
    /*!
     * \brief Retrieve server connection details for a client.
     * \param[in] clientConn - Connection object describing server's address and port.
     * \return - Connection object describing target server's address and port.
     */
    defs::connection_t ServerConnection(defs::connection_t const& clientConn) const;
    /*!
     * \brief Check if the client is connected to the server.
     * \param[in] server - Connection object describing server's address and port.
     * \return True if conneced, false otherwise.
     */
    bool Connected(defs::connection_t const& server) const;
    /*!
     * \brief Retrieve this client's connection details.
     * \param[in] server - Connection object describing server's address and port.
     * \return Connection object describing this client's address and port.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
    defs::connection_t GetClientDetailsForServer(defs::connection_t const& server) const;
    /*!
     * \brief Manually close the client's connection.
     * \param[in] server - Connection object describing server's address and port.
     *
     * Note that this object uses RAII so will close all connections when destroyed.
     */
    void CloseConnection(defs::connection_t const& server);
    /*!
     * \brief Manually close the all client connections.
     *
     * Note that this object uses RAII so will close all connections when destroyed.
     */
    void CloseConnections();
    /*!
     * \brief Pre-emptively create connection.
     * \param[in] server - Connection object describing server's address and port.
     * \return Returns the success state of the send as a boolean.
     *
     * NOTE: The various send functions automatically create connecstions as necessary
     * but this function is useful if you want to establish a connection before
     * sending for the first time.
     */
    bool CreateConnectionToServer(defs::connection_t const& server);
    /*!
     * \brief Send a message buffer to the server asynchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
    bool SendMessageToServerAsync(defs::connection_t const&  server,
                             defs::char_buffer_t const& message);
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(defs::connection_t const&  server,
                            defs::char_buffer_t const& message);

    /*! \brief Clear all TCP clients from list. */
    void ClearList();
    /*!
     * \brief Get list of connections.
     * \param[out] serverDetailsList - list of server connection details.
     */
    void GetServerList(std::list<defs::connection_t>& serverDetailsList) const;
    /*!
     * \brief Get list of connections.
     * \return - list of server connection details.
     */
    std::vector<defs::connection_t> GetServerList() const;
    /*!
     * \brief Get number of unsent async messages.
     * \param[in] server - Target connection details.
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages(const defs::connection_t& server) const;

private:
    /*!
     * \brief Create the client connection for the server.
     * \param[in] server - Connection object describing server's address and port.
     * \return A std::shared_ptr to a TcpClient or a null pointer of not found.
     */
    client_ptr_t CreateTcpClient(defs::connection_t const& server);
    /*!
     * \brief Find the client connection for the server.
     * \param[in] server - Connection object describing server's address and port.
     * \return A std::shared_ptr to a TcpClient or a null pointer of not found.
     */
    client_ptr_t FindTcpClient(defs::connection_t const& server) const;

private:
    /*! \brief Mutex to protect client map. */
    mutable std::mutex m_mapMutex;
    /*! \brief External boost IO service to manage ASIO. */
    asio_compat::io_service_t* m_ioServicePtr{nullptr};
    /*! \brief Structure holding socket connection options and behavioural settings. */
    TcpConnSettings m_settings;
    /*! \brief Function object capable of decoding the message and computing how many bytes are left
     * until a complete message. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Function object capable of decoding the message and computing how many bytes are left
     * until a complete message. */
    defs::check_bytes_left_to_read_ex_t m_checkBytesLeftToReadEx;
    /*! \brief Function object capable of handling a received message and dispatching it
     * accordingly.*/
    defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Function object capable of handling a received message and dispatching it
     * accordingly.*/
    defs::message_received_handler_ex_t m_messageReceivedHandlerEx;
    /*! \brief Map of TCP clients. */
    client_map_t m_clientMap;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCLIENTLIST_H
