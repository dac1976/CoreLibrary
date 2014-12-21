
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

#include "MessageUtils.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace messages {

MessageHandler::MessageHandler(defs::message_dispatcher messageDispatcher)
	: m_messageDispatcher{messageDispatcher}
{
}

void MessageHandler::CheckMessage(const defs::char_buffer& message)
{
	if (message.size() < sizeof(defs::MessageHeader))
	{
		throw std::length_error("message buffer contains too few bytes");
	}
}

size_t MessageHandler::CheckBytesLeftToRead(const defs::char_buffer& message)
{
	CheckMessage(message);

	const defs::MessageHeader* pHeader
		= reinterpret_cast<const defs::MessageHeader*>(&message.front());

	if (std::string(pHeader->magicString) != defs::MAGIC_STRING)
	{
		throw std::runtime_error("cannot find magic string");
	}

	if (pHeader->totalLength < message.size())
	{
		throw std::length_error("invalid total length in header");
	}

	return pHeader->totalLength - message.size();
}

void MessageHandler::MessageReceivedHandler(const defs::char_buffer& message)
{
	CheckMessage(message);

	const defs::MessageHeader* pHeader
		= reinterpret_cast<const defs::MessageHeader*>(&message.front());

	defs::ReceivedMessage receivedMessage;
	receivedMessage.header = *pHeader;
	receivedMessage.body.assign(message.begin() + sizeof(defs::MessageHeader), message.end());

	m_messageDispatcher(receivedMessage);
}

} // namespace messages
} // namespace asio
} // namespace core_lib
