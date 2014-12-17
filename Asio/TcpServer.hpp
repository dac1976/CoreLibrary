
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
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
 * \file TcpServer.hpp
 * \brief File containing TCP server class declaration.
 */

#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "../SyncEvent.hpp"
#include "AsioDefines.hpp"
#include "IoServiceThreadGroup.hpp"
#include "TcpConnections.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class TcpServer final
{
public:
	TcpServer(boost_ioservice& ioService
			  , const unsigned short listenPort
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read& checkBytesLeftToRead
			  , const defs::message_received_handler& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);

	TcpServer(const unsigned short listenPort
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read& checkBytesLeftToRead
			  , const defs::message_received_handler& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);

	~TcpServer();

	TcpServer(const TcpServer& ) = delete;

	TcpServer& operator=(const TcpServer& ) = delete;

	void CloseAcceptor();

	void OpenAcceptor();

	void SendMessageToClientAsync(const defs::connection_address& client
								  , const defs::char_buffer& message);

	bool SendMessageToClientSync(const defs::connection_address& client
								 , const defs::char_buffer& message);

	void SendMessageToAllClients(const defs::char_buffer& message);

	// Throws xUnknownConnectionError is remoteEnd is not valid.
	auto GetServerDetailsForClient(const defs::connection_address& client)
			 -> defs::connection_address const;

private:
	std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
	boost_ioservice& m_ioService;
	std::unique_ptr<boost_tcp_acceptor> m_acceptor;
	const unsigned short m_listenPort;
	const size_t m_minAmountToRead;
	defs::check_bytes_left_to_read m_checkBytesLeftToRead;
	defs::message_received_handler m_messageReceivedHandler;
	const eSendOption m_sendOption;
	TcpConnections m_clientConnections;
	threads::SyncEvent m_closedEvent;

	void AcceptConnection();

	void AcceptHandler(defs::tcp_conn_ptr connection
					   , const boost_sys::error_code& error);

	void ProcessCloseAcceptor();
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPSERVER_HPP
