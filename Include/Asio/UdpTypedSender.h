// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2015 Duncan Crutchley
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
 * \file UdpTypedSender.h
 * \brief File containing UDP typed sender class declaration.
 */

#ifndef UDPTYPEDSENDER
#define UDPTYPEDSENDER

#include "UdpSender.h"
#include "MessageUtils.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

template<typename MsgBldr>
class UdpTypedSender final
{
public:
	UdpTypedSender(boost_ioservice_t& ioService
				   , const defs::connection_t& receiver
				   , const MsgBldr& messageBuilder
				   , const eUdpOption sendOption = eUdpOption::broadcast
				   , const size_t sendBufferSize = DEFAULT_UDP_BUF_SIZE)
		: m_messageBuilder{messageBuilder}
		, m_udpSender{ioService, receiver, sendOption, sendBufferSize}
	{
	}

	UdpTypedSender(const defs::connection_t& receiver
				   , const MsgBldr& messageBuilder
				   , const eUdpOption sendOption = eUdpOption::broadcast
				   , const size_t sendBufferSize = DEFAULT_UDP_BUF_SIZE)

		: m_messageBuilder{messageBuilder}
		, m_udpSender{receiver, sendOption, sendBufferSize}
	{
	}

	UdpTypedSender(const UdpTypedSender& ) = delete;
	UdpTypedSender& operator=(const UdpTypedSender& ) = delete;
	~UdpTypedSender() = default;

	auto ReceiverConnection() const -> defs::connection_t
	{
		return m_udpSender.ReceiverConnection();
	}

	bool SendMessage(const uint32_t messageId
					 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        return m_udpSender.SendMessage(m_messageBuilder.Build(messageId, responseAddress));
	}

    template <typename T, class A = serialize::archives::out_port_bin_t>
    bool SendMessage(const T& message
                     , const uint32_t messageId
					 , const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
	{
        return m_udpSender.SendMessage(m_messageBuilder.template Build<T, A>(message, messageId, responseAddress));
	}

private:
	const MsgBldr& m_messageBuilder;
	UdpSender m_udpSender;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // UDPTYPEDSENDER