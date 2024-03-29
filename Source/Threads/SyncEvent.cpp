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
 * \file SyncEvent.cpp
 * \brief File containing definition of SyncEvent class.
 */

#include "Threads/SyncEvent.h"
#include <chrono>

namespace core_lib
{
namespace threads
{

// ****************************************************************************
// 'class SyncEvent' definition
// ****************************************************************************
SyncEvent::SyncEvent(eNotifyType notifyCondition, eResetCondition resetCondition,
                     eIntialCondition initialCondition, Condition* condition)
    : m_signalAllThreads(notifyCondition == eNotifyType::signalAllThreads)
    , m_autoReset(m_signalAllThreads ? false : resetCondition == eResetCondition::autoReset)
    , m_getCondition(nullptr == condition ? std::bind(&SyncEvent::SignalFlag, this)
                                          : condition->getCondition)
    , m_setCondition(nullptr == condition
                         ? std::bind(&SyncEvent::SetSignalFlag, this, std::placeholders::_1)
                         : condition->setCondition)
{
    m_setCondition(initialCondition == eIntialCondition::signalled);
}

void SyncEvent::Wait()
{
    std::unique_lock<std::mutex> lock(m_signalMutex);
    m_signalCondVar.wait(lock, [this] { return m_getCondition(); });

    if (m_autoReset && m_getCondition())
    {
        m_setCondition(false);
    }
}

bool SyncEvent::WaitForTime(unsigned int period, eWaitTimeUnit timeUnit)
{
    std::unique_lock<std::mutex> lock(m_signalMutex);
    bool                         result;

    switch (timeUnit)
    {
    case eWaitTimeUnit::seconds:
        result = m_signalCondVar.wait_for(
            lock, std::chrono::seconds(period), [this] { return m_getCondition(); });
        break;
    case eWaitTimeUnit::microseconds:
        result = m_signalCondVar.wait_for(
            lock, std::chrono::microseconds(period), [this] { return m_getCondition(); });
        break;
    case eWaitTimeUnit::nanoseconds:
        result = m_signalCondVar.wait_for(
            lock, std::chrono::nanoseconds(period), [this] { return m_getCondition(); });
        break;
    case eWaitTimeUnit::milliseconds:
    default:
        result = m_signalCondVar.wait_for(
            lock, std::chrono::milliseconds(period), [this] { return m_getCondition(); });
        break;
    }

    if (m_autoReset && m_getCondition())
    {
        m_setCondition(false);
    }

    return result;
}

void SyncEvent::Signal()
{
    {
        std::lock_guard<std::mutex> lock(m_signalMutex);
        m_setCondition(true);
    }

    if (m_signalAllThreads)
    {
        m_signalCondVar.notify_all();
    }
    else
    {
        m_signalCondVar.notify_one();
    }
}

void SyncEvent::Reset()
{
    std::lock_guard<std::mutex> lock(m_signalMutex);

    if (!m_autoReset)
    {
        m_setCondition(false);
    }
}

bool SyncEvent::SignalFlag() const
{
    return m_signalFlag;
}

void SyncEvent::SetSignalFlag(bool signalFlag)
{
    m_signalFlag = signalFlag;
}

} // namespace threads
} // namespace core_lib
