
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <duncan.crutchley+corelibrary@gmail.com>
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

#include "TcpServer.h"
#include "MessageUtils.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {
/*!
 * \brief A generic bi-directional TCP server.
 *
 * The template argument defines a message builder object that
 * must have an interface compatible with that of the class
 * core_lib::asio::messages::MessageBuilder.
 */
template<typename MsgBldr>
class TcpTypedServer final
{
public:
    /*! \brief Default constructor - deleted. */
    TcpTypedServer() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and disptaching it accordingly.
     * \param[in] messageBuilder - A const reference to our persistent message builder object of type MsgBldr.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
	TcpTypedServer(boost_ioservice_t& ioService
				   , const uint16_t listenPort
				   , const size_t minAmountToRead
				   , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				   , const defs::message_received_handler_t& messageReceivedHandler
				   , const MsgBldr& messageBuilder
				   , const eSendOption sendOption = eSendOption::nagleOn)
		: m_messageBuilder{messageBuilder}
		, m_tcpServer{ioService, listenPort, minAmountToRead
					  , checkBytesLeftToRead, messageReceivedHandler
					  , sendOption}
	{
	}
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and disptaching it accordingly.
     * \param[in] messageBuilder - A const reference to our persistent message builder object of type MsgBldr.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommened.
     */
	TcpTypedServer(const uint16_t listenPort
				   , const size_t minAmountToRead
				   , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				   , const defs::message_received_handler_t& messageReceivedHandler
				   , const MsgBldr& messageBuilder
				   , const eSendOption sendOption = eSendOption::nagleOn)
		: m_messageBuilder{messageBuilder}
		, m_tcpServer{listenPort, minAmountToRead
					  , checkBytesLeftToRead, messageReceivedHandler
					  , sendOption}
	{
	}
    /*! \brief Default destructor. */
	~TcpTypedServer() = default;
    /*! \brief Copy constructor - deleted. */
	TcpTypedServer(const TcpTypedServer& ) = delete;
    /*! \brief Copy assignment operator - deleted. */
	TcpTypedServer& operator=(const TcpTypedServer& ) = delete;
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
     * \param[in] responseAddress - (Optional) The address and port where the client should send a response, the default value will mean the response address will point to this server socket.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
	void SendMessageToClientAsync(const defs::connection_t& client, const uint32_t messageId
                                  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        auto messageBuffer = messages::BuildMessage(messageId, responseAddress
                                                    , GetServerDetailsForClient(client)
                                                    , m_messageBuilder);
		m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
	}
    /*!
     * \brief Send a header-only message to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a response, the default value will mean the response address will point to this server socket.
     * \return Returns the success state of the send as a boolean.
     */
	bool SendMessageToClientSync(const defs::connection_t& client, const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        auto messageBuffer = messages::BuildMessage(messageId, responseAddress
                                                    , GetServerDetailsForClient(client)
                                                    , m_messageBuilder);
		return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
	}
    /*!
     * \brief Send a header-only message to all clients asynchronously.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where a client should send a response, the default value will mean the response address will point to this server socket.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
	void SendMessageToAllClients(const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        auto messageBuffer = messages::BuildMessage(messageId, responseAddress
                                                    , GetServerDetailsForClient(defs::NULL_CONNECTION)
                                                    , m_messageBuilder);
		m_tcpServer.SendMessageToAllClients(messageBuffer);
	}
    /*!
     * \brief Send a full message to a client asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an boost::serialization-compatible archive of type A.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a response, the default value will mean the response address will point to this server socket.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown.
     */
    template<typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToClientAsync(const T& message
                                  , const defs::connection_t& client, const uint32_t messageId
                                  , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        auto messageBuffer = messages::BuildMessage<T, A, MsgBldr>(message, messageId, responseAddress
                                                , GetServerDetailsForClient(client)
                                                , m_messageBuilder);
		m_tcpServer.SendMessageToClientAsync(client, messageBuffer);
	}
    /*!
     * \brief Send a full message to a client synchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an boost::serialization-compatible archive of type A.
     * \param[in] client - Client connection details.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the client should send a response, the default value will mean the response address will point to this server socket.
     * \return Returns the success state of the send as a boolean.
     */
    template<typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToClientSync(const T& message
                                 , const defs::connection_t& client, const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        auto messageBuffer = messages::BuildMessage<T, A, MsgBldr>(message, messageId, responseAddress
                                                          , GetServerDetailsForClient(client)
                                                          , m_messageBuilder);
		return m_tcpServer.SendMessageToClientSync(client, messageBuffer);
	}
    /*!
     * \brief Send a full message to all clients asynchronously.
     * \param[in] message - The message of type T to send behind the header serialized to an boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the clients should send a response, the default value will mean the response address will point to this server socket.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown.
     */
    template<typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToAllClients(const T& message
                                 , const uint32_t messageId
                                 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION) const
	{
        auto messageBuffer = messages::BuildMessage<T, A, MsgBldr>(message, messageId, responseAddress
                                                          , GetServerDetailsForClient(defs::NULL_CONNECTION)
                                                          , m_messageBuilder);
		m_tcpServer.SendMessageToAllClients(messageBuffer);
	}

private:
    /*! \brief Referece to our message builder object. */
	const MsgBldr& m_messageBuilder;
    /*! \brief General purpose TCP server object. */
	TcpServer m_tcpServer;
};


} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT
