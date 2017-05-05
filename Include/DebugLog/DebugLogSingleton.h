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
#include "loki/Singleton.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The log namespace. */
namespace log {

/*! \brief Typedef defining our default log's type. */
typedef DebugLog<DefaultLogFormat> default_log_t;

/*! \brief Typedef defining our actual log's singelton. */
typedef Loki::SingletonHolder<default_log_t
                              , Loki::CreateUsingNew
                              , Loki::DeletableSingleton>
    DebugLogSingleton;

/*! \brief Typedef defining a singleton deleter. */
typedef Loki::DeletableSingleton<default_log_t>
    DebugLogSingletonDeleter;

} // namespace log
} // namespace core_lib

#endif // DEBUGLOGSINGLETON
