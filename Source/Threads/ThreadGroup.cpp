// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2015 Duncan Crutchley
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
 * \file ThreadGroup.cpp
 * \brief File containing thread group definition.
 */

#include "Threads/ThreadGroup.h"
#include "Threads/JoinThreads.h"
#include <algorithm>

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xThreadNotStartedError' definition
// ****************************************************************************
xThreadGroupError::xThreadGroupError()
	: exceptions::xCustomException("thread group error")
{
}

xThreadGroupError::xThreadGroupError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xThreadGroupError::~xThreadGroupError()
{
}

// ****************************************************************************
// 'class ThreadGroup' definition
// ****************************************************************************

ThreadGroup::~ThreadGroup()
{
	std::for_each(m_threadGroup.begin()
				  , m_threadGroup.end()
				  , DeleteThread);
}

bool ThreadGroup::IsThisThreadIn() const
{
	return IsThreadIn(std::this_thread::get_id());
}

bool ThreadGroup::IsThreadIn(std::thread* threadPtr) const
{
	if (!threadPtr)
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
	if (!threadPtr)
	{
		return;
	}

	std::lock_guard<std::mutex> lock{m_mutex};

	if (IsThreadInNoMutex(threadPtr->get_id()))
	{
		BOOST_THROW_EXCEPTION(xThreadGroupError("thread already in group"));
	}

	m_threadGroup.push_back(threadPtr);
}

void ThreadGroup::RemoveThread(std::thread* threadPtr)
{
	if (!threadPtr)
	{
		return;
	}

	std::lock_guard<std::mutex> lock{m_mutex};

	if (!IsThreadInNoMutex(threadPtr->get_id()))
	{
		return;
	}

	thread_list_iter threadIt{std::find(m_threadGroup.begin()
										, m_threadGroup.end()
										, threadPtr)};
	if (threadIt != m_threadGroup.end())
	{
		// cppcheck-suppress invalidIterator1
		m_threadGroup.erase(threadIt);
	}
}

std::thread* ThreadGroup::RemoveThread(const std::thread::id& id)
{
	std::lock_guard<std::mutex> lock{m_mutex};
	std::thread* t{};

	for (thread_list_iter tIt = m_threadGroup.begin()
		 ; tIt != m_threadGroup.end()
		 ; ++tIt)
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

void ThreadGroup::JoinAll()
{
	std::lock_guard<std::mutex> lock{m_mutex};

	if (IsThisThreadInNoMutex())
	{
		BOOST_THROW_EXCEPTION(xThreadGroupError("thread cannot join itself"));
	}

	JoinThreadsP<std::list> joiner(m_threadGroup);
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

bool ThreadGroup::IsThisThreadInNoMutex() const
{
	return IsThreadInNoMutex(std::this_thread::get_id());
}

bool ThreadGroup::IsThreadInNoMutex(const std::thread::id& id) const
{
	for (const auto threadPtr : m_threadGroup)
	{
		if (threadPtr->get_id() == id)
		{
			return true;
		}
	}

	return false;
}

void ThreadGroup::DeleteThread(std::thread* threadPtr)
{
	delete threadPtr;
}

} // namespace threads
} // namespace core_lib
