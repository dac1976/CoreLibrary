
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
 * \file MessageUtils.h
 * \brief File containing message utils declaration.
 */

#ifndef MESSAGEUTILS
#define MESSAGEUTILS

#include "AsioDefines.h"
#include "Exceptions/DetailedException.h"
#include "Serialization/SerializeToVector.h"
#include <iterator>
#include <algorithm>
#include <cstring>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace messages
{

/*!
 * \brief Message length error exception.
 *
 * This exception class is intended to be thrown when a message
 * is received whose length doesn't match what is in the message
 * header.
 */
class CORE_LIBRARY_DLL_SHARED_API xMessageLengthError : public exceptions::DetailedException
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
    ~xMessageLengthError() override = default;
    /*! \brief Copy constructor. */
    xMessageLengthError(const xMessageLengthError&) = default;
    /*! \brief Copy assignment operator. */
    xMessageLengthError& operator=(const xMessageLengthError&) = default;
    /*! \brief Move constructor. */
    xMessageLengthError(xMessageLengthError&&) = default;
    /*! \brief Move assignment operator. */
    xMessageLengthError& operator=(xMessageLengthError&&) = default;
};

/*!
 * \brief Magic string error exception.
 *
 * This exception class is intended to be thrown when a message
 * is received whose magic string does not match what is expected.
 */
class CORE_LIBRARY_DLL_SHARED_API xMagicStringError : public exceptions::DetailedException
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
    ~xMagicStringError() override = default;
    /*! \brief Copy constructor. */
    xMagicStringError(const xMagicStringError&) = default;
    /*! \brief Copy assignment operator. */
    xMagicStringError& operator=(const xMagicStringError&) = default;
    /*! \brief Move constructor. */
    xMagicStringError(xMagicStringError&&) = default;
    /*! \brief Move assignment operator. */
    xMagicStringError& operator=(xMagicStringError&&) = default;
};

/*!
 * \brief Archive type error.
 *
 * This exception class is intended to be thrown when a message
 * is being constructed using an incorrect archive type.
 */
class CORE_LIBRARY_DLL_SHARED_API xArchiveTypeError : public exceptions::DetailedException
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
    ~xArchiveTypeError() override = default;
    /*! \brief Copy constructor. */
    xArchiveTypeError(const xArchiveTypeError&) = default;
    /*! \brief Copy assignment operator. */
    xArchiveTypeError& operator=(const xArchiveTypeError&) = default;
    /*! \brief Move constructor. */
    xArchiveTypeError(xArchiveTypeError&&) = default;
    /*! \brief Move assignment operator. */
    xArchiveTypeError& operator=(xArchiveTypeError&&) = default;
};

/*!
 * \brief Default message handler class.
 *
 * This is an example of a message handler functor that can be used by the network classes to handle
 * a received network message with header type MessageHeader and is used in the simple network
 * classes: SimpleTcpClient, SimpleTcpServer etc.
 */
class CORE_LIBRARY_DLL_SHARED_API MessageHandler final
{
public:
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Default constructor. */
    MessageHandler();
#else
    /*! \brief Default constructor. */
    MessageHandler() = default;
#endif
    /*!
     * \brief Initialisation constructor.
     * \param[in] messageDispatcher - Function object defining the message dispatcher callback
     * \param[in] magicString - Magic string used to identify the start of valid messages.
     */
    MessageHandler(const defs::default_message_dispatcher_t& messageDispatcher,
                   const std::string&                        magicString);
    /*! \brief Default destructor. */
    ~MessageHandler() = default;
    /*! \brief Deleted copy constructor. */
    MessageHandler(const MessageHandler&) = delete;
    /*! \brief Deleted copy assignment operator. */
    MessageHandler& operator=(const MessageHandler&) = delete;
    /*!
     * \brief Check bytes left to read method.
     * \param[in] message - A received message buffer.
     */
    size_t CheckBytesLeftToRead(const defs::char_buffer_t& message) const;
    /*!
     * \brief Message received handler method.
     * \param[in] message - A received message buffer.
     */
    void MessageReceivedHandler(const defs::char_buffer_t& message) const;

private:
    /*! \brief Message dispatcher function object. */
    defs::default_message_dispatcher_t m_messageDispatcher;
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Magic string. */
    const std::string m_magicString;
#else
    /*! \brief Magic string. */
    const std::string m_magicString{defs::DEFAULT_MAGIC_STRING};
#endif
    /*!
     * \brief Check message method.
     * \param[in] message - A received message buffer.
     */
    static void CheckMessage(const defs::char_buffer_t& message);
};

/*!
 * \brief Header filler function.
 * \param[in] magicString - A received message buffer.
 * \param[in] archiveType - The archive type used for message serialization.
 * \param[in] messageId - The unique message ID.
 * \param[in] responseAddress - The response connection details describing sender's address and
 * port.
 * \return A filled message header.
 *
 * This is an example of a function used within the network clases to build the message header that
 * is always prepended to the message to be sent over a socket. It is used in the simple network
 * classes, e.g. SimpleTcpClient, SimpleTcpServer etc., via the MessageBuilder functor.
 *
 * This function only works with headers of the type MessageHeader.
 */
