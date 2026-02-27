
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
    /*! \brief Deleted copy constructor. */
    SimpleTcpClient(const SimpleTcpClient&) = delete;
    /*! \brief Deleted copy assignment operator. */
    SimpleTcpClient& operator=(const SimpleTcpClient&) = delete;
    /*! \brief Deleted move constructor. */
    SimpleTcpClient(SimpleTcpClient&&) = delete;
    /*! \brief Deleted move assignment operator. */
    SimpleTcpClient& operator=(SimpleTcpClient&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] settings - structure containing connection options and behavioural settings.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to use process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleTcpClient(asio_compat::io_service_t& ioService, const defs::connection_t& server,
                    const defs::default_message_dispatcher_t& messageDispatcher,
                    SimpleTcpSettings const&                  settings = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] settings - structure containing connection options and behavioural settings.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommended.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to use process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleTcpClient(const defs::connection_t&                 server,
                    const defs::default_message_dispatcher_t& messageDispatcher,
                    SimpleTcpSettings const&                  settings = {});
    /*! \brief Default destructor. */
    ~SimpleTcpClient() = default;
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
     * \brief Reconnect the socket to a particular server target connection.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] settings - structure containing connection options and behavioural settings.
     */
    void Reconnect(defs::connection_t const& server, TcpConnSettings const& settings = {});
    /*!
     * \brief Send a header-only message to the server asynchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * This function is asynchronous so will return immediately
     * return
     *	false indicates failure
     * 	true  means the message has been passed in to the io service to be sent out later,
     * 	      but does not indicate that the message has been sent out successfully.
     * This method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::HGL_MSG_HDR object to
     * the server.
     */
    bool
    SendMessageToServerAsync(int32_t messageId,
                        const defs::connection_t& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header-only message to the server synchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method only sends a simple core_lib::asio::HGL_MSG_HDR
     * object to the server.
     */
    bool SendMessageToServerSync(int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header plus message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * This function is asynchronous so will return immediately
     * return
     *	false indicates failure
     * 	true  means the message has been passed in to the io service to be sent out later,
     * 	      but does not indicate that the message has been sent out successfully.
     * This method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::HGL_MSG_HDR object to
     * the server.
     */
    bool
    SendMessageToServerAsync(defs::char_buf_cspan_t message,
	                    int32_t messageId,
                        const defs::connection_t& responseAddress = defs::NULL_CONNECTION);

    /*!
     * \brief Send a header plus message buffer to the server synchronously.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(defs::char_buf_cspan_t message,
	                        int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION);

    /*!
     * \brief Send a full message to the server asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * This function is asynchronous so will return immediately
     * return
     *	false indicates failure
     * 	true  means the message has been passed in to the io service to be sent out later,
     * 	      but does not indicate that the message has been sent out successfully.
     * This method gives best performance when sending. Furthermore this method
     * uses the a core_lib::asio::HGL_MSG_HDR object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerAsync(const T& message,
	                         int32_t messageId,
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
     *
     * This method uses the a core_lib::asio::HGL_MSG_HDR object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerSync(const T& message,
	                        int32_t messageId,
                            const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        return m_tcpTypedClient.SendMessageToServerSync<T, A>(message, messageId, responseAddress);
    }
    /*!
     * \brief Send a message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
    bool SendMessageToServerAsync(defs::char_buf_cspan_t message);
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(defs::char_buf_cspan_t message);
    /*!
     * \brief Get number of unsent async messages.
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages() const;

private:
    /*! \brief Default message builder object of type asio::messages::MessageBuilder. */
    messages::MessageBuilder m_messageBuilder{};
    /*! \brief Default message handler object of type asio::messages::MessageHandler. */
    messages::MessageHandler m_messageHandler;
    /*! \brief Our actual typed TCP client object. */
    TcpTypedClient<messages::MessageBuilder> m_tcpTypedClient;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // SIMPLETCPCLIENT
