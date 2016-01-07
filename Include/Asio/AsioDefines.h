// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014,2015 Duncan Crutchley
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
 * \file AsioDefines.h
 * \brief File containing useful definitions.
 */

#ifndef ASIODEFINES
#define ASIODEFINES

#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"
#include <vector>
#include <functional>
#include <memory>
#include <utility>
#include <string>
#include <cstdint>
#include "boost/asio.hpp"

namespace boost_sys = boost::system;
namespace boost_asio = boost::asio;
namespace boost_placeholders = boost::asio::placeholders;

/*! \brief Boost IO service convenience typedef. */
typedef boost_asio::io_service boost_ioservice_t;
/*! \brief Boost tcp convenience typedef. */
typedef boost_asio::ip::tcp boost_tcp_t;
/*! \brief Boost tcp acceptor convenience typedef. */
typedef boost::asio::ip::tcp::acceptor boost_tcp_acceptor_t;
/*! \brief Boost udp convenience typedef. */
typedef boost::asio::ip::udp boost_udp_t;
/*! \brief Boost IP address convenience typedef. */
typedef boost::asio::ip::address boost_address_t;

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {

/*! \brief The tcp namespace. */
namespace tcp {

	/*! \brief Default internal receive buffer's initial reserved size in bytes. */
    __CONSTEXPR__ size_t DEFAULT_RESERVED_SIZE{ 512 * 1024 };

	/*! \brief Enumeration to control nagle algorithm. */
	enum class eSendOption
	{
		/*! \brief nagleOff - Send immediately. */
		nagleOff,
		/*! \brief nagleOn - Send when possible. */
		nagleOn 
	};

	class TcpConnection;
} // namespace tcp

/*! \brief The udp namespace. */
namespace udp {

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
    __CONSTEXPR__ size_t UDP_DATAGRAM_MAX_SIZE{65507};

	/*! \brief UDP default buffer size.
	*
	* By default we use a size of 8KiB  but this can be 
	* changed by the user.
	*/
    __CONSTEXPR__ size_t DEFAULT_UDP_BUF_SIZE{8192};

} // namespace udp

/*! \brief The asio_defs namespace. */
namespace defs {

/*! \brief Typedef describing a network connection as (address, port). */
typedef std::pair<std::string, uint16_t> connection_t;
/*! \brief Constant defining a null network connection as ("0.0.0.0", 0). */
__CONSTEXPR__ connection_t NULL_CONNECTION = std::make_pair("0.0.0.0", 0);
/*! \brief Typedef describing shared_ptr to a TcpConnection object. */
typedef std::shared_ptr<tcp::TcpConnection> tcp_conn_ptr_t;
/*! \brief Constant defining response IP address length in bytes. */
__CONSTEXPR__ uint32_t RESPONSE_ADDRESS_LEN{16};
/*! \brief Constant defining message header magic string length in bytes. */
__CONSTEXPR__ uint32_t MAGIC_STRING_LEN{16};
/*! \brief Constant defining default magc string as "_BEGIN_MESSAGE_". */
__CONSTEXPR__ char DEFAULT_MAGIC_STRING[]{"_BEGIN_MESSAGE_"};

/*! \brief Message serialization archive type enumeration. */
enum class eArchiveType : uint8_t
{
	/*! \brief Portable binary archive, requires EOS portable archive library. */
    portableBinary, 
	/*! \brief Binary archive, requires boost serialization. */
    binary,
	/*! \brief Text archive, requires boost serialization. */
    text, 
	/*! \brief XML archive, requires boost serialization. */
    xml,
	/*! \brief Raw data, only for POD objects. */
    raw 
};

#pragma pack(push, 1)
/*! \brief Default message header structure that is POD. */
struct CORE_LIBRARY_DLL_SHARED_API MessageHeader
{
	/*! \brief Magic string to identify message start. */
    char magicString[MAGIC_STRING_LEN];
	/*! \brief Response address; can be used by receiver to identify sender. */
	char responseAddress[RESPONSE_ADDRESS_LEN];
	/*! \brief Response port. */
	uint16_t responsePort{};
	/*! \brief Unique message identifier. */
	uint32_t messageId{};
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
#ifdef __USE_EXPLICIT_MOVE__
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
#pragma pack(pop)

/*! \brief Typedef to generic char buffer based on s std::vector<char>. */
typedef std::vector<char> char_buffer_t;

/*! \brief Template class to act as a generic wrapper around a received message for a given header type. */
template <typename Header>
struct ReceivedMessage
{
	/*! \brief Typedef for header template type. */
	typedef Header header_t;
	/*! \brief Message header. */
	header_t header;
	/*! \brief Message body as a char buffer. */
	char_buffer_t body;
	/*! \brief Default constructor. */
	ReceivedMessage() = default;
	/*! \brief Default destructor. */
	~ReceivedMessage() = default;
	/*! \brief Default copy constructor. */
	ReceivedMessage(const ReceivedMessage&) = default;	
	/*! \brief Default copy assignment operator. */
    ReceivedMessage& operator=(const ReceivedMessage&) = default;
#ifdef __USE_EXPLICIT_MOVE__
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
typedef ReceivedMessage<MessageHeader> default_received_message_t;
/*! \brief Typedef to default version of received message shared pointer. */
typedef std::shared_ptr<default_received_message_t> default_received_message_ptr_t;
/*! \brief Typedef to default message dispatcher function object. */
typedef std::function< void (default_received_message_ptr_t ) > default_message_dispatcher_t;
/*! \brief Typedef to bytes left to reading checking utility function object. */
typedef std::function< size_t (const char_buffer_t& ) > check_bytes_left_to_read_t;
/*! \brief Typedef to message received handler function object. */
typedef std::function< void (const char_buffer_t& ) > message_received_handler_t;

} // namespace defs
} // namespace asio
} // namespace core_lib

#endif // ASIODEFINES
