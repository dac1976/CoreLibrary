// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
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
 * \file DebugLog.cpp
 * \brief File containing definition of DebugLog class.
 */

#include "DebugLog/DebugLog.h"
#ifndef USE_EXPLICIT_MOVE_
#include <utility>
#endif
#include <iomanip>

namespace core_lib
{
namespace log
{

// ****************************************************************************
// 'class xMsgHandlerError' definition
// ******************************B*********************************************
xLogMsgHandlerError::xLogMsgHandlerError()
    : exceptions::xCustomException("log message handler error")
{
}

xLogMsgHandlerError::xLogMsgHandlerError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xLogMsgHandlerError::~xLogMsgHandlerError()
{
}

// ****************************************************************************
// 'class xInstantiationError' definition
// ******************************B*********************************************
xInstantiationError::xInstantiationError()
    : exceptions::xCustomException("instantiation error")
{
}

xInstantiationError::xInstantiationError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xInstantiationError::~xInstantiationError()
{
}

// ****************************************************************************
// 'struct DefaultLogFormat' definition
// ****************************************************************************
void DefaultLogFormat::operator()(std::ostream& os, std::time_t timeStamp,
                                  const std::string& message, const std::string& logMsgLevel,
                                  const std::string& file, const std::string& function, int lineNo,
                                  const std::thread::id& threadID) const
{
    if (timeStamp != 0)
    {
#if BOOST_COMP_MSVC
        struct tm result;
        localtime_s(&result, &timeStamp);
        os << std::put_time(&result, "%F %T") << " | ";
#else
        os << std::put_time(std::localtime(&timeStamp), "%F %T") << " | ";
#endif

        // Legacy alternative...
        //
        // std::string time = ctime(&timeStamp);
        // std::replace_if(time.begin(), time.end(),
        //                 [](char c) { return (c == '\n') || (c == '\r'); }, 0);
        // os << time.c_str() << " | " ;
    }

    if (logMsgLevel != "")
    {
        os << logMsgLevel << " | ";
    }

    os << message;

    if (file != "")
    {
        os << " | " << file;
    }

    if (function != "")
    {
        os << " | " << function;
    }

    if (lineNo >= 0)
    {
        os << " | Line = " << lineNo;
    }

    std::thread::id noThread;

    if (threadID != noThread)
    {
        os << " | Thread ID = " << threadID;
    }

    os << std::endl;
}

// ****************************************************************************
// 'class LogQueueMessage' definition
// ****************************************************************************
namespace dl_private
{
LogQueueMessage::LogQueueMessage(const std::string& message, const time_t timeStamp,
                                 const std::string& file, const std::string& function,
                                 const int lineNo, const std::thread::id& threadID,
                                 const eLogMessageLevel errorLevel)
    : m_message(message)
    , m_timeStamp(timeStamp)
    , m_file(file)
    , m_function(function)
    , m_lineNo(lineNo)
    , m_threadID(threadID)
    , m_errorLevel(errorLevel)
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

} // namespace dl_private

} // namespace log
} // namespace core_lib
