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
 * \file AsioDefines.hpp
 * \brief File containing useful definitions.
 */

#ifndef ASIODEFINES
#define ASIODEFINES

#include "boost/asio.hpp"
#include <vector>
#include <functional>
#include <memory>
#include <utility>
#include <string>
#include <cstdint>
#include <cstring>

namespace boost_sys = boost::system;
namespace boost_asio = boost::asio;
namespace boost_placeholders = boost::asio::placeholders;

typedef boost_asio::io_service boost_ioservice_t;
typedef boost_asio::ip::tcp boost_tcp_t;
typedef boost::asio::ip::tcp::acceptor boost_tcp_acceptor_t;
typedef boost::asio::ip::udp boost_udp_t;
typedef boost::asio::ip::address boost_address_t;

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {

/*! \brief The tcp namespace. */
namespace tcp {

    // Reserve 0.5 MiB for each receive buffer.
    static constexpr size_t DEFAULT_RESERVED_SIZE{512 * 1024};

	enum class eSendOption
	{
		nagleOff, // Implies send immediately.
		nagleOn
	};

	class TcpConnection;
} // namespace tcp

/*! \brief The udp namespace. */
namespace udp {

    enum class eUdpOption
    {
        broadcast,
        unicast
    };

    // A UDP datagram can have a total max size of 65535 bytes,
    // however the size available for "user" data is a bit less
    // as we have to allow 8 bytes for UDP header and 20 bytes
    // for the IP header.
    static constexpr size_t UDP_DATAGRAM_MAX_SIZE{65507};

} // namespace udp

/*! \brief The asio_defs namespace. */
namespace defs {

typedef std::pair<std::string, uint16_t> connection_t;

static const connection_t NULL_CONNECTION = std::make_pair("0.0.0.0", 0);

typedef std::shared_ptr<tcp::TcpConnection> tcp_conn_ptr_t;

static constexpr uint32_t MAGIC_STRING_LEN{16};

static constexpr uint32_t RESPONSE_ADDRESS_LEN{16};

static constexpr char DEFAULT_MAGIC_STRING[]{"_BEGIN_MESSAGE_"};

enum class eArchiveType : uint8_t
{
	portableBinary,
	text,
	binary,
	xml
};

#pragma pack(push, 1)
struct MessageHeader
{
	char magicString[MAGIC_STRING_LEN];
	char responseAddress[RESPONSE_ADDRESS_LEN];
	uint16_t responsePort{};
	uint32_t messageId{};
	eArchiveType archiveType{eArchiveType::portableBinary};
	uint32_t totalLength{sizeof(*this)};

	MessageHeader()
		: responseAddress{"0.0.0.0"}
	{
		strncpy(magicString, DEFAULT_MAGIC_STRING, sizeof(magicString));
		magicString[MAGIC_STRING_LEN - 1] = 0;
	}

	~MessageHeader() = default;
	MessageHeader(const MessageHeader&) = default;
	MessageHeader(MessageHeader&&) = default;
	MessageHeader& operator=(const MessageHeader&) = default;
	MessageHeader& operator=(MessageHeader&&) = default;
};
#pragma pack(pop)

typedef std::vector<char> char_buffer_t;

template <typename Header>
struct ReceivedMessage
{
    typedef Header header_t;
    header_t header;
    char_buffer_t body;

	ReceivedMessage() = default;
	~ReceivedMessage() = default;
	ReceivedMessage(const ReceivedMessage&) = default;
	ReceivedMessage(ReceivedMessage&&) = default;
	ReceivedMessage& operator=(const ReceivedMessage&) = default;
	ReceivedMessage& operator=(ReceivedMessage&&) = default;
};

typedef ReceivedMessage<MessageHeader> default_received_message_t;

typedef std::shared_ptr<default_received_message_t> default_received_message_ptr_t;

typedef std::function< void (default_received_message_ptr_t ) > default_message_dispatcher_t;

typedef std::function< size_t (const char_buffer_t& ) > check_bytes_left_to_read_t;

typedef std::function< void (const char_buffer_t& ) > message_received_handler_t;

} // namespace defs
} // namespace asio
} // namespace core_lib

#endif // ASIODEFINES
