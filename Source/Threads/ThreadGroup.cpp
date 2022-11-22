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
 * \file ThreadGroup.cpp
 * \brief File containing thread group definition.
 */

#include "Threads/ThreadGroup.h"
#include <algorithm>
#include <stdexcept>
#include <boost/throw_exception.hpp>
#include "Threads/JoinThreads.h"

namespace core_lib
{
namespace threads
{

// ****************************************************************************
// 'class ThreadGroup' definition
// ****************************************************************************

ThreadGroup::~ThreadGroup()
{
	Clear();
}

bool ThreadGroup::IsThisThreadIn() const
{
    return IsThreadIn(std::this_thread::get_id());
}

bool ThreadGroup::IsThreadIn(std::thread* threadPtr) const
{
    if (threadPtr == nullptr)
    {
        return false;
    }
    else
    {
        return IsThreadIn(threadPtr->get_id());
    }
}

bool ThreadGroup::IsThreadIn(const std::thread::id& id) const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return IsThreadInNoMutex(id);
}

void ThreadGroup::AddThread(std::thread* threadPtr)
{
    if (threadPtr == nullptr)
    {
        return;
    }

    std::lock_guard<std::mutex> lock{m_mutex};

    if (IsThreadInNoMutex(threadPtr->get_id()))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("thread already in group"));
    }

    m_threadGroup.push_back(threadPtr);
}

void ThreadGroup::RemoveThread(std::thread* threadPtr)
{
    if (threadPtr == nullptr)
    {
        return;
    }

    std::lock_guard<std::mutex> lock{m_mutex};

    if (!IsThreadInNoMutex(threadPtr->get_id()))
    {
        return;
    }

    auto threadIt = std::find(m_threadGroup.begin(), m_threadGroup.end(), threadPtr);

    if (threadIt != m_threadGroup.end())
    {
        m_threadGroup.erase(threadIt);
    }
}

std::thread* ThreadGroup::RemoveThread(const std::thread::id& id)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    std::thread*                t{};

    for (auto tIt = m_threadGroup.begin(); tIt != m_threadGroup.end(); ++tIt)
    {
        if ((*tIt)->get_id() == id)
        {
            t = *tIt;
            m_threadGroup.erase(tIt);
            break;
        }
    }

    return t;
}

bool ThreadGroup::JoinAll()
{
    std::lock_guard<std::mutex> lock{m_mutex};

    if (IsThisThreadInNoMutex())
    {
        // Thread cannot join itself.
        return false;
    }

    JoinThreadsP<std::list> joiner(m_threadGroup);
    return true;
}

size_t ThreadGroup::Size() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_threadGroup.size();
}

bool ThreadGroup::Empty() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_threadGroup.empty();
}

void ThreadGroup::Clear()
{
    std::for_each(m_threadGroup.begin(), m_threadGroup.end(), DeleteThread);
	m_threadGroup.clear();
}

bool ThreadGroup::IsThisThreadInNoMutex() const
{
    return IsThreadInNoMutex(std::this_thread::get_id());
}

bool ThreadGroup::IsThreadInNoMutex(const std::thread::id& id) const
{
    return std::any_of(m_threadGroup.begin(),
                       m_threadGroup.end(),
                       [&](std::thread const* threadPtr) { return threadPtr->get_id() == id; });
}

void ThreadGroup::DeleteThread(std::thread* threadPtr)
{
    delete threadPtr;
}

} // namespace threads
} // namespace core_lib
