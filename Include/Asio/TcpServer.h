
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
 * \file TcpServer.h
 * \brief File containing TCP server class declaration.
 */

#ifndef TCPSERVER
#define TCPSERVER

#include "IoServiceThreadGroup.h"
#include "TcpConnections.h"
#include "Threads/SyncEvent.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {
/*! \brief A bi-directional TCP server. */
class CORE_LIBRARY_DLL_SHARED_API TcpServer final
{
public:
    /*! \brief Default constructor - deleted. */
    TcpServer() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and disptaching it accordingly.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
	TcpServer(boost_ioservice_t& ioService
			  , const uint16_t listenPort
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and disptaching it accordingly.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommened.
     */
	TcpServer(const uint16_t listenPort
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);
    /*! \brief Default destructor. */
	~TcpServer();
    /*! \brief Copy constructor - deleted. */
	TcpServer(const TcpServer& ) = delete;
    /*! \brief Copy assignment operator - deleted. */
	TcpServer& operator=(const TcpServer& ) = delete;
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
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
	void SendMessageToClientAsync(const defs::connection_t& client
                                  , const defs::char_buffer_t& message) const;
    /*!
     * \brief Send a message buffer to a client synchronously.
     * \param[in] client - Client connection details.
     * \param[in] message - Message buffer.
     */
	bool SendMessageToClientSync(const defs::connection_t& client
                                 , const defs::char_buffer_t& message) const;
    /*!
     * \brief Send a message buffer to all clients asynchronously.
     * \param[in] message - Message buffer.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
    void SendMessageToAllClients(const defs::char_buffer_t& message) const;

private:
    /*! \brief Accept a connection. */
    void AcceptConnection();
    /*!
     * \brief Accept handler for new connections.
     * \param[in] connection - Client connection.
     * \param[in] error - An error code if fault occurred.
     */
    void AcceptHandler(defs::tcp_conn_ptr_t connection
                       , const boost_sys::error_code& error);
    /*! \brief Process closing the acceptor. */
    void ProcessCloseAcceptor();

private:
    /*! \brief I/O service thread group. */
	std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service reference. */
	boost_ioservice_t& m_ioService;
    /*! \brief The connection acceptor. */
	std::unique_ptr<boost_tcp_acceptor_t> m_acceptor;
    /*! \brief Server listen port. */
	const uint16_t m_listenPort{0};
    /*! \brief Minimum amount to read from socket. */
	const size_t m_minAmountToRead{0};
    /*! \brief Callback to check number of bytes left to read. */
	defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Callback to handle received message. */
	defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Socket receive option. */
	const eSendOption m_sendOption{eSendOption::nagleOn};
    /*! \brief TCP connections object. */
	TcpConnections m_clientConnections;
    /*! \brief Close event. */
	threads::SyncEvent m_closedEvent;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPSERVER
