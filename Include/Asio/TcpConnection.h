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

class TcpConnections;

class CORE_LIBRARY_DLL_SHARED_API TcpConnection final
	: public std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(boost_ioservice_t& ioService
				  , TcpConnections& connections
				  , const size_t minAmountToRead
				  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				  , const defs::message_received_handler_t& messageReceivedHandler
				  , const eSendOption sendOption = eSendOption::nagleOn);

	TcpConnection(const TcpConnection& ) = delete;

	TcpConnection& operator=(const TcpConnection& ) = delete;

	virtual ~TcpConnection() = default;

	boost_tcp_t::socket& Socket();

	const boost_tcp_t::socket& Socket() const;

	void Connect(const defs::connection_t& endPoint);

	void CloseConnection();

	void StartAsyncRead();

	void SendMessageAsync(const defs::char_buffer_t& message);

	bool SendMessageSync(const defs::char_buffer_t& message);

private:
	mutable std::mutex m_mutex;
	threads::SyncEvent m_closedEvent;
	threads::SyncEvent m_sendEvent;
	bool m_closing{false};
	boost_ioservice_t& m_ioService;
	boost_ioservice_t::strand m_strand;
	boost_tcp_t::socket m_socket;
	TcpConnections& m_connections;
	const size_t m_minAmountToRead{0};
	defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
	defs::message_received_handler_t m_messageReceivedHandler;
	const eSendOption m_sendOption{eSendOption::nagleOn};
	defs::char_buffer_t m_receiveBuffer;
	defs::char_buffer_t m_messageBuffer;
	bool m_sendSuccess{false};

	void SetClosing(const bool closing);

	bool IsClosing() const;

	void ProcessCloseSocket();

	void DestroySelf();

	void AsyncReadFromSocket(const size_t amountToRead);

	void ReadComplete(const boost_sys::error_code& error
					  , const size_t bytesReceived
					  , const size_t bytesExpected);

	void AsyncWriteToSocket(defs::char_buffer_t message
							, const bool setSuccessFlag);

	void SyncWriteToSocket(const defs::char_buffer_t& message
						   , const bool setSuccessFlag);

	void WriteComplete(const boost_sys::error_code& error
					   , const std::size_t bytesSent
					   , const bool setSuccessFlag);
};


} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTION
