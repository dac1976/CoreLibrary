
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
#include "DebugLog/DebugLogging.h"
#include "StringUtils/StringUtils.h"
#include "Asio/MemoryUtils.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace messages
{

constexpr size_t MESSAGE_LENGTH_OFFSET = sizeof(defs::MessageHeader) - sizeof(uint32_t);

// ****************************************************************************
// 'class MessageHandler' definition
// ****************************************************************************

#ifdef USE_DEFAULT_CONSTRUCTOR_
MessageHandler::MessageHandler()
    : m_magicString(defs::DEFAULT_MAGIC_STRING)
{
    InitialiseMsgPool(0, 0);
}
#endif

#ifdef USE_EXPLICIT_MOVE_
MessageHandler::MessageHandler(MessageHandler&& mh)
    : m_magicString(defs::DEFAULT_MAGIC_STRING)
{
    InitialiseMsgPool(0, 0);
    *this = std::move(mh);
}

MessageHandler& MessageHandler::operator=(MessageHandler&& mh)
{
    std::swap(m_messageDispatcher, mh.m_messageDispatcher);
    m_magicString.swap(mh.m_magicString);
    std::swap(m_msgPoolIndex, mh.m_msgPoolIndex);
    m_msgPool.swap(mh.m_msgPool);
}
#endif

MessageHandler::MessageHandler(const defs::default_message_dispatcher_t& messageDispatcher,
                               std::string_view magicString, size_t memPoolMsgCount,
                               size_t defaultMsgSize)
    : m_messageDispatcher(messageDispatcher)
    , m_magicString(magicString)
{
    InitialiseMsgPool(memPoolMsgCount, defaultMsgSize);
}

size_t MessageHandler::CheckBytesLeftToRead(defs::char_buf_cspan_t message) const
{
    if (!CheckMessage(message))
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_WARNING("CheckBytesLeftToRead has not found complete header, "
                                 << sizeof(defs::MessageHeader) - message.size()
                                 << " bytes left to read for full header");
#endif
        return sizeof(defs::MessageHeader) - message.size();
    }

    if (std::strncmp(m_magicString.c_str(), message.data(), defs::MAGIC_STRING_LEN) != 0)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR(
            "Magic string error, received: "
            << string_utils::SafeConvertCharArrayToStdString(message.data(), defs::MAGIC_STRING_LEN)
            << ", expected: " << m_magicString);
#endif
        return std::numeric_limits<size_t>::max();
    }

    uint32_t totalLength;
    std::memcpy(&totalLength, message.data() + MESSAGE_LENGTH_OFFSET, sizeof(totalLength));

    if (totalLength < message.size())
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Message length error, header length field ("
                               << totalLength << ") < physical message size (" << message.size()
                               << ")");
#endif
        return std::numeric_limits<size_t>::max();
    }

    return totalLength - message.size();
}

void MessageHandler::MessageReceivedHandler(defs::char_buf_cspan_t message) const
{
    if (!CheckMessage(message))
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Incomplete message header");
#endif
    }

    auto receivedMessage = GetNewMessageObject();

    if (!TryConvertToPod<defs::MessageHeader>(receivedMessage->header, message))
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Failed to convert first 80 bytes of buffer to HGL_MSG_HDR");
#endif
        return;
    }

    if (receivedMessage->header.totalLength > defs::MESSAGE_HEADER_LEN)
    {
        receivedMessage->body.resize(message.size() - defs::MESSAGE_HEADER_LEN);
        std::memcpy(receivedMessage->body.data(), message.data() + defs::MESSAGE_HEADER_LEN, receivedMessage->body.size());
    }
	else
	{
		receivedMessage->body.clear();
	}

    m_messageDispatcher(receivedMessage);
}

bool MessageHandler::CheckMessage(defs::char_buf_cspan_t message)
{
    return message.size() >= sizeof(defs::MessageHeader);
}

void MessageHandler::InitialiseMsgPool(size_t memPoolMsgCount, size_t defaultMsgSize)
{
    m_msgPoolIndex = 0;

    if (0 == memPoolMsgCount)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_DEBUG("Receive message pool NOT being used because memPoolMsgCount = "
                               << memPoolMsgCount << " and defaultMsgSize = " << defaultMsgSize);
#endif
        m_msgPool.clear();
        return;
    }

#if defined(USE_SOCKET_DEBUG)
    DEBUG_MESSAGE_EX_DEBUG("Receive message pool will be used with memPoolMsgCount = "
                           << memPoolMsgCount << " and defaultMsgSize = " << defaultMsgSize);
#endif

    m_msgPool.resize(memPoolMsgCount);

    auto generateMsg = [defaultMsgSize]()
    {
        auto msg = std::make_shared<defs::default_received_message_t>();

        if (defaultMsgSize > 0)
        {
            msg->body.reserve(defaultMsgSize);
        }

        return msg;
    };

    std::generate(m_msgPool.begin(), m_msgPool.end(), generateMsg);
}

