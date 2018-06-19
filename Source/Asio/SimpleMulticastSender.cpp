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
 * \file SimpleMulticastSender.cpp
 * \brief File containing simple multicast sender class definition.
 */

#include "Asio/SimpleMulticastSender.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{

SimpleMulticastSender::SimpleMulticastSender(boost_ioservice_t&        ioService,
                                             const defs::connection_t& multicastConnection,
                                             const std::string&        interfaceAddress,
                                             bool enableLoopback, eMulticastTTL ttl,
                                             size_t sendBufferSize)
    : m_multicastTypedSender{ioService,
                             multicastConnection,
                             m_messageBuilder,
                             interfaceAddress,
                             enableLoopback,
                             ttl,
                             sendBufferSize}
{
}

SimpleMulticastSender::SimpleMulticastSender(const defs::connection_t& multicastConnection,
                                             const std::string&        interfaceAddress,
                                             bool enableLoopback, eMulticastTTL ttl,
                                             size_t sendBufferSize)
    : m_multicastTypedSender{multicastConnection,
                             m_messageBuilder,
                             interfaceAddress,
                             enableLoopback,
                             ttl,
                             sendBufferSize}
{
}

auto SimpleMulticastSender::MulticastConnection() const -> defs::connection_t
{
    return m_multicastTypedSender.MulticastConnection();
}

std::string SimpleMulticastSender::InterfaceAddress() const
{
    return m_multicastTypedSender.InterfaceAddress();
}

bool SimpleMulticastSender::SendMessage(int32_t                   messageId,
                                        const defs::connection_t& responseAddress)
{
    return m_multicastTypedSender.SendMessage(messageId, responseAddress);
}

} // namespace udp
} // namespace asio
} // namespace core_lib
