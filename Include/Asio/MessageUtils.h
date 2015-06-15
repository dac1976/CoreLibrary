
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
 * \file MessageUtils.h
 * \brief File containing message utils declaration.
 */

#ifndef MESSAGEUTILS
#define MESSAGEUTILS

#include "AsioDefines.h"
#include "../Exceptions/CustomException.h"
#include "../Serialization/SerializeToVector.h"
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

/*!
 * \brief Archive type error.
 *
 * This exception class is intended to be thrown when a message
 * is being constructed using an incorrect archive type.
 */
class xArchiveTypeError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xArchiveTypeError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xArchiveTypeError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xArchiveTypeError();
    /*! \brief Copy constructor. */
    xArchiveTypeError(const xArchiveTypeError&) = default;
    /*! \brief Move constructor. */
    xArchiveTypeError(xArchiveTypeError&&) = default;
    /*! \brief Copy assignment operator. */
    xArchiveTypeError& operator=(const xArchiveTypeError&) = default;
    /*! \brief Move assignment operator. */
    xArchiveTypeError& operator=(xArchiveTypeError&&) = default;
};

class MessageHandler final
{
public:
    MessageHandler(const defs::default_message_dispatcher_t& messageDispatcher
				   , const std::string& magicString);
	~MessageHandler() = default;
	MessageHandler(const MessageHandler& ) = delete;
	MessageHandler& operator=(const MessageHandler& ) = delete;

    size_t CheckBytesLeftToRead(const defs::char_buffer_t& message) const;

    void MessageReceivedHandler(const defs::char_buffer_t& message) const;

private:
    defs::default_message_dispatcher_t m_messageDispatcher;
	const std::string m_magicString{defs::DEFAULT_MAGIC_STRING};

    static void CheckMessage(const defs::char_buffer_t& message);
};

auto FillHeader(const std::string& magicString, const defs::eArchiveType archiveType
                , const uint32_t messageId, const defs::connection_t& responseAddress)
         -> defs::MessageHeader;

/*! \brief Archive type enumerators using template specialization. */
template<typename A>
struct ArchiveTypeToEnum
{
    static defs::eArchiveType Enumerate()
    {
        BOOST_THROW_EXCEPTION(xArchiveTypeError("unknown archive type"));
        return defs::eArchiveType::raw;
    }
};

template<>
struct ArchiveTypeToEnum<serialize::archives::out_bin_t>
{
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::binary;
    }
};

template<>
struct ArchiveTypeToEnum<serialize::archives::out_port_bin_t>
{
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::portableBinary;
    }
};

template<>
struct ArchiveTypeToEnum<serialize::archives::out_raw_t>
{
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::raw;
    }
};

template<>
struct ArchiveTypeToEnum<serialize::archives::out_txt_t>
{
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::text;
    }
};

template<>
struct ArchiveTypeToEnum<serialize::archives::out_xml_t>
{
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::xml;
    }
};

class MessageBuilder final
{
public:
    MessageBuilder() = default;
    explicit MessageBuilder(const std::string& magicString);
    ~MessageBuilder() = default;

    MessageBuilder(const MessageBuilder& ) = delete;
    MessageBuilder& operator=(const MessageBuilder& ) = delete;

    auto Build(const uint32_t messageId
               , const defs::connection_t& responseAddress) const -> defs::char_buffer_t;

    template<typename T, typename A>
    auto Build(const T& message
               , const uint32_t messageId
               , const defs::connection_t& responseAddress) const -> defs::char_buffer_t
    {
        const defs::eArchiveType archiveType = ArchiveTypeToEnum<A>().Enumerate();

        auto header = FillHeader(m_magicString, archiveType, messageId, responseAddress);
        serialize::char_vector_t body
            = serialize::ToCharVector<T, A>(message);

		// cppcheck-suppress functionStatic
        if (body.empty())
        {
            BOOST_THROW_EXCEPTION(xArchiveTypeError());
        }

        header.totalLength += body.size();

        defs::char_buffer_t messageBuffer;
        messageBuffer.reserve(header.totalLength);

        auto pHeaderCharBuf = reinterpret_cast<const char*>(&header);
        std::copy(pHeaderCharBuf, pHeaderCharBuf + sizeof(header)
                  , std::back_inserter(messageBuffer));

        std::copy(body.begin(), body.end()
                  , std::back_inserter(messageBuffer));

        return messageBuffer;
    }

private:
    const std::string m_magicString{defs::DEFAULT_MAGIC_STRING};
};

} // namespace messages
} // namespace asio
} // namespace core_lib

#endif // MESSAGEUTILS
