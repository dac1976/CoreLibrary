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
 * \file UdpSender.cpp
 * \brief File containing UDP sender class definition.
 */

#include "Asio/UdpSender.h"
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
// 'class UdpSender' definition
// ****************************************************************************
UdpSender::UdpSender(boost_iocontext_t& ioContext, const defs::connection_t& receiver,
                     eUdpOption sendOption, size_t sendBufferSize)
    : m_receiver{receiver}
    , m_receiverResolver{ioContext}
    , m_socket{ioContext}
{
    CreateUdpSocket(sendOption, sendBufferSize);
}

UdpSender::UdpSender(const defs::connection_t& receiver, eUdpOption sendOption,
                     size_t sendBufferSize)
    : m_ioThreadGroup{new IoContextThreadGroup(1)}
    // 1 thread is sufficient only receive one message at a time
    , m_receiver{receiver}
    , m_receiverResolver{m_ioThreadGroup->IoContext()}
    , m_socket{m_ioThreadGroup->IoContext()}
{
    CreateUdpSocket(sendOption, sendBufferSize);
}

auto UdpSender::ReceiverConnection() const -> defs::connection_t
{
    return m_receiver;
}

bool UdpSender::SendMessage(const defs::char_buffer_t& message)
{
    return SyncSendTo(message.data(), message.size());
}

bool UdpSender::SendMessage(const char* message, size_t length)
{
    return SyncSendTo(message, length);
}

void UdpSender::CreateUdpSocket(eUdpOption sendOption, size_t sendBufferSize)
{
    m_socket.open(boost_udp_t::v4());

    boost_asio::socket_base::broadcast broadcastOption(sendOption == eUdpOption::broadcast);
    m_socket.set_option(broadcastOption);

    boost_asio::socket_base::send_buffer_size sendBufOption(static_cast<int>(sendBufferSize));
    m_socket.set_option(sendBufOption);
}

bool UdpSender::SyncSendTo(const char* message, size_t length)
{
    try
    {
        if (m_receiverEndpoint.port() != m_receiver.second)
        {
            m_receiverEndpoint = *m_receiverResolver.resolve(
                boost_udp_t::v4(), m_receiver.first, std::to_string(m_receiver.second));
        }

        return length == m_socket.send_to(boost_asio::buffer(message, length), m_receiverEndpoint);
    }
    catch (...)
    {
        return false;
    }
}

} // namespace udp
} // namespace asio
} // namespace core_lib
