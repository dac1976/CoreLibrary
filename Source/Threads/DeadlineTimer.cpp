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
 * \file DeadlineTimer.cpp
 * \brief File containing definition of DeadlineTimer class.
 */
 
#include "DeadlineTimer.h"
#include <stdexcept>

namespace hgl
{

DeadlineTimer::DeadlineTimer()
    : m_timeoutMillisecs(0)
    , m_timeoutEvent(hgl::eNotifyType::signalOneThread, hgl::eResetCondition::manualReset,
                     hgl::eIntialCondition::notSignalled)
{
}

DeadlineTimer::~DeadlineTimer()
{
    Cancel();
}

void DeadlineTimer::Start(uint32_t timeoutMillisecs, callback_t const& onTimedOut)
{
    Cancel();

    m_timeoutMillisecs = timeoutMillisecs;
    m_onTimedOut       = onTimedOut;

    if (m_timeoutMillisecs == 0)
    {
        throw std::invalid_argument("incorrect timeout period");
    }

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
    if (!m_timeoutEvent.WaitForTime(m_timeoutMillisecs))
    {
        if (m_onTimedOut)
        {
            m_onTimedOut();
        }
    }
}

} // namespace hgl
