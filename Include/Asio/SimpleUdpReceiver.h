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
 * \file SimpleUdpReceiver.h
 * \brief File containing simple UDP receiver class declaration.
 */

#ifndef SIMPLEUDPRECEIVER
#define SIMPLEUDPRECEIVER

#include "UdpReceiver.h"
#include "MessageUtils.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

class SimpleUdpReceiver final
{
public:
    SimpleUdpReceiver(boost_ioservice_t& ioService
                      , const uint16_t listenPort
                      , const defs::default_message_dispatcher_t& messageDispatcher
                      , const eUdpOption receiveOptions = eUdpOption::broadcast
                      , const size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE);

    SimpleUdpReceiver(const uint16_t listenPort
                      , const defs::default_message_dispatcher_t& messageDispatcher
                      , const eUdpOption receiveOptions = eUdpOption::broadcast
                      , const size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE);

    SimpleUdpReceiver(const SimpleUdpReceiver& ) = delete;
    SimpleUdpReceiver& operator=(const SimpleUdpReceiver& ) = delete;
    ~SimpleUdpReceiver() = default;

    uint16_t ListenPort() const;

private:
    messages::MessageHandler m_messageHandler;
    UdpReceiver m_udpReceiver;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // SIMPLEUDPRECEIVER

