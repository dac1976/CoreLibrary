
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
 * \file MessageUtils.hpp
 * \brief File containing message utils declaration.
 */

#ifndef MESSAGEUTILS_HPP
#define MESSAGEUTILS_HPP

#include "AsioDefines.hpp"
#include "../Exceptions/CustomException.hpp"
#include "../Serialization/SerializeToVector.hpp"
#include <iterator>
#include <algorithm>
#include <cstring>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace messages {

/*!
 * \brief Message length error exception.
 *
 * This exception class is intended to be thrown when a message
 * is received whose length doesn't match what is in the message
 * header.
 */
class xMessageLengthError : public exceptions::xCustomException
{
public:
	/*! \brief Default constructor. */
	xMessageLengthError();
	/*!
	 * \brief Initializing constructor.
	 * \param[in] message - A user specified message string.
	 */
	explicit xMessageLengthError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xMessageLengthError();
	/*! \brief Copy constructor. */
	xMessageLengthError(const xMessageLengthError&) = default;
	/*! \brief Move constructor. */
	xMessageLengthError(xMessageLengthError&&) = default;
	/*! \brief Copy assignment operator. */
	xMessageLengthError& operator=(const xMessageLengthError&) = default;
	/*! \brief Move assignment operator. */
	xMessageLengthError& operator=(xMessageLengthError&&) = default;
};

/*!
 * \brief Magic string error exception.
 *
 * This exception class is intended to be thrown when a message
 * is received whose magic string does not match what is expected.
 */
class xMagicStringError : public exceptions::xCustomException
{
public:
	/*! \brief Default constructor. */
	xMagicStringError();
	/*!
	 * \brief Initializing constructor.
	 * \param[in] message - A user specified message string.
	 */
	explicit xMagicStringError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xMagicStringError();
	/*! \brief Copy constructor. */
	xMagicStringError(const xMagicStringError&) = default;
	/*! \brief Move constructor. */
	xMagicStringError(xMagicStringError&&) = default;
	/*! \brief Copy assignment operator. */
	xMagicStringError& operator=(const xMagicStringError&) = default;
	/*! \brief Move assignment operator. */
	xMagicStringError& operator=(xMagicStringError&&) = default;
};

class MessageHandler final
{
public:
	MessageHandler(defs::message_dispatcher messageDispatcher);
	~MessageHandler() = default;
	MessageHandler(const MessageHandler& ) = delete;
	MessageHandler& operator=(const MessageHandler& ) = delete;

	static size_t CheckBytesLeftToRead(const defs::char_buffer& message);

	void MessageReceivedHandler(const defs::char_buffer& message);

private:
	defs::message_dispatcher m_messageDispatcher;

	static void CheckMessage(const defs::char_buffer& message);
};

auto BuildMessageBufferHeaderOnly(const uint32_t messageId, const defs::connection& responseAddress
							   , const defs::eArchiveType archive) -> defs::char_buffer;

template<typename T>
auto BuildMessageBuffer(const T& message, const uint32_t messageId, const defs::connection& responseAddress
						, const defs::eArchiveType archive)
	-> defs::char_buffer
{
	defs::MessageHeader header;
	strncpy(header.responseAddress, responseAddress.first.c_str(), responseAddress.first.length());
	header.responseAddress[defs::RESPONSE_ADDRESS_LEN - 1] = 0;
	header.responsePort = responseAddress.second;
	header.messageId = messageId;
	header.archiveType = archive;

	serialize::char_vector body;

	switch(archive)
	{
		case defs::eArchiveType::text:
			body = serialize::ToCharVector<T, boost_arch::text_oarchive>(message);
			break;
		case defs::eArchiveType::binary:
			body = serialize::ToCharVector<T, boost_arch::binary_oarchive>(message);
			break;
		case defs::eArchiveType::xml:
			body = serialize::ToCharVector<T, boost_arch::xml_oarchive>(message);
			break;
		case defs::eArchiveType::portableBinary:
		default:
			body = serialize::ToCharVector<T, eos::portable_oarchive>(message);
			break;
	}

	header.totalLength += body.size();

	defs::char_buffer messageBuffer;
	messageBuffer.reserve(header.totalLength);

	auto pHeaderCharBuf = reinterpret_cast<const char*>(&header);
	std::copy(pHeaderCharBuf, pHeaderCharBuf + sizeof(header)
			  , std::back_inserter(messageBuffer));

	std::copy(body.begin(), body.end()
			  , std::back_inserter(messageBuffer));

	return messageBuffer;
}

} // namespace messages
} // namespace asio
} // namespace core_lib

#endif // MESSAGEUTILS_HPP
