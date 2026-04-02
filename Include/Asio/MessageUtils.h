
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
     * \param[in] memPoolMsgCount - (Optional) Pool size as number of messages.
     * \param[in] defaultMsgSize - (Optional) Initial size of a message in the pool.
     *
     * NOTE: We deliberately pass string by value and use std::move to make the copy.
     *       In C++11 onwards this is more efficient.
     *
     *       If you want to use a memory pool rather that dynamic allocations
     *       then set memPoolMsgCount > 0 and the pool will allow for the specified
     *       number of messages. If memPollMsgCount == 0 then dynamic memory allocation
     *       is used when handling each new message.
     */
    MessageHandler(const defs::default_message_dispatcher_t& messageDispatcher,
                 std::string_view magicString,
				 size_t memPoolMsgCount = 0,
                 size_t defaultMsgSize = defs::RECV_POOL_DEFAULT_MSG_SIZE);
    /*! \brief Default destructor. */
    ~MessageHandler() = default;
    /*! \brief Deleted copy constructor. */
    MessageHandler(const MessageHandler&) = delete;
    /*! \brief Deleted copy assignment operator. */
    MessageHandler& operator=(const MessageHandler&) = delete;
    /*!
     * \brief Check bytes left to read method.
     * \param[in] message - A received message buffer.
     * \return Num bytes left to read or std::numeric_limits<size_t>::max()
     *         if there is a problem.
     */
    size_t CheckBytesLeftToRead(defs::char_buf_cspan_t message) const;
    /*!
     * \brief Message received handler method.
     * \param[in] message - A received message buffer.
     */
    void MessageReceivedHandler(defs::char_buf_cspan_t message) const;

private:
    /*!
     * \brief Check message method.
     * \param[in] message - A received message buffer.
     */
    static bool CheckMessage(defs::char_buf_cspan_t message);
    /*!
     * \brief Initialise message pool.
     * \param[in] memPoolMsgCount - Pool size as number of messages.
     */
    void InitialiseMsgPool(size_t memPoolMsgCount);
    /*!
     * \brief Get next message to use from pool.
     * \return A new message object or a one from the pool.
     */
    defs::default_received_message_ptr_t GetNewMessageObject(size_t requiredSize) const;

private:
    mutable std::mutex m_mutex;
    /*! \brief Message dispatcher function object. */
    defs::default_message_dispatcher_t m_messageDispatcher;
#if defined(USE_DEFAULT_CONSTRUCTOR_)
    /*! \brief Magic string. */
    std::string m_magicString;
#else
    /*! \brief Magic string. */
    std::string m_magicString{defs::DEFAULT_MAGIC_STRING};
#endif
    /*! \brief Default pool message size. */
    size_t m_defaultMsgSize{defs::RECV_POOL_DEFAULT_MSG_SIZE};
	/*! \brief Message pool index tracker */
    mutable size_t  m_msgPoolIndex{0};
	/*! \brief The message pool. */
    std::vector<asio::defs::default_received_message_ptr_t> m_msgPool;
};

/*!
 * \brief Stringify archive type.
 * \param[in] archiveType - Type of archive used.
 * \return The human readable name of the archive type.
 */
CORE_LIBRARY_DLL_SHARED_API std::string_view ArchiveTypeToString(defs::eArchiveType archiveType);

/*!
 * \brief Unstringify archive type.
 * \param[in] archiveName - Type of archive used as a string.
 * \return The archive type.
 */
