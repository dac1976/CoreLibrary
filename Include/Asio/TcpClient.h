
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
 * \file TcpClient.h
 * \brief File containing TCP client class declaration.
 */

#ifndef TCPCLIENT
#define TCPCLIENT

#include "TcpConnections.h"
#include "IoContextThreadGroup.h"

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
 * \brief A bi-directional TCP client.
 *
 * This class forms the underpinnings of the TcpTypedClient class.
 *
 * This class is also suitable when the user only wants to deal with char buffers for network
 * messages.
 */
class CORE_LIBRARY_DLL_SHARED_API TcpClient final
{
public:
    /*! \brief Default constructor - deleted. */
    TcpClient() = delete;
    /*! \brief Deleted copy constructor. */
    TcpClient(const TcpClient&) = delete;
    /*! \brief Deleted copy assignment operator. */
    TcpClient& operator=(const TcpClient&) = delete;
    /*! \brief Deleted move constructor. */
    TcpClient(TcpClient&&) = delete;
    /*! \brief Deleted move assignment operator. */
    TcpClient& operator=(TcpClient&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] server - Connection object describing target server's address and port.
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
     * Typically use this constructor when managing a bool of threads using an instance of
     * IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    TcpClient(asio_compat::io_service_t& ioService,
	        defs::connection_t const& server,
            defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
            defs::message_received_handler_t const& messageReceivedHandler,
            TcpConnSettings const& settings = {},
            defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
            defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] server - Connection object describing target server's address and port.
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
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommended.
     */
    TcpClient(defs::connection_t const& server,
            defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
            defs::message_received_handler_t const& messageReceivedHandler,
            TcpConnSettings const& settings = {},
            defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
            defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx = {});
    /*! \brief Default destructor. */
    ~TcpClient();
    /*!
     * \brief Retrieve server connection details.
     * \return - Connection object describing target server's address and port.
     */
    defs::connection_t ServerConnection() const;
    /*!
     * \brief Check if the client is connected to the server.
     * \return True if conneced, false otherwise.
     */
    bool Connected() const;
    /*!
     * \brief Retrieve this client's connection details
     * \return Connection object describing this client's address and port.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
    defs::connection_t GetClientDetailsForServer() const;
    /*!
     * \brief Check connection and create if required.
     * \return True if connection exists, false otherwise.
     */
    bool CheckAndCreateConnection();
    /*! \brief Manually close the connection.
     *
     * Note that this object uses RAII so will close the connection when destroyed.
     */
    void CloseConnection();
    /*!
     * \brief Reconnect the socket to a particular server target connection.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] settings - structure containing connection options and behavioural settings.
     */
    void Reconnect(defs::connection_t const& server, TcpConnSettings const& settings = {});
    /*!
     * \brief Send a message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
    bool SendMessageToServerAsync(const defs::char_buffer_t& message);
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(const defs::char_buffer_t& message);
    /*!
     * \brief Get number of unsent async messages.
     * \return Number of nsent messages
     */
    size_t NumberOfUnsentAsyncMessages() const;

private:
    /*! \brief Create conenction to server. */
    void CreateConnection();

private:
    /*! \brief I/O service thread group. */
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service reference. */
    asio_compat::io_service_t& m_ioService;
    /*! \brief Server connection details. */
    defs::connection_t m_server;
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
    std::shared_ptr<TcpConnections> m_serverConnection;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCLIENT
