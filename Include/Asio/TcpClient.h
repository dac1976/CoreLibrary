
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

#include "AsioDefines.h"
#include "IoServiceThreadGroup.h"
#include "TcpConnections.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class TcpClient final
{
public:
	TcpClient(boost_ioservice_t& ioService
			  , const defs::connection_t& server
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);

	TcpClient(const defs::connection_t& server
			  , const size_t minAmountToRead
			  , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
			  , const defs::message_received_handler_t& messageReceivedHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);

	~TcpClient();

	TcpClient(const TcpClient& ) = delete;

	TcpClient& operator=(const TcpClient& ) = delete;

	defs::connection_t ServerConnection() const;

	// Throws xUnknownConnectionError is remoteEnd is not valid.
	defs::connection_t GetClientDetailsForServer() const;

	void CloseConnection();

	void SendMessageToServerAsync(const defs::char_buffer_t& message);

	bool SendMessageToServerSync(const defs::char_buffer_t& message);

private:
	std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
	boost_ioservice_t& m_ioService;
	const defs::connection_t m_server;
	const size_t m_minAmountToRead{0};
	defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
	defs::message_received_handler_t m_messageReceivedHandler;
	const eSendOption m_sendOption{eSendOption::nagleOn};
	TcpConnections m_serverConnection;

	void CreateConnection();

	void CheckAndCreateConnection();
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCLIENT
