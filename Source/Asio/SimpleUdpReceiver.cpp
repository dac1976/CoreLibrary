// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
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
 * \file SimpleUdpReceiver.cpp
 * \brief File containing simple UDP receiver class definition.
 */

#include "Asio/SimpleUdpReceiver.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{

SimpleUdpReceiver::SimpleUdpReceiver(asio_compat::io_service_t& ioService, 
                              uint16_t listenPort,
							  const defs::default_message_dispatcher_t& messageDispatcher,
							  eUdpOption receiveOptions, 
							  size_t receiveBufferSize,
							  size_t memPoolMsgCount, 
							  size_t recvPoolMsgSize,
							  std::string const& listenAddress)
    : m_messageHandler{messageDispatcher,
                       defs::DEFAULT_MAGIC_STRING,
                       memPoolMsgCount,
                       recvPoolMsgSize}
    , m_udpReceiver{ioService,
                    listenPort,
                    std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                              std::placeholders::_1),
                    std::bind(&messages::MessageHandler::MessageReceivedHandler, &m_messageHandler,
                              std::placeholders::_1),
                    receiveOptions,
                    receiveBufferSize,
                    listenAddress,
                    defs::message_received_handler_ex_t(),
                    defs::check_bytes_left_to_read_ex_t()}
{
}

SimpleUdpReceiver::SimpleUdpReceiver(uint16_t listenPort,
							  const defs::default_message_dispatcher_t& messageDispatcher,
							  eUdpOption receiveOptions, 
							  size_t receiveBufferSize,
							  size_t memPoolMsgCount, 
							  size_t recvPoolMsgSize,
							  std::string const& listenAddress)
    : m_messageHandler{messageDispatcher,
                       defs::DEFAULT_MAGIC_STRING,
                       memPoolMsgCount,
                       recvPoolMsgSize}
    , m_udpReceiver{listenPort,
                    std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                              std::placeholders::_1),
                    std::bind(&messages::MessageHandler::MessageReceivedHandler, &m_messageHandler,
                              std::placeholders::_1),
                    receiveOptions,
                    receiveBufferSize,
                    listenAddress,
                    defs::message_received_handler_ex_t(),
                    defs::check_bytes_left_to_read_ex_t()}
{
}

uint16_t SimpleUdpReceiver::ListenPort() const
{
    return m_udpReceiver.ListenPort();
}

std::string SimpleUdpReceiver::ListenAddress() const
{
    return m_udpReceiver.ListenAddress();
}

} // namespace udp
} // namespace asio
} // namespace core_lib