CORE_LIBRARY_DLL_SHARED_API defs::eArchiveType StringToArchiveType(std::string_view archiveName);

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
CORE_LIBRARY_DLL_SHARED_API void FillHeader(std::string_view magicString,
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

/*! \brief Archive type enumerators as a specialized template class for MessagePack archives. */
template <> struct ArchiveTypeToEnum<serialize::archives::out_msgpack_t>
{
    /*!
     * \brief Enumerate method.
     * \return The enumerated type.
     */
    static defs::eArchiveType Enumerate()
    {
        return defs::eArchiveType::messagePack;
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
    explicit MessageBuilder(std::string_view magicString);
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
    defs::char_buf_cspan_t Build(int32_t                   messageId,
                                 const defs::connection_t& responseAddress) const;
    /*!
     * \brief Build message method for header plus buffer.
     * \param[in] message - Message buffer created outside of the message builder.
     * \param[in] messageId - Unique ID for this message instance.
     * \param[in] responseAddress - Connection object describing sender's response address and port.
     * \param[in] archiveType - Archive type used to when creating the messageBuffer.
     * \return A const reference to a filled message buffer.
     */
    defs::char_buf_cspan_t
    Build(defs::char_buf_cspan_t message, int32_t messageId,
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
    defs::char_buf_cspan_t Build(const T& message, int32_t messageId,
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
defs::char_buf_cspan_t
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
 * \param[in] archiveType - Archive type used to when creating the messageBuffer. Depends on how
 * message buffer has been serialised.
 * \return Returns a const reference to a filled message buffer as a vector of bytes.
 *
 * This is the header + payload convenience function to build an outgoing network message in all the
 * network classes. It takes a templated arg to provide an actual message builder functor, such as
 * the MessageBuilder functor.
 */
template <typename MsgBldr>
defs::char_buf_cspan_t BuildMessage(defs::char_buf_cspan_t message, int32_t messageId,
                                    const defs::connection_t& responseAddress,
                                    const defs::connection_t& fallbackResponseAddress,
                                    const MsgBldr&            messageBuilder,
                                    defs::eArchiveType        archiveType = defs::eArchiveType::raw)
{
    auto responseConn =
        (responseAddress == defs::NULL_CONNECTION) ? fallbackResponseAddress : responseAddress;
    return messageBuilder.Build(message, messageId, responseConn, archiveType);
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
defs::char_buf_cspan_t
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
 */
template <typename T>
T DeserializeMessage(defs::char_buf_cspan_t messageBuffer, defs::eArchiveType archiveType)
{
    assert((archiveType != defs::eArchiveType::raw) &&
           (archiveType != defs::eArchiveType::protobuf) &&
           (archiveType != defs::eArchiveType::flatBuffer) &&
           (archiveType != defs::eArchiveType::messagePack));

    switch (archiveType)
    {
    case defs::eArchiveType::binary:
        return serialize::ToObject<T, serialize::archives::in_bin_t>(messageBuffer);
    case defs::eArchiveType::portableBinary:
        return serialize::ToObject<T, serialize::archives::in_port_bin_t>(messageBuffer);
     case defs::eArchiveType::json:
        return serialize::ToObject<T, serialize::archives::in_json_t>(messageBuffer);
    case defs::eArchiveType::xml:
        return serialize::ToObject<T, serialize::archives::in_xml_t>(messageBuffer);
    case defs::eArchiveType::raw:
    case defs::eArchiveType::protobuf:
    case defs::eArchiveType::flatBuffer:
	case defs::eArchiveType::messagePack:
    // Do nothing;
        break;

    }

    return T();
}

/*!
 * \brief Templated message deserializer function for POD data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \return The deserialization object T.
 */
template <typename T> T DeserializeMessage(defs::char_buf_cspan_t messageBuffer)
{
    return serialize::ToObject<T, serialize::archives::in_raw_t>(messageBuffer);
}

/*!
 * \brief Templated message deserializer function for Google protocol buffer data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \return The deserialization object T.
 */
template <typename T> T DeserializeProtobuf(defs::char_buf_cspan_t messageBuffer)
{
    return serialize::ToObject<T, serialize::archives::in_protobuf_t>(messageBuffer);
}

/*!
 * \brief Templated message deserializer function for MessagePack data.
 * \param[in] messageBuffer - Message buffer to be deserialized.
 * \return The deserialization object T.
 */
template <typename T> T DeserializeMessagePack(defs::char_buf_cspan_t messageBuffer)
{
    return serialize::ToObject<T, serialize::archives::in_msgpack_t>(messageBuffer);
}

} // namespace messages
} // namespace asio
} // namespace core_lib

#endif // MESSAGEUTILS
