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
 * \file SimpleMulticastReceiver.cpp
 * \brief File containing simple multicast receiver class definition.
 */

#include "Asio/SimpleMulticastReceiver.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{

SimpleMulticastReceiver::SimpleMulticastReceiver(
    boost_iocontext_t& ioContext, const defs::connection_t& multicastConnection,
    const defs::default_message_dispatcher_t& messageDispatcher,
    const std::string& interfaceAddress, size_t receiveBufferSize, size_t memPoolMsgCount,
    size_t recvPoolMsgSize)
    : m_messageHandler{messageDispatcher,
                       defs::DEFAULT_MAGIC_STRING,
                       memPoolMsgCount,
                       recvPoolMsgSize}
    , m_mcastReceiver{ioContext,
                      multicastConnection,
                      std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                                std::placeholders::_1),
                      std::bind(&messages::MessageHandler::MessageReceivedHandler,
                                &m_messageHandler, std::placeholders::_1),
                      interfaceAddress,
                      receiveBufferSize}
{
}

SimpleMulticastReceiver::SimpleMulticastReceiver(
    const defs::connection_t&                 multicastConnection,
    const defs::default_message_dispatcher_t& messageDispatcher,
    const std::string& interfaceAddress, size_t receiveBufferSize, size_t memPoolMsgCount,
    size_t recvPoolMsgSize)
    : m_messageHandler{messageDispatcher,
                       defs::DEFAULT_MAGIC_STRING,
                       memPoolMsgCount,
                       recvPoolMsgSize}
    , m_mcastReceiver{multicastConnection,
                      std::bind(&messages::MessageHandler::CheckBytesLeftToRead, &m_messageHandler,
                                std::placeholders::_1),
                      std::bind(&messages::MessageHandler::MessageReceivedHandler,
                                &m_messageHandler, std::placeholders::_1),
                      interfaceAddress,
                      receiveBufferSize}
{
}

defs::connection_t SimpleMulticastReceiver::MulticastConnection() const
{
    return m_mcastReceiver.MulticastConnection();
}

std::string SimpleMulticastReceiver::InterfaceAddress() const
{
    return m_mcastReceiver.InterfaceAddress();
}

} // namespace udp
} // namespace asio
} // namespace core_lib
