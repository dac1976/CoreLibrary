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
 * \file DebugLogSingleton.hpp
 * \brief File containing declaration of DebugLog Singleton.
 */

#ifndef DEBUGLOGSINGLETON_HPP
#define DEBUGLOGSINGLETON_HPP

#include "DebugLog.hpp"
#include <loki/Singleton.h>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The log namespace. */
namespace log {

typedef DebugLog<DefaultLogFormat> default_log_t;

typedef Loki::SingletonHolder<default_log_t
                              , Loki::CreateUsingNew
                              , Loki::DeletableSingleton>
            DebugLogSingleton;


} // namespace log
} // namespace core_lib

#endif // DEBUGLOGSINGLETON_HPP
