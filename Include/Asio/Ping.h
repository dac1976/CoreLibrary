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
 * \file Ping.h
 * \brief File containing useful definitions.
 */
#ifndef PING_H
#define PING_H

#include <mutex>
#include <string>
#include <string_view>
#include <cstdint>
#include <memory>
#include <boost/asio.hpp>
#include <functional>
#include "Threads/SyncEvent.h"
#include "IoContextThreadGroup.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The icmp namespace. */
namespace icmp
{

class CORE_LIBRARY_DLL_SHARED_API Ping
{
public:
    enum class eResult
    {
        ok = 0,
        unknown,
        error,
        timedOut,
        aborted,
        wrongMode
    };

private:
    using ping_response_t =
        std::function<void(std::string const& /*from IP*/, eResult /*result status*/)>;

public:
    // pingResponseCallback only required for SendSync usage.
    // If using Send you do not need to set the callback.
    // Only use the this class for Send or SendSync not both.

    // Use internal IO Service thread group with 1 thread
    Ping(std::string_view destination, ping_response_t const& pingResponseCallback = {});

    // Use external IO Service
    Ping(asio_compat::io_service_t& ioService, std::string_view destination,
         ping_response_t const& pingResponseCallback = {});

    ~Ping();

    std::string Destination() const;

    // Do not call from multiple threads.
    eResult Send(uint32_t waitTimeoutMillisecs = 1000);
    // Requires that m_pingResponseCallback was set in constructor.
    // Returns true if ping sent, false if it failed to send.
    // m_pingResponseCallback  will get called  if a response arrives.
    bool SendAync();

private:
    static uint16_t GetIdentifier();
    static uint16_t GetSequenceNumber();
    bool            Closing() const;
    void            SetClosing(bool close);
    bool            Pending() const;
    void            SetPending(bool pending);
    void            CloseSocket(bool* waitToClose = nullptr);
    void            CloseAndWait();
    void            Receive();
    void            HandleReceive(boost::system::error_code error,
                                  size_t bytesTransferred);

private:
    mutable std::mutex                m_closingMutex;
    mutable std::mutex                m_pendingMutex;
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    asio_compat::io_service_t&          m_ioServiceRef;
    SyncEvent                        m_closeEvent;
    SyncEvent                        m_replyEvent;
    boost::asio::ip::icmp::socket       m_socket;
    boost::asio::ip::icmp::endpoint      m_destination;
    ping_response_t                   m_pingResponseCallback;
    uint16_t                         m_identifier{0};
    uint16_t                        m_sequenceNumber{0};
    boost::asio::streambuf             m_replyBuffer;
    eResult                          m_result{eResult::unknown};
    bool                            m_closing{false};
    bool                            m_pending{false};
};

} // namespace icmp
} // namespace asio
} // namespace core_lib

#endif // PING_H
