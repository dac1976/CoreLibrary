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
 * \file UdpSender.cpp
 * \brief File containing UDP sender class definition.
 */

#include "../../Include/Asio/UdpSender.hpp"
#include "boost/bind.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The udp namespace. */
namespace udp {

// ****************************************************************************
// 'class UdpSender' definition
// ****************************************************************************
UdpSender::UdpSender(boost_ioservice_t& ioService
                     , const defs::connection_t& receiver
                     , const eUdpOption sendOptions
                     , const size_t sendBufferSize)
    : m_ioService(ioService)
    , m_receiver{receiver}
    , m_strand{ioService}
    , m_socket{ioService}
    , m_sendSuccess{false}
{
    boost_udp_t::resolver receiverResolver(m_ioService);
    boost_udp_t::resolver::query resolverQuery(boost_udp_t::v4()
                                               , m_receiver.first
                                               , std::to_string(m_receiver.second));
    m_receiverEndpoint = *receiverResolver.resolve(resolverQuery);

    m_socket.open(boost_udp_t::v4());

    boost_asio::socket_base::broadcast broadcastOption(sendOptions
                                                        == eUdpOption::broadcast);
    m_socket.set_option(broadcastOption);

    boost_asio::socket_base::send_buffer_size sendBufOption(sendBufferSize);
    m_socket.set_option(sendBufOption);
}

auto UdpSender::ReceiverConnection() const -> defs::connection_t
{
    return m_receiver;
}

void UdpSender::SendMessageAsync(const defs::char_buffer_t& message)
{
    // Wrap in a strand to make sure we don't get weird issues
    // with the send event signalling and waiting. As we're
    // sending async in this case so we could get another
    // call to this method before the original async write
    // has completed.
    m_ioService.post(m_strand.wrap(boost::bind(&UdpSender::AsyncSendTo
                                               , this
                                               , message
                                               , false)));
}

bool UdpSender::SendMessageSync(const defs::char_buffer_t& message)
{
    SyncSendTo(message, true);
    return m_sendSuccess;
}

void UdpSender::AsyncSendTo(defs::char_buffer_t message
                            , const bool setSuccessFlag)
{
    SyncSendTo(message, setSuccessFlag);
}

void UdpSender::SyncSendTo(const defs::char_buffer_t& message
                           , const bool setSuccessFlag)
{
    m_socket.async_send_to(boost_asio::buffer(message)
                           , m_receiverEndpoint
                           , boost::bind(&UdpSender::SendComplete
                                         , this
                                         , boost_placeholders::error
                                         , setSuccessFlag));
    // Wait here until WriteComplete signals, this makes sure the
    // message vector remains viable.
    m_sendEvent.Wait();
}

void UdpSender::SendComplete(const boost_sys::error_code& error
                             , const bool setSuccessFlag)
{
    if (setSuccessFlag)
    {
        m_sendSuccess = !error;
    }

    m_sendEvent.Signal();
}

} // namespace udp
} // namespace asio
} // namespace core_lib
