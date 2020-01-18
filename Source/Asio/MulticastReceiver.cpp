// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
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
#include <boost/bind.hpp>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{

// ****************************************************************************
// 'class MulticastReceiver' definition
// ****************************************************************************
MulticastReceiver::MulticastReceiver(boost_iocontext_t&                      ioContext,
                                     const defs::connection_t&               multicastConnection,
                                     const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                                     const defs::message_received_handler_t& messageReceivedHandler,
                                     const std::string& interfaceAddress, size_t receiveBufferSize)
    : m_ioContext(ioContext)
    , m_strand(ioContext)
    , m_multicastConnection(multicastConnection)
    , m_interfaceAddress(interfaceAddress)
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
    , m_socket{ioContext}
{
    CreateMulticastSocket(receiveBufferSize);
}

MulticastReceiver::MulticastReceiver(const defs::connection_t&               multicastConnection,
                                     const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                                     const defs::message_received_handler_t& messageReceivedHandler,
                                     const std::string& interfaceAddress, size_t receiveBufferSize)
    : m_ioThreadGroup{new IoContextThreadGroup(1)}
    // 1 thread is sufficient only receive one message at a time
    , m_ioContext(m_ioThreadGroup->IoContext())
    , m_strand(m_ioThreadGroup->IoContext())
    , m_multicastConnection(multicastConnection)
    , m_interfaceAddress(interfaceAddress)
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
    , m_socket{m_ioThreadGroup->IoContext()}
{
    CreateMulticastSocket(receiveBufferSize);
}

MulticastReceiver::~MulticastReceiver()
{
    CloseSocket();
}

defs::connection_t MulticastReceiver::MulticastConnection() const
{
    return m_multicastConnection;
}

std::string MulticastReceiver::InterfaceAddress() const
{
    return m_interfaceAddress;
}

void MulticastReceiver::CloseSocket()
{
    if (!m_socket.is_open())
    {
        return;
    }

    SetClosing(true);

    boost_asio::post(m_strand, boost::bind(&MulticastReceiver::ProcessCloseSocket, this));

    m_closedEvent.Wait();

    // To make sure we shutdown cleanly.
    std::this_thread::sleep_for(std::chrono::milliseconds(defs::SOCKET_CLOSE_SLEEP_MS));
}

void MulticastReceiver::CreateMulticastSocket(size_t receiveBufferSize)
{
    m_messageBuffer.reserve(UDP_DATAGRAM_MAX_SIZE);

    boost_udp_t::endpoint receiveEndpoint(boost_udp_t::v4(), m_multicastConnection.second);
    m_socket.open(receiveEndpoint.protocol());
    m_socket.set_option(boost_udp_t::socket::reuse_address(true));

    boost::asio::socket_base::receive_buffer_size sizeOption(static_cast<int>(receiveBufferSize));
    m_socket.set_option(sizeOption);

    m_socket.bind(receiveEndpoint);

    const boost_address_t mcastAddr = boost_address_t::from_string(m_multicastConnection.first);

    if (!m_interfaceAddress.empty() && (m_interfaceAddress.compare("0.0.0.0") != 0))
    {
        const boost_address_t listenAddr = boost_address_t::from_string(m_interfaceAddress);

        m_socket.set_option(boost_mcast::join_group(mcastAddr.to_v4(), listenAddr.to_v4()));
    }
    else
    {
        m_socket.set_option(boost_mcast::join_group(mcastAddr));
    }

    boost_asio::post(m_strand, boost::bind(&MulticastReceiver::StartAsyncRead, this));
}

void MulticastReceiver::StartAsyncRead()
{
    if (IsClosing())
    {
        return;
    }

    m_messageBuffer.clear();

    m_socket.async_receive_from(
        boost_asio::buffer(m_receiveBuffer),
        m_senderEndpoint,
        boost::asio::bind_executor(m_strand,
                                   boost::bind(&MulticastReceiver::ReadComplete,
                                               this,
                                               boost_placeholders::error,
                                               boost_placeholders::bytes_transferred)));
}

void MulticastReceiver::ReadComplete(const boost_sys::error_code& error, size_t bytesReceived)
{
    if (IsClosing() || error)
    {
        // This will be because we are closing our socket.
        return;
    }

    bool clearMsgBuf = false;

    try
    {
        std::copy(m_receiveBuffer.begin(),
                  std::next(m_receiveBuffer.begin(), static_cast<int32_t>(bytesReceived)),
                  std::back_inserter(m_messageBuffer));

        const size_t numBytesLeft = m_checkBytesLeftToRead(m_messageBuffer);

        if (numBytesLeft == 0)
        {
            m_messageReceivedHandler(m_messageBuffer);
            clearMsgBuf = true;
        }
    }
    catch (const std::exception& /*e*/)
    {
        // Nothing to do here for now.
        clearMsgBuf = true;
    }

    if (clearMsgBuf)
    {
        m_messageBuffer.clear();
    }

    boost_asio::post(m_strand, boost::bind(&MulticastReceiver::StartAsyncRead, this));
}

void MulticastReceiver::SetClosing(bool closing)
{
    std::lock_guard<std::mutex> lock(m_closingMutex);
    m_closing = closing;
}

bool MulticastReceiver::IsClosing() const
{
    std::lock_guard<std::mutex> lock(m_closingMutex);
    return m_closing;
}

void MulticastReceiver::ProcessCloseSocket()
{
    try
    {
        m_socket.shutdown(m_socket.shutdown_both);
        m_socket.close();
    }
    catch (...)
    {
        // Consume error...do nothing.
    }

    m_closedEvent.Signal();
}

} // namespace udp
} // namespace asio
} // namespace core_lib
