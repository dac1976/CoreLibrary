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
 * \file DebugLog.cpp
 * \brief File containing definition of DebugLog class.
 */
#include "DebugLog/DebugLog.h"
#if defined(_MSC_VER) && (_MSC_VER < 1920)
#include "Asio/MemoryUtils.hpp"
#endif
#include <iomanip>

namespace core_lib
{
namespace log
{

// ****************************************************************************
// General constant definitions
// ****************************************************************************
// ANSI Escape Codes
const char ANSI_RESET[]{"\x1b[0m"};

// Bright Foreground Colors (often denoted by 90-97)
const char ANSI_DEFAULT[]{"\x1b[0m"};
const char ANSI_RED[]{"\x1b[91m"};
const char ANSI_YELLOW[]{"\x1b[93m"};
const char ANSI_MAGENTA[]{"\x1b[95m"};
const char ANSI_CYAN[]{"\x1b[96m"};
const char ANSI_WHITE[]{"\x1b[97m"};

// ****************************************************************************
// 'struct DefaultLogFormat' definition
// ****************************************************************************
CONSTEXPR_ char DEFAULT_FMTR_DIVIDER[]{" | "};
CONSTEXPR_ char DEFAULT_FMTR_DT[]{"%Y %b %d %H:%M:%S"};
CONSTEXPR_ char DEFAULT_FMTR_DT_TZ[]{"%Y %b %d %H:%M:%S%z"};
CONSTEXPR_ char DEFAULT_FMTR_FILE[]{" | File = "};
CONSTEXPR_ char DEFAULT_FMTR_FUNC[]{" | Function = "};
CONSTEXPR_ char DEFAULT_FMTR_LINE[]{" | Line = "};
CONSTEXPR_ char DEFAULT_FMTR_THREAD[]{" | Thread ID = "};

void DefaultLogFormat::operator()(std::ostream& os, std::time_t timeStamp,
                                  const std::string& message, const std::string& logMsgLevel,
                                  const std::string& file, const std::string& function, int lineNo,
                                  const std::thread::id& threadID, bool utcTimeStamps,
                                  bool tzOffset) const
{
    if (!os.good())
    {
        return;
    }

    if (timeStamp != 0)
    {
#if defined(_MSC_VER) && (_MSC_VER < 1920)
        struct tm result;
        ZeroPodObject(result);

        if (utcTimeStamps)
        {
            gmtime_s(&result, &timeStamp);

            // When UTC time we do not ever add the offsets to the string
            // that is only for local time.
            tzOffset = false;
        }
        else
        {
            localtime_s(&result, &timeStamp);
        }

        os << std::put_time(&result, tzOffset ? DEFAULT_FMTR_DT_TZ : DEFAULT_FMTR_DT)
           << DEFAULT_FMTR_DIVIDER;
#else
        if (utcTimeStamps)
        {
            // When UTC time we do not ever add the offsets to the string
            // that is only for local time.
            os << std::put_time(std::gmtime(&timeStamp), DEFAULT_FMTR_DT) << DEFAULT_FMTR_DIVIDER;
        }
        else
        {
            os << std::put_time(std::localtime(&timeStamp),
                                tzOffset ? DEFAULT_FMTR_DT_TZ : DEFAULT_FMTR_DT)
               << DEFAULT_FMTR_DIVIDER;
        }
#endif

        // Legacy alternative...
        //
        // std::string time = ctime(&timeStamp);
        // std::replace_if(time.begin(), time.end(),
        //                 [](char c) { return (c == '\n') || (c == '\r'); }, 0);
        // os << time.c_str() << " | " ;
    }

    if (logMsgLevel.compare("") != 0)
    {
        os << logMsgLevel << DEFAULT_FMTR_DIVIDER;
    }

    os << "\"" << message << "\"";

    if (file.compare("") != 0)
    {
        os << DEFAULT_FMTR_FILE << file;
    }

    if (function.compare("") != 0)
    {
        os << DEFAULT_FMTR_FUNC << function;
    }

    if (lineNo >= 0)
    {
        os << DEFAULT_FMTR_LINE << lineNo;
    }

    std::thread::id noThread;

    if (threadID != noThread)
    {
        os << DEFAULT_FMTR_THREAD << threadID;
    }

    os << std::endl;
}

void DefaultLogFormat::operator()(std::ostream& os, std::time_t timeStamp,
                                  const std::string& message, const std::string& logMsgLevel,
                                  bool utcTimeStamps, bool tzOffset, const char* colourCode) const
{
    if (!os.good())
    {
        return;
    }

    if (nullptr != colourCode)
    {
        os << colourCode;
    }

    if (timeStamp != 0)
    {
#if defined(_MSC_VER) && (_MSC_VER < 1920)
        struct tm result;
        ZeroPodObject(result);

        if (utcTimeStamps)
        {
            gmtime_s(&result, &timeStamp);

            // When UTC time we do not ever add the offsets to the string
            // that is only for local time.
            tzOffset = false;
        }
        else
        {
            localtime_s(&result, &timeStamp);
        }

        os << std::put_time(&result, tzOffset ? DEFAULT_FMTR_DT_TZ : DEFAULT_FMTR_DT)
           << DEFAULT_FMTR_DIVIDER;
#else
        if (utcTimeStamps)
        {
            // When UTC time we do not ever add the offsets to the string
            // that is only for local time.
            os << std::put_time(std::gmtime(&timeStamp), DEFAULT_FMTR_DT) << DEFAULT_FMTR_DIVIDER;
        }
        else
        {
            os << std::put_time(std::localtime(&timeStamp),
                                tzOffset ? DEFAULT_FMTR_DT_TZ : DEFAULT_FMTR_DT)
               << DEFAULT_FMTR_DIVIDER;
        }
#endif

        // Legacy alternative...
        //
        // std::string time = ctime(&timeStamp);
        // std::replace_if(time.begin(), time.end(),
        //                 [](char c) { return (c == '\n') || (c == '\r'); }, 0);
        // os << time.c_str() << " | " ;
    }

    if (logMsgLevel.compare("") != 0)
    {
        os << logMsgLevel << DEFAULT_FMTR_DIVIDER;
    }

    os << "\"" << message << "\"";

    if (nullptr != colourCode)
    {
        os << ANSI_RESET;
    }

    os << std::endl;
}

namespace dl_private
{
// ****************************************************************************
// 'class LogQueueMessage' definition
// ****************************************************************************

LogQueueMessage::LogQueueMessage(std::string const& message, time_t timeStamp,
                                 std::string const& file, std::string const& function, int lineNo,
                                 const std::thread::id& threadID, eLogMessageLevel errorLevel,
                                 eMsgTarget msgTarget)
    : m_message(message)
    , m_timeStamp(timeStamp)
    , m_file(file)
    , m_function(function)
    , m_lineNo(lineNo)
    , m_threadID(threadID)
    , m_errorLevel(errorLevel)
    , m_msgTarget(msgTarget)
{
}

#ifdef USE_EXPLICIT_MOVE_
LogQueueMessage::LogQueueMessage(LogQueueMessage&& msg)
{
    *this = std::move(msg);
}

LogQueueMessage& LogQueueMessage::operator=(LogQueueMessage&& msg)
{
    std::swap(m_message, msg.m_message);
    std::swap(m_timeStamp, msg.m_timeStamp);
    std::swap(m_file, msg.m_file);
    std::swap(m_function, msg.m_function);
    std::swap(m_lineNo, msg.m_lineNo);
    std::swap(m_threadID, msg.m_threadID);
    std::swap(m_errorLevel, msg.m_errorLevel);
    std::swap(m_msgTarget, msg.m_msgTarget);
    return *this;
}
#endif

const std::string& LogQueueMessage::Message() const
{
    return m_message;
}

time_t LogQueueMessage::TimeStamp() const
{
    return m_timeStamp;
}

const std::string& LogQueueMessage::File() const
{
    return m_file;
}

const std::string& LogQueueMessage::Function() const
{
    return m_function;
}

int LogQueueMessage::LineNo() const
{
    return m_lineNo;
}

const std::thread::id& LogQueueMessage::ThreadID() const
{
    return m_threadID;
}

eLogMessageLevel LogQueueMessage::ErrorLevel() const
{
    return m_errorLevel;
}

auto LogQueueMessage::MsgTarget() const -> eMsgTarget
{
    return m_msgTarget;
}

} // namespace dl_private

} // namespace log
} // namespace core_lib
