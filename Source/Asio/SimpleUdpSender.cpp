// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014,2015 Duncan Crutchley
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
 * \file SimpleUdpSender.cpp
 * \brief File containing simple UDP sender class definition.
 */

#include "Asio/SimpleUdpSender.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

SimpleUdpSender::SimpleUdpSender(boost_ioservice_t& ioService
                                 , const defs::connection_t& receiver
                                 , const eUdpOption sendOption
                                 , const size_t sendBufferSize)
    : m_udpTypedSender{ioService, receiver, m_messageBuilder
                       , sendOption, sendBufferSize}
{
}

SimpleUdpSender::SimpleUdpSender(const defs::connection_t& receiver
                                 , const eUdpOption sendOption
                                 , const size_t sendBufferSize)
    : m_udpTypedSender{receiver, m_messageBuilder
                       , sendOption, sendBufferSize}
{
}

auto SimpleUdpSender::ReceiverConnection() const -> defs::connection_t
{
    return m_udpTypedSender.ReceiverConnection();
}

bool SimpleUdpSender::SendMessage(const uint32_t messageId
                                  , const defs::connection_t& responseAddress)
{
    return m_udpTypedSender.SendMessage(messageId, responseAddress);
}

} // namespace udp
} // namespace asio
} // namespace core_lib
