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
 * \file DebugLogging.h
 * \brief Wrapper header linking to other logging headers.
 */

#ifndef DEBUGLOGGING
#define DEBUGLOGGING

#include "DebugLogSingleton.h"

/*!
 * \brief Simple macro to simplify logging.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via std::ostringstream).
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
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via std::ostringstream).
 * \param[in] l - Log message level from enum eLogMessageLevel.
 */
#define DEBUG_LOG_EX(x, m, l)   \
	do                          \
{                           \
	std::ostringstream os;  \
	os << m;                \
	x.AddLogMessage(os.str(), std::string(__FILE__), __LINE__, l); \
	} while(false)

/*!
 * \brief Macro to add message level to filter set.
 * \param[in] x - DebugLog object.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 */
#define DEBUG_LOG_ADD_FILTER(x, l) \
	x.AddLogMsgLevelFilter(l)

/*!
 * \brief Macro to remove message level to filter set.
 * \param[in] x- DebugLog object.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 */
#define DEBUG_LOG_REMOVE_FILTER(x, l) \
	x.RemoveLogMsgLevelFilter(l)

/*!
 * \brief Macro to clear filter set.
 * \param[in] x - DebugLog object.
 */
#define DEBUG_LOG_CLEAR_FILTERS(x) \
	x.ClearLogMsgLevelFilters()

/*!
 * \brief Macro to simplify instantiation of debug log.
 * \param[in] v - Software version string must be convertible to std::string.
 * \param[in] p - Log file path string with trailing backslash must be convertible to std::string.
 * \param[in] f - Log file name string without file extension with trailing backslash must be convertible to std::string.
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_INSTANTIATE(v, p, f) \
	core_lib::log::DebugLogSingleton::Instance().Instantiate(v, p, f)

/*!
 * \brief Macro to simplify instantiation of debug log.
 * \param[in] v - Software version string must be convertible to std::string.
 * \param[in] p - Log file path string with trailing backslash must be convertible to std::string.
 * \param[in] f - Log file name string without file extension with trailing backslash must be convertible to std::string.
 * \param[in] s - The maximum size for the log file.
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_INSTANTIATE_EX(v, p, f, s) \
	core_lib::log::DebugLogSingleton::Instance().Instantiate(v, p, f, s)

/*!
 * \brief Macro to gracefully delete the debug singleton.
 *
 * This macro should rarely need to be used and only in the case
 * where you do not want to let the singleton manage the destruction
 * of the debug log object.
 */
#define DEBUG_MESSAGE_DELETE_SINGLETON() \
    Loki::DeletableSingleton<core_lib::log::default_log_t>::GracefulDelete()

/*!
 * \brief Simple macro to simplify logging.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via std::ostringstream).
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE(m) \
	DEBUG_LOG(core_lib::log::DebugLogSingleton::Instance(), m)

/*!
 * \brief Macro to simplify logging adding message and level.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via std::ostringstream).
 * \param[in] l - Log message level from enum eLogMessageLevel.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_EX(m, l) \
	DEBUG_LOG_EX(core_lib::log::DebugLogSingleton::Instance(), m, l)

/*!
 * \brief Macro to add message level to filter set.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_ADD_FILTER(l) \
	DEBUG_LOG_ADD_FILTER(core_lib::log::DebugLogSingleton::Instance(), l)

/*!
 * \brief Macro to remove message level from filter set.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_REMOVE_FILTER(l) \
	DEBUG_LOG_ADD_FILTER(core_lib::log::DebugLogSingleton::Instance(), l)

/*!
 * \brief Macro to clear filter set.
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_CLEAR_FILTERS() \
	DEBUG_LOG_CLEAR_FILTERS(core_lib::log::DebugLogSingleton::Instance())

#endif // DEBUGLOGGING
