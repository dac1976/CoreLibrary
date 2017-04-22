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
 * \file MulticastSender.cpp
 * \brief File containing multicast sender class definition.
 */

#include "Asio/MulticastSender.h"
#include "boost/bind.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

// ****************************************************************************
// 'class MulticastSender' definition
// ****************************************************************************
MulticastSender::MulticastSender(boost_ioservice_t& ioService
			  , const defs::connection_t& multicastConnection
              , const std::string& interfaceAddress
			  , const bool enableLoopback
			  , const eMulticastTTL ttl
			  , const size_t sendBufferSize)
	: m_ioService(ioService)
	, m_multicastConnection(multicastConnection)
	, m_interfaceAddress(interfaceAddress)
	, m_socket{ioService}
	, m_multicastEndpoint(boost_address_t::from_string(multicastConnection.first)
	                                                   , multicastConnection.second)
{
	CreateMulticastSocket(enableLoopback, ttl, sendBufferSize);
}

MulticastSender::MulticastSender(const defs::connection_t& multicastConnection
              , const std::string& interfaceAddress
			  , const bool enableLoopback
			  , const eMulticastTTL ttl
			  , const size_t sendBufferSize)
	: m_ioThreadGroup{new IoServiceThreadGroup(1)}// 1 thread is sufficient only receive one message at a time
	, m_ioService(m_ioThreadGroup->IoService())
	, m_multicastConnection(multicastConnection)
	, m_interfaceAddress(interfaceAddress)
	, m_socket{m_ioService}
	, m_multicastEndpoint(boost_address_t::from_string(multicastConnection.first)
	                                                   , multicastConnection.second)
{
	CreateMulticastSocket(enableLoopback, ttl, sendBufferSize);
}

auto MulticastSender::MulticastConnection() const -> defs::connection_t
{
	return m_multicastConnection;
}

auto MulticastSender::InterfaceAddress() const -> std::string
{
	return m_interfaceAddress;
}


bool MulticastSender::SendMessage(const defs::char_buffer_t& message)
{
	return SyncSendTo(message);
}

void MulticastSender::CreateMulticastSocket(const bool enableLoopback
                     , const eMulticastTTL ttl
					 , const size_t sendBufferSize)
{
	m_socket.open(m_multicastEndpoint.protocol());
	
    m_socket.set_option(boost_mcast::hops(static_cast<int>(ttl)));
	
    boost_mcast::enable_loopback option(enableLoopback);
	m_socket.set_option(option);

	boost::asio::socket_base::send_buffer_size sizeOption(static_cast<int>(sendBufferSize));
    m_socket.set_option(sizeOption);

    // If interface is empty then the OS will pick the default multicast
    // enabled NIC to send the multicast through, which may not always
    // be the one that was intended.
	if (!m_interfaceAddress.empty())
	{
        m_socket.set_option(boost_mcast::outbound_interface(boost_address_v4_t::from_string(m_interfaceAddress)));
	}
}

bool MulticastSender::SyncSendTo(const defs::char_buffer_t& message)
{
	try
	{
		return message.size()
			   == m_socket.send_to(boost_asio::buffer(message)
								   , m_multicastEndpoint);
	}
	catch(const boost::system::system_error& /*e*/)
	{
		return false;
	}
}

} // namespace udp
} // namespace asio
} // namespace core_lib
