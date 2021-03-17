
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
 * \file SimpleTcpServer.h
 * \brief File containing the simple TCP server class declaration.
 */

#ifndef SIMPLETCPSERVER
#define SIMPLETCPSERVER

#include "TcpTypedServer.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{
/*! \brief A simple bi-directional multi-client TCP server, which uses the class MessageHeader as
 * the message header type. */
class CORE_LIBRARY_DLL_SHARED_API SimpleTcpServer final
{
public:
    /*! \brief Default constructor - deleted. */
    SimpleTcpServer() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
	 * \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of unsent async messages.
	 * \param[in] memPoolMsgCount - Number of messages in pool for received message handling,
     *                              defaults to 0, which implies no pool used.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asioIoServoceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO context. This is the recommended constructor.
	 *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleTcpServer(boost_iocontext_t& ioContext, uint16_t listenPort,
                    const defs::default_message_dispatcher_t& messageDispatcher,
                    eSendOption                               sendOption = eSendOption::nagleOn,
					size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT,
					size_t memPoolMsgCount = 0);
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
	 * \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of unsent async messages.
	 * \param[in] memPoolMsgCount - Number of messages in pool for received message handling,
     *                              defaults to 0, which implies no pool used.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommend.
	 *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleTcpServer(uint16_t                                  listenPort,
                    const defs::default_message_dispatcher_t& messageDispatcher,
                    eSendOption                               sendOption = eSendOption::nagleOn,
					size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT,
					size_t memPoolMsgCount = 0);
    /*! \brief Default destructor. */
    ~SimpleTcpServer() = default;
    /*! \brief Copy constructor - deleted. */
    SimpleTcpServer(const SimpleTcpServer&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    SimpleTcpServer& operator=(const SimpleTcpServer&) = delete;
    /*! \brief Move constructor - deleted. */
    SimpleTcpServer(SimpleTcpServer&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    SimpleTcpServer& operator=(SimpleTcpServer&&) = delete;
    /*!
     * \brief Retrieve this server's connection details for a given client.
     * \param[in] client - A client's connection details.
     * \return - Connection object describing target server's address and port.
     *
     * If no such client is known to the server then it returns { "0.0.0.0", listenPort}.
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
     * \brief Send a header-only message to a client asynchronously.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     * response, the default value will mean the response address will point to this server socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the client.
     */
    bool SendMessageToClientAsync(
        const defs::connection_t& client, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;
    /*!
     * \brief Send a header-only message to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     * response, the default value will mean the response address will point to this server socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method only sends a simple core_lib::asio::defs::MessageHeader
     * object to the client.
     */
    bool SendMessageToClientSync(
        const defs::connection_t& client, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;
    /*!
     * \brief Send a header-only message to all clients asynchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where a client should send a
     * response, the default value will mean the response address will point to this server socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the clients.
     */
    bool SendMessageToAllClients(
        int32_t messageId, const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;
    /*!
     * \brief Send a header plus message buffer to a client asynchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     * response, the default value will mean the response address will point to this server socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the client.
     */
    bool SendMessageToClientAsync(
        const defs::connection_t& client, const defs::char_buffer_t& message, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;
    /*!
     * \brief Send a header plus message buffer to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     * response, the default value will mean the response address will point to this server socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method only sends a simple core_lib::asio::defs::MessageHeader
     * object to the client.
     */
    bool SendMessageToClientSync(
        const defs::connection_t& client, const defs::char_buffer_t& message, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;
    /*!
     * \brief Send a header plus message buffer to all clients asynchronously.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where a client should send a
     * response, the default value will mean the response address will point to this server socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the clients.
     */
    bool SendMessageToAllClients(
        const defs::char_buffer_t& message, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const;
    /*!
     * \brief Send a full message to a client asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     * response, the default value will mean the response address will point to this server socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * uses the a core_lib::asio::defs::MessageHeader object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToClientAsync(
        const T& message, const defs::connection_t& client, int32_t messageId,
        const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        return m_tcpTypedServer.SendMessageToClientAsync<T, A>(
            message, client, messageId, responseAddress);
    }
    /*!
     * \brief Send a full message to a client synchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a
     * response, the default value will mean the response address will point to this server socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method uses the a core_lib::asio::defs::MessageHeader object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool
    SendMessageToClientSync(const T& message, const defs::connection_t& client, int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        return m_tcpTypedServer.SendMessageToClientSync<T, A>(
            message, client, messageId, responseAddress);
    }
    /*!
     * \brief Send a full message to all clients asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the clients should send a
     * response, the default value will mean the response address will point to this server socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * uses the a core_lib::asio::defs::MessageHeader object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool
    SendMessageToAllClients(const T& message, int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
    {
        return m_tcpTypedServer.SendMessageToAllClients<T, A>(message, messageId, responseAddress);
    }
    /*!
     * \brief Send a message buffer to a client asynchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless a an exception is thrown. This
     * method gives best performance when sending.
     */
    bool SendMessageToClientAsync(const defs::connection_t&  client,
                                  const defs::char_buffer_t& message) const;
    /*!
     * \brief Send a message buffer to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     */
    bool SendMessageToClientSync(const defs::connection_t&  client,
                                 const defs::char_buffer_t& message) const;
    /*!
     * \brief Send a message buffer to all clients asynchronously.
     * \param[in] message - Message buffer.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless a an exception is thrown. This
     * method gives best performance when sending.
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
    /*! \brief Default message builder object of type core_lib::asio::messages::MessageBuilder. */
    messages::MessageBuilder m_messageBuilder{};
    /*! \brief Default message handler object of type core_lib::asio::messages::MessageHandler. */
    messages::MessageHandler m_messageHandler{};
    /*! \brief Our actual typed TCP server object. */
    TcpTypedServer<messages::MessageBuilder> m_tcpTypedServer;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // SIMPLETCPSERVER
