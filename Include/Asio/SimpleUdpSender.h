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
 * \file SimpleUdpSender.h
 * \brief File containing simple UDP sender class declaration.
 */

#ifndef SIMPLEUDPSENDER
#define SIMPLEUDPSENDER

#include "UdpTypedSender.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

class SimpleUdpSender final
{
public:
    SimpleUdpSender(boost_ioservice_t& ioService
                   , const defs::connection_t& receiver
                   , const eUdpOption sendOption = eUdpOption::broadcast
                   , const size_t sendBufferSize = DEFAULT_UDP_BUF_SIZE);

    SimpleUdpSender(const defs::connection_t& receiver
                   , const eUdpOption sendOption = eUdpOption::broadcast
                   , const size_t sendBufferSize = DEFAULT_UDP_BUF_SIZE);

    SimpleUdpSender(const SimpleUdpSender& ) = delete;
    SimpleUdpSender& operator=(const SimpleUdpSender& ) = delete;
    ~SimpleUdpSender() = default;

	defs::connection_t ReceiverConnection() const;

    bool SendMessage(const uint32_t messageId
                     , const defs::connection_t& responseAddress = defs::NULL_CONNECTION);

    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessage(const T& message
                     , const uint32_t messageId
                     , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        return m_udpTypedSender.SendMessage<T, A>(message, messageId, responseAddress);
    }

private:
    messages::MessageBuilder m_messageBuilder;
    UdpTypedSender<messages::MessageBuilder> m_udpTypedSender;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // SIMPLEUDPSENDER

