
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
 * \file TcpTypedClient.h
 * \brief File containing TCP typed client class declaration.
 */

#ifndef TCPTYPEDCLIENT
#define TCPTYPEDCLIENT

#include <mutex>
#include "TcpClient.h"
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
 * \brief A generic bi-directional TCP client.
 *
 * The template argument defines a message builder object that
 * must have an interface compatible with that of the class
 * core_lib::asio::messages::MessageBuilder.
 *
 * This class forms the underpinnings of the SimpleTcpClient class.
 *
 * This is also the class to be used when the user wants to specify their own message builder type
 * and message header type.
 */
template <typename MsgBldr> class TcpTypedClient final
{
public:
    /*! \brief Default constructor - deleted. */
    TcpTypedClient() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and
     * dispatching it accordingly.
     * \param[in] messageBuilder - A const reference to our persistent message builder object of
     * type MsgBldr.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
	 * \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of unsent async messages.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     */
    TcpTypedClient(boost_iocontext_t& ioContext, const defs::connection_t& server,
                   size_t                                  minAmountToRead,
                   const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                   const defs::message_received_handler_t& messageReceivedHandler,
                   const MsgBldr& messageBuilder, eSendOption sendOption = eSendOption::nagleOn,
				   size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT)
        : m_messageBuilder{messageBuilder}
        , m_tcpClient{ioContext,
                      server,
                      minAmountToRead,
                      checkBytesLeftToRead,
                      messageReceivedHandler,
                      sendOption,
					  maxAllowedUnsentAsyncMessages}
    {
    }
    /*!
     * \brief Initialisation constructor.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message and
     * dispatching it accordingly.
     * \param[in] messageBuilder - A message builder object of type MsgBldr.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
	 * \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of unsent async messages.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommended.
     */
    TcpTypedClient(const defs::connection_t& server, size_t minAmountToRead,
                   const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                   const defs::message_received_handler_t& messageReceivedHandler,
                   const MsgBldr& messageBuilder, eSendOption sendOption = eSendOption::nagleOn,
				   size_t maxAllowedUnsentAsyncMessages = MAX_UNSENT_ASYNC_MSG_COUNT)
        : m_messageBuilder{messageBuilder}
        , m_tcpClient{
              server, minAmountToRead, checkBytesLeftToRead, messageReceivedHandler, 
			  sendOption, maxAllowedUnsentAsyncMessages}
    {
    }
    /*! \brief Default destructor. */
    ~TcpTypedClient() = default;
    /*! \brief Copy constructor - deleted. */
    TcpTypedClient(const TcpTypedClient&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    TcpTypedClient& operator=(const TcpTypedClient&) = delete;
    /*! \brief Move constructor - deleted. */
    TcpTypedClient(TcpTypedClient&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    TcpTypedClient& operator=(TcpTypedClient&&) = delete;
    /*!
     * \brief Retrieve server connection details.
     * \return - Connection object describing target server's address and port.
     */
    defs::connection_t ServerConnection() const
    {
        return m_tcpClient.ServerConnection();
    }
    /*!
     * \brief Check if the client is connected to the server.
     * \return True if connected, false otherwise.
     */
    bool Connected() const
    {
        return m_tcpClient.Connected();
    }
    /*!
     * \brief Retrieve this client's connection details
     * \return Connection object describing this client's address and port.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
    defs::connection_t GetClientDetailsForServer()
    {
        m_tcpClient.CheckAndCreateConnection();
        return m_tcpClient.GetClientDetailsForServer();
    }
    /*! \brief Manually close the connection.
     *
     * Note that this object uses RAII so will close the connection when destroyed.
     */
    void CloseConnection()
    {
        m_tcpClient.CloseConnection();
    }
    /*!
     * \brief Send a header-only message to the server asynchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the server.
     */
    bool SendMessageToServerAsync(int32_t                   messageId,
                                  const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);
		
		try
		{
			auto const& messageBuffer = messages::BuildMessage(
				messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
				
			return m_tcpClient.SendMessageToServerAsync(messageBuffer);
		}
		catch(...)
		{
		    // Do nothing.
			return false;
		}
    }
    /*!
     * \brief Send a header-only message to the server synchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(int32_t                   messageId,
                                 const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage(
                messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
            return m_tcpClient.SendMessageToServerSync(messageBuffer);
        }
        catch (...)
        {
            return false;
        }
    }
    /*!
     * \brief Send a header plus message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the server.
     */
    bool SendMessageToServerAsync(const defs::char_buffer_t& message, int32_t messageId,
                                  const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

		try
		{
			auto const& messageBuffer = messages::BuildMessage(
				message, messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
			
			m_tcpClient.SendMessageToServerAsync(messageBuffer);
			return true;
		}
		catch(...)
		{
		    // Do nothing.
			return false;
		}
    }
    /*!
     * \brief Send a header plus message buffer to the server synchronously.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(const defs::char_buffer_t& message, int32_t messageId,
                                 const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage(
                message, messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
            return m_tcpClient.SendMessageToServerSync(messageBuffer);
        }
        catch (...)
        {
            return false;
        }
    }
    /*!
     * \brief Send a full message to the server asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerAsync(const T& message, int32_t messageId,
                                  const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);

		try
		{
			auto const& messageBuffer = messages::BuildMessage<T, A, MsgBldr>(
				message, messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
				
			m_tcpClient.SendMessageToServerAsync(messageBuffer);
			return true;
		}
		catch(...)
		{
		    // Do nothing.
			return false;
		}
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
        std::lock_guard<std::mutex> lock(m_sendMutex);

        try
        {
            auto const& messageBuffer = messages::BuildMessage<T, A, MsgBldr>(
                message, messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
            return m_tcpClient.SendMessageToServerSync(messageBuffer);
        }
        catch (...)
        {
            return false;
        }
    }
    /*!
     * \brief Send a message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
	 * \return Returns the success state of whether the message was posted to the send queue.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
    bool SendMessageToServerAsync(const defs::char_buffer_t& message)
    {
		try
		{
			m_tcpClient.SendMessageToServerAsync(message);
			return true;
		}
		catch(...)
		{
			// Do nothing.
			return false;
		}
    }
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(const defs::char_buffer_t& message)
    {
		try
		{
			return m_tcpClient.SendMessageToServerSync(message);
		}
		catch(...)
		{
		    return false;
		}
    }
	/*!
     * \brief Get number of unsent async messages.
     * \return Number of pending queued async messages
     */
    size_t NumberOfUnsentAsyncMessages() const
    {
        return m_tcpClient.NumberOfUnsentAsyncMessages();
    }

private:
    /*! \brief Send message mutex. */
    mutable std::mutex m_sendMutex;
    /*! \brief Referece to our message builder object. */
    const MsgBldr& m_messageBuilder;
    /*! \brief General purpose TCP client object. */
    TcpClient m_tcpClient;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT
