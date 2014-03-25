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
 * \file DebugLogging.hpp
 * \brief Wrapper header linking to other logging headers.
 */

#ifndef DEBUGLOGGING_HPP
#define DEBUGLOGGING_HPP

#include "DebugLog/DebugLog.hpp"

/*!
 * \brief Simple macro to simplify logging.
 * \param [IN] DebugLog object.
 * \param [IN] Object to be used as message in DebugLog (must be convertible to string via std::ostringstream).
 */
#define DEBUG_LOG(x, m)            \
    do                             \
    {                              \
        std::ostringstream os;     \
        os << m;    		       \
        x.AddLogMessage(os.str()); \
    } while(false)

/*!
 * \brief Macro to simplify logging adding message and level.
 * \param [IN] DebugLog object.
 * \param [IN] Object to be used as message in DebugLog (must be convertible to string via std::ostringstream).
 * \param [IN] Level associated with message.
 */
#define DEBUG_LOG_EX(x, m, l)     \
    do                            \
    {                             \
        std::ostringstream os;    \
        os << m;    		      \
        x.AddLogMessage(os.str(), std::string(__FILE__), __LINE__, l); \
    } while(false)

/*!
 * \brief Macro to add message level to filter set.
 * \param [IN] DebugLog object.
 * \param [IN] Log message level, e.g. info, debug, warning, error etc.
 */
#define DEBUG_LOG_ADD_FILTER(x, l) \
    x.AddLogMsgLevelFilter(l)

/*!
 * \brief Macro to remove message level to filter set.
 * \param [IN] DebugLog object.
 * \param [IN] Log message level, e.g. info, debug, warning, error etc.
 */
#define DEBUG_LOG_REMOVE_FILTER(x, l) \
    x.RemoveLogMsgLevelFilter(l)

/*!
 * \brief Macro to clear filter set.
 * \param [IN] DebugLog object.
 */
#define DEBUG_LOG_CLEAR_FILTER(x) \
    x.ClearLogMsgLevelFilters()

#endif // DEBUGLOGGING_HPP
