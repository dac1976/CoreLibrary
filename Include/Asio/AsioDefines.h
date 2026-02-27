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
#include <string_view>
#include <cstdint>
#include <span>
#include "../CoreLibraryDllGlobal.h"
#include "AsioCompatibility.hpp"
#include "Platform/PlatformDefines.h"

namespace boost_sys          = boost::system;
namespace boost_asio         = boost::asio;
namespace boost_placeholders = boost::asio::placeholders;
namespace boost_mcast        = boost::asio::ip::multicast;

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

/*! \brief The asio_defs namespace. */
namespace defs
{

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
/*! \brief Constant defining default magic string as "_BEGIN_MESSAGE_". */
extern const char DEFAULT_MAGIC_STRING[];

/*! \brief Message serialization archive type enumeration. See SerializeToVector.h.*/
enum class eArchiveType : uint8_t
{
	 /*! \brief Portable binary archive, requires Cereal serialization. */
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

/*! \brief Thi is the default/initial reserved message size for each message on the recevie mesasge
 *         pool (if pool is used) */
enum eDefRecvPoolMsgSize : size_t
{
    RECV_POOL_DEFAULT_MSG_SIZE = 8192
};

} // namespace defs

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
    MAX_UNSENT_ASYNC_MSG_COUNT = 100
};

/*! \brief Enumeration to control nagle algorithm. */
enum class eSendOption
{
    /*! \brief nagleOff - Send immediately. */
    nagleOff,
    /*! \brief nagleOn - Send when possible. */
    nagleOn
};

/*! \brief Enumeration to control keep alive behaviour of the connection. */
enum class eKeepAliveOption
{
    /*! \brief off - keep alive not active. */
    off,
    /*! \brief on - keep alive active. */
    on
};

/*! \brief Maximum time to wait for TCP socket to connect in milliseconds. */
enum eDefTcpConnectTimeout : uint32_t
{
    MAX_TCP_CONNECT_TIMEOUT = 3000
};

/*! \brief TCP connection settings structure. */
struct CORE_LIBRARY_DLL_SHARED_API TcpConnSettings
{
    /*! \brief Minimum amount of data to read on each receive, typical size of header block. */
    size_t minAmountToRead{defs::MESSAGE_HEADER_LEN};
    /*! \brief Socket send option. */
    eSendOption sendOption{eSendOption::nagleOn};
    /*! \brief Maximum allowed number of unsent async messages.*/
    size_t maxAllowedUnsentAsyncMessages{MAX_UNSENT_ASYNC_MSG_COUNT};
    /*! \brief  Default size of message in pool. Set to 0 to not use the pool and instead use
     *          dynamic allocation. */
    size_t sendPoolMsgSize{0};
    /*! \brief Maximum time allowed when waiting for TCP socket to connect to target. */
    uint32_t maxTcpConnectTimeout{MAX_TCP_CONNECT_TIMEOUT};
    /*! \brief Size to set for send buffer within socket in bytes. 0 - implies use default. */
    size_t sendBufferSize{0};
    /*! \brief Size to set for receive buffer within socket in bytes. 0 - implies use default. */
    size_t recvBufferSize{0};
    /*! \brief Keep alive option. */
    eKeepAliveOption keepAliveOption{eKeepAliveOption::off};

    TcpConnSettings()
    {
    }

    TcpConnSettings(size_t minAmountToRead_, eSendOption sendOption_,
                    size_t maxAllowedUnsentAsyncMessages_, size_t sendPoolMsgSize_,
                    uint32_t maxTcpConnectTimeout_, size_t sendBufferSize_ = 0,
                    size_t           recvBufferSize_  = 0,
                    eKeepAliveOption keepAliveOption_ = eKeepAliveOption::off)
        : minAmountToRead(minAmountToRead_)
        , sendOption(sendOption_)
        , maxAllowedUnsentAsyncMessages(maxAllowedUnsentAsyncMessages_)
        , sendPoolMsgSize(sendPoolMsgSize_)
        , maxTcpConnectTimeout(maxTcpConnectTimeout_)
        , sendBufferSize(sendBufferSize_)
        , recvBufferSize(recvBufferSize_)
        , keepAliveOption(keepAliveOption_)
    {
    }

    ~TcpConnSettings()                                 = default;
    TcpConnSettings(const TcpConnSettings&)            = default;
    TcpConnSettings& operator=(const TcpConnSettings&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Default move constructor. */
    TcpConnSettings(TcpConnSettings&& settings)
    {
        *this = std::move(settings);
    }
    /*! \brief Default move assignment operator. */
    TcpConnSettings& operator=(TcpConnSettings&& settings)
    {
        std::swap(minAmountToRead, settings.minAmountToRead);
        std::swap(sendOption, settings.sendOption);
        std::swap(maxAllowedUnsentAsyncMessages, settings.maxAllowedUnsentAsyncMessages);
        std::swap(sendPoolMsgSize, settings.sendPoolMsgSize);
        std::swap(maxTcpConnectTimeout, settings.maxTcpConnectTimeout);
        std::swap(sendBufferSize, settings.sendBufferSize);
        std::swap(recvBufferSize, settings.recvBufferSize);
        std::swap(keepAliveOption, settings.keepAliveOption);
        return *this;
    }
// If noexcept is supported as a functiont type, not a dynamic exception specification since C++17.
#elif __cpp_noexcept_function_type
    TcpConnSettings(TcpConnSettings&&) NO_EXCEPT_                  = default;
    TcpConnSettings& operator=(TcpConnSettings&&) NO_EXCEPT_       = default;
#else
    TcpConnSettings(TcpConnSettings&&)                  = default;
    TcpConnSettings& operator=(TcpConnSettings&&)       = default;
#endif
};

/*! \brief Simple TCP client/server settings structure. */
struct CORE_LIBRARY_DLL_SHARED_API SimpleTcpSettings
{
    /*! \brief TCP connection settings. */
    TcpConnSettings connSettings{};
    /*! \brief Number of messages in pool for received message handling, defaults to 0, which
     *         implies no pool used. This is also per client connection. */
    size_t memPoolMsgCount{0};
    /*! \brief Default size of message in received message pool. Only used when
     *         memPoolMsgCount > 0. */
    size_t recvPoolMsgSize{defs::RECV_POOL_DEFAULT_MSG_SIZE};

    SimpleTcpSettings()
    {
    }

    SimpleTcpSettings(size_t minAmountToRead_, eSendOption sendOption_,
                      size_t maxAllowedUnsentAsyncMessages_, size_t sendPoolMsgSize_,
                      uint32_t maxTcpConnectTimeout_, size_t memPoolMsgCount_,
                      size_t recvPoolMsgSize_, size_t sendBufferSize_ = 0,
                      size_t           recvBufferSize_  = 0,
                      eKeepAliveOption keepAliveOption_ = eKeepAliveOption::off)
        : connSettings(minAmountToRead_, sendOption_, maxAllowedUnsentAsyncMessages_,
                       sendPoolMsgSize_, maxTcpConnectTimeout_, sendBufferSize_, recvBufferSize_,
                       keepAliveOption_)
        , memPoolMsgCount(memPoolMsgCount_)
        , recvPoolMsgSize(recvPoolMsgSize_)
    {
    }

    ~SimpleTcpSettings()                                   = default;
    SimpleTcpSettings(const SimpleTcpSettings&)            = default;
    SimpleTcpSettings& operator=(const SimpleTcpSettings&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Default move constructor. */
    SimpleTcpSettings(SimpleTcpSettings&& settings)
    {
        *this = std::move(settings);
    }
    /*! \brief Default move assignment operator. */
    SimpleTcpSettings& operator=(SimpleTcpSettings&& settings)
    {
        std::swap(connSettings, settings.connSettings);
        std::swap(memPoolMsgCount, settings.memPoolMsgCount);
        std::swap(recvPoolMsgSize, settings.recvPoolMsgSize);
        return *this;
    }
// If noexcept is supported as a functiont type, not a dynamic exception specification since C++17.
#elif __cpp_noexcept_function_type
    SimpleTcpSettings(SimpleTcpSettings&&) NO_EXCEPT_              = default;
    SimpleTcpSettings& operator=(SimpleTcpSettings&&) NO_EXCEPT_   = default;
#else
    SimpleTcpSettings(SimpleTcpSettings&&)              = default;
    SimpleTcpSettings& operator=(SimpleTcpSettings&&)   = default;
#endif
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

/*! \brief The serial namespace. */
namespace serial
{

/*! \brief Default baud rate in bps.*/
enum eSerialBaudRate : uint32_t
{
    DEFAULT_SERIAL_BAUD_RATE = 115200
};

/*! \brief Default character size in bits.*/
enum eSerialCharSize : uint32_t
{
    DEFAULT_SERIAL_CHAR_SIZE = 8
};

/*! \brief Default internal receive buffer's initial reserved size in bytes. */
enum eSerialMessageBufSize : size_t
{
    DEFAULT_RECV_BUF_LEN = 65536,
    DEFAULT_MSG_BUF_LEN  = 1024 * 1024
};

/*! \brief The serial port settings. */
struct CORE_LIBRARY_DLL_SHARED_API SerialPortSettings
{
    size_t                                     minAmountToRead{DEFAULT_RECV_BUF_LEN};
    size_t                                     recvBufLengthLength{DEFAULT_RECV_BUF_LEN};
    size_t                                     msgBufLength{DEFAULT_MSG_BUF_LEN};
    uint32_t                                   baudRate{DEFAULT_SERIAL_BAUD_RATE};
    boost_asio::serial_port_base::flow_control flowControl{
        boost_asio::serial_port_base::flow_control::none};
    boost_asio::serial_port_base::parity    parity{boost_asio::serial_port_base::parity::none};
    boost_asio::serial_port_base::stop_bits stopBits{boost_asio::serial_port_base::stop_bits::one};
    uint32_t                                characterSize{DEFAULT_SERIAL_CHAR_SIZE};

    SerialPortSettings()
    {
    }

    SerialPortSettings(size_t minAmountToRead_, size_t recvBufLengthLength_, size_t msgBufLength_,
                       uint32_t baudRate_, boost_asio::serial_port_base::flow_control flowControl_,
                       boost_asio::serial_port_base::parity    parity_,
                       boost_asio::serial_port_base::stop_bits stopBits_, uint32_t characterSize_)
        : minAmountToRead(minAmountToRead_)
        , recvBufLengthLength(recvBufLengthLength_)
        , msgBufLength(msgBufLength_)
        , baudRate(baudRate_)
        , flowControl(flowControl_)
        , parity(parity_)
        , stopBits(stopBits_)
        , characterSize(characterSize_)
    {
    }

    ~SerialPortSettings()                                    = default;
    SerialPortSettings(const SerialPortSettings&)            = default;
    SerialPortSettings& operator=(const SerialPortSettings&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Default move constructor. */
    SerialPortSettings(SerialPortSettings&& settings)
    {
        *this = std::move(settings);
    }
    /*! \brief Default move assignment operator. */
    SerialPortSettings& operator=(SerialPortSettings&& settings)
    {
        std::swap(minAmountToRead, settings.minAmountToRead);
        std::swap(recvBufLengthLength, settings.recvBufLengthLength);
        std::swap(msgBufLength, settings.msgBufLength);
        std::swap(baudRate, settings.baudRate);
        std::swap(flowControl, settings.flowControl);
        std::swap(parity, settings.parity);
        std::swap(stopBits, settings.stopBits);
        std::swap(characterSize, settings.characterSize);
        return *this;
    }
// If noexcept is supported as a functiont type, not a dynamic exception specification since C++17.
#elif __cpp_noexcept_function_type
    SerialPortSettings(SerialPortSettings&&) NO_EXCEPT_            = default;
    SerialPortSettings& operator=(SerialPortSettings&&) NO_EXCEPT_ = default;
#else
    SerialPortSettings(SerialPortSettings&&)            = default;
    SerialPortSettings& operator=(SerialPortSettings&&) = default;
#endif
};

} // namespace serial

/*! \brief The asio_defs namespace. */
namespace defs
{

/*! \brief Typedef describing a network connection as (address, port). */
using connection_t = std::pair<std::string, uint16_t>;
/*! \brief Constant defining a null network connection as ("0.0.0.0", 0). */
extern const connection_t NULL_CONNECTION;
/*! \brief Typedef describing shared_ptr to a TcpConnection object. */
using tcp_conn_ptr_t = std::shared_ptr<tcp::TcpConnection>;

/*! \brief Typedef to generic char buffer based on s std::vector<char>. */
using char_buffer_t = std::vector<char>;

/*! \brief Typedef to char buffer span - mutable */
using char_buf_span_t = std::span<char>;

/*! \brief Typedef to char buffer span - const */
using char_buf_cspan_t = std::span<const char>;

/*! \brief Template class to act as a generic wrapper around a received message for a given header
 * type. */
template <typename Header> struct ReceivedMessage
{
    /*! \brief Typedef for header template type. */
    using header_t = Header;
    /*! \brief Message header. */
    header_t header;
    /*! \brief Message body as a char buffer as all data received form socket is fundamentally an
     * array of chars. */
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
// If noexcept is supported as a functiont type, not a dynamic exception specification since C++17.
#elif __cpp_noexcept_function_type
    /*! \brief Default move constructor. */
    ReceivedMessage(ReceivedMessage&&) NO_EXCEPT_ = default;
    /*! \brief Default move assignment operator. */
    ReceivedMessage& operator=(ReceivedMessage&&) NO_EXCEPT_ = default;
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
using default_message_dispatcher_t = std::function<void(default_received_message_ptr_t const&)>;
/*! \brief Typedef to bytes left to reading checking utility function object.
           If there is a problem with the message size this should return
           std::numeric_limits<size_t>::max().*/
using check_bytes_left_to_read_t = std::function<size_t(char_buf_cspan_t)>;
/*! \brief Typedef to bytes left to reading checking utility function object.
           If there is a problem with the message size this should return
           std::numeric_limits<size_t>::max().*/
using check_bytes_left_to_read_ex_t =
    std::function<size_t(char_buf_cspan_t, std::string_view, uint16_t)>;
/*! \brief Typedef to message received handler function object. */
using message_received_handler_t = std::function<void(char_buf_cspan_t)>;
/*! \brief Typedef to extended message received handler function object. */
using message_received_handler_ex_t =
    std::function<void(char_buf_cspan_t, std::string_view, uint16_t)>;
/*! \brief Typedef for a TCP connection OnClose callback. */
using on_close_t = std::function<void(const connection_t&)>;

} // namespace defs

} // namespace asio
} // namespace core_lib

#endif // ASIODEFINES

