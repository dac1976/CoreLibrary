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
 * \file MulticastSender.cpp
 * \brief File containing multicast sender class definition.
 */

#include "MulticastSender.h"
#include <boost/bind.hpp>
#if defined(USE_SOCKET_DEBUG)
#include <boost/exception/all.hpp>
#include "DebugLogging.h"
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

// ****************************************************************************
// 'class MulticastSender' definition
// ****************************************************************************
// cppcheck-suppress constParameter
MulticastSender::MulticastSender(asio_compat::io_service_t& ioService,
                                 defs::connection_t const&  multicastConnection,
                                 std::string const& interfaceAddress, bool enableLoopback,
                                 int32_t ttl, size_t sendBufferSize)
    : m_multicastConnection(multicastConnection)
    , m_interfaceAddress(interfaceAddress)
    , m_multicastEndpoint(asio_compat::make_address(m_multicastConnection.first),
                          m_multicastConnection.second)
    , m_socket(ioService)
{
    CreateMulticastSocket(enableLoopback, ttl, sendBufferSize);
}

MulticastSender::MulticastSender(defs::connection_t const& multicastConnection,
                                 std::string const& interfaceAddress, bool enableLoopback,
                                 int32_t ttl, size_t sendBufferSize)
    : m_ioThreadGroup{new IoContextThreadGroup(1)}
    // 1 thread is sufficient only receive one message at a time
    , m_multicastConnection(multicastConnection)
    , m_interfaceAddress(interfaceAddress)
    , m_multicastEndpoint(asio_compat::make_address(m_multicastConnection.first),
                          m_multicastConnection.second)
    , m_socket(m_ioThreadGroup->IoService())
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

bool MulticastSender::SendMsg(const defs::char_buffer_t& message)
{
    return SyncSendTo(message);
}

void MulticastSender::CreateMulticastSocket(bool enableLoopback, int32_t ttl, size_t sendBufferSize)
{
    m_socket.open(m_multicastEndpoint.protocol());

    m_socket.set_option(boost_mcast::hops(ttl));

    boost_mcast::enable_loopback option(enableLoopback);
    m_socket.set_option(option);

    boost::asio::socket_base::send_buffer_size sizeOption(static_cast<int>(sendBufferSize));
    m_socket.set_option(sizeOption);

    // If interface is empty then the OS will pick the default multicast
    // enabled NIC to send the multicast through, which may not always
    // be the one that was intended.
    if (!m_interfaceAddress.empty())
    {
        m_socket.set_option(
            boost_mcast::outbound_interface(asio_compat::make_address(m_interfaceAddress).to_v4()));
    }
}

bool MulticastSender::SyncSendTo(const defs::char_buffer_t& message)
{
    try
    {
        return message.size() == m_socket.send_to(boost_asio::buffer(message), m_multicastEndpoint);
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR(
            "Error in SyncSendTo, error: " << boost::current_exception_diagnostic_information());
#endif
        return false;
    }
}

} // namespace udp
} // namespace asio
} // namespace core_lib
