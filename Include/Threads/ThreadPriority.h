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
 * \file ThreadPriority.h
 * \brief File containing declaration of SyncEvent class.
 */
 
#ifndef THREADPRIORITY_H
#define THREADPRIORITY_H

#include <thread>
#include <boost/predef.h>

namespace hgl
{
#if BOOST_OS_LINUX
enum class eHglThreadSchedAlgo
{
    // Regular scheduling (same as SCHED_OTHER)
    regular = 0,
    // FIFO RT schedulding (same as SCHED_FIFO)
    fifo = 1,
    // Round-robin RT scheduling  (same as SCHED_RR)
    roundRobin = 2
};

// When scheduler algorithm is FIFO or Round-robin the priority
// can be any value from 1 (lowest priority) to 99 (highest
// priority).
enum class eHglThreadPriority
{
    idle         = 1,
    veryLow      = 15,
    low          = 33,
    belowNormal  = 45,
    normal       = 50,
    aboveNormal  = 55,
    high         = 66,
    veryHigh     = 85,
    timeCritical = 99
};

// The arg priority only applies when schedAlgo != eHglThreadSchedAlgo::other
bool SetThreadPriority(std::thread::native_handle_type const& threadId,
                       eHglThreadSchedAlgo schedAlgo, eHglThreadPriority priority);
#else
// Values set to match priority constant values from Windows API.
// Except we have included 2 extra ones (veryLow and veryHigh).
enum class eHglThreadPriority
{
    idle         = -15,
    veryLow      = -10,
    low          = -2,
    belowNormal  = -1,
    normal       = 0,
    aboveNormal  = 1,
    high         = 2,
    veryHigh     = 10,
    timeCritical = 15
};

bool SetThreadPriority(std::thread::native_handle_type const& threadId,
                       eHglThreadPriority                     priority);
#endif

} // namespace hgl

#endif // THREADPRIORITY_H
