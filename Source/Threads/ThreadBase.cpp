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
 * \file ThreadBase.cpp
 * \brief File containing definition of ThreadBase class.
 */

#include "Threads/ThreadBase.h"
#include <chrono>
#include <stdexcept>
#include <boost/throw_exception.hpp>
namespace core_lib
{
namespace threads
{

// ****************************************************************************
// 'class ThreadBase' definition
// ****************************************************************************
bool ThreadBase::IsStarted() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_started;
}

bool ThreadBase::Start()
{
    if (!IsStarted() && !IsTerminating())
    {
        m_thread = std::thread(&ThreadBase::Run, this);
        SetStarted(true);
        SetThreadIdAndNativeHandle(m_thread.get_id(), m_thread.native_handle());
    }

    return IsStarted();
}

bool ThreadBase::Stop()
{
    if (IsStarted() && !IsTerminating())
    {
        SetTerminating(true);
        ProcessTerminationConditions();
    }

    if (m_thread.joinable())
    {
        m_thread.join();
    }
    else
    {
        SetTerminating(false);
    }

    return !IsStarted();
}

std::thread::id ThreadBase::ThreadID() const
{
    if (!IsStarted() || IsTerminating())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("thread not running"));
    }

    std::lock_guard<std::mutex> lock{m_mutex};
    return m_threadId;
}

std::thread::native_handle_type ThreadBase::NativeHandle() const
{
    if (!IsStarted() || IsTerminating())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("thread not running"));
    }

    std::lock_guard<std::mutex> lock{m_mutex};
    return m_nativeHandle;
}

bool ThreadBase::IsTerminating() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_terminating;
}

void ThreadBase::SleepForTime(unsigned int milliSecs) const
{
    if (!IsStarted() || IsTerminating())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("thread not running"));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(milliSecs));
}

void ThreadBase::ProcessTerminationConditions() NO_EXCEPT_
{
    // nothing required here but override in derived class
}

void ThreadBase::SetThreadIdAndNativeHandle(const std::thread::id&                 threadId,
                                            const std::thread::native_handle_type& nativeHandle)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_threadId     = threadId;
    m_nativeHandle = nativeHandle;
}

void ThreadBase::SetStarted(bool started)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_started = started;
}

void ThreadBase::SetTerminating(bool terminating)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_terminating = terminating;
}

void ThreadBase::Run()
{
	SetStarted(true);
	
    while (!IsTerminating())
    {
        ThreadIteration();
    }

    SetStarted(false);
    SetTerminating(false);
}

} // namespace threads
} // namespace core_lib
