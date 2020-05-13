
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

#include <iterator>
#include <algorithm>
#include <cstring>
#include <cassert>
#include "AsioDefines.h"
#include "Serialization/SerializeToVector.h"

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
    /*! \brief Default copy constructor. */
    MessageHandler(const MessageHandler&) = default;
    /*! \brief Default copy assignment operator. */
    MessageHandler& operator=(const MessageHandler&) = default;

#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Default move constructor. */
    MessageHandler(MessageHandler&& mh);
    /*! \brief Default move assignment operator. */
    MessageHandler& operator=(MessageHandler&& mh);
#else
    /*! \brief Default move constructor. */
    MessageHandler(MessageHandler&&) = default;
    /*! \brief Default move assignment operator. */
    MessageHandler& operator=(MessageHandler&&) = default;
#endif
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
    std::string m_magicString;
#else
    /*! \brief Magic string. */
    std::string m_magicString{static_cast<char const*>(defs::DEFAULT_MAGIC_STRING)};
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
 * \param[in] messageLength - The length of the message not including the header.
 * \param[out] header - The message header to be filled out.
 * \return A filled message header.
 *
 * This is an example of a function used within the network clases to build the message header that
 * is always prepended to the message to be sent over a socket. It is used in the simple network
 * classes, e.g. SimpleTcpClient, SimpleTcpServer etc., via the MessageBuilder functor.
 *
 * This function only works with headers of the type MessageHeader.
 */
void CORE_LIBRARY_DLL_SHARED_API FillHeader(const std::string& magicString,
                                            defs::eArchiveType archiveType, int32_t messageId,
                                            const defs::connection_t& responseAddress,
                                            uint32_t messageLength, defs::MessageHeader& header);

/*!
 * \brief Archive type enumerator as a template class.
 *
 * This is the general case and always throws an std::invalid_argument exception as a specialised
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
        BOOST_THROW_EXCEPTION(std::invalid_argument("unknown archive type"));
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

/*! \brief Archive type enumerator as a specialized template class for Google protocol buffer data.
 */
template <> struct ArchiveTypeToEnum<serialize::archives::out_protobuf_t>
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::protobuf;
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
    /*! \brief Default copy constructor. */
    MessageBuilder(const MessageBuilder&) = default;
    /*! \brief Default copy assignment operator. */
    MessageBuilder& operator=(const MessageBuilder&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Default move constructor. */
    MessageBuilder(MessageBuilder&& mb);
    /*! \brief Default move assignment operator. */
    MessageBuilder& operator=(MessageBuilder&& mb);
#else
    /*! \brief Default move constructor. */
    MessageBuilder(MessageBuilder&&) = default;
    /*! \brief Default move assignment operator. */
    MessageBuilder& operator=(MessageBuilder&&) = default;
#endif
    /*!
     * \brief Build message method for header only messages.
     * \param[in] messageId - Unique ID for this message instance.
     * \param[in] responseAddress - Connection object describing sender's response address and port.
     * \return A const reference to a filled message buffer.
     *
     * Sometimes network messages don't require a message body to be sent just some header
     * information such as a command to possibly request some data in response. In this case
     * we invoke this Build method.
     */
    defs::char_buffer_t const& Build(int32_t                   messageId,
                                     const defs::connection_t& responseAddress) const;
    /*!
     * \brief Build message method for header plus buffer.
     * \param[in] message - Message buffer created outside of the message builder.
     * \param[in] messageId - Unique ID for this message instance.
     * \param[in] responseAddress - Connection object describing sender's response address and port.
     * \param[in] archiveType - Archive type used to when creating the messageBuffer.
     * \return A const reference to a filled message buffer.
     */
    defs::char_buffer_t const&
    Build(const defs::char_buffer_t& message, int32_t messageId,
          const defs::connection_t& responseAddress,
          defs::eArchiveType        archiveType = defs::eArchiveType::raw) const;
    /*!
     * \brief Build message method for header plus buffer.
     * \param[in] message - Start of message buffer created outside of the message builder.
     * \param[in] messageLength - Length of the message buffer in bytes.
     * \param[in] messageId - Unique ID for this message instance.
     * \param[in] responseAddress - Connection object describing sender's response address and port.
     * \param[in] archiveType - Archive type used to when creating the messageBuffer.
     * \return A const reference to a filled message buffer.
     */
    defs::char_buffer_t const&
    Build(const void* message, size_t messageLength, int32_t messageId,
          const defs::connection_t& responseAddress,
          defs::eArchiveType        archiveType = defs::eArchiveType::raw) const;
    /*!
     * \brief Build message method for header + messaage body messages.
     * \param[in] message - Object to be sent as message body, to be serialized as chosen archive
     * type
     * \param[in] messageId - Unique ID for this message instance.
     * \param[in] responseAddress - Connection opject describing sender's response address and port.
     * \return A const reference to a filled message buffer.
     *
     * The first template argument T defines the message object's type.
     * The second template argument A defines the archive type for serialization.
     *
     * Invoke thid overload of the Build function when a message comprising a header and body is to
     * be sent.
     */
    template <typename T, typename A>
    defs::char_buffer_t const& Build(const T& message, int32_t messageId,
                                     const defs::connection_t& responseAddress) const
    {
        // Serialise message.
        serialize::ToCharVector<T, A>(message, m_serialisationBuffer);

        if (m_serialisationBuffer.empty())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot serialize message"));
        }

        // Resize message buffer.
        auto totalLength = sizeof(defs::MessageHeader) + m_serialisationBuffer.size();
        m_messageBuffer.resize(totalLength);

        // Fill header.
        const defs::eArchiveType archiveType = ArchiveTypeToEnum<A>().Enumerate();
        defs::MessageHeader*     header =
            reinterpret_cast<defs::MessageHeader*>(m_messageBuffer.data());
        FillHeader(m_magicString,
                   archiveType,
                   messageId,
                   responseAddress,
                   static_cast<uint32_t>(m_serialisationBuffer.size()),
                   *header);

        auto writePosIter = std::next(m_messageBuffer.begin(), sizeof(defs::MessageHeader));
        std::copy(m_serialisationBuffer.begin(), m_serialisationBuffer.end(), writePosIter);

        return m_messageBuffer;
    }