defs::default_received_message_ptr_t MessageHandler::GetNewMessageObject() const
{
    defs::default_received_message_ptr_t newMessage;

    if (m_msgPool.empty())
    {
        newMessage = std::make_shared<defs::default_received_message_t>();
    }
    else
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        newMessage = m_msgPool[m_msgPoolIndex];

        if (++m_msgPoolIndex >= m_msgPool.size())
        {
            m_msgPoolIndex = 0;
        }
    }

    return newMessage;
}

// ****************************************************************************
// Utility functions
// ****************************************************************************

constexpr const char ARCH_BIN[]{"binary"};
constexpr const char ARCH_PORTBIN[]{"portableBinary"};
constexpr const char ARCH_JSON[]{"json"};
constexpr const char ARCH_XML[]{"xml"};
constexpr const char ARCH_PROTOBUF[]{"protobuf"};
constexpr const char ARCH_FLATBUFFER[]{"flatBuffer"};
constexpr const char ARCH_MSGPACK[]{"messagePack"};
constexpr const char ARCH_RAW[]{"raw"};
constexpr const char ARCH_NULL[]{""};

std::string_view ArchiveTypeToString(defs::eArchiveType archiveType)
{
    switch (archiveType)
    {
    case defs::eArchiveType::binary:
        return ARCH_BIN;
    case defs::eArchiveType::portableBinary:
        return ARCH_PORTBIN;
    case defs::eArchiveType::raw:
        return ARCH_RAW;
    case defs::eArchiveType::json:
        return ARCH_JSON;
    case defs::eArchiveType::xml:
        return ARCH_XML;
    case defs::eArchiveType::protobuf:
        return ARCH_PROTOBUF;
    case defs::eArchiveType::flatBuffer:
        return ARCH_FLATBUFFER;
	case defs::eArchiveType::messagePack:
        return ARCH_MSGPACK;
    }

    return ARCH_NULL;
}

defs::eArchiveType StringToArchiveType(std::string_view archiveName)
{
    defs::eArchiveType archiveType;

    if (archiveName == ARCH_BIN)
    {
        archiveType = defs::eArchiveType::binary;
    }
    else if (archiveName == ARCH_PORTBIN)
    {
        archiveType = defs::eArchiveType::portableBinary;
    }
    else if (archiveName == ARCH_RAW)
    {
        archiveType = defs::eArchiveType::raw;
    }
    else if (archiveName == ARCH_JSON)
    {
        archiveType = defs::eArchiveType::json;
    }
    else if (archiveName == ARCH_XML)
    {
        archiveType = defs::eArchiveType::xml;
    }
    else if (archiveName == ARCH_PROTOBUF)
    {
        archiveType = defs::eArchiveType::protobuf;
    }
    else if (archiveName == ARCH_FLATBUFFER)
    {
        archiveType = defs::eArchiveType::flatBuffer;
    }
	else if (archiveName == ARCH_MSGPACK)
    {
        archiveType = defs::eArchiveType::messagePack;
    }
    else
    {
        archiveType = defs::eArchiveType::raw;
    }

    return archiveType;
}

void FillHeader(std::string_view magicString, defs::eArchiveType archiveType, int32_t messageId,
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
    std::sprintf(static_cast<char*>(header.magicString), "%s", magicString.data());
    std::sprintf(static_cast<char*>(header.responseAddress), "%s", responseAddress.first.data());
#else
    std::snprintf(static_cast<char*>(header.magicString),
                  sizeof(header.magicString),
                  "%s",
                  magicString.data());
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

MessageBuilder::MessageBuilder(std::string_view magicString)
    : m_magicString(magicString)
{
}

auto MessageBuilder::Build(int32_t messageId, const defs::connection_t& responseAddress) const
    -> defs::char_buf_cspan_t
{
    // Resize message buffer.
    auto totalLength = sizeof(defs::MessageHeader);
    m_messageBuffer.resize(totalLength);

    defs::MessageHeader* header = reinterpret_cast<defs::MessageHeader*>(m_messageBuffer.data());
    FillHeader(m_magicString, defs::eArchiveType::raw, messageId, responseAddress, 0, *header);

    return m_messageBuffer;
}

auto MessageBuilder::Build(defs::char_buf_cspan_t message, int32_t messageId,
                           const defs::connection_t& responseAddress,
                           defs::eArchiveType archiveType) const -> defs::char_buf_cspan_t
{
    if (message.empty())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("message is empty"));
    }

    // Resize message buffer.
    auto totalLength = sizeof(defs::MessageHeader) + message.size();
    m_messageBuffer.resize(totalLength);

    // Fill header.
    defs::MessageHeader* header = reinterpret_cast<defs::MessageHeader*>(m_messageBuffer.data());
    FillHeader(m_magicString,
               archiveType,
               messageId,
               responseAddress,
               static_cast<uint32_t>(message.size()),
               *header);

    auto writePosIter = std::next(m_messageBuffer.begin(), sizeof(defs::MessageHeader));
    std::copy(message.data(), message.data() + message.size(), writePosIter);

    return m_messageBuffer;
}

} // namespace messages
} // namespace asio
} // namespace core_lib
