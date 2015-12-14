
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014,2015 Duncan Crutchley
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
 * \file TcpClient.h
 * \brief File containing TCP client class declaration.
 */

#ifndef TCPCLIENT
#define TCPCLIENT

#include "IoServiceThreadGroup.h"
#include "TcpConnections.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

/*! \brief A bi-directional TCP client. */
class CORE_LIBRARY_DLL_SHARED_API TcpClient final
{
public:
    /*! \brief Default constructor - deleted. */
    TcpClient() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] server - Connection object describing target server's address and port.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and disptaching it accordingly.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asioIoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
	TcpClient(boost_ioservice_t& ioService
			  , const defs::connection_t& server
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);
    /*!
     * \brief Initialisation constructor.
     * \param[in] server - Connection object describing target server's address and port.
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
	TcpClient(const defs::connection_t& server
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);
    /*! \brief Default destructor. */
	~TcpClient();
    /*! \brief Copy constructor - deleted. */
	TcpClient(const TcpClient& ) = delete;
    /*! \brief Copy assignment operator - deleted. */
	TcpClient& operator=(const TcpClient& ) = delete;
    /*!
     * \brief Retrieve server connection details.
     * \return - Connection object describing target server's address and port.
     */
	defs::connection_t ServerConnection() const;
    /*!
     * \brief Retrieve this client's connection details
     * \return Connection object describing this client's address and port.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
	defs::connection_t GetClientDetailsForServer() const;
    /*! \brief Manually close the connection.
     *
     * Note that this object uses RAII so will close the connection when destroyed.
     */
	void CloseConnection();
    /*!
     * \brief Send a message buffer to the server asynchronously.
     * \param[in] message - Message buffer.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
	void SendMessageToServerAsync(const defs::char_buffer_t& message);
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
	bool SendMessageToServerSync(const defs::char_buffer_t& message);

private:
    /*! \brief I/O service thread group. */
	std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service reference. */
	boost_ioservice_t& m_ioService;
    /*! \brief Server connection details. */
	const defs::connection_t m_server;
    /*! \brief Minimum amount to read from socket. */
	const size_t m_minAmountToRead{0};
    /*! \brief Callback to check number of bytes left to read. */
	defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Callback to handle received message. */
	defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Socket send option. */
	const eSendOption m_sendOption{eSendOption::nagleOn};
    /*! \brief TCP connections object. */
	TcpConnections m_serverConnection;
    /*! \brief Create conenction to server. */
	void CreateConnection();
    /*! \brief Check connection and create if required. */
	void CheckAndCreateConnection();
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCLIENT
