
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2015 Duncan Crutchley
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

#include "Threads/SyncEvent.h"
#include "AsioDefines.h"
#include "IoServiceThreadGroup.h"
#include "TcpConnections.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class TcpServer final
{
public:
	TcpServer(boost_ioservice_t& ioService
			  , const uint16_t listenPort
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);

	TcpServer(const uint16_t listenPort
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);

	~TcpServer();

	TcpServer(const TcpServer& ) = delete;

	TcpServer& operator=(const TcpServer& ) = delete;

	// Throws xUnknownConnectionError is remoteEnd is not valid.
	defs::connection_t GetServerDetailsForClient(const defs::connection_t& client) const;

	uint16_t ListenPort() const;

	void CloseAcceptor();

	void OpenAcceptor();

	void SendMessageToClientAsync(const defs::connection_t& client
                                  , const defs::char_buffer_t& message) const;

	bool SendMessageToClientSync(const defs::connection_t& client
                                 , const defs::char_buffer_t& message) const;

    void SendMessageToAllClients(const defs::char_buffer_t& message) const;

private:
	std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
	boost_ioservice_t& m_ioService;
	std::unique_ptr<boost_tcp_acceptor_t> m_acceptor;
	const uint16_t m_listenPort{0};
	const size_t m_minAmountToRead{0};
	defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
	defs::message_received_handler_t m_messageReceivedHandler;
	const eSendOption m_sendOption{eSendOption::nagleOn};
	TcpConnections m_clientConnections;
	threads::SyncEvent m_closedEvent;

	void AcceptConnection();

	void AcceptHandler(defs::tcp_conn_ptr_t connection
					   , const boost_sys::error_code& error);

	void ProcessCloseAcceptor();
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPSERVER
