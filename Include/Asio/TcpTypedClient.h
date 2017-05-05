
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
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and
     * disptaching it accordingly.
     * \param[in] messageBuilder - A const reference to our persistent message builder object of
     * type MsgBldr.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    TcpTypedClient(boost_ioservice_t& ioService, const defs::connection_t& server,
                   const size_t                            minAmountToRead,
                   const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                   const defs::message_received_handler_t& messageReceivedHandler,
                   const MsgBldr&                          messageBuilder,
                   const eSendOption                       sendOption = eSendOption::nagleOn)
        : m_messageBuilder{messageBuilder}
        , m_tcpClient{ioService,
                      server,
                      minAmountToRead,
                      checkBytesLeftToRead,
                      messageReceivedHandler,
                      sendOption}
    {
    }
    /*!
     * \brief Initialisation constructor.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and
     * disptaching it accordingly.
     * \param[in] messageBuilder - A message builder object of type MsgBldr.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommened.
     */
    TcpTypedClient(const defs::connection_t& server, const size_t minAmountToRead,
                   const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                   const defs::message_received_handler_t& messageReceivedHandler,
                   const MsgBldr&                          messageBuilder,
                   const eSendOption                       sendOption = eSendOption::nagleOn)
        : m_messageBuilder{messageBuilder}
        , m_tcpClient{
              server, minAmountToRead, checkBytesLeftToRead, messageReceivedHandler, sendOption}
    {
    }
    /*! \brief Default destructor. */
    ~TcpTypedClient() = default;
    /*! \brief Copy constructor - deleted. */
    TcpTypedClient(const TcpTypedClient&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    TcpTypedClient& operator=(const TcpTypedClient&) = delete;
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
     * \return True if conneced, false otherwise.
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
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the server.
     */
    void SendMessageToServerAsync(const uint32_t            messageId,
                                  const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        auto messageBuffer = messages::BuildMessage(
            messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
        m_tcpClient.SendMessageToServerAsync(messageBuffer);
    }
    /*!
     * \brief Send a header-only message to the server synchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(const uint32_t            messageId,
                                 const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        try
        {
            auto messageBuffer = messages::BuildMessage(
                messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
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
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToServerAsync(const T& message, const uint32_t messageId,
                                  const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        auto messageBuffer = messages::BuildMessage<T, A, MsgBldr>(
            message, messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
        m_tcpClient.SendMessageToServerAsync(messageBuffer);
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
    bool SendMessageToServerSync(const T& message, const uint32_t messageId,
                                 const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        try
        {
            auto messageBuffer = messages::BuildMessage<T, A, MsgBldr>(
                message, messageId, responseAddress, GetClientDetailsForServer(), m_messageBuilder);
            return m_tcpClient.SendMessageToServerSync(messageBuffer);
        }
        catch (...)
        {
            return false;
        }
    }

private:
    /*! \brief Referece to our message builder object. */
    const MsgBldr& m_messageBuilder;
    /*! \brief General purpose TCP client object. */
    TcpClient m_tcpClient;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT
