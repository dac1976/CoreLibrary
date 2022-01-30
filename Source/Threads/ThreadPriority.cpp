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
 * \file ThreadPriority.cpp
 * \brief File containing definition of ThreadPriority functions.
 */
#include "ThreadPriority.h"
#if BOOST_OS_LINUX
#include <pthread.h>
#else
#include <Windows.h>
#endif

namespace hgl
{

#if BOOST_OS_LINUX
bool SetThreadPriority(std::thread::native_handle_type const& threadId,
                       eHglThreadSchedAlgo schedAlgo, eHglThreadPriority priority)
{
    sched_param sch;
    int         policy;
    pthread_getschedparam(threadId, &policy, &sch);

    sch.sched_priority = eHglThreadSchedAlgo::regular == schedAlgo ? 0 : static_cast<int>(priority);

    if (pthread_setschedparam(threadId, static_cast<int>(schedAlgo), &sch) != 0)
    {
        return false;
    }

    return true;
}
#else
bool SetThreadPriority(std::thread::native_handle_type const& threadId, eHglThreadPriority priority)
{
    // Set thread priority to time critical
    return ::SetThreadPriority(reinterpret_cast<HANDLE>(threadId), static_cast<int>(priority));
}
#endif

} // namespace hgl
