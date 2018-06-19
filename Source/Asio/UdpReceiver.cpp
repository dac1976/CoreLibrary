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
 * \file UdpReceiver.cpp
 * \brief File containing UDP receiver class definition.
 */

#include "Asio/UdpReceiver.h"
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
// 'class UdpReceiver' definition
// ****************************************************************************
UdpReceiver::UdpReceiver(boost_ioservice_t& ioService, uint16_t listenPort,
                         const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                         const defs::message_received_handler_t& messageReceivedHandler,
                         eUdpOption receiveOptions, size_t receiveBufferSize)
    : m_ioService(ioService)
    , m_listenPort{listenPort}
    , m_socket{m_ioService}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
{
    CreateUdpSocket(receiveOptions, receiveBufferSize);
}

UdpReceiver::UdpReceiver(uint16_t                                listenPort,
                         const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                         const defs::message_received_handler_t& messageReceivedHandler,
                         eUdpOption receiveOptions, size_t receiveBufferSize)
    : m_ioThreadGroup{new IoServiceThreadGroup(1)}
    // 1 thread is sufficient only receive one message at a time
    , m_ioService(m_ioThreadGroup->IoService())
    , m_listenPort{listenPort}
    , m_socket{m_ioService}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
{
    CreateUdpSocket(receiveOptions, receiveBufferSize);
}

UdpReceiver::~UdpReceiver()
{
    if (!m_socket.is_open())
    {
        return;
    }

    SetClosing(true);
    m_ioService.post(boost::bind(&UdpReceiver::ProcessCloseSocket, this));
    m_closedEvent.Wait();
}

uint16_t UdpReceiver::ListenPort() const
{
    return m_listenPort;
}

void UdpReceiver::CreateUdpSocket(eUdpOption receiveOptions, size_t receiveBufferSize)
{
    m_messageBuffer.reserve(UDP_DATAGRAM_MAX_SIZE);

    boost_udp_t::endpoint receiveEndpoint(boost_udp_t::v4(), m_listenPort);

    m_socket.open(receiveEndpoint.protocol());

    boost_udp_t::socket::reuse_address reuseAddrOption(receiveOptions == eUdpOption::broadcast);
    m_socket.set_option(reuseAddrOption);

    boost_asio::socket_base::receive_buffer_size receiveBufOption(
        static_cast<int>(receiveBufferSize));
    m_socket.set_option(receiveBufOption);

    m_socket.bind(receiveEndpoint);

    StartAsyncRead();
}

void UdpReceiver::StartAsyncRead()
{
    m_messageBuffer.clear();
    m_socket.async_receive_from(boost_asio::buffer(m_receiveBuffer),
                                m_senderEndpoint,
                                boost::bind(&UdpReceiver::ReadComplete,
                                            this,
                                            boost_placeholders::error,
                                            boost_placeholders::bytes_transferred));
}

void UdpReceiver::ReadComplete(const boost_sys::error_code& error, size_t bytesReceived)
{
    if (IsClosing() || error)
    {
        // This will be because we are closing our socket.
        return;
    }

    try
    {
        std::copy(m_receiveBuffer.begin(),
                  m_receiveBuffer.begin() + bytesReceived,
                  std::back_inserter(m_messageBuffer));

        const size_t numBytesLeft = m_checkBytesLeftToRead(m_messageBuffer);

        if (numBytesLeft == 0)
        {
            m_messageReceivedHandler(m_messageBuffer);
        }
    }
    catch (const std::exception& /*e*/)
    {
        // Nothing to do here for now.
    }

    StartAsyncRead();
}

void UdpReceiver::SetClosing(bool closing)
{
    std::lock_guard<std::mutex> lock(m_closingMutex);
    m_closing = closing;
}

bool UdpReceiver::IsClosing() const
{
    std::lock_guard<std::mutex> lock(m_closingMutex);
    return m_closing;
}

void UdpReceiver::ProcessCloseSocket()
{
    m_socket.close();
    m_closedEvent.Signal();
}

} // namespace udp
} // namespace asio
} // namespace core_lib
