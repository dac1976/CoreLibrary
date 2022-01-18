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
 * \file EventThread.cpp
 * \brief File containing definition of EventThread class.
 */
#include "Threads/EventThread.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

// ****************************************************************************
// 'class EventThread' definition
// ****************************************************************************
EventThread::EventThread(event_callback_t const& eventCallback, unsigned int eventPeriod,
                         bool delayedStart, eWaitTimeUnit timeUnit)
    : m_eventCallback(eventCallback)
    , m_eventPeriod(eventPeriod)
    , m_timeUnit(timeUnit)
{
    if (!delayedStart)
    {
        Start();
    }
}

EventThread::~EventThread()
{
    Stop();
}

void EventThread::EventPeriod(unsigned int eventPeriod, eWaitTimeUnit timeUnit)
{
    std::lock_guard<std::mutex> lock(m_eventPeriodMutex);
    m_eventPeriod = eventPeriod;
    m_timeUnit    = timeUnit;
}

unsigned int EventThread::EventPeriod(eWaitTimeUnit* timeUnit) const
{
    std::lock_guard<std::mutex> lock(m_eventPeriodMutex);

    if (nullptr != timeUnit)
    {
        *timeUnit = m_timeUnit;
    }

    return m_eventPeriod;
}

void EventThread::ForceTick()
{
	m_updateEvent.Signal();
}

void EventThread::ThreadIteration() NO_EXCEPT_
{
	try
	{
		if (m_eventCallback)
		{
			m_eventCallback();
		}

	}
	catch(...)
	{
		// Do nothing.
	}
	
    eWaitTimeUnit timeUnit;
    auto          period = EventPeriod(&timeUnit);
    m_updateEvent.WaitForTime(period, timeUnit);
}

void EventThread::ProcessTerminationConditions() NO_EXCEPT_
{
    m_updateEvent.Signal();
}

} // namespace threads
} // namespace core_lib
