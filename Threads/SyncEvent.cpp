// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
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
 * \file SyncEvent.cpp
 * \brief File containing definition of SyncEvent class.
 */

#include "../SyncEvent.hpp"
#include <chrono>

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class SyncEvent' definition
// ****************************************************************************
SyncEvent::SyncEvent(eNotifyType notifyCondition
                     , eResetCondition resetCondition
                     , eIntialCondition initialCondition)
        : m_signalAllThreads{notifyCondition == eNotifyType::signalAllThreads}
        , m_autoReset{m_signalAllThreads
                      ? false
                      : resetCondition == eResetCondition::autoReset}
        , m_signalFlag{initialCondition == eIntialCondition::signalled}
{
}

void SyncEvent::Wait()
{
    std::unique_lock<std::mutex> lock{m_signalMutex};
    m_signalCondVar.wait(lock, [this]{ return m_signalFlag; });

    if (m_autoReset && m_signalFlag)
    {
        m_signalFlag = false;
    }
}

bool SyncEvent::WaitForTime(size_t milliseconds)
{
    std::unique_lock<std::mutex> lock{m_signalMutex};
    bool result
            = m_signalCondVar.wait_for(lock, std::chrono::milliseconds(milliseconds)
                                       , [this]{ return m_signalFlag; });

    if (m_autoReset && m_signalFlag)
    {
        m_signalFlag = false;
    }

    return result;
}

void SyncEvent::Signal()
{
    {
        std::lock_guard<std::mutex> lock{m_signalMutex};
        m_signalFlag = true;
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
    std::lock_guard<std::mutex> lock{m_signalMutex};

    if (!m_autoReset)
    {
        m_signalFlag = false;
    }
}

} // namespace threads
} // namespace core_lib
