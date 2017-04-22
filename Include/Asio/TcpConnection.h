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
 * \file TcpConnection.h
 * \brief File containing TCP connection class declaration.
 */

#ifndef TCPCONNECTION
#define TCPCONNECTION

#include "AsioDefines.h"
#include "Threads/SyncEvent.h"
#include <mutex>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

/*! \brief TForward declaration of TCP connections class. */
class TcpConnections;

/*! \brief TCP connection class. */
class CORE_LIBRARY_DLL_SHARED_API TcpConnection final
	: public std::enable_shared_from_this<TcpConnection>
{
public:
    /*! \brief Default constructor - deleted. */
    TcpConnection() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - Reference to I/O service.
     * \param[in] connections - Reference to TCP connections object.
     * \param[in] minAmountToRead - Minimum amount to read.
     * \param[in] checkBytesLeftToRead - Check bytes left to read callback.
     * \param[in] messageReceivedHandler - Message received handler callback.
     * \param[in] sendOption - Socket send option.
     */
	TcpConnection(boost_ioservice_t& ioService
				  , TcpConnections& connections
				  , const size_t minAmountToRead
				  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				  , const defs::message_received_handler_t& messageReceivedHandler
				  , const eSendOption sendOption = eSendOption::nagleOn);
    /*! \brief Copy constructor deleted. */
	TcpConnection(const TcpConnection& ) = delete;
    /*! \brief Copy assignment operator - deleted. */
	TcpConnection& operator=(const TcpConnection& ) = delete;
    /*! \brief Default virtual destructor. */
	virtual ~TcpConnection() = default;
    /*!
     * \brief Get mutable reference to the socket object.
     * \return Socket reference.
     */
	boost_tcp_t::socket& Socket();
    /*!
     * \brief Get a const reference to the socket object.
     * \return Const socket reference.
     */
    const boost_tcp_t::socket& Socket() const;
    /*!
     * \brief Connect to an endpoint.
     * \param[in] endPoint - Connection details of some endpoint.
     */
	void Connect(const defs::connection_t& endPoint);
    /*! \brief Close this connection. */
	void CloseConnection();
    /*! \brief Start aysnchronously reading data when available. */
	void StartAsyncRead();
    /*!
     * \brief Send an asynchronous message.
     * \param[in] message - Message buffer to send.
     */
	void SendMessageAsync(const defs::char_buffer_t& message);
    /*!
     * \brief Send a synchronous message.
     * \param[in] message - Message buffer to send.
     * \return True if sent successfuilly, false otherwise.
     */
	bool SendMessageSync(const defs::char_buffer_t& message);

private:
    /*!
     * \brief Set closing flag.
     * \param[in] closing - Closing state flag.
     */
    void SetClosing(const bool closing);
    /*!
     * \brief Is the connection closing?
     * \return True if closing, false otherwise.
     */
    bool IsClosing() const;
    /*! \brief Process closing of socket. */
    void ProcessCloseSocket();
    /*! \brief Self destruct this object. */
    void DestroySelf();
    /*!
     * \brief Asyncrhonously read an amount.
     * \param[in] amountToRead - Number of bytes to read from socket.
     */
    void AsyncReadFromSocket(const size_t amountToRead);
    /*!
     * \brief Read completion handler.
     * \param[in] error - Error flag if fault occurred.
     * \param[in] bytesReceived - Number of bytes received.
     * \param[in] bytesExpected - Number of bytes expected.
     */
    void ReadComplete(const boost_sys::error_code& error
                      , const size_t bytesReceived
                      , const size_t bytesExpected);
    /*!
     * \brief Asynchrounously write to the socket.
     * \param[in] message - Message buffer to write.
     * \param[in] setSuccessFlag - control setting of success flag.
     */
    void AsyncWriteToSocket(defs::char_buffer_t message
                            , const bool setSuccessFlag);
    /*!
     * \brief Synchrounously write to the socket.
     * \param[in] message - Message buffer to write.
     * \param[in] setSuccessFlag - control setting of success flag.
     */
    void SyncWriteToSocket(const defs::char_buffer_t& message
                           , const bool setSuccessFlag);
    /*!
     * \brief Write completion handler.
     * \param[in] error - Error flag if fault occurred.
     * \param[in] bytesSent - Number of bytes sent.
     * \param[in] setSuccessFlag - control setting of success flag.
     */
    void WriteComplete(const boost_sys::error_code& error
                       , const std::size_t bytesSent
                       , const bool setSuccessFlag);

private:
    /*! \brief Access mutex for thread safety. */
	mutable std::mutex m_mutex;
    /*! \brief Connection close event. */
	threads::SyncEvent m_closedEvent;
    /*! \brief Connection send event. */
	threads::SyncEvent m_sendEvent;
    /*! \brief Closing connection flag. */
	bool m_closing{false};
    /*! \brief I/O serviceobject reference. */
	boost_ioservice_t& m_ioService;
    /*! \brief I/O service strand. */
	boost_ioservice_t::strand m_strand;
    /*! \brief TCP socket. */
	boost_tcp_t::socket m_socket;
    /*! \brief Reference to TCP connections object. */
	TcpConnections& m_connections;
    /*! \brief Minimum amount to read from socket. */
	const size_t m_minAmountToRead{0};
    /*! \brief Check bytes left to read callback. */
	defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Message received handler callback. */
	defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Socket send option. */
	const eSendOption m_sendOption{eSendOption::nagleOn};
    /*! \brief Socket receive buffer. */
	defs::char_buffer_t m_receiveBuffer;
    /*! \brief Message buffer. */
	defs::char_buffer_t m_messageBuffer;
    /*! \brief Send message success flag. */
	bool m_sendSuccess{false};
};


} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTION
