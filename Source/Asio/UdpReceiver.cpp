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

#include "UdpReceiver.h"
#include <boost/bind.hpp>
#if defined(USE_SOCKET_DEBUG)
#include <boost/exception/all.hpp>
#include "DebugLog/DebugLogging.h"
#endif

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{

CONSTEXPR_ uint32_t CLOSE_WAIT_MS = 1000;

// ****************************************************************************
// 'class UdpReceiver' definition
// ****************************************************************************
UdpReceiver::UdpReceiver(asio_compat::io_service_t& ioService, 
                    uint16_t listenPort,
					defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
					defs::message_received_handler_t const& messageReceivedHandler,
					eUdpOption receiveOptions, size_t receiveBufferSize,
					std::string const& listenAddress,
					defs::message_received_handler_ex_t const& messageReceivedHandlerEx,
					defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx)
    : m_closeEvent(eNotifyType::signalOneThread, eResetCondition::manualReset,
                   eIntialCondition::notSignalled)
    , m_strand(asio_compat::make_strand(ioService))
    , m_listenPort{listenPort}
    , m_listenAddress{listenAddress}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_checkBytesLeftToReadEx(checkBytesLeftToReadEx)
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_messageReceivedHandlerEx(messageReceivedHandlerEx)
    , m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
    , m_socket{ioService}
{
    CreateUdpSocket(receiveOptions, receiveBufferSize);
}

UdpReceiver::UdpReceiver(uint16_t listenPort,
                    defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                    defs::message_received_handler_t const& messageReceivedHandler,
                    eUdpOption receiveOptions, size_t receiveBufferSize,
                    std::string const& listenAddress,
                    defs::message_received_handler_ex_t const& messageReceivedHandlerEx,
                    defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx)
    : m_closeEvent(eNotifyType::signalOneThread, eResetCondition::manualReset,
                   eIntialCondition::notSignalled)
    , m_ioThreadGroup{new IoContextThreadGroup(1)}
    // 1 thread is sufficient only receive one message at a time
    , m_strand(asio_compat::make_strand(m_ioThreadGroup->IoService()))
    , m_listenPort{listenPort}
    , m_listenAddress{listenAddress}
    , m_checkBytesLeftToRead{checkBytesLeftToRead}
    , m_checkBytesLeftToReadEx(checkBytesLeftToReadEx)
    , m_messageReceivedHandler{messageReceivedHandler}
    , m_messageReceivedHandlerEx(messageReceivedHandlerEx)
    , m_receiveBuffer(UDP_DATAGRAM_MAX_SIZE, 0)
    , m_socket{m_ioThreadGroup->IoService()}
{
    CreateUdpSocket(receiveOptions, receiveBufferSize);
}

UdpReceiver::~UdpReceiver()
{
    CloseSocket();
}

uint16_t UdpReceiver::ListenPort() const
{
    return m_listenPort;
}

std::string UdpReceiver::ListenAddress() const
{
    return m_listenAddress;
}

void UdpReceiver::CloseSocket()
{
    SetClosing(true);

    if (!m_socket.is_open())
    {
        return;
    }

    try
    {
        m_socket.shutdown(m_socket.shutdown_both);
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error shutting down sends and receives for socket, error: "
                           << boost::current_exception_diagnostic_information());
#endif
        // Do nothing consume error.
    }

    try
    {
        m_socket.close();
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR(
            "Error closing socket, error: " << boost::current_exception_diagnostic_information());
#endif
        // Do nothing consume error.
    }

    // Wait for socket to finish closing so we don't have a race
    // condition in ReadComplete.

    m_closeEvent.WaitForTime(CLOSE_WAIT_MS);
    m_closeEvent.Reset();
}

