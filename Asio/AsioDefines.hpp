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

/*! \brief The tcp_conn namespace. */
namespace tcp {

    enum class eSendOption
	{
		nagleOff, // Implies send immediately.
		nagleOn
	};
    
	class TcpConnection;
} // namespace tcp_conn

/*! \brief The asio_defs namespace. */
namespace defs {

typedef std::vector<char> char_buffer;

typedef std::function< size_t (const char_buffer& ) > check_bytes_left_to_read;

typedef std::function< void (const char_buffer& ) > message_received_handler;

typedef std::pair<std::string, unsigned short> connection_address;

typedef std::shared_ptr<tcp::TcpConnection> tcp_conn_ptr;

} // namespace defs
} // namespace asio
} // namespace core_lib

#endif // ASIODEFINES_H