private:
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Magic string. */
    std::string m_magicString;
#else
    /*! \brief Magic string. */
    std::string m_magicString{static_cast<char const*>(defs::DEFAULT_MAGIC_STRING)};
#endif
    mutable defs::char_buffer_t m_messageBuffer;
    mutable defs::char_buffer_t m_serialisationBuffer;
};

/*!
 * \brief Message builder wrapper function for header only messages.
 * \param[in] messageId - Unique message ID to insert into message header.
 * \param[in] responseAddress - The address and port where the server should send the response.
 * \param[in] fallbackResponseAddress - The address and port where the server should send the
 * response if the main responseAddress is a NULL_CONNECTION.
 * \param[in] messageBuilder - A message builder object of type MsgBldr that must have an interface
 * compatible with that of the class core_lib::asio::messages::MessageBuilder.
 * \return Returns a const reference to a filled message buffer as a vector of bytes.
 *
 * This is the "header only" convenience function to build an outgoing network message in all the
 * network classes. It takes a templated arg to provide an actual message builder functor, such as
 * the MessageBuilder functor.
 */
template <typename MsgBldr>
defs::char_buffer_t const&
BuildMessage(int32_t messageId, const defs::connection_t& responseAddress,
             const defs::connection_t& fallbackResponseAddress, const MsgBldr& messageBuilder)
{
    auto responseConn =
        (responseAddress == defs::NULL_CONNECTION) ? fallbackResponseAddress : responseAddress;
    return messageBuilder.Build(messageId, responseConn);
}
/*!
 * \brief Message builder wrapper function for header plus message buffer.
 * \param[in] message - Message buffer.
 * \param[in] messageId - Unique message ID to insert into message header.
 * \param[in] responseAddress - The address and port where the server should send the response.
 * \param[in] fallbackResponseAddress - The address and port where the server should send the
 * response if the main responseAddress is a NULL_CONNECTION.
 * \param[in] messageBuilder - A message builder object of type MsgBldr that must have an interface
 * compatible with that of the class core_lib::asio::messages::MessageBuilder.
 * \return Returns a const reference to a filled message buffer as a vector of bytes.
 *
 * This is the "header only" convenience function to build an outgoing network message in all the
 * network classes. It takes a templated arg to provide an actual message builder functor, such as
 * the MessageBuilder functor.
 */
