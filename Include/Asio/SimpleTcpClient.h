
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
 * \file SimpleTcpClient.h
 * \brief File containing the simple TCP client class declaration.
 */

#ifndef SIMPLETCPCLIENT
#define SIMPLETCPCLIENT

#include "TcpTypedClient.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{

/*! \brief A simple bi-directional TCP client, which uses the class MessageHeader as the message
 * header type. */
class CORE_LIBRARY_DLL_SHARED_API SimpleTcpClient final
{
public:
    /*! \brief Default constructor - deleted. */
    SimpleTcpClient() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     * \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of unsent async messages.
     * \param[in] memPoolMsgCount - Number of messages (per client) in pool for received message
     *                              handling, defaults to 0, which implies no pool used.
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
    SimpleTcpClient(boost_iocontext_t& ioContext, const defs::connection_t& server,
                    const defs::default_message_dispatcher_t& messageDispatcher,
                    eSendOption                               sendOption = eSendOption::nagleOn,
                    size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT,
                    size_t memPoolMsgCount               = 0);
    /*!
     * \brief Initialisation constructor.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     * \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of unsent async messages.
     * \param[in] memPoolMsgCount - Number of messages (per client) in pool for received message
     *                              handling, defaults to 0, which implies no pool used.
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
    SimpleTcpClient(const defs::connection_t&                 server,
                    const defs::default_message_dispatcher_t& messageDispatcher,
                    eSendOption                               sendOption = eSendOption::nagleOn,
                    size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT,
                    size_t memPoolMsgCount               = 0);
    /*! \brief Default destructor. */
    ~SimpleTcpClient() = default;
    /*! \brief Copy constructor - deleted. */
    SimpleTcpClient(const SimpleTcpClient&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    SimpleTcpClient& operator=(const SimpleTcpClient&) = delete;
    /*! \brief Move constructor - deleted. */
    SimpleTcpClient(SimpleTcpClient&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    SimpleTcpClient& operator=(SimpleTcpClient&&) = delete;
    /*!
     * \brief Retrieve server connection details.
     * \return - Connection object describing target server's address and port.
     */
    defs::connection_t ServerConnection() const;
    /*!
     * \brief Check if the client is connected to the server.
     * \return True if connected, false otherwise.
     */
    bool Connected() const;
    /*!
     * \brief Retrieve this client's connection details
     * \return Connection object describing this client's address and port.
     */
    defs::connection_t GetClientDetailsForServer();
    /*! \brief Manually close the connection.
     *
     * Note that this object uses RAII so will close the connection when destroyed.
     */
    void CloseConnection();
    /*!
     * \brief Send a header-only message to the server asynchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of whether the message was posted to the send queue.
     */
    bool
    SendMessageToServerAsync(int32_t                   messageId,
                             const defs::connection_t& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header-only message to the server synchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(int32_t                   messageId,
                                 const defs::connection_t& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header plus message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of whether the message was posted to the send queue.
     */
    bool
    SendMessageToServerAsync(const defs::char_buffer_t& message, int32_t messageId,
                             const defs::connection_t& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header plus message buffer to the server synchronously.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(const defs::char_buffer_t& message, int32_t messageId,
                                 const defs::connection_t& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a full message to the server asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of whether the message was posted to the send queue.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerAsync(const T& message, int32_t messageId,
                                  const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        return m_tcpTypedClient.SendMessageToServerAsync<T, A>(message, messageId, responseAddress);
    }
    /*!
     * \brief Send a full message to the server synchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerSync(const T& message, int32_t messageId,
                                 const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        return m_tcpTypedClient.SendMessageToServerSync<T, A>(message, messageId, responseAddress);
    }
    /*!
     * \brief Send a message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
     * \return Returns the success state of whether the message was posted to the send queue.
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
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages() const;

private:
    /*! \brief Default message builder object of type core_lib::asio::messages::MessageBuilder. */
    messages::MessageBuilder m_messageBuilder{};
    /*! \brief Default message handler object of type core_lib::asio::messages::MessageHandler. */
    messages::MessageHandler m_messageHandler{};
    /*! \brief Our actual typed TCP client object. */
    TcpTypedClient<messages::MessageBuilder> m_tcpTypedClient;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // SIMPLETCPCLIENT
