
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
 * \file MessageUtils.cpp
 * \brief File containing message utils definitions.
 */

#include "Asio/MessageUtils.h"
#include <cassert>
#include <cstdio>
#include <stdexcept>
#ifdef USE_EXPLICIT_MOVE_
#include <utility>
#endif
#include <boost/throw_exception.hpp>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace messages
{

// ****************************************************************************
// 'class MessageHandler' definition
// ****************************************************************************

#ifdef USE_DEFAULT_CONSTRUCTOR_
MessageHandler::MessageHandler()
    : m_magicString(defs::DEFAULT_MAGIC_STRING)
{
}
#endif

#ifdef USE_EXPLICIT_MOVE_
MessageHandler::MessageHandler(MessageHandler&& mh)
    : m_magicString(defs::DEFAULT_MAGIC_STRING)
{
    *this = std::move(mh);
}

MessageHandler& MessageHandler::operator=(MessageHandler&& mh)
{
    std::swap(m_messageDispatcher, mh.m_messageDispatcher);
    m_magicString.swap(mh.m_magicString);
}
#endif

MessageHandler::MessageHandler(const defs::default_message_dispatcher_t& messageDispatcher,
                               const std::string&                        magicString)
    : m_messageDispatcher(messageDispatcher)
    , m_magicString(magicString)
{
}

size_t MessageHandler::CheckBytesLeftToRead(const defs::char_buffer_t& message) const
{
    if (!CheckMessage(message))
    {
        return sizeof(defs::MessageHeader) - message.size();
    }

    auto pHeader = reinterpret_cast<const defs::MessageHeader*>(&message.front());

    if (m_magicString.compare(pHeader->magicString) != 0)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("incorrect magic string"));
    }

    if (pHeader->totalLength < message.size())
    {
        BOOST_THROW_EXCEPTION(std::length_error("message length error"));
    }

    return pHeader->totalLength - message.size();
}

void MessageHandler::MessageReceivedHandler(const defs::char_buffer_t& message) const
{
    if (!CheckMessage(message))
    {
        BOOST_THROW_EXCEPTION(std::length_error("incomplete message header"));
    }

    auto pHeader            = reinterpret_cast<const defs::MessageHeader*>(&message.front());
    auto receivedMessage    = std::make_shared<defs::default_received_message_t>();
    receivedMessage->header = *pHeader;

    if (pHeader->totalLength > sizeof(defs::MessageHeader))
    {
        receivedMessage->body.assign(message.begin() + sizeof(defs::MessageHeader), message.end());
    }

    m_messageDispatcher(receivedMessage);
}

bool MessageHandler::CheckMessage(const defs::char_buffer_t& message)
{	
	return message.size() >= sizeof(defs::MessageHeader);
}

// ****************************************************************************
// Utility functions
// ****************************************************************************

void FillHeader(const std::string& magicString, defs::eArchiveType archiveType, int32_t messageId,
                const defs::connection_t& responseAddress, uint32_t messageLength,
                defs::MessageHeader& header)
{
    assert(magicString.size() < defs::MAGIC_STRING_LEN);

    if (magicString.size() >= defs::MAGIC_STRING_LEN)
    {
        BOOST_THROW_EXCEPTION(std::length_error("magic string too long"));
    }

    assert(responseAddress.first.size() < defs::RESPONSE_ADDRESS_LEN);

    if (responseAddress.first.size() >= defs::RESPONSE_ADDRESS_LEN)
    {
        BOOST_THROW_EXCEPTION(std::length_error("response address too long"));
    }

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    std::sprintf(static_cast<char*>(header.magicString), "%s", magicString.c_str());
    std::sprintf(static_cast<char*>(header.responseAddress), "%s", responseAddress.first.c_str());
#else
    std::snprintf(static_cast<char*>(header.magicString),
                  sizeof(header.magicString),
                  "%s",
                  magicString.c_str());
    std::snprintf(static_cast<char*>(header.responseAddress),
                  sizeof(header.responseAddress),
                  "%s",
                  responseAddress.first.c_str());
#endif
    header.responsePort = responseAddress.second;
    header.messageId    = messageId;
    header.archiveType  = archiveType;
    header.totalLength  = static_cast<uint32_t>(sizeof(defs::MessageHeader)) + messageLength;
}

// ****************************************************************************
// 'class MessageBuilder' definition
// ****************************************************************************

#ifdef USE_DEFAULT_CONSTRUCTOR_
MessageBuilder::MessageBuilder()
    : m_magicString(defs::DEFAULT_MAGIC_STRING)
{
}
#endif

#ifdef USE_EXPLICIT_MOVE_
MessageBuilder::MessageBuilder(MessageBuilder&& mb)
    : m_magicString(defs::DEFAULT_MAGIC_STRING)
{
    *this = std::move(mb);
}

MessageBuilder& MessageBuilder::operator=(MessageBuilder&& mb)
{
    m_magicString.swap(mb.m_magicString);
}
#endif

MessageBuilder::MessageBuilder(const std::string& magicString)
    : m_magicString(magicString)
{
}

auto MessageBuilder::Build(int32_t messageId, const defs::connection_t& responseAddress) const
    -> defs::char_buffer_t const&
{
    // Resize message buffer.
    auto totalLength = sizeof(defs::MessageHeader);
    m_messageBuffer.resize(totalLength);

    defs::MessageHeader* header = reinterpret_cast<defs::MessageHeader*>(m_messageBuffer.data());
    FillHeader(m_magicString, defs::eArchiveType::raw, messageId, responseAddress, 0, *header);

    return m_messageBuffer;
}

auto MessageBuilder::Build(const defs::char_buffer_t& message, int32_t messageId,
                           const defs::connection_t& responseAddress,
                           defs::eArchiveType archiveType) const -> defs::char_buffer_t const&
{
    return Build(message.data(), message.size(), messageId, responseAddress, archiveType);
}

auto MessageBuilder::Build(const void* message, size_t messageLength, int32_t messageId,
                           const defs::connection_t& responseAddress,
                           defs::eArchiveType archiveType) const -> defs::char_buffer_t const&
{
    if ((0 == messageLength) || (message == nullptr))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("message pointer or length is invalid"));
    }

    // Resize message buffer.
    auto totalLength = sizeof(defs::MessageHeader) + messageLength;
    m_messageBuffer.resize(totalLength);

    // Fill header.
    defs::MessageHeader* header = reinterpret_cast<defs::MessageHeader*>(m_messageBuffer.data());
    FillHeader(m_magicString,
               archiveType,
               messageId,
               responseAddress,
               static_cast<uint32_t>(messageLength),
               *header);

    auto writePosIter = std::next(m_messageBuffer.begin(), sizeof(defs::MessageHeader));
    auto charMsgPtr   = reinterpret_cast<char const*>(message);
    std::copy(charMsgPtr, charMsgPtr + messageLength, writePosIter);

    return m_messageBuffer;
}

} // namespace messages
} // namespace asio
} // namespace core_lib
