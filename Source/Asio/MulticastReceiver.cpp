// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2016 Duncan Crutchley
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
 * \file MulticastReceiver.cpp
 * \brief File containing multicast receiver class definition.
 */

#include "Asio/MulticastReceiver.h"
#include "boost/bind.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

// ****************************************************************************
// 'class MulticastReceiver' definition
// ****************************************************************************
MulticastReceiver::MulticastReceiver(boost_ioservice_t& ioService
						 , const uint16_t listenPort
                         , const std::string& listenAddress
				         , const std::string& multicastAddress
						 , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
						 , const defs::message_received_handler_t& messageReceivedHandler
						 , const size_t receiveBufferSize)
    : m_destructing(false)
    , m_ioService(ioService)
	, m_listenPort{listenPort}
    , m_listenAddress(listenAddress)
    , m_multicastAddress(multicastAddress)
	, m_socket{m_ioService}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
{
	CreateMulticastSocket(receiveBufferSize);
}

MulticastReceiver::MulticastReceiver(const uint16_t listenPort
                         , const std::string& listenAddress
				         , const std::string& multicastAddress
						 , const defs::check_bytes_left_to_read_t& checkBytesLeftToRead
						 , const defs::message_received_handler_t& messageReceivedHandler
						 , const size_t receiveBufferSize)
    : m_destructing(false)
    , m_ioThreadGroup{new IoServiceThreadGroup(1)}// 1 thread is sufficient only receive one message at a time
	, m_ioService(m_ioThreadGroup->IoService())
	, m_listenPort{listenPort}
    , m_listenAddress(listenAddress)
    , m_multicastAddress(multicastAddress)
	, m_socket{m_ioService}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
{
	CreateMulticastSocket(receiveBufferSize);
}

MulticastReceiver::~MulticastReceiver()
{
    // We need to flag destruction so we can
    // stop handling received messages in a
    // thread-safe way.
    std::lock_guard<std::mutex> lock(m_destructionMutex);
    m_destructing = true;
}

uint16_t MulticastReceiver::ListenPort() const
{
	return m_listenPort;
}

std::string MulticastReceiver::ListenAddress() const
{
	return m_listenAddress;
}

std::string MulticastReceiver::MulticastAddress() const
{
	return m_multicastAddress;
}

void MulticastReceiver::CreateMulticastSocket(const size_t receiveBufferSize)
{
	m_messageBuffer.reserve(UDP_DATAGRAM_MAX_SIZE);
    
    boost_address_t listenAddr = boost_address_t::from_string(m_listenAddress);
    boost_address_t mcastAddr = boost_address_t::from_string(m_multicastAddress);

    boost_udp_t::endpoint receiveEndpoint(listenAddr, m_listenPort);
    
    m_socket.open(receiveEndpoint.protocol());    
    m_socket.set_option(boost_udp_t::socket::reuse_address(true));
    
    boost::asio::socket_base::receive_buffer_size sizeOption(static_cast<int>(receiveBufferSize));
    m_socket.set_option(sizeOption);
    
    m_socket.bind(receiveEndpoint);
	
    m_socket.set_option(boost::asio::ip::multicast::join_group(mcastAddr.to_v4()
                                                               , listenAddr.to_v4()));

	StartAsyncRead();
}

void MulticastReceiver::StartAsyncRead()
{
	m_messageBuffer.clear();

	m_socket.async_receive_from(boost_asio::buffer(m_receiveBuffer)
								, m_senderEndpoint
								, boost::bind(&MulticastReceiver::ReadComplete
											  , this
											  , boost_placeholders::error
											  , boost_placeholders::bytes_transferred));
}

void MulticastReceiver::ReadComplete(const boost_sys::error_code& error
							   , const size_t bytesReceived)
{
	if (error)
	{
		// This will be because we are closing our socket.
		return;
	}

    // Lock in this scope to make it safe if the destructor is called.
    // We'll short-circuit if we're destructing so we don't reference
    // objects going out of scope as the MulticastReceiver is destructed.
    std::lock_guard<std::mutex> lock(m_destructionMutex);

    if (m_destructing)
    {
        return;
    }

	try
	{
		std::copy(m_receiveBuffer.begin()
				  , m_receiveBuffer.begin() + bytesReceived
				  , std::back_inserter(m_messageBuffer));

		const size_t numBytesLeft = m_checkBytesLeftToRead(m_messageBuffer);

		if (numBytesLeft == 0)
		{
			m_messageReceivedHandler(m_messageBuffer);
		}
	}
	catch(const std::exception& /*e*/)
	{
		// Nothing to do here for now.
	}

	StartAsyncRead();
}

} // namespace udp
} // namespace asio
} // namespace core_lib
