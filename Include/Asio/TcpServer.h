
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
 * \file TcpServer.h
 * \brief File containing TCP server class declaration.
 */

#ifndef TCPSERVER
#define TCPSERVER

#include "IoContextThreadGroup.h"
#include "TcpConnections.h"
#include "Threads/SyncEvent.h"

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
 * \brief A bi-directional TCP server.
 *
 *  This class forms the underpinnings of the TcpTypedServer class.
 *
 * This class is also suitable when the user only wants to deal with char buffers for network
 * messages.
 */
class CORE_LIBRARY_DLL_SHARED_API TcpServer final
{
public:
    /*! \brief Default constructor - deleted. */
    TcpServer() = delete;
    /*! \brief Deleted copy constructor. */
    TcpServer(const TcpServer&) = delete;
    /*! \brief Deleted copy assignment operator. */
    TcpServer& operator=(const TcpServer&) = delete;
    /*! \brief Deleted move constructor. */
    TcpServer(TcpServer&&) = delete;
    /*! \brief Deleted move assignment operator. */
    TcpServer& operator=(TcpServer&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     *            computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] settings - structure containing connection options
     * and behavioural settings.
     * \param[in] messageReceivedHandlerEx - Special callback for when
     * socket is used for special use cases where the message handler needs the endpoint details
     * passed to it. If this is defined then you ideally would set messageReceivedHandler = {}.
     * \param[in] checkBytesLeftToReadEx - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message. Extended to take endpoint
     * details.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    TcpServer(asio_compat::io_service_t& ioService,
	        uint16_t listenPort,
            defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
            defs::message_received_handler_t const& messageReceivedHandler,
            TcpConnSettings const& settings = {},
            defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
            defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx   = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     *            computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] settings - structure containing connection options
     * and behavioural settings.
     * \param[in] messageReceivedHandler - Function object capable of
     * handling a received message and dispatching it accordingly.
     * \param[in] settings - structure
     * containing connection options and behavioural settings.
     * \param[in] messageReceivedHandlerEx -
     * Special callback for when socket is used for special use cases where the message handler
     * needs the endpoint details passed to it. If this is defined then you ideally would set
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
    TcpServer(uint16_t listenPort,
	        defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
            defs::message_received_handler_t const& messageReceivedHandler,
            TcpConnSettings const& settings = {},
            defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
            defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx = {});
    /*! \brief Default destructor. */
    ~TcpServer();
    /*!
     * \brief Optional function to set a callback to fire when a connection is closed.
     * \param[in] callback - The callback function
     */
    void SetOnCloseCallback(defs::on_close_t const& onClose);
    /*!
     * \brief Retrieve this server's connection details for a given client.
     * \param[in] client - A client's connection details.
     * \return - Connection object describing target server's address and port.
     *
     * If no such client is known to the server then it returns { "0.0.0.0", listenPort}.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
    defs::connection_t GetServerDetailsForClient(const defs::connection_t& client) const;
    /*!
     * \brief Retrieve this server's listen port.
     * \return The listen port.
     */
    uint16_t ListenPort() const;
    /*!
     * \brief Retrieve this server's number of clients.
     * \return The number of clients.
     */
    size_t NumberOfClients() const;
    /*! \brief Manually close the acceptor.
     *
     * Note that this object is RAII so will automatically close the acceptor in its destructor.
     */
    void CloseAcceptor();
    /*! \brief Manually open the acceptor. */
    void OpenAcceptor();
    /*!
     * \brief Send a message buffer to a client asynchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
    bool SendMessageToClientAsync(const defs::connection_t&  client,
                                  defs::char_buf_cspan_t message) const;
    /*!
     * \brief Send a message buffer to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     */
    bool SendMessageToClientSync(const defs::connection_t&  client,
                                 defs::char_buf_cspan_t message) const;
    /*!
     * \brief Send a message buffer to all clients asynchronously.
     * \param[in] message - Message buffer.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless a an exception is thrown. This
     * method gives best performance when sending.
     */
    void SendMessageToAllClients(defs::char_buf_cspan_t message) const;
    /*!
     * \brief Get number of unsent async messages.
     * \param[in] client - Target connection details.
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages(const defs::connection_t& client) const;

    /*!
     * \brief Tells if a given client is currently connected to the server
     * \param[in] target - Target connection details.
     * \return true if connected, false if not
     */
    bool IsConnected(const defs::connection_t& client) const;

private:
    /*! \brief Accept a connection. */
    void AcceptConnection();
    /*!
     * \brief Accept handler for new connections.
     * \param[in] connection - Client connection.
     * \param[in] error - An error code if fault occurred.
     */
    void AcceptHandler(const defs::tcp_conn_ptr_t& connection, const boost_sys::error_code& error);
    /*! \brief Process closing the acceptor. */
    void ProcessCloseAcceptor();

private:
    /*! \brief I/O service thread group. */
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service reference. */
    asio_compat::io_service_t& m_ioService;
    /*! \brief I/O service strand. */
    asio_compat::strand_t m_strand;
    /*! \brief The connection acceptor. */
    std::unique_ptr<boost_tcp_acceptor_t> m_acceptor;
    /*! \brief Server listen port. */
    uint16_t m_listenPort{0};
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_ex_t m_checkBytesLeftToReadEx;
    /*! \brief Callback to handle received message. */
    defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Message received handler extended callback. */
    defs::message_received_handler_ex_t m_messageReceivedHandlerEx;
    /*! \brief Structure holding socket connection options and behavioural settings. */
    TcpConnSettings m_settings;
    /*! \brief TCP connections object. */
    std::shared_ptr<TcpConnections> m_clientConnections;
    /*! \brief Close event. */
    threads::SyncEvent m_closedEvent;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPSERVER
