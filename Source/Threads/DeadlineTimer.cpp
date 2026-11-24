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
 * \file DeadlineTimer.cpp
 * \brief File containing definition of DeadlineTimer class.
 */

#include "Threads/DeadlineTimer.h"
#include <stdexcept>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

DeadlineTimer::DeadlineTimer()
    : m_timeoutEvent(hgl::eNotifyType::signalOneThread, hgl::eResetCondition::manualReset,
                     hgl::eIntialCondition::notSignalled)
{
}

DeadlineTimer::~DeadlineTimer()
{
    Cancel();
}

void DeadlineTimer::Start(int64_t timeout, callback_t const& onTimedOut, eWaitTimeUnit timeoutUnits)
{
    if (timeout <= 0)
    {
        throw std::invalid_argument("incorrect timeout period");
    }

    time_point_t wakeupTime;

    switch (timeoutUnits)
    {
    case eWaitTimeUnit::microseconds:
        wakeupTime = std::chrono::steady_clock::now() + std::chrono::microseconds{timeout};
        break;
    case eWaitTimeUnit::nanoseconds:
        wakeupTime = std::chrono::steady_clock::now() + std::chrono::nanoseconds{timeout};
        break;
    case eWaitTimeUnit::seconds:
        wakeupTime = std::chrono::steady_clock::now() + std::chrono::seconds{timeout};
        break;
    case eWaitTimeUnit::milliseconds:
    default:
        wakeupTime = std::chrono::steady_clock::now() + std::chrono::milliseconds{timeout};
        break;
    }

    StartTP(wakeupTime, onTimedOut);
}

void DeadlineTimer::StartTP(time_point_t const& wakeupTime, callback_t const& onTimedOut)
{
    Cancel();

    m_wakeupTime = wakeupTime;
    m_onTimedOut = onTimedOut;

    if (!m_onTimedOut)
    {
        throw std::invalid_argument("invalid timeout callback");
    }

    m_timeoutThread = std::thread(std::bind(&DeadlineTimer::TimeoutThread, this));
}

void DeadlineTimer::Cancel()
{
    m_timeoutEvent.Signal();

    if (m_timeoutThread.joinable())
    {
        m_timeoutThread.join();
    }

    m_timeoutEvent.Reset();
}

void DeadlineTimer::TimeoutThread()
{
    if (!m_timeoutEvent.WaitUntilTime(m_wakeupTime))
    {
        if (m_onTimedOut)
        {
            m_onTimedOut();
        }
    }
}

} // namespace threads
} // namespace core_lib
