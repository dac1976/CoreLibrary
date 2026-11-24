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
//

/*!
 * \file PeriodicThread.cpp
 * \brief File containing definition of PeriodicThread class.
 */
#include "PeriodicThread.h"
#include <stdexcept>
#include <limits>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

template <typename TargetDuration, typename SourceDuration>
TargetDuration CheckedDurationCast(SourceDuration const& source)
{
    using namespace std::chrono;

    using floating_duration_t = duration<long double, typename TargetDuration::period>;

    auto const requested = duration_cast<floating_duration_t>(source).count();

    auto const maximum = static_cast<long double>((TargetDuration::max() / 2).count());

    if (requested > maximum)
    {
        throw std::overflow_error("PeriodicThread period exceeds maximum supported duration");
    }

    auto const result = duration_cast<TargetDuration>(floating_duration_t{requested});

    if (result <= TargetDuration::zero())
    {
        throw std::invalid_argument("Period is below the native clock resolution");
    }

    return result;
}

// ****************************************************************************
// 'class PeriodicThread' definition
// ****************************************************************************
PeriodicThread::PeriodicThread(PeriodicThreadSettings const& settings)
    : m_settings(settings)
    , m_periodDuration(ConvertPeriod(m_settings.period, m_settings.timeUnit))
{
    if (static_cast<bool>(m_settings.eventCallback) ==
        static_cast<bool>(m_settings.eventCallbackEx))
    {
        throw std::invalid_argument("PeriodicThread requires exactly one callback");
    }

    if (!m_settings.delayedStart)
    {
        Start();
    }
}

PeriodicThread::~PeriodicThread()
{
    Stop();
}

void PeriodicThread::SetPeriod(int64_t period, eWaitTimeUnit timeUnit)
{
    auto duration = ConvertPeriod(period, timeUnit);

    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings.period   = period;
    m_settings.timeUnit = timeUnit;
    m_periodDuration    = duration;
}

int64_t PeriodicThread::Period(eWaitTimeUnit* timeUnit) const
{
    std::lock_guard<std::mutex> lock(m_settingsMutex);

    if (nullptr != timeUnit)
    {
        *timeUnit = m_settings.timeUnit;
    }

    return m_settings.period;
}

void PeriodicThread::SetOverrunPolicy(eOverrunPolicy overrunPolicy)
{
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings.overrunPolicy = overrunPolicy;
}

eOverrunPolicy PeriodicThread::OverrunPolicy() const
{
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return m_settings.overrunPolicy;
}

void PeriodicThread::SetMaxCatchUpTicks(uint32_t maxCatchUpTicks)
{
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings.maxCatchUpTicks = maxCatchUpTicks;
}

uint32_t PeriodicThread::MaxCatchUpTicks() const
{
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return m_settings.maxCatchUpTicks;
}