defs::MessageHeader CORE_LIBRARY_DLL_SHARED_API
                    FillHeader(const std::string& magicString, const defs::eArchiveType archiveType,
                               const uint32_t messageId, const defs::connection_t& responseAddress);

/*!
 * \brief Archive type enumerator as a template class.
 *
 * This is the general case and always throws an xArchiveTypeError exception as a specialised
 * template version should always be created for each archive type.
 */
template <typename A> struct ArchiveTypeToEnum
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        BOOST_THROW_EXCEPTION(xArchiveTypeError("unknown archive type"));
        return defs::eArchiveType::raw;
    }
};

/*! \brief Archive type enumerator as a specialized template class for binary archives. */
template <> struct ArchiveTypeToEnum<serialize::archives::out_bin_t>
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::binary;
    }
};

/*! \brief Archive type enumerator as a specialized template class for portable binary archives. */
template <> struct ArchiveTypeToEnum<serialize::archives::out_port_bin_t>
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::portableBinary;
    }
};

/*! \brief Archive type enumerator as a specialized template class for raw data. */
template <> struct ArchiveTypeToEnum<serialize::archives::out_raw_t>
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::raw;
    }
};

/*! \brief Archive type enumerator as a specialized template class for json archives. */
template <> struct ArchiveTypeToEnum<serialize::archives::out_json_t>
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::json;
    }
};

/*! \brief Archive type enumerators as a specialized template class for xml archives. */
template <> struct ArchiveTypeToEnum<serialize::archives::out_xml_t>
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::xml;
    }
};

/*!
 * \brief Default message builder class.
 *
 * This is used in the case of the simple network classes: SimpleTcpClient, SimpleTcpServer etc. It
 * is used to build messages to be sent that require a MessageHeader followed by a message body.
 */
class CORE_LIBRARY_DLL_SHARED_API MessageBuilder final
{
public:
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Default constructor. */
    MessageBuilder();
#else
    /*! \brief Default constructor. */
    MessageBuilder() = default;
#endif
    /*!
     * \brief Initialisatn constructor.
     * \param[in] magicString - Magic stirng used to identify start of valid message.
     */
    explicit MessageBuilder(const std::string& magicString);
    /*! \brief Default destructor. */
    ~MessageBuilder() = default;
    /*! \brief Deleted copy constructor. */
    MessageBuilder(const MessageBuilder&) = delete;
    /*! \brief Deleted copy assignment operator. */
    MessageBuilder& operator=(const MessageBuilder&) = delete;
    /*!
     * \brief Build message method for header only messages.
     * \param[in] messageId - Unique ID for this message instance.
     * \param[in] responseAddress - Connection object describing sender's response address and port.
     * \return A filled message buffer.
     *
     * Sometimes network messages don't require a message body to be sent just some header
     * information such as a command to possibly request some data in response. In this case
     * we invoke this Build method.
     */
    defs::char_buffer_t Build(const uint32_t            messageId,
                              const defs::connection_t& responseAddress) const;
    /*!
     * \brief Build message method for header + messaage body messages.
     * \param[in] message - Object to be sent as message body, to be serialized as chosen archive
     * type
     * \param[in] messageId - Unique ID for this message instance.
     * \param[in] responseAddress - Connection opject describing sender's response address and port.
     * \return A filled message buffer.
     *
     * The first template argument T defines the message object's type.
     * The second template argument A defines the archive type for serialization.
     *
     * Invoke thid overload of the Build function when a message comprising a header and body is to
     * be sent.
     */
    template <typename T, typename A>
    defs::char_buffer_t Build(const T& message, const uint32_t messageId,
                              const defs::connection_t& responseAddress) const
    {
        const defs::eArchiveType archiveType = ArchiveTypeToEnum<A>().Enumerate();

        auto header = FillHeader(m_magicString, archiveType, messageId, responseAddress);
        serialize::char_vector_t body = serialize::ToCharVector<T, A>(message);

        // cppcheck-suppress functionStatic
        if (body.empty())
        {
            BOOST_THROW_EXCEPTION(xArchiveTypeError());
        }

        header.totalLength += static_cast<uint32_t>(body.size());

        defs::char_buffer_t messageBuffer;
        messageBuffer.reserve(header.totalLength);

        auto pHeaderCharBuf = reinterpret_cast<const char*>(&header);
        std::copy(
            pHeaderCharBuf, pHeaderCharBuf + sizeof(header), std::back_inserter(messageBuffer));

        std::copy(body.begin(), body.end(), std::back_inserter(messageBuffer));

        return messageBuffer;
    }

private:
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Magic string. */
    const std::string m_magicString;
#else
    /*! \brief Magic string. */
    const std::string m_magicString{defs::DEFAULT_MAGIC_STRING};
#endif
};

