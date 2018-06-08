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
 * \file DebugLogging.h
 * \brief Wrapper header linking to other logging headers.
 */

#ifndef DEBUGLOGGING
#define DEBUGLOGGING

#include "DebugLogSingleton.h"
#include <boost/current_function.hpp>

/*!
 * \brief Constants for each log error level.
 */
static constexpr core_lib::log::eLogMessageLevel LOG_LEVEL_DEBUG{
    core_lib::log::eLogMessageLevel::debug};
static constexpr core_lib::log::eLogMessageLevel LOG_LEVEL_INFO{
    core_lib::log::eLogMessageLevel::info};
static constexpr core_lib::log::eLogMessageLevel LOG_LEVEL_WARNING{
    core_lib::log::eLogMessageLevel::warning};
static constexpr core_lib::log::eLogMessageLevel LOG_LEVEL_ERROR{
    core_lib::log::eLogMessageLevel::error};
static constexpr core_lib::log::eLogMessageLevel LOG_LEVEL_FATAL{
    core_lib::log::eLogMessageLevel::fatal};

/*!
 * \brief Simple macro to simplify logging.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 */
#define DEBUG_LOG(x, m)                                                                            \
    do                                                                                             \
    {                                                                                              \
        std::ostringstream os;                                                                     \
        os << m;                                                                                   \
        x.AddLogMessage(os.str());                                                                 \
    } while (false)

/*!
 * \brief Macro to simplify logging adding message and level.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 * \param[in] l - Log message level from enum eLogMessageLevel.
 */
#define DEBUG_LOG_EX(x, m, l)                                                                      \
    do                                                                                             \
    {                                                                                              \
        std::ostringstream os;                                                                     \
        os << m;                                                                                   \
        x.AddLogMessage(os.str(), std::string(__FILE__), BOOST_CURRENT_FUNCTION, __LINE__, l);     \
    } while (false)

/*!
 * \brief Simple macro to simplify logging generating message with level debug.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 */
#define DEBUG_LOG_EX_DEBUG(x, m) DEBUG_LOG_EX(x, m, LOG_LEVEL_DEBUG)

/*!
 * \brief Simple macro to simplify logging generating message with level info.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 */
#define DEBUG_LOG_EX_INFO(x, m) DEBUG_LOG_EX(x, m, LOG_LEVEL_INFO)

/*!
 * \brief Simple macro to simplify logging generating message with level warning.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 */
#define DEBUG_LOG_EX_WARNING(x, m) DEBUG_LOG_EX(x, m, LOG_LEVEL_WARNING)

/*!
 * \brief Simple macro to simplify logging generating message with level error.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 */
#define DEBUG_LOG_EX_ERROR(x, m) DEBUG_LOG_EX(x, m, LOG_LEVEL_ERROR)

/*!
 * \brief Simple macro to simplify logging generating message with level fatal.
 * \param[in] x - DebugLog object.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 */
#define DEBUG_LOG_EX_FATAL(x, m) DEBUG_LOG_EX(x, m, LOG_LEVEL_FATAL)

/*!
 * \brief Macro to add message level to filter set.
 * \param[in] x - DebugLog object.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 */
#define DEBUG_LOG_ADD_FILTER(x, l) x.AddLogMsgLevelFilter(l)

/*!
 * \brief Macro to add message level debug to filter set.
 * \param[in] x - DebugLog object.
 */
#define DEBUG_LOG_ADD_FILTER_DEBUG(x) DEBUG_LOG_ADD_FILTER(x, LOG_LEVEL_DEBUG)

/*!
 * \brief Macro to add message level info to filter set.
 * \param[in] x - DebugLog object.
 */
#define DEBUG_LOG_ADD_FILTER_INFO(x) DEBUG_LOG_ADD_FILTER(x, LOG_LEVEL_INFO)

/*!
 * \brief Macro to add message level warning to filter set.
 * \param[in] x - DebugLog object.
 */
#define DEBUG_LOG_ADD_FILTER_WARNING(x) DEBUG_LOG_ADD_FILTER(x, LOG_LEVEL_WARNING)

/*!
 * \brief Macro to add message level error to filter set.
 * \param[in] x - DebugLog object.
 */
#define DEBUG_LOG_ADD_FILTER_ERROR(x) DEBUG_LOG_ADD_FILTER(x, LOG_LEVEL_ERROR)

/*!
 * \brief Macro to add message level fatal to filter set.
 * \param[in] x - DebugLog object.
 */
#define DEBUG_LOG_ADD_FILTER_FATAL(x) DEBUG_LOG_ADD_FILTER(x, LOG_LEVEL_FATAL)

/*!
 * \brief Macro to remove message level from filter set.
 * \param[in] x- DebugLog object.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 */
#define DEBUG_LOG_REMOVE_FILTER(x, l) x.RemoveLogMsgLevelFilter(l)

/*!
 * \brief Macro to remove message level debug from filter set.
 * \param[in] x- DebugLog object.
 */
#define DEBUG_LOG_REMOVE_FILTER_DEBUG(x) DEBUG_LOG_REMOVE_FILTER(x, LOG_LEVEL_DEBUG)

/*!
 * \brief Macro to remove message level info from filter set.
 * \param[in] x- DebugLog object.
 */
#define DEBUG_LOG_REMOVE_FILTER_INFO(x) DEBUG_LOG_REMOVE_FILTER(x, LOG_LEVEL_INFO)

/*!
 * \brief Macro to remove message level warning from filter set.
 * \param[in] x- DebugLog object.
 */
#define DEBUG_LOG_REMOVE_FILTER_WARNING(x) DEBUG_LOG_REMOVE_FILTER(x, LOG_LEVEL_WARNING)

