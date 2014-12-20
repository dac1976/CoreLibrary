// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
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
 * \file TcpTypedClient.cpp
 * \brief File containing TCP typed client class definition.
 */

#include "../TcpTypedClient.hpp"

namespace core_lib {
namespace asio {
namespace tcp {

TcpTypedClient::TcpTypedClient(boost_ioservice& ioService
                               , const defs::connection& server
                               , const defs::message_dispatcher& messageDispatcher
                               , const eSendOption sendOption)
    : m_messageDispatcher(messageDispatcher)
    , m_tcpClient(ioService, server, sizeof(messages::MessageHeader)
                  , std::bind(&TcpTypedClient::CheckBytesLeftToRead, std::placeholders::_1)
                  , std::bind(&TcpTypedClient::MessageReceivedHandler, std::placeholders::_1)
                  , sendOption)
{
}

TcpTypedClient::TcpTypedClient(const defs::connection& server
                               , const defs::message_dispatcher& messageHandler
                               , const eSendOption sendOption)
    : m_messageDispatcher(messageDispatcher)
    , m_tcpClient(server, sizeof(messages::MessageHeader)
                  , std::bind(&TcpTypedClient::CheckBytesLeftToRead, std::placeholders::_1)
                  , std::bind(&TcpTypedClient::MessageReceivedHandler, std::placeholders::_1)
                  , sendOption)
{
}

void TcpTypedClient::CloseConnection()
{
    m_tcpClient.CloseConnection();
}

static void TcpTypedClient::CheckMessage(const char_buffer& message)
{
    if (message.size() < sizeof(messages::MessageHeader))
    {
        throw std::length_error("message buffer contains too few bytes");
    }
}

static size_t TcpTypedClient::CheckBytesLeftToRead(const defs::char_buffer& message)
{
    CheckMessage(message);

    const messages::MessageHeader* pHeader
        = reinterpret_cast<const messages::MessageHeader*>(&message.front());

    if (std::string(pHeader->magicString) != messages::MAGIC_STRING)
    {
        throw std::runtime_error("cannot find magic string");
    }

    if (pHeader->totalLength < message.size())
    {
        throw std::length_error("invalid total length in header");
    }

    return pHeader->totalLength - message.size();
}

static void TcpTypedClient::MessageReceivedHandler(const defs::char_buffer& message)
{
    CheckMessage(message);

    const messages::MessageHeader* pHeader
        = reinterpret_cast<const messages::MessageHeader*>(&message.front());
    defs::char_buffer messageBody{message.begin() + sizeof(messages::MessageHeader), message.end()};
    
    m_messageDispatcher(*pHeader, messageBody);
}

} // namespace tcp
} // namespace asio
} // namespace core_lib