/*!
 * \brief Message builder wrapper function for header only messages.
 * \param[in] messageId - Unique message ID to insert into message header.
 * \param[in] responseAddress - The address and port where the server should send the response.
 * \param[in] fallbackResponseAddress - The address and port where the server should send the
 * response if the main responseAddress is a NULL_CONNECTION.
 * \param[in] messageBuilder - A message builder object of type MsgBldr that must have an interface
 * compatible with that of the class core_lib::asio::messages::MessageBuilder.
 * \return Returns a filled message buffer as a vector of bytes.
 *
 * This is the "header only" convenience function to build an outgoing network message in all the
 * network classes. It takes a templated arg to provide an actual message builder functor, such as
 * the MessageBuilder functor.
 */
template <typename MsgBldr>
defs::char_buffer_t
BuildMessage(const uint32_t messageId, const defs::connection_t& responseAddress,
             const defs::connection_t& fallbackResponseAddress, const MsgBldr& messageBuilder)
{
    auto responseConn =
        (responseAddress == defs::NULL_CONNECTION) ? fallbackResponseAddress : responseAddress;
    return messageBuilder.Build(messageId, responseConn);
}
/*!
 * \brief Message builder wrapper function for full messages with a header and a body.
 * \param[in] message - The message of type T to send behind the header serialized to an
 * boost::serialization-compatible archive of type A.
 * \param[in] messageId - Unique message ID to insert into message header.
 * \param[in] responseAddress - The address and port where the server should send the response.
 * \param[in] fallbackResponseAddress - The address and port where the server should send the
 * response if the main responseAddress is a NULL_CONNECTION.
 * \param[in] messageBuilder - A message builder object of type MsgBldr that must have an interface
 * compatible with that of the class core_lib::asio::messages::MessageBuilder.
 * \return Returns a filled message buffer as a vector of bytes.
 *
 * This is the "header plus body" convenience function to build an outgoing network message in all
 * the network classes. It takes a templated arg to provide an actual message builder functor, such
 * as the MessageBuilder functor. This variant as stated is for header only messages.
 */
template <typename T, typename A, typename MsgBldr>
defs::char_buffer_t
BuildMessage(const T& message, const uint32_t messageId, const defs::connection_t& responseAddress,
             const defs::connection_t& fallbackResponseAddress, const MsgBldr& messageBuilder)
{
    auto responseConn =
        (responseAddress == defs::NULL_CONNECTION) ? fallbackResponseAddress : responseAddress;
    return messageBuilder.template Build<T, A>(message, messageId, responseConn);
}

/*!
 * \brief Message deserialization error exception.
 *
 * This exception class is intended to be thrown when a message
 * cannot be deserialized correctly.
 */
class CORE_LIBRARY_DLL_SHARED_API xMessageDeserializationError : public exceptions::DetailedException
{
public:
    /*! \brief Default constructor. */
    xMessageDeserializationError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xMessageDeserializationError(const std::string& message);
    /*! \brief Virtual destructor. */
    ~xMessageDeserializationError() override = default;
    /*! \brief Copy constructor. */
    xMessageDeserializationError(const xMessageDeserializationError&) = default;
    /*! \brief Copy assignment operator. */
    xMessageDeserializationError& operator=(const xMessageDeserializationError&) = default;
    /*! \brief Move constructor. */
    xMessageDeserializationError(xMessageDeserializationError&&) = default;
    /*! \brief Move assignment operator. */
    xMessageDeserializationError& operator=(xMessageDeserializationError&&) = default;
};

/*!
 * \brief Templated message deserializer function.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \param[in] archiveType - Serialization archive type.
 * \return The deserialization object T.
 */
template <typename T>
T DeserializeMessage(const defs::char_buffer_t& messageBuffer, const defs::eArchiveType archiveType)
{
    switch (archiveType)
    {
    case defs::eArchiveType::binary:
        return serialize::ToObject<T, serialize::archives::in_bin_t>(messageBuffer);
    case defs::eArchiveType::portableBinary:
        return serialize::ToObject<T, serialize::archives::in_port_bin_t>(messageBuffer);
    case defs::eArchiveType::raw:
        return serialize::ToObject<T, serialize::archives::in_raw_t>(messageBuffer);
    case defs::eArchiveType::json:
        return serialize::ToObject<T, serialize::archives::in_json_t>(messageBuffer);
    case defs::eArchiveType::xml:
        return serialize::ToObject<T, serialize::archives::in_xml_t>(messageBuffer);
    default:
        BOOST_THROW_EXCEPTION(xMessageDeserializationError("unsupported archive type"));
    }

    return T();
}

} // namespace messages
} // namespace asio
} // namespace core_lib

#endif // MESSAGEUTILS
