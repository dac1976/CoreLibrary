
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
 * \file MessageUtils.cpp
 * \brief File containing message utils definitions.
 */

#include "../../Include/Asio/MessageUtils.hpp"
#include "boost/throw_exception.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace messages {

// ****************************************************************************
// 'class xMessageLengthError' definition
// ****************************************************************************
xMessageLengthError::xMessageLengthError()
	: exceptions::xCustomException("incorrect message length")
{
}

xMessageLengthError::xMessageLengthError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xMessageLengthError::~xMessageLengthError()
{
}

// ****************************************************************************
// 'class xMagicStringError' definition
// ****************************************************************************
xMagicStringError::xMagicStringError()
	: exceptions::xCustomException("incorrect magic string")
{
}

xMagicStringError::xMagicStringError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xMagicStringError::~xMagicStringError()
{
}


// ****************************************************************************
// 'class MessageHandler' definition
// ****************************************************************************

MessageHandler::MessageHandler(const defs::default_message_dispatcher_t& messageDispatcher
							   , const std::string& magicString)
	: m_messageDispatcher{messageDispatcher}
	, m_magicString{magicString}
{
}

size_t MessageHandler::CheckBytesLeftToRead(const defs::char_buffer_t& message) const
{
	CheckMessage(message);

	auto pHeader = reinterpret_cast<const defs::MessageHeader*>(&message.front());

	if (m_magicString != pHeader->magicString)
	{
		BOOST_THROW_EXCEPTION(xMagicStringError());
	}

	if (pHeader->totalLength < message.size())
	{
		BOOST_THROW_EXCEPTION(xMessageLengthError());
	}

	return pHeader->totalLength - message.size();
}

void MessageHandler::MessageReceivedHandler(const defs::char_buffer_t& message) const
{
	CheckMessage(message);

	auto pHeader = reinterpret_cast<const defs::MessageHeader*>(&message.front());
    auto receivedMessage = std::make_shared<defs::default_received_message_t>();
	receivedMessage->header = *pHeader;

    if (pHeader->totalLength > sizeof(defs::MessageHeader))
    {
        receivedMessage->body.assign(message.begin() + sizeof(defs::MessageHeader), message.end());
    }

	m_messageDispatcher(receivedMessage);
}

void MessageHandler::CheckMessage(const defs::char_buffer_t& message)
{
	if (message.size() < sizeof(defs::MessageHeader))
	{
		BOOST_THROW_EXCEPTION(xMessageLengthError());
	}
}

// ****************************************************************************
// Utility functions
// ****************************************************************************

auto FillHeader(const std::string& magicString, const defs::eArchiveType archiveType
                , const uint32_t messageId, const defs::connection_t& responseAddress)
    -> defs::MessageHeader
{
    defs::MessageHeader header;

    strncpy(header.magicString, magicString.c_str(), defs::MAGIC_STRING_LEN - 1);
    header.magicString[defs::MAGIC_STRING_LEN - 1] = 0;
    strncpy(header.responseAddress, responseAddress.first.c_str(), defs::RESPONSE_ADDRESS_LEN - 1);
    header.responseAddress[defs::RESPONSE_ADDRESS_LEN - 1] = 0;
    header.responsePort = responseAddress.second;
    header.messageId = messageId;
    header.archiveType = archiveType;

    return header;
}

// ****************************************************************************
// 'class MessageBuilder' definition
// ****************************************************************************

MessageBuilder::MessageBuilder(const defs::eArchiveType archiveType
                               , const std::string& magicString)
    : m_archiveType{archiveType}, m_magicString{magicString}
{
}

auto MessageBuilder::operator()(const uint32_t messageId
                                , const defs::connection_t& responseAddress) const
    -> defs::char_buffer_t
{
    auto header = FillHeader(m_magicString, m_archiveType, messageId, responseAddress);

    defs::char_buffer_t messageBuffer;
    messageBuffer.reserve(header.totalLength);

    const char* headerCharBuf = reinterpret_cast<const char*>(&header);
    std::copy(headerCharBuf, headerCharBuf + sizeof(header)
              , std::back_inserter(messageBuffer));

    return messageBuffer;
}

} // namespace messages
} // namespace asio
} // namespace core_lib
