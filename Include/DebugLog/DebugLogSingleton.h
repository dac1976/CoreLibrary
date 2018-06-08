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
 * \file DebugLogSingleton.h
 * \brief File containing declaration of DebugLog Singleton.
 */

#ifndef DEBUGLOGSINGLETON
#define DEBUGLOGSINGLETON

#include "DebugLog.h"
#include <loki/Singleton.h>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The log namespace. */
namespace log
{

/*! \brief Typedef defining our default log's type. */
using default_log_t = hgl::log::DebugLog<DefaultLogFormat>;

} // namespace log
} // namespace core_lib

using debug_singelton_t = Loki::SingletonHolder<core_lib::log::default_log_t, Loki::CreateUsingNew,
                                                Loki::DeletableSingleton>;

using debug_singelton_deleter_t = Loki::DeletableSingleton<core_lib::log::default_log_t>;

inline core_lib::log::default_log_t& DebugLogInstance()
{
    return debug_singelton_t::Instance();
}

inline void DebugLogGracefulDelete()
{
    debug_singelton_deleter_t::GracefulDelete();
}

/*! \brief Macro defining our actual log's singelton. */
#define DEBUG_LOG_SINGLETON DebugLogInstance()

/*! \brief Macro defining a singleton deleter. */
#define DEBUG_LOG_SINGLETON_DELETER DebugLogGracefulDelete()

#endif // DEBUGLOGSINGLETON