void UdpReceiver::CreateUdpSocket(eUdpOption receiveOptions, size_t receiveBufferSize)
{
    m_socket.open(boost_udp_t::v4());

    boost_udp_t::socket::reuse_address reuseAddrOption(receiveOptions == eUdpOption::broadcast);
    m_socket.set_option(reuseAddrOption);

    boost_asio::socket_base::receive_buffer_size receiveBufOption(
        static_cast<int>(receiveBufferSize));
    m_socket.set_option(receiveBufOption);

    auto receiveEndpoint =
        m_listenAddress.empty()
            ? boost_udp_t::endpoint(boost_udp_t::v4(), m_listenPort)
            : boost_udp_t::endpoint(asio_compat::make_address(m_listenAddress), m_listenPort);

    m_socket.bind(receiveEndpoint);

    StartAsyncRead();
}

void UdpReceiver::StartAsyncRead()
{
    // This won't be expensive to resize as we initially sized it in constructor.
    // We need to set back to full datagram size again after a read because we always
    // clear the buffer down after a read.
    m_receiveBuffer.resize(UDP_DATAGRAM_MAX_SIZE);

    m_socket.async_receive_from(
        boost_asio::buffer(m_receiveBuffer),
        m_senderEndpoint,
        asio_compat::wrap(m_strand,
                          boost::bind(&UdpReceiver::ReadComplete,
                                      this,
                                      boost_placeholders::error,
                                      boost_placeholders::bytes_transferred)));
}

void UdpReceiver::ReadComplete(const boost_sys::error_code& error, size_t bytesReceived)
{
    if (error)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("ReadComplete called but error reported: "
                               << error.message() << ", on connection: " << m_listenAddress << ":"
                               << m_listenPort);
#endif

        if (Closing())
        {
            m_closeEvent.Signal();
        }

        // This will be because we are closing our socket.
        return;
    }

    bool clearMsgBuf = false;

    try
    {
        // NOTE: Boost UDP sockets only ever give complete datagrams
        // to user level code so we do not need to handle partial reads
        // from the socket like we have to do with TCP. No need for double
        // buffering like we need for TCP version.
        //
        // Initially before read we pass the socket the receive buffer at max
        // datagram size. Need to efficiently truncate the receive buffer to
        // actual num bytes received.
        m_receiveBuffer.resize(bytesReceived);

        size_t numBytesLeft = 0;

        if (m_checkBytesLeftToReadEx)
        {
            numBytesLeft = m_checkBytesLeftToReadEx(
                m_receiveBuffer, m_senderEndpoint.address().to_string(), m_senderEndpoint.port());
        }
        else
        {
            if (m_checkBytesLeftToRead)
            {
                numBytesLeft = m_checkBytesLeftToRead(m_receiveBuffer);
            }
        }

        if (numBytesLeft == 0)
        {
            // Ideally only one of m_messageReceivedHandler or m_messageReceivedHandlerEx should
            // be defined at any one time.

            if (m_messageReceivedHandlerEx)
            {
                m_messageReceivedHandlerEx(m_receiveBuffer,
                                           m_senderEndpoint.address().to_string(),
                                           m_senderEndpoint.port());
            }
            else
            {
                if (m_messageReceivedHandler)
                {
                    m_messageReceivedHandler(m_receiveBuffer);
                }
            }

            clearMsgBuf = true;
        }
        else if (std::numeric_limits<size_t>::max() == numBytesLeft)
        {
            // We have a problem.
            clearMsgBuf = true;
        }
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR(
            "Error in ReadComplete, error: " << boost::current_exception_diagnostic_information());
#endif
        // Nothing to do here for now.
        clearMsgBuf = true;
    }

    if (clearMsgBuf)
    {
        m_receiveBuffer.clear();
    }
    StartAsyncRead();
}

bool UdpReceiver::Closing() const NO_EXCEPT_
{
    std::lock_guard<std::mutex> lock(m_closingMutex);

    return m_closing;
}

void UdpReceiver::SetClosing(bool close) NO_EXCEPT_
{
    std::lock_guard<std::mutex> lock(m_closingMutex);

    m_closing = close;
}

} // namespace udp
} // namespace asio
} // namespace core_lib
