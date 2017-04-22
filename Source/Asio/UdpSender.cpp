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
 * \file UdpSender.cpp
 * \brief File containing UDP sender class definition.
 */

#include "Asio/UdpSender.h"
#include "boost/bind.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

// ****************************************************************************
// 'class UdpSender' definition
// ****************************************************************************
UdpSender::UdpSender(boost_ioservice_t& ioService
					 , const defs::connection_t& receiver
					 , const eUdpOption sendOption
					 , const size_t sendBufferSize)
	: m_ioService(ioService)
	, m_receiver{receiver}
	, m_socket{ioService}
{
	CreateUdpSocket(sendOption, sendBufferSize);
}

UdpSender::UdpSender(const defs::connection_t& receiver
					 , const eUdpOption sendOption
					 , const size_t sendBufferSize)
	: m_ioThreadGroup{new IoServiceThreadGroup(1)}// 1 thread is sufficient only receive one message at a time
	, m_ioService(m_ioThreadGroup->IoService())
	, m_receiver{receiver}
	, m_socket{m_ioService}
{
	CreateUdpSocket(sendOption, sendBufferSize);
}

auto UdpSender::ReceiverConnection() const -> defs::connection_t
{
	return m_receiver;
}

bool UdpSender::SendMessage(const defs::char_buffer_t& message)
{
	return SyncSendTo(message);
}

void UdpSender::CreateUdpSocket(const eUdpOption sendOption
					 , const size_t sendBufferSize)
{
	boost_udp_t::resolver receiverResolver(m_ioService);
	boost_udp_t::resolver::query resolverQuery(boost_udp_t::v4()
											   , m_receiver.first
											   , std::to_string(m_receiver.second));
	m_receiverEndpoint = *receiverResolver.resolve(resolverQuery);

	m_socket.open(boost_udp_t::v4());

	boost_asio::socket_base::broadcast broadcastOption(sendOption
														== eUdpOption::broadcast);
	m_socket.set_option(broadcastOption);

    boost_asio::socket_base::send_buffer_size sendBufOption(static_cast<int>(sendBufferSize));
	m_socket.set_option(sendBufOption);
}

bool UdpSender::SyncSendTo(const defs::char_buffer_t& message)
{
	try
	{
		return message.size()
			   == m_socket.send_to(boost_asio::buffer(message)
								   , m_receiverEndpoint);
	}
	catch(const boost::system::system_error& /*e*/)
	{
		return false;
	}
}

} // namespace udp
} // namespace asio
} // namespace core_lib
