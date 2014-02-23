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
 * \file DebugLog.cpp
 * \brief File containing definition of DebugLog class.
 */

#include "../DebugLog.hpp"
#include <utility>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The log namespace. */
namespace log {

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

DebugLog::LogQueueMessage::LogQueueMessage()
    : m_timeStamp(0)
    , m_lineNo(0)
    , m_errorLevel(DebugLog::not_defined)
{
}

// ****************************************************************************
// 'class DebugLog::LogQueueMessage' definition
// ******************************B*********************************************
DebugLog::LogQueueMessage::LogQueueMessage(const std::string& message,
                                           time_t timeStamp,
                                           const std::string& file,
                                           const std::string& function,
                                           int lineNo,
                                           std::thread::id threadID,
                                           DebugLog::eLogMessageLevel errorLevel)
    : m_message(message)
    , m_timeStamp(timeStamp)
    , m_file(file)
    , m_function(function)
    , m_lineNo(lineNo)
    , m_threadID(threadID)
    , m_errorLevel(errorLevel)
{
}   
    
DebugLog::LogQueueMessage::LogQueueMessage(const LogQueueMessage& msg)
    : m_message(msg.m_message)
    , m_timeStamp(msg.m_timeStamp)
    , m_file(msg.m_file)
    , m_function(msg.m_function)
    , m_lineNo(msg.m_lineNo)
    , m_threadID(msg.m_threadID)
    , m_errorLevel(msg.m_errorLevel)
{
}

DebugLog::LogQueueMessage::LogQueueMessage(LogQueueMessage&& msg)
    : m_timeStamp(0)
    , m_lineNo(0)
    , m_errorLevel(not_defined)
{
    *this = std::move(msg);
}

DebugLog::LogQueueMessage::LogQueueMessage& operator=(const LogQueueMessage& msg)
{
    if (this != &msg)
    {
        m_message = msg.m_message;
        m_timeStamp = msg.m_timeStamp;
        m_file = msg.m_file;
        m_function = msg.m_function;
        m_lineNo = msg.m_lineNo;
        m_threadID = msg.m_threadID;
        m_errorLevel = msg.m_errorLevel;
    }
    
    return *this;
}

DebugLog::LogQueueMessage::LogQueueMessage& operator=(LogQueueMessage&& msg)
{
    m_message.swap(msg.m_message);
    std::swap(m_timeStamp, msg.m_timeStamp);
    m_file.swap(msg.m_file);
    m_function.swap(msg.m_function);
    std::swap(m_lineNo, msg.m_lineNo);
    std::swap(m_threadID, msg.m_threadID);
    std::swap(m_errorLevel, msg.m_errorLevel);
    return *this;
}

const std::string& DebugLog::LogQueueMessage::Message() const
{
    return m_message;
}

time_t DebugLog::LogQueueMessage::TimeStamp() const
{
    return m_timeStamp;
}

const std::string& DebugLog::LogQueueMessage::File() const
{
    return m_file;
}

const std::string& DebugLog::LogQueueMessage::Function() const
{
    return m_function;
}

int DebugLog::LogQueueMessage::LineNo() const
{
    return m_lineNo;
}

std::thread::id DebugLog::LogQueueMessage::ThreadID() const
{
    return m_threadID;
}

DebugLog::eLogMessageLevel DebugLog::LogQueueMessage::ErrorLevel() const
{
    return m_errorLevel;
}   

} // namespace log
} // namespace core_lib

#endif // DEBUGLOG_HPP