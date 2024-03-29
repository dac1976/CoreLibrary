
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
 * \file TcpTypedServer.h
 * \brief File containing TCP typed server class declaration.
 */

#ifndef TCPTYPEDSERVER
#define TCPTYPEDSERVER

#include <mutex>
#include "TcpServer.h"
#include "MessageUtils.h"

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
 * \brief A generic bi-directional TCP server.
 *
 * The template argument defines a message builder object that
 * must have an interface compatible with that of the class
 * core_lib::asio::messages::MessageBuilder.
 *
 * This class forms the underpinnings of the SimpleTcpServer class.
 *
 * This is also the class to be used when the user wants to specify their own message builder type
 * and message header type.
 */
template <typename MsgBldr> class TcpTypedServer final
{
public:
    /*! \brief Default constructor - deleted. */
    TcpTypedServer() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     *            header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     *            computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     *            and dispatching it accordingly.
     * \param[in] messageBuilder - A const reference to our
     *            persistent message builder object of type MsgBldr.
     * \param[in] sendOption - Socket send option
     *            to control the use of the Nagle algorithm.
     * \param[in] maxAllowedUnsentAsyncMessages - Maximum
     *            allowed number of unsent async messages.
     * \param[in] sendPoolMsgSize - Default size of message in pool. Set to 0 to not use the pool
     *            and instead use dynamic allocation.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     */
    TcpTypedServer(boost_iocontext_t& ioContext, uint16_t listenPort, size_t minAmountToRead,
                   const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                   const defs::message_received_handler_t& messageReceivedHandler,
                   const MsgBldr& messageBuilder, eSendOption sendOption = eSendOption::nagleOn,
                   size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT,
                   size_t sendPoolMsgSize               = 0)
        : m_messageBuilder{messageBuilder}
        , m_tcpServer{ioContext,
                      listenPort,
                      minAmountToRead,
                      checkBytesLeftToRead,
                      messageReceivedHandler,
                      sendOption,
                      maxAllowedUnsentAsyncMessages,
                      sendPoolMsgSize}
    {
    }
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     *            header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     *            computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     *            and dispatching it accordingly.
     * \param[in] messageBuilder - A const reference to our
     *            persistent message builder object of type MsgBldr.
     * \param[in] sendOption - Socket send option
     *            to control the use of the Nagle algorithm.
     * \param[in] maxAllowedUnsentAsyncMessages - Maximum
     *            allowed number of unsent async messages.
     * \param[in] sendPoolMsgSize - Default size of message in pool. Set to 0 to not use the pool
     *            and instead use dynamic allocation.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommended.
     */
    TcpTypedServer(uint16_t listenPort, size_t minAmountToRead,
                   const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                   const defs::message_received_handler_t& messageReceivedHandler,
                   const MsgBldr& messageBuilder, eSendOption sendOption = eSendOption::nagleOn,
                   size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT,
                   size_t sendPoolMsgSize               = 0)
        : m_messageBuilder{messageBuilder}
        , m_tcpServer{listenPort,
                      minAmountToRead,
                      checkBytesLeftToRead,
                      messageReceivedHandler,
                      sendOption,
                      maxAllowedUnsentAsyncMessages,
                      sendPoolMsgSize}
    {
    }
    /*! \brief Default destructor. */
    ~TcpTypedServer() = default;
    /*! \brief Copy constructor - deleted. */
    TcpTypedServer(const TcpTypedServer&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    TcpTypedServer& operator=(const TcpTypedServer&) = delete;
    /*! \brief Move constructor - deleted. */
    TcpTypedServer(TcpTypedServer&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    TcpTypedServer& operator=(TcpTypedServer&&) = delete;
    /*!
     * \brief Retrieve this server's connection details for a given client.
     * \param[in] client - A client's connection details.
     * \return - Connection object describing target server's address and port.
     *
     * If no such client is known to the server then it returns { "0.0.0.0", listenPort}.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
    defs::connection_t GetServerDetailsForClient(const defs::connection_t& client) const
    {
        return m_tcpServer.GetServerDetailsForClient(client);
    }
    /*!
     * \brief Retrieve this server's listen port.
     * \return The listen port.
     */
    uint16_t ListenPort() const
    {
        return m_tcpServer.ListenPort();
    }
    /*!
     * \brief Retrieve this server's number of clients.
     * \return The number of clients.
     */
    size_t NumberOfClients() const
    {
        return m_tcpServer.NumberOfClients();
    }
    /*! \brief Manually close the acceptor.
     *
     * Note that this object is RAII so will automatically close the acceptor in its destructor.
     */
    void CloseAcceptor()
    {
        m_tcpServer.CloseAcceptor();
    }
    /*! \brief Manually open the acceptor. */
    void OpenAcceptor()
    {
        m_tcpServer.OpenAcceptor();
    }
    /*!
     * \brief Send a header-only message to a client asynchronously.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     * response, the default value will mean the response address will point to this server socket.
     * \return Returns the success state of whether the message was posted to the send queue.
     */
    bool SendMessageToClientAsync(
        const defs::connection_t& client, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage(
                messageId, responseAddress, GetServerDetailsForClient(client), m_messageBuilder);

            return m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Send a header-only message to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of the send as a boolean.
     */
    bool
    SendMessageToClientSync(const defs::connection_t& client, int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage(
                messageId, responseAddress, GetServerDetailsForClient(client), m_messageBuilder);

            return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
        }
        catch (...)
        {
            return false;
        }
    }
    /*!
     * \brief Send a header-only message to all clients asynchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where a client should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of whether the message was posted to the
     * send queue.
     */
    bool
    SendMessageToAllClients(int32_t                   messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer =
                messages::BuildMessage(messageId,
                                       responseAddress,
                                       GetServerDetailsForClient(defs::NULL_CONNECTION),
                                       m_messageBuilder);

            return m_tcpServer.SendMessageToAllClients(messageBuffer);
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Send a header plus message buffer to a client asynchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of whether the message was posted to the
     * send queue.
     */
    bool SendMessageToClientAsync(
        const defs::connection_t& client, const defs::char_buffer_t& message, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage(message,
                                                               messageId,
                                                               responseAddress,
                                                               GetServerDetailsForClient(client),
                                                               m_messageBuilder);

            return m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Send a header plus message buffer to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of the send as a boolean.
     */
    bool
    SendMessageToClientSync(const defs::connection_t& client, const defs::char_buffer_t& message,
                            int32_t                   messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage(message,
                                                               messageId,
                                                               responseAddress,
                                                               GetServerDetailsForClient(client),
                                                               m_messageBuilder);

            return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
        }
        catch (...)
        {
            return false;
        }
    }
    /*!
     * \brief Send a header plus message buffer to all clients asynchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where a client should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of whether the message was posted to the
     * send queue.
     */
    bool
    SendMessageToAllClients(int32_t messageId, const defs::char_buffer_t& message,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer =
                messages::BuildMessage(message,
                                       messageId,
                                       responseAddress,
                                       GetServerDetailsForClient(defs::NULL_CONNECTION),
                                       m_messageBuilder);

            return m_tcpServer.SendMessageToAllClients(messageBuffer);
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Send a full message to a client asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     *            boost::serialization-compatible archive of type A.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of whether the message was posted to the
     * send queue.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToClientAsync(
        const T& message, const defs::connection_t& client, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer =
                messages::BuildMessage<T, A, MsgBldr>(message,
                                                      messageId,
                                                      responseAddress,
                                                      GetServerDetailsForClient(client),
                                                      m_messageBuilder);

            return m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Send a full message to a client synchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     *            boost::serialization-compatible archive of type A.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of the send as a boolean.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool
    SendMessageToClientSync(const T& message, const defs::connection_t& client, int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer =
                messages::BuildMessage<T, A, MsgBldr>(message,
                                                      messageId,
                                                      responseAddress,
                                                      GetServerDetailsForClient(client),
                                                      m_messageBuilder);

            return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
        }
        catch (...)
        {
            return false;
        }
    }
    /*!
     * \brief Send a full message to all clients asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     *            boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the clients should send a
     *            response, the default value will mean the response address will point to this
     * server socket. \return Returns the success state of whether the message was posted to the
     * send queue.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool
    SendMessageToAllClients(const T& message, int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage<T, A, MsgBldr>(
                message,
                messageId,
                responseAddress,
                GetServerDetailsForClient(defs::NULL_CONNECTION),
                m_messageBuilder);

            return m_tcpServer.SendMessageToAllClients(messageBuffer);
#
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Send a message buffer to a client asynchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \return Returns the success state of whether the message was posted to the send queue.
     */
    bool SendMessageToClientAsync(const defs::connection_t&  client,
                                  const defs::char_buffer_t& message) const
    {
        try
        {
            return m_tcpServer.SendMessageToClientAsync(client, message);
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Send a message buffer to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToClientSync(const defs::connection_t&  client,
                                 const defs::char_buffer_t& message) const
    {
        try
        {
            return m_tcpServer.SendMessageToClientSync(client, message);
        }
        catch (...)
        {
            return false;
        }
    }
    /*!
     * \brief Send a message buffer to all clients asynchronously.
     * \param[in] message - Message buffer.
     * \return Returns the success state of whether the message was posted to the send queue.
     */
    bool SendMessageToAllClients(const defs::char_buffer_t& message) const
    {
        try
        {
            return m_tcpServer.SendMessageToAllClients(message);
        }
        catch (...)
        {
            // Do nothing.
            return false;
        }
    }
    /*!
     * \brief Get number of unsent async messages.
     * \param[in] client - Target connection details.
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages(const defs::connection_t& client) const
    {
        return m_tcpServer.NumberOfUnsentAsyncMessages(client);
    }

    /*!
     * \brief Tells if a given client is currently connected to the server
     * \param[in] target - Target connection details.
     * \return true if connected, false if not
     */
    bool IsConnected(const defs::connection_t& client) const
    {
        return m_tcpServer.IsConnected(client);
    }

private:
    /*! \brief Send message mutex. */
    mutable std::mutex m_sendMutex;
    /*! \brief Referece to our message builder object. */
    const MsgBldr& m_messageBuilder;
    /*! \brief General purpose TCP server object. */
    TcpServer m_tcpServer;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT
