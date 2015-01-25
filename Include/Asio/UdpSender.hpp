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
 * \file UdpSender.hpp
 * \brief File containing UDP sender class declaration.
 */

#ifndef UDPSENDER
#define UDPSENDER

#include "../Threads/SyncEvent.hpp"
#include "AsioDefines.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

class UdpSender final
{
public:
    UdpSender(boost_ioservice_t& ioService
              , const defs::connection_t& receiver
              , const eUdpOption sendOptions = eUdpOption::broadcast
              , const size_t sendBufferSize = 8192);

    UdpSender(const UdpSender& ) = delete;

    UdpSender& operator=(const UdpSender& ) = delete;

    ~UdpSender() = default;

    auto ReceiverConnection() const -> defs::connection_t;

    void SendMessageAsync(const defs::char_buffer_t& message);

    bool SendMessageSync(const defs::char_buffer_t& message);

private:
    threads::SyncEvent m_sendEvent;
    boost_ioservice_t& m_ioService;
    const defs::connection_t m_receiver;
    boost_ioservice_t::strand m_strand;
    boost_udp_t::socket m_socket;
    boost_udp_t::endpoint m_receiverEndpoint;
    bool m_sendSuccess;

    void AsyncSendTo(defs::char_buffer_t message
                     , const bool setSuccessFlag);

    void SyncSendTo(const defs::char_buffer_t& message
                    , const bool setSuccessFlag);

    void SendComplete(const boost_sys::error_code& error
                      , const bool setSuccessFlag);
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // UDPSENDER