void PeriodicThread::ThreadFunction() NO_EXCEPT_
{
    // Reduce scope of mutex.
    duration_t     duration;
    eOverrunPolicy overrunPolicy;
    uint32_t       maxCatchUpTicks;

    {
        std::lock_guard<std::mutex> lock(m_settingsMutex);

        duration        = m_periodDuration;
        overrunPolicy   = m_settings.overrunPolicy;
        maxCatchUpTicks = m_settings.maxCatchUpTicks;
    }

    // Initialise timing reference.
    if (!m_timingInitialised)
    {
        m_tickInfo                = PeriodicTickInfo{};
        m_tickInfo.scheduledTime  = clock_t::now();
        m_timingInitialised       = true;
        m_consecutiveCatchUpTicks = 0;
    }

    // Populate timing information for current tick.
    m_tickInfo.period     = duration;
    m_tickInfo.actualTime = clock_t::now();

    m_tickInfo.wakeupLatency = m_tickInfo.actualTime - m_tickInfo.scheduledTime;

    // Execute registered callback.
    try
    {
        if (m_settings.eventCallback)
        {
            m_settings.eventCallback();
        }
        else if (m_settings.eventCallbackEx)
        {
            m_settings.eventCallbackEx(m_tickInfo);
        }
    }
    catch (...)
    {
        // Do nothing.
    }

    auto const now = clock_t::now();

    // Store callback duration for next invocation.
    m_tickInfo.previousCallbackDuration = now - m_tickInfo.actualTime;

    // Advance logical tick index and absolute deadline.
    ++m_tickInfo.tickIndex;
    m_tickInfo.scheduledTime += duration;

    // Reset recovery information for next invocation.
    m_tickInfo.skippedTicks   = 0;
    m_tickInfo.catchUpTick    = false;
    m_tickInfo.resynchronised = false;

    // Check for overrun.
    if (now > m_tickInfo.scheduledTime)
    {
        // Determine whether the catch-up limit has been reached.
        if (overrunPolicy == eOverrunPolicy::catchUp && maxCatchUpTicks != 0 &&
            m_consecutiveCatchUpTicks >= maxCatchUpTicks)
        {
            // Skip remaining missed ticks.
            overrunPolicy = eOverrunPolicy::skipMissedTicks;
        }

        switch (overrunPolicy)
        {
        case eOverrunPolicy::catchUp:
        {
            // Retain the original deadline.
            m_tickInfo.catchUpTick = true;

            if (maxCatchUpTicks != 0)
            {
                ++m_consecutiveCatchUpTicks;
            }
            else
            {
                // Unlimited catch-up.
                m_consecutiveCatchUpTicks = 0;
            }

            break;
        }

        case eOverrunPolicy::skipMissedTicks:
        {
            // Advance to the first future deadline.
            auto const missedTicks = ((now - m_tickInfo.scheduledTime) / duration) + 1;

            m_tickInfo.scheduledTime += duration * missedTicks;

            m_tickInfo.skippedTicks = static_cast<uint64_t>(missedTicks);

            m_tickInfo.tickIndex += m_tickInfo.skippedTicks;

            m_consecutiveCatchUpTicks = 0;
            break;
        }

        case eOverrunPolicy::resynchronise:
        {
            // Count expired deadlines on the old timeline.
            auto const missedTicks = ((now - m_tickInfo.scheduledTime) / duration) + 1;

            m_tickInfo.skippedTicks = static_cast<uint64_t>(missedTicks);

            m_tickInfo.tickIndex += m_tickInfo.skippedTicks;

            // Establish a new timing reference.
            m_tickInfo.scheduledTime  = now + duration;
            m_tickInfo.resynchronised = true;

            m_consecutiveCatchUpTicks = 0;
            break;
        }
        }
    }
    else
    {
        // Normal timing cadence recovered.
        m_consecutiveCatchUpTicks = 0;
    }

    // Wait until the scheduled deadline or until termination.
    // Ignore any stale event signal from a previous execution.
    while (m_updateEvent.WaitUntilTime(m_tickInfo.scheduledTime))
    {
        if (IsTerminating())
        {
            break;
        }
    }
}

void PeriodicThread::OnThreadTermination() NO_EXCEPT_
{
    m_timingInitialised = false;
}

void PeriodicThread::ProcessTerminationConditions() NO_EXCEPT_
{
    m_updateEvent.Signal();
}

auto PeriodicThread::ConvertPeriod(int64_t period, eWaitTimeUnit timeUnit) -> duration_t
{
    using namespace std::chrono;

    if (period <= 0)
    {
        throw std::invalid_argument("PeriodicThread period must be greater than zero");
    }

    switch (timeUnit)
    {
    case eWaitTimeUnit::nanoseconds:
        return CheckedDurationCast<duration_t>(nanoseconds{period});

    case eWaitTimeUnit::microseconds:
        return CheckedDurationCast<duration_t>(microseconds{period});

    case eWaitTimeUnit::milliseconds:
        return CheckedDurationCast<duration_t>(milliseconds{period});

    case eWaitTimeUnit::seconds:
        return CheckedDurationCast<duration_t>(seconds{period});

    default:
        throw std::invalid_argument("Invalid time unit");
    }
}

} // namespace threads
} // namespace core_lib