template <typename MsgBldr>
defs::char_buffer_t const& BuildMessage(defs::char_buffer_t const& message, int32_t messageId,
                                        const defs::connection_t& responseAddress,
                                        const defs::connection_t& fallbackResponseAddress,
                                        const MsgBldr&            messageBuilder)
{
    auto responseConn =
        (responseAddress == defs::NULL_CONNECTION) ? fallbackResponseAddress : responseAddress;
    return messageBuilder.Build(message, messageId, responseConn);
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
 * \return Returns a const reference to a  filled message buffer as a vector of bytes.
 *
 * This is the "header plus body" convenience function to build an outgoing network message in all
 * the network classes. It takes a templated arg to provide an actual message builder functor, such
 * as the MessageBuilder functor. This variant as stated is for header only messages.
 */
template <typename T, typename A, typename MsgBldr>
defs::char_buffer_t const&
BuildMessage(const T& message, int32_t messageId, const defs::connection_t& responseAddress,
             const defs::connection_t& fallbackResponseAddress, const MsgBldr& messageBuilder)
{
    auto responseConn =
        (responseAddress == defs::NULL_CONNECTION) ? fallbackResponseAddress : responseAddress;
    return messageBuilder.template Build<T, A>(message, messageId, responseConn);
}

/*!
 * \brief Templated message deserializer function for non-POD data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \param[in] archiveType - Serialization archive type.
 * \return The deserialization object T.
 *
 * This overload creates new memory, i.e. the object of type T that is returned.
 */
template <typename T>
T DeserializeMessage(const defs::char_buffer_t& messageBuffer, defs::eArchiveType archiveType)
{
    assert((archiveType != defs::eArchiveType::raw) &&
           (archiveType != defs::eArchiveType::protobuf));

    switch (archiveType)
    {
    case defs::eArchiveType::binary:
        return serialize::ToObject<T, serialize::archives::in_bin_t>(messageBuffer);
    case defs::eArchiveType::portableBinary:
        return serialize::ToObject<T, serialize::archives::in_port_bin_t>(messageBuffer);
    case defs::eArchiveType::raw:
        // Do nothing.
        break;
    case defs::eArchiveType::json:
        return serialize::ToObject<T, serialize::archives::in_json_t>(messageBuffer);
    case defs::eArchiveType::xml:
        return serialize::ToObject<T, serialize::archives::in_xml_t>(messageBuffer);
    case defs::eArchiveType::protobuf:
        // Do nothing;
        break;
    }

    return T();
}

/*!
 * \brief Templated message deserializer function for non-POD data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \param[in] archiveType - Serialization archive type.
 * \param[out] result - The deserialization object T.
 *
 * This overload uses the memory passed in, i.e. the object of type T.
 */
template <typename T>
void DeserializeMessage(const defs::char_buffer_t& messageBuffer, defs::eArchiveType archiveType,
                        T& result)
{
    assert((archiveType != defs::eArchiveType::raw) &&
           (archiveType != defs::eArchiveType::protobuf));

    switch (archiveType)
    {
    case defs::eArchiveType::binary:
        serialize::ToObject<T, serialize::archives::in_bin_t>(messageBuffer, result);
        break;
    case defs::eArchiveType::portableBinary:
        serialize::ToObject<T, serialize::archives::in_port_bin_t>(messageBuffer, result);
        break;
    case defs::eArchiveType::raw:
        // Do nothing.
        break;
    case defs::eArchiveType::json:
        serialize::ToObject<T, serialize::archives::in_json_t>(messageBuffer, result);
        break;
    case defs::eArchiveType::xml:
        serialize::ToObject<T, serialize::archives::in_xml_t>(messageBuffer, result);
        break;
    case defs::eArchiveType::protobuf:
        // Do nothing;
        break;
    }
}

/*!
 * \brief Templated message deserializer function for POD data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \return The deserialization object T.
 *
 * This overload creates new memory, i.e. the object of type T that is returned.
 */
template <typename T> T DeserializeMessage(const defs::char_buffer_t& messageBuffer)
{
    return serialize::ToObject<T, serialize::archives::in_raw_t>(messageBuffer);
}

/*!
 * \brief Templated message deserializer function for POD data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \param[out] result - The deserialization object T.
 *
 * This overload uses the memory passed in, i.e. the object of type T.
 */
template <typename T> void DeserializeMessage(const defs::char_buffer_t& messageBuffer, T& result)
{
    serialize::ToObject<T, serialize::archives::in_raw_t>(messageBuffer, result);
}

/*!
 * \brief Templated message deserializer function for Google protocol buffer data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \return The deserialization object T.
 *
 * This overload creates new memory, i.e. the object of type T that is returned.
 */
template <typename T> T DeserializeProtobuf(const defs::char_buffer_t& messageBuffer)
{
    return serialize::ToObject<T, serialize::archives::in_protobuf_t>(messageBuffer);
}

/*!
 * \brief Templated message deserializer function for Google protocol buffer data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \param[out] result - The deserialization object T.
 *
 * This overload uses the memory passed in, i.e. the object of type T.
 */
template <typename T> void DeserializeProtobuf(const defs::char_buffer_t& messageBuffer, T& result)
{
    serialize::ToObject<T, serialize::archives::in_protobuf_t>(messageBuffer, result);
}

} // namespace messages
} // namespace asio
} // namespace core_lib

#endif // MESSAGEUTILS