/*!
 * \brief Macro to remove message level error from filter set.
 * \param[in] x- DebugLog object.
 */
#define DEBUG_LOG_REMOVE_FILTER_ERROR(x) DEBUG_LOG_REMOVE_FILTER(x, LOG_LEVEL_ERROR)

/*!
 * \brief Macro to remove message level fatal from filter set.
 * \param[in] x- DebugLog object.
 */
#define DEBUG_LOG_REMOVE_FILTER_FATAL(x) DEBUG_LOG_REMOVE_FILTER(x, LOG_LEVEL_FATAL)

/*!
 * \brief Macro to clear filter set.
 * \param[in] x - DebugLog object.
 */
#define DEBUG_LOG_CLEAR_FILTERS(x) x.ClearLogMsgLevelFilters()

/*!
 * \brief Macro to simplify instantiation of debug log.
 * \param[in] v - Software version string must be convertible to std::string.
 * \param[in] p - Log file path string with trailing backslash must be convertible to std::string.
 * \param[in] f - Log file name string without file extension with trailing backslash must be
 * convertible to std::string.
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_INSTANTIATE(v, p, f) DEBUG_LOG_SINGLETON.Instantiate(v, p, f)

/*!
 * \brief Macro to simplify instantiation of debug log.
 * \param[in] v - Software version string must be convertible to std::string.
 * \param[in] p - Log file path string with trailing backslash must be convertible to std::string.
 * \param[in] f - Log file name string without file extension with trailing backslash must be
 * convertible to std::string.
 * \param[in] s - The maximum size for the log file.
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_INSTANTIATE_EX(v, p, f, s) DEBUG_LOG_SINGLETON.Instantiate(v, p, f, s)

/*!
 * \brief Macro to gracefully delete the debug singleton.
 *
 * This macro should rarely need to be used and only in the case
 * where you do not want to let the singleton manage the destruction
 * of the debug log object.
 */
#define DEBUG_MESSAGE_DELETE_SINGLETON() DEBUG_LOG_SINGLETON_DELETER

/*!
 * \brief Simple macro to simplify logging.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE(m) DEBUG_LOG(DEBUG_LOG_SINGLETON, m)

/*!
 * \brief Macro to simplify logging adding message and level.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 * \param[in] l - Log message level from enum eLogMessageLevel.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_EX(m, l) DEBUG_LOG_EX(DEBUG_LOG_SINGLETON, m, l)

/*!
 * \brief Simple macro to simplify logging generating message with level debug.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_EX_DEBUG(m) DEBUG_LOG_EX_DEBUG(DEBUG_LOG_SINGLETON, m)

/*!
 * \brief Simple macro to simplify logging generating message with level info.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_EX_INFO(m) DEBUG_LOG_EX_INFO(DEBUG_LOG_SINGLETON, m)

/*!
 * \brief Simple macro to simplify logging generating message with level warning.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_EX_WARNING(m) DEBUG_LOG_EX_WARNING(DEBUG_LOG_SINGLETON, m)

/*!
 * \brief Simple macro to simplify logging generating message with level error.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_EX_ERROR(m) DEBUG_LOG_EX_ERROR(DEBUG_LOG_SINGLETON, m)

/*!
 * \brief Simple macro to simplify logging generating message with level fatal.
 * \param[in] m - Object to be used as message in DebugLog (must be convertible to string via
 * std::ostringstream).
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_EX_FATAL(m) DEBUG_LOG_EX_FATAL(DEBUG_LOG_SINGLETON, m)

/*!
 * \brief Macro to add message level to filter set.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_ADD_FILTER(l) DEBUG_LOG_ADD_FILTER(DEBUG_LOG_SINGLETON, l)

/*!
 * \brief Macro to add message level debug to filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_ADD_FILTER_DEBUG() DEBUG_LOG_ADD_FILTER_DEBUG(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to add message level info to filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_ADD_FILTER_INFO() DEBUG_LOG_ADD_FILTER_INFO(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to add message level warning to filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_ADD_FILTER_WARNING() DEBUG_LOG_ADD_FILTER_WARNING(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to add message level error to filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_ADD_FILTER_ERROR() DEBUG_LOG_ADD_FILTER_ERROR(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to add message level fatal to filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_ADD_FILTER_FATAL() DEBUG_LOG_ADD_FILTER_FATAL(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to remove message level from filter set.
 * \param[in] l - Log message level from enum eLogMessageLevel.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_REMOVE_FILTER(l) DEBUG_LOG_ADD_FILTER(DEBUG_LOG_SINGLETON, l)

/*!
 * \brief Macro to remove message level debug from filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_REMOVE_FILTER_DEBUG() DEBUG_LOG_ADD_FILTER_DEBUG(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to remove message level info from filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_REMOVE_FILTER_INFO() DEBUG_LOG_ADD_FILTER_INFO(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to remove message level warning from filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_REMOVE_FILTER_WARNING() DEBUG_LOG_ADD_FILTER_WARNING(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to remove message level error from filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_REMOVE_FILTER_ERROR() DEBUG_LOG_ADD_FILTER_ERROR(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to remove message level fatal from filter set.
 *
 *  This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_REMOVE_FILTER_FATAL() DEBUG_LOG_ADD_FILTER_FATAL(DEBUG_LOG_SINGLETON)

/*!
 * \brief Macro to clear filter set.
 *
 * This version uses a singleton to maintain a global log object.
 */
#define DEBUG_MESSAGE_CLEAR_FILTERS() DEBUG_LOG_CLEAR_FILTERS(DEBUG_LOG_SINGLETON)

#endif // DEBUGLOGGING
