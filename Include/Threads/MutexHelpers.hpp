// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
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

#ifndef MutexHelpersHPP
#define MutexHelpersHPP

//-----------------------------------------------------------------------------
#include <mutex>

#if !defined(HGL_NO_SHARED_MUTEX)
#include <shared_mutex>
#endif

//-----------------------------------------------------------------------------
#define STD_LOCK_GUARD(m) std::lock_guard<decltype(m)> lock(m)

#define STD_UNIQUE_LOCK(m) std::unique_lock<decltype(m)> lock(m)

#if !defined(HGL_NO_SHARED_MUTEX)
#define STD_SHARED_LOCK(m) std::shared_lock<decltype(m)> lock(m)
#endif

#define STD_SCOPED_LOCK(...) std::scoped_lock lock(__VA_ARGS__)

//-----------------------------------------------------------------------------
#endif //MutexHelpersHPP
