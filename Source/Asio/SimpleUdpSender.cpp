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
 * \file SimpleUdpSender.cpp
 * \brief File containing simple UDP sender class definition.
 */

#include "Asio/SimpleUdpSender.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{

SimpleUdpSender::SimpleUdpSender(boost_iocontext_t& ioContext, const defs::connection_t& receiver,
                                 eUdpOption sendOption, size_t sendBufferSize)
    : m_udpTypedSender{ioContext, receiver, m_messageBuilder, sendOption, sendBufferSize}
{
}

SimpleUdpSender::SimpleUdpSender(const defs::connection_t& receiver, eUdpOption sendOption,
                                 size_t sendBufferSize)
    : m_udpTypedSender{receiver, m_messageBuilder, sendOption, sendBufferSize}
{
}

auto SimpleUdpSender::ReceiverConnection() const -> defs::connection_t
{
    return m_udpTypedSender.ReceiverConnection();
}

bool SimpleUdpSender::SendMessage(int32_t messageId, const defs::connection_t& responseAddress)
{
    return m_udpTypedSender.SendMessage(messageId, responseAddress);
}

bool SimpleUdpSender::SendMessage(const defs::char_buffer_t& message, int32_t messageId,
                                  const defs::connection_t& responseAddress)
{
    return m_udpTypedSender.SendMessage(message, messageId, responseAddress);
}

bool SimpleUdpSender::SendMessage(const defs::char_buffer_t& message)
{
    return m_udpTypedSender.SendMessage(message);
}

bool SimpleUdpSender::SendMessage(const char* message, size_t length)
{
    return m_udpTypedSender.SendMessage(message, length);
}

} // namespace udp
} // namespace asio
} // namespace core_lib
