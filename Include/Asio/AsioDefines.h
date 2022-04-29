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
 * \file AsioDefines.h
 * \brief File containing useful definitions.
 */

#ifndef ASIODEFINES
#define ASIODEFINES

#include <vector>
#include <functional>
#include <memory>
#include <utility>
#include <string>
#include <cstdint>
#include <boost/asio.hpp>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"

namespace boost_sys          = boost::system;
namespace boost_asio         = boost::asio;
namespace boost_placeholders = boost::asio::placeholders;
namespace boost_mcast        = boost::asio::ip::multicast;

/*! \brief Boost IO context convenience typedef. */
using boost_iocontext_t = boost_asio::io_context;
/*! \brief Boost tcp convenience typedef. */
using boost_tcp_t = boost_asio::ip::tcp;
/*! \brief Boost tcp acceptor convenience typedef. */
using boost_tcp_acceptor_t = boost::asio::ip::tcp::acceptor;
/*! \brief Boost udp convenience typedef. */
using boost_udp_t = boost::asio::ip::udp;
/*! \brief Boost general IP address convenience typedef. */
using boost_address_t = boost::asio::ip::address;
/*! \brief Boost IPV4 address convenience typedef. */
using boost_address_v4_t = boost::asio::ip::address_v4;

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{

/*! \brief The tcp namespace. */
namespace tcp
{

/*! \brief Default internal receive buffer's initial reserved size in bytes. */
enum eDefReservedSize : size_t
{
    DEFAULT_RESERVED_SIZE = 512 * 1024
};

/*! \brief Maximum number of unsent async messages allowed on TCP socket IO Service queue */
enum eDefUnsentAsyncCount : size_t
{
    MAX_UNSENT_ASYNC_MSG_COUNT = 1000
};

/*! \brief Enumeration to control nagle algorithm. */
enum class eSendOption
{
    /*! \brief nagleOff - Send immediately. */
    nagleOff,
    /*! \brief nagleOn - Send when possible. */
    nagleOn
};

/*! \brief Maximum time to wait for TCP socket to connect in milliseconds. */
enum eDefTcpConnectTimeout : uint32_t
{
    MAX_TCP_CONNECT_TIMEOUT = 3000
};

class TcpConnection;
} // namespace tcp

/*! \brief The udp namespace. */
namespace udp
{

/*! \brief The udp options enumeration. */
enum class eUdpOption
{
    /*! \brief udp broadcasts. */
    broadcast,
    /*! \brief udp unicasts. */
    unicast
};

/*! \brief UDP datagram maximum size for user data.
 *
 * A UDP datagram can have a total max size of 65535 bytes,
 * however the size available for "user" data is a bit less
 * as we have to allow 8 bytes for UDP header and 20 bytes
 * for the IP header.
 */
enum eUdpDatagramMaxSize : size_t
{
    UDP_DATAGRAM_MAX_SIZE = 65507
};

/*! \brief UDP default buffer size.
 *
 * By default we use a size of 8KiB but this can be
 * changed by the user.
 */
enum eDefaultUdpSize : size_t
{
    DEFAULT_UDP_BUF_SIZE = 8192
};

/*! \brief The multicast TTL enumeration. */
enum class eMulticastTTL
{
    /*! \brief Multicast only to same host. */
    sameHost = 0,
    /*! \brief Multicast only to same subnet. */
    sameSubnet = 1,
    /*! \brief Multicast only to same site. */
    sameSite = 32,
    /*! \brief Multicast only to same region. */
    sameRegion = 64,
    /*! \brief Multicast only to same continent. */
    sameContinent = 128,
    /*! \brief Multicasts are unrestricted. */
    unrestricted = 255
};

} // namespace udp

/*! \brief The asio_defs namespace. */
namespace defs
{

/*! \brief This is the default/initial reserved message size for each message on the recevie mesasge
 *         pool (if pool is used) */
enum eDefRecvPoolMsgSize : size_t
{
    RECV_POOL_DEFAULT_MSG_SIZE = 8192
};

/*! \brief Typedef describing a network connection as (address, port). */
using connection_t = std::pair<std::string, uint16_t>;
/*! \brief Constant defining a null network connection as ("0.0.0.0", 0). */
extern const connection_t NULL_CONNECTION;
/*! \brief Typedef describing shared_ptr to a TcpConnection object. */
using tcp_conn_ptr_t = std::shared_ptr<tcp::TcpConnection>;
/*! \brief Constant defining response IP address length in bytes. */
enum eRespAddressLen : uint32_t
{
    RESPONSE_ADDRESS_LEN = 16
};
/*! \brief Constant defining message header magic string length in bytes. */
enum eMagicStringLen : uint32_t
{
    MAGIC_STRING_LEN = 16
};
/*! \brief Constant defining default magc string as "_BEGIN_MESSAGE_". */
extern const char DEFAULT_MAGIC_STRING[];

/*! \brief Message serialization archive type enumeration. */
enum class eArchiveType : uint8_t
{ /*! \brief Portable binary archive, requires Cereal serialization. */
  portableBinary,
  /*! \brief Binary archive, requires Cereal serialization. */
  binary,
  /*! \brief JSON archive, requires Cereal serialization. */
  json,
  /*! \brief XML archive, requires Cereal serialization. */
  xml,
  /*! \brief Raw data, only for POD objects. */
  raw,
  /*! \brief Google protocol buffer. */
  protobuf
};

// Push single byte alignment for the MessageHeader strcuture for maximum portability.
#pragma pack(push, 1)
/*!
 * \brief Default message header structure that is also POD.
 *
 * This structure is used for all the network classes prepended with Simple, e.g.
 * SimpleTcpClient, SimpleTcpServer etc.
 */
struct CORE_LIBRARY_DLL_SHARED_API MessageHeader
{
    /*! \brief Magic string to identify message start. */
    char magicString[MAGIC_STRING_LEN]{};
    /*! \brief Response address; can be used by receiver to identify sender. */
    char responseAddress[RESPONSE_ADDRESS_LEN]{};
    /*! \brief Response port. */
    uint16_t responsePort{0};
    /*! \brief Unique message identifier. */
    int32_t messageId{0};
    /*! \brief Archive type used to serialize payload following this header. */
    eArchiveType archiveType{eArchiveType::portableBinary};
    /*! \brief Total message length including this header. */
    uint32_t totalLength{sizeof(*this)};

    /*! \brief Default constructor. */
    MessageHeader();
    /*! \brief Destructor. */
    ~MessageHeader() = default;
    /*! \brief Default copy constructor. */
    MessageHeader(const MessageHeader&) = default;
    /*! \brief Default copy assignment operator. */
    MessageHeader& operator=(const MessageHeader&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Default move constructor. */
    MessageHeader(MessageHeader&& header);
    /*! \brief Default move assignment operator. */
    MessageHeader& operator=(MessageHeader&& header);
#else
    /*! \brief Default move constructor. */
    MessageHeader(MessageHeader&&) = default;
    /*! \brief Default move assignment operator. */
    MessageHeader& operator=(MessageHeader&&) = default;
#endif
};
// Pop single byte alignment.
#pragma pack(pop)

/*! \brief Constant defining message header magic string length in bytes. */
enum eMessageHeaderLen : size_t
{
    MESSAGE_HEADER_LEN = sizeof(MessageHeader)
};

/*! \brief Typedef to generic char buffer based on s std::vector<char>. */
using char_buffer_t = std::vector<char>;

/*! \brief Template class to act as a generic wrapper around a received message for a given header
 * type. */
template <typename Header> struct ReceivedMessage
{
    /*! \brief Typedef for header template type. */
    using header_t = Header;
    /*! \brief Message header. */
    header_t header;
    /*! \brief Message body as a char buffer as all data received form socket is fundamentally an
     * array pf chars. */
    char_buffer_t body;
    /*! \brief Default constructor. */
    ReceivedMessage() = default;
    /*! \brief Default destructor. */
    ~ReceivedMessage() = default;
    /*! \brief Default copy constructor. */
    ReceivedMessage(const ReceivedMessage&) = default;
    /*! \brief Default copy assignment operator. */
    ReceivedMessage& operator=(const ReceivedMessage&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Default move constructor. */
    ReceivedMessage(ReceivedMessage&& message)
    {
        *this = std::move(message);
    }
    /*! \brief Default move assignment operator. */
    ReceivedMessage& operator=(ReceivedMessage&& message)
    {
        std::swap(header, message.header);
        body.swap(message.body);
        return *this;
    }
#else
    /*! \brief Default move constructor. */
    ReceivedMessage(ReceivedMessage&&) = default;
    /*! \brief Default move assignment operator. */
    ReceivedMessage& operator=(ReceivedMessage&&) = default;
#endif
};

/*! \brief Typedef to default version of received message typed to default message header struct. */
using default_received_message_t = ReceivedMessage<MessageHeader>;
/*! \brief Typedef to default version of received message shared pointer. */
using default_received_message_ptr_t = std::shared_ptr<default_received_message_t>;
/*! \brief Typedef to default message dispatcher function object. */
using default_message_dispatcher_t = std::function<void(default_received_message_ptr_t)>;
/*! \brief Typedef to bytes left to reading checking utility function object. */
using check_bytes_left_to_read_t = std::function<size_t(const char_buffer_t&)>;
/*! \brief Typedef to message received handler function object. */
using message_received_handler_t = std::function<void(const char_buffer_t&)>;

} // namespace defs
} // namespace asio
} // namespace core_lib

#endif // ASIODEFINES
