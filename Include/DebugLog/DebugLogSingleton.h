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

/*!
 * \file DebugLogSingleton.h
 * \brief File containing declaration of DebugLog Singleton.
 */

#ifndef DEBUGLOGSINGLETON
#define DEBUGLOGSINGLETON

#include "DebugLog.h"

#if defined(CORE_LIB_LOKI)
#include "LokiPatch/LokiSingleton.hpp"

using debug_singelton_t =
    Loki::SingletonHolder<core_lib::log::default_log_t, Loki::CreateUsingNew, Loki::DeletableSingleton>;

using debug_singelton_deleter_t = Loki::DeletableSingleton<core_lib::log::default_log_t>;
#else
#include "Singleton/ManagedSingleton.hpp"

using debug_singelton_t = core_lib::ManagedSingleton<core_lib::log::default_log_t>;
#endif // CORE_LIB_LOKI

namespace core_lib
{
core_lib::log::default_log_t& DebugLogInstance();

bool DebugLogExists();

void DebugLogGracefulDelete();
}

/*! \brief Macro defining our actual log's singelton. */
#define DEBUG_LOG_SINGLETON core_lib::DebugLogInstance()

/*! \brief Macro tetsing existence of log's singelton. */
#define DEBUG_LOG_SINGLETON_EXISTS core_lib::DebugLogExists()

/*! \brief Macro to help instantiate singleton. */
#define DEBUG_LOG_SINGLETON_INSTANTIATE(v, p, f) DEBUG_LOG_SINGLETON.Instantiate(v, p, f)

/*! \brief Macro to help instantiate singleton (extended version 1). */
#define DEBUG_LOG_SINGLETON_INSTANTIATE_EX(v, p, f, s) DEBUG_LOG_SINGLETON.Instantiate(v, p, f, s)

/*! \brief Macro to help instantiate singleton (extended version 2). */
#define DEBUG_LOG_SINGLETON_INSTANTIATE_EX2(v, p, f, s, e)                                         \
    DEBUG_LOG_SINGLETON.Instantiate(v, p, f, s, e)

/*! \brief Macro to help instantiate singleton (extended version 3). */
#define DEBUG_LOG_SINGLETON_INSTANTIATE_EX3(v, p, f, e)                                            \
    DEBUG_LOG_SINGLETON.Instantiate(v, p, f, 5 * core_lib::log::BYTES_IN_MEBIBYTE, e)

/*! \brief Macro to help instantiate singleton (extended version 4). */
#define DEBUG_LOG_SINGLETON_INSTANTIATE_EX4(v, p, f, s, e, u, z)                                   \
    DEBUG_LOG_SINGLETON.Instantiate(v, p, f, s, e, u, z)

/*! \brief Macro to help instantiate singleton (extended version 5). */
#define DEBUG_LOG_SINGLETON_INSTANTIATE_EX5(v, p, f, s, e, u, z, m)                                \
    DEBUG_LOG_SINGLETON.Instantiate(v, p, f, s, e, u, z, m)

/*! \brief Macro defining a singleton deleter. */
#define DEBUG_LOG_SINGLETON_DELETER core_lib::DebugLogGracefulDelete()

#endif // DEBUGLOGSINGLETON
