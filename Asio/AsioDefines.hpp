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

#ifndef ASIODEFINES_H
#define ASIODEFINES_H

#include <boost/asio.hpp>
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

typedef boost_asio::io_service boost_ioservice;
typedef boost_asio::ip::tcp boost_tcp;
typedef boost::asio::ip::tcp::acceptor boost_tcp_acceptor;
typedef boost::asio::ip::address boost_address;

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {

/*! \brief The tcp namespace. */
namespace tcp {

	enum class eSendOption
	{
		nagleOff, // Implies send immediately.
		nagleOn
	};

	class TcpConnection;
} // namespace tcp

/*! \brief The asio_defs namespace. */
namespace defs {

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

typedef std::vector<char> char_buffer;

struct ReceivedMessage
{
	MessageHeader header;
	char_buffer body;

	ReceivedMessage() = default;
	~ReceivedMessage() = default;
	ReceivedMessage(const ReceivedMessage&) = default;
	ReceivedMessage(ReceivedMessage&&) = default;
	ReceivedMessage& operator=(const ReceivedMessage&) = default;
	ReceivedMessage& operator=(ReceivedMessage&&) = default;
};

typedef std::shared_ptr<ReceivedMessage> received_message_ptr;

typedef std::function< void (received_message_ptr ) > message_dispatcher;

typedef std::function< size_t (const char_buffer& ) > check_bytes_left_to_read;

typedef std::function< void (const char_buffer& ) > message_received_handler;

typedef std::pair<std::string, uint16_t> connection;

static const connection NULL_CONNECTION = std::make_pair("0.0.0.0", 0);

typedef std::shared_ptr<tcp::TcpConnection> tcp_conn_ptr;

} // namespace defs
} // namespace asio
} // namespace core_lib

#endif // ASIODEFINES_H
