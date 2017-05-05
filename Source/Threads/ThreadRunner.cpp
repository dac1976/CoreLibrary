// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
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
 * \file ThreadRunner.cpp
 * \brief File containing definition of ThreadRunner class.
 */
#include "Threads/ThreadRunner.h"

namespace core_lib
{
namespace threads
{

// ****************************************************************************
// 'class ThreadRunner' definition
// ****************************************************************************
ThreadRunner::ThreadRunner(const thread_function_t& threadFunction,
                           const thread_function_t& processTerminationConditions,
                           const bool               autoStart)
    : ThreadBase()
    , m_threadFunction(threadFunction)
    , m_processTerminationConditions(processTerminationConditions)
{
    if (autoStart)
    {
        Start();
    }
}

ThreadRunner::~ThreadRunner()
{
    Stop();
}

void ThreadRunner::SleepThreadForTime(const unsigned int milliSecs) const
{
    SleepForTime(milliSecs);
}

void ThreadRunner::ThreadIteration() NO_EXCEPT_
{
    if (m_threadFunction)
    {
        m_threadFunction();
    }
}

void ThreadRunner::ProcessTerminationConditions() NO_EXCEPT_
{
    if (m_processTerminationConditions)
    {
        m_processTerminationConditions();
    }
}

} // namespace threads
} // namespace core_lib
