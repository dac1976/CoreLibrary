// CoreLibrary containing useful reusable utility classes.
// Copyright (C) 2014 Duncan Crutchley
//
// Contact <duncan.crutchley+corelibrary@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
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
 * \file ThreadBase.cpp
 * \brief File containing definition of ThreadBase class.
 */

#include "../ThreadBase.hpp"
#include <chrono>

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xThreadNotStartedError' definition
// ****************************************************************************
xThreadNotStartedError::xThreadNotStartedError()
    : exceptions::xCustomException{"thread not started"}
{
}

xThreadNotStartedError::xThreadNotStartedError(const std::string& message)
    : exceptions::xCustomException{message}
{
}

xThreadNotStartedError::~xThreadNotStartedError()
{
}

// ****************************************************************************
// 'class ThreadBase' definition
// ****************************************************************************
bool ThreadBase::IsStarted() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_started;
}

void ThreadBase::Start()
{
    if (!IsStarted() && !IsTerminating())
    {
        m_thread = std::thread(&ThreadBase::Run, this);
        SetStarted(true);
        SetThreadIdAndNativeHandle(m_thread.get_id()
                                   , m_thread.native_handle());
    }
}

void ThreadBase::Stop()
{
    if (IsStarted() && !IsTerminating())
    {
        SetTerminating(true);
        ProcessTerminationConditions();

        if (m_thread.joinable())
        {
            m_thread.join();
        }

        SetTerminating(false);
    }
}

std::thread::id ThreadBase::ThreadID() const
{
    if (!IsStarted() || IsTerminating())
    {
        BOOST_THROW_EXCEPTION(xThreadNotStartedError());
    }

    std::lock_guard<std::mutex> lock{m_mutex};
    return m_threadId;
}

std::thread::native_handle_type ThreadBase::NativeHandle() const
{
    if (!IsStarted() || IsTerminating())
    {
        BOOST_THROW_EXCEPTION(xThreadNotStartedError{});
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
        BOOST_THROW_EXCEPTION(xThreadNotStartedError{});
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(milliSecs));
}

void ThreadBase::ProcessTerminationConditions()
{
    // nothing required here but override in derived class
}

void ThreadBase::SetThreadIdAndNativeHandle(const std::thread::id& threadId
                                            , const std::thread::native_handle_type&
                                                nativeHandle)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_threadId = threadId;
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
    while (!IsTerminating())
    {
        ThreadIteration();
    }

    SetStarted(false);
}

} // namespace threads
} // namespace core_lib
