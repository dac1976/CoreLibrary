
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
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly. \param[in] sendOption - Socket send option to control the use
     * of the Nagle algorithm. \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of
     * unsent async messages.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     */
    TcpServer(boost_iocontext_t& ioContext, uint16_t listenPort, size_t minAmountToRead,
              const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
              const defs::message_received_handler_t& messageReceivedHandler,
              eSendOption                             sendOption = eSendOption::nagleOn,
              size_t maxAllowedUnsentAsyncMessages               = MAX_UNSENT_ASYNC_MSG_COUNT);
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly. \param[in] sendOption - Socket send option to control the use
     * of the Nagle algorithm.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommended.
     */
    TcpServer(uint16_t listenPort, size_t minAmountToRead,
              const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
              const defs::message_received_handler_t& messageReceivedHandler,
              eSendOption                             sendOption = eSendOption::nagleOn,
              size_t maxAllowedUnsentAsyncMessages               = MAX_UNSENT_ASYNC_MSG_COUNT);
    /*! \brief Default destructor. */
    ~TcpServer();
    /*! \brief Copy constructor - deleted. */
    TcpServer(const TcpServer&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    TcpServer& operator=(const TcpServer&) = delete;
    /*! \brief Move constructor - deleted. */
    TcpServer(TcpServer&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    TcpServer& operator=(TcpServer&&) = delete;
    /*!
     * \brief Retrieve this server's connection details for a given client.
     * \param[in] client - A client's connection details.
     * \return - Connection object describing target server's address and port.
     *
     * If no such client is known to the server then it returns { "0.0.0.0", listenPort}.
     *
     * Throws std::invalid_argument is remoteEnd is not valid.
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
     * \return True if sent successfully, false otherwise.
     */
    bool SendMessageToClientAsync(const defs::connection_t&  client,
                                  const defs::char_buffer_t& message) const;
    /*!
     * \brief Send a message buffer to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \return True if sent successfully, false otherwise.
     */
    bool SendMessageToClientSync(const defs::connection_t&  client,
                                 const defs::char_buffer_t& message) const;
    /*!
     * \brief Send a message buffer to all clients asynchronously.
     * \param[in] message - Message buffer.
     * \return True if sent successfully, false otherwise.
     */
    bool SendMessageToAllClients(const defs::char_buffer_t& message) const;
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
    void AcceptHandler(defs::tcp_conn_ptr_t connection, const boost_sys::error_code& error);
    /*! \brief Process closing the acceptor. */
    void ProcessCloseAcceptor();

private:
    /*! \brief I/O context thread group. */
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O context reference. */
    boost_iocontext_t& m_ioContext;
    /*! \brief I/O context strand. */
    boost_iocontext_t::strand m_strand;
    /*! \brief The connection acceptor. */
    std::unique_ptr<boost_tcp_acceptor_t> m_acceptor{};
    /*! \brief Server listen port. */
    uint16_t m_listenPort{0};
    /*! \brief Minimum amount to read from socket. */
    size_t m_minAmountToRead{0};
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead{};
    /*! \brief Callback to handle received message. */
    defs::message_received_handler_t m_messageReceivedHandler{};
    /*! \brief Socket receive option. */
    eSendOption m_sendOption{eSendOption::nagleOn};
    /*! \brief Max allowed unsent async message counter. */
    size_t m_maxAllowedUnsentAsyncMessages{MAX_UNSENT_ASYNC_MSG_COUNT};
    /*! \brief TCP connections object. */
    TcpConnections m_clientConnections{};
    /*! \brief Close event. */
    threads::SyncEvent m_closedEvent{};
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPSERVER
