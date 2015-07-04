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
 * \file UdpReceiver.h
 * \brief File containing UDP receiver class declaration.
 */

#ifndef UDPRECEIVER
#define UDPRECEIVER

#include "Threads/SyncEvent.h"
#include "AsioDefines.h"
#include "IoServiceThreadGroup.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

class UdpReceiver final
{
public:
	UdpReceiver(boost_ioservice_t& ioService
				, const uint16_t listenPort
				, const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				, const defs::message_received_handler_t& messageReceivedHandler
				, const eUdpOption receiveOptions = eUdpOption::broadcast
				, const size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE);

	UdpReceiver(const uint16_t listenPort
				, const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
				, const defs::message_received_handler_t& messageReceivedHandler
				, const eUdpOption receiveOptions = eUdpOption::broadcast
				, const size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE);

	UdpReceiver(const UdpReceiver& ) = delete;

	UdpReceiver& operator=(const UdpReceiver& ) = delete;

	~UdpReceiver() = default;

	uint16_t ListenPort() const;

private:
	std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
	boost_ioservice_t& m_ioService;
	const uint16_t m_listenPort{0};
	boost_udp_t::socket m_socket;
	defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
	defs::message_received_handler_t m_messageReceivedHandler;
	defs::char_buffer_t m_receiveBuffer;
	defs::char_buffer_t m_messageBuffer;
	boost_udp_t::endpoint m_senderEndpoint;

	void CreateUdpSocket(const eUdpOption receiveOptions
						 , const size_t receiveBufferSize);

	void StartAsyncRead();

	void ReadComplete(const boost_sys::error_code& error
					  , const size_t bytesReceived);

};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // UDPRECEIVER

