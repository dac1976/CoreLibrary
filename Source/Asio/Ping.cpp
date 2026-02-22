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
 * \file Ping.cpp
 * \brief File containing useful definitions.
 */
#include "Asio/Ping.h"
#include <iostream>
#include <random>
#include <limits>
#include <boost/predef.h>
#include <boost/bind.hpp>
#if defined(USE_SOCKET_DEBUG)
#include <boost/exception/all.hpp>
#include "DebugLog/DebugLogging.h"
#endif
#include "Threads/MutexHelpers.hpp"
#include "Asio/support/icmp_hdr.hpp"
#include "Asio/support/ipv4_hdr.hpp"

namespace boost_ip    = boost::asio::ip;
namespace posix_time  = boost::posix_time;

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The icmp namespace. */
namespace icmp
{

constexpr uint32_t    CLOSE_WAIT_MS = 1000;
constexpr size_t      REPLY_BUF_SIZE = 65536;
constexpr const char* GREETING_MSG{"CoreLibrary - ICMP Echo Request"};

Ping::Ping(std::string_view destination, 
         ping_response_t const& pingResponseCallback)
    : m_ioThreadGroup(std::make_unique<IoContextThreadGroup>(1))
    , m_ioServiceRef(m_ioThreadGroup->IoService())
    , m_closeEvent(threads::eNotifyType::signalOneThread, threads::eResetCondition::manualReset,
                threads::eIntialCondition::notSignalled)
    , m_socket(m_ioServiceRef)
    , m_pingResponseCallback(pingResponseCallback)
    , m_identifier(GetIdentifier())
{
    boost_ip::icmp::resolver resolver(m_ioServiceRef);
    m_destination = *resolver.resolve(boost_ip::icmp::v4(), destination, "").begin();
}

Ping::Ping(asio_compat::io_service_t& ioService, 
         std::string_view destination,
         ping_response_t const& pingResponseCallback)
    : m_ioServiceRef(ioService)
    , m_closeEvent(threads::eNotifyType::signalOneThread, threads::eResetCondition::manualReset,
                 threads::eIntialCondition::notSignalled)
    , m_socket(m_ioServiceRef)
    , m_pingResponseCallback(pingResponseCallback)
    , m_identifier(GetIdentifier())
{
    boost_ip::icmp::resolver resolver(m_ioServiceRef);
    m_destination = *resolver.resolve(boost_ip::icmp::v4(), destination, "").begin();
}

Ping::~Ping()
{
    CloseAndWait();
}

std::string Ping::Destination() const
{
    return m_destination.address().to_string();
}

auto Ping::Send(uint32_t waitTimeoutMillisecs) -> eResult
{
    if (m_pingResponseCallback)
    {
        return eResult::wrongMode;
    }

    if (waitTimeoutMillisecs < 100)
    {
        waitTimeoutMillisecs = 100;
    }
    
    // Create an ICMP header for an echo request.
	icmp_header echoRequest;
    echoRequest.type(icmp_header::echo_request);
    echoRequest.code(0);
    echoRequest.identifier(m_identifier);
    m_sequenceNumber = GetSequenceNumber();
    echoRequest.sequence_number(m_sequenceNumber);

    auto greetingMsg = std::string_view(GREETING_MSG);
    compute_checksum(echoRequest, greetingMsg.begin(), greetingMsg.end());

    // Encode the request packet.
    boost::asio::streambuf requestBuffer;
    std::ostream           os(&requestBuffer);
    os << echoRequest << greetingMsg;

    eResult result;
    m_result = eResult::unknown;

    try
    {
        if (!m_socket.is_open())
        {
            m_socket.open(boost_ip::icmp::v4());
        }
        
	    // Send the request.
	    SetPending(true);
	    m_socket.send_to(requestBuffer.data(), m_destination);

        Receive();

        auto signalled = m_replyEvent.WaitForTime(waitTimeoutMillisecs);

        if (signalled)
        {
            result = m_result;
        }
        else
        {
            SetPending(false);
            CloseSocket();
            result = eResult::timedOut;
        }
	}
	catch(...)
	{
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error sending icmp echo request, error: "
                               << boost::current_exception_diagnostic_information());
#endif
        SetPending(false);
        CloseSocket();
        result = eResult::error;
    }
	
    return result;
}

bool Ping::SendAync()
{
    bool result = false;

    if (!m_pingResponseCallback)
    {
        return result;
    }

    // Cancelling pending actions.
    if (Pending())
    {
        CloseAndWait();
    }

    // Create an ICMP header for an echo request.
    icmp_header echoRequest;
    echoRequest.type(icmp_header::echo_request);
    echoRequest.code(0);
    echoRequest.identifier(m_identifier);
    m_sequenceNumber = GetSequenceNumber();
    echoRequest.sequence_number(m_sequenceNumber);

    auto greetingMsg = std::string_view(GREETING_MSG);
    compute_checksum(echoRequest, greetingMsg.begin(), greetingMsg.end());

    // Encode the request packet.
    boost::asio::streambuf requestBuffer;
    std::ostream           os(&requestBuffer);
    os << echoRequest << greetingMsg;

    try
    {
        if (!m_socket.is_open())
        {
            m_socket.open(boost_ip::icmp::v4());
        }

        // Send the request.
        SetPending(true);
        m_socket.send_to(requestBuffer.data(), m_destination);

        Receive();

        result = true;
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error sending icmp echo request, error: "
                               << boost::current_exception_diagnostic_information());
#endif
        SetPending(false);
        CloseSocket();
    }

    return result;
}

uint16_t Ping::GetIdentifier()
{
#if defined(BOOST_ASIO_WINDOWS)
	return static_cast<uint16_t>(::GetCurrentProcessId());
#else
	return static_cast<uint16_t>(::getpid());
#endif
}

uint16_t Ping::GetSequenceNumber()
{
    // Initialize random number generator with a random device
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Define the range for the random uint16_t value
    std::uniform_int_distribution<uint16_t> uni_dist(
        0, std::numeric_limits<uint16_t>::max());
    
    // Generate a random uint16_t value
    return uni_dist(gen);
}

bool Ping::Closing() const
{
    STD_LOCK_GUARD(m_closingMutex);
    return m_closing;
}

void Ping::SetClosing(bool close)
{
    STD_LOCK_GUARD(m_closingMutex);
    m_closing = close;
}

bool Ping::Pending() const
{
    STD_LOCK_GUARD(m_pendingMutex);
    return m_pending;
}

void Ping::SetPending(bool pending)
{
    STD_LOCK_GUARD(m_pendingMutex);
    m_pending = pending;
}

void Ping::CloseSocket(bool* waitToClose)
{
    if (!m_socket.is_open())
    {
        if (nullptr != waitToClose)
        {
            *waitToClose = false;
        }

        return;
    }

    try
    {
        m_socket.shutdown(m_socket.shutdown_both);
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error shutting down sends and receives for socket, error: "
                               << boost::current_exception_diagnostic_information());
#endif
    }

    try
    {
        m_socket.close();
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR(
            "Error closing socket, error: " << boost::current_exception_diagnostic_information());
#endif
    }

    if (nullptr != waitToClose)
    {
        *waitToClose = Pending();
    }
}

void Ping::CloseAndWait()
{
    SetClosing(true);

    bool waitToClose = false;
    CloseSocket(&waitToClose);

    if (waitToClose)
    {
        m_closeEvent.WaitForTime(CLOSE_WAIT_MS);
        m_closeEvent.Reset();
    }
}

void Ping::Receive()
{
	// Discard any data already in the buffer.
	m_replyBuffer.consume(m_replyBuffer.size());

	// Wait for a reply. We prepare the buffer to receive up to 64KB.
	m_socket.async_receive(m_replyBuffer.prepare(REPLY_BUF_SIZE),
		boost::bind(&Ping::HandleReceive, 
		            this, 
		            boost::asio::placeholders::error,
		            boost::asio::placeholders::bytes_transferred));
}

void Ping::HandleReceive(boost::system::error_code error, size_t bytesTransferred)
{
    if (error)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Ping::HandleReceive called but error reported: "
                               << error.message());
#endif
        if (Pending())
        {
            m_result = eResult::aborted;

            if (m_pingResponseCallback)
            {
                m_pingResponseCallback(m_destination.address().to_string(), m_result);
            }
            else
            {
                m_replyEvent.Signal();
            }

            SetPending(false);
        }
                  
        if (Closing())
        {
            m_closeEvent.Signal();
        }    
        
        return;   
    }
    
	// The actual number of bytes received is committed to the buffer so that we
	// can extract it using a std::istream object.
    m_replyBuffer.commit(bytesTransferred);

    // Decode the reply packet.
    std::istream is(&m_replyBuffer);
    ipv4_header  ipv4_hdr;
    icmp_header  icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;

    // We can receive all ICMP packets received by the host, so we need to
    // filter out only the echo replies that match the our identifier and
    // expected sequence number.
    if (is && (icmp_hdr.type() == icmp_header::echo_reply) &&
        (icmp_hdr.identifier() == m_identifier) && (icmp_hdr.sequence_number() == m_sequenceNumber))
    {
        if (Pending())
        {
            m_result = eResult::ok;

            if (m_pingResponseCallback)
            {
                m_pingResponseCallback(m_destination.address().to_string(), m_result);
            }
            else
            {
                m_replyEvent.Signal();
            }

            SetPending(false);
        }
    }
    else
    {
        // Go back to waiting for our response
        Receive();
    }
}

} // namespace icmp
} // namespace asio
} // namespace core_lib
