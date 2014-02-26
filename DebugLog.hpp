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
 * \file DebugLog.hpp
 * \brief File containing declaration of DebugLog class.
 */

#ifndef DEBUGLOG_HPP
#define DEBUGLOG_HPP

#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#include <map>
#include <algorithm>
#include "MessageQueueThread.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The log namespace. */
namespace log {

/*!
 * \brief Message handler exception.
 *
 * This exception class is intended to be thrown by functions in
 * DebugLog class when a message handler error occurs.
 */
class xLogMsgHandlerError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xLogMsgHandlerError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specified message string.
     */
    explicit xLogMsgHandlerError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xLogMsgHandlerError();
};

enum class eLogMessageLevel
{
    not_defined = 0,
    debug,
    info,
    warning,
    error,
    fatal
};

struct DefaultLogFormat
{
    std::string operator() (std::string& logMsgLevel
                            , std::time_t timeStamp
                            , const std::string& message
                            , const std::string& file
                            , int lineNo
                            , const std::thread::id& threadID) const
    {
        std::stringstream ss;

        if (logMsgLevel != "")
        {
            ss << logMsgLevel;
        }

        if (timeStamp != 0)
        {
            // Should use lines below but not necessarily implemented yet
            //     struct std::tm * ptm = std::localtime(&timeStamp);
            //     ss << "\t" << std::put_time(ptm,"%F %T");
            // so instead we use...
            std::string time = ctime(&timeStamp);
            std::replace_if(time.begin(), time.end(),
                            [](char c) { return (c == '\n') || (c == '\r'); }, 0);
            ss << "\t" << time;
        }

        ss << message;

        if (file != "")
        {
            ss << "\tFile Name: " << file;
        }

        if (lineNo >= 0)
        {
            ss << "\tLine Number: " << lineNo;
        }

        std::thread::id noThread;
        if (threadID != noThread)
        {
            ss << "\tThread ID: " << threadID;
        }

        ss << std::endl;

        return ss.str();
    }
};

static const size_t BYTES_IN_MEBIBYTE = 1024 * 1024;

template<typename Formatter = DefaultLogFormat
         , size_t maxSizeInBytes = 5 * BYTES_IN_MEBIBYTE>
class DebugLog final
{
public:
    DebugLog()
        : m_softwareVersion("")
        , m_logFilePath("")
        , m_oldLogFilePath("")
        , m_unknownLogMsgLevel(" ?   ")
        , m_logMsgQueueThread(std::bind(&DebugLog::MessageDecoder
                                        , this
                                        , std::placeholders::_1
                                        , std::placeholders::_2)
                              , threads::eOnDestroyOptions::processRemainingItems)
    {
        SetupLogMsgLevelLookup();
        RegisterLogQueueMessageId();
    }

    explicit DebugLog(const std::string& softwareVersion)
        : m_softwareVersion(softwareVersion)
        , m_logFilePath("")
        , m_oldLogFilePath("")
        , m_unknownLogMsgLevel(" ?   ")
        , m_logMsgQueueThread(std::bind(&DebugLog::MessageDecoder
                                        , this
                                        , std::placeholders::_1
                                        , std::placeholders::_2)
                              , threads::eOnDestroyOptions::processRemainingItems)
    {
        SetupLogMsgLevelLookup();
        RegisterLogQueueMessageId();
    }

    DebugLog(const std::string& softwareVersion,
             const std::string& logFolderPath,
             const std::string& logName)
        : m_softwareVersion(softwareVersion)
        , m_logFilePath(logFolderPath + logName + ".txt")
        , m_oldLogFilePath(logFolderPath + logName + "_old.txt")
        , m_unknownLogMsgLevel(" ?   ")
        , m_logMsgQueueThread(std::bind(&DebugLog::MessageDecoder
                                        , this
                                        , std::placeholders::_1
                                        , std::placeholders::_2)
                              , threads::eOnDestroyOptions::processRemainingItems)
    {
        SetupLogMsgLevelLookup();
        RegisterLogQueueMessageId();
    }

    ~DebugLog()
    {
    }

    void AddLogMsgLevelFilter(eLogMessageLevel logMessageLevel)
    {
        if (!IsLogMsgLevelFilterSet(logMessageLevel))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_logMsgFilterSet.insert(logMessageLevel);
        }
    }

    void RemoveLogMsgLevelFilter(eLogMessageLevel logMessageLevel)
    {
        if (IsLogMsgLevelFilterSet(logMessageLevel))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_logMsgFilterSet.insert(logMessageLevel);
        }
    }

    void AddLogMessage(const std::string& message
                       , const std::string& file = "" /*e.g. std::string(__FILE__)*/
                       , int lineNo = -1 /*e.g.  __LINE__*/
                       , eLogMessageLevel logMsgLevel = eLogMessageLevel::not_defined)
    {
        if (!IsLogMsgLevelFilterSet(logMsgLevel))
        {
            time_t messageTime;
            time(&messageTime);
            m_logMsgQueueThread.Push(new LogQueueMessage(message,
                                                         messageTime,
                                                         file,
                                                         lineNo,
                                                         std::this_thread::get_id(),
                                                         logMsgLevel));
        }
    }

private:
    class LogQueueMessage
    {
    public:
        static const int MESSAGE_ID = 1;

        LogQueueMessage()
            : m_timeStamp(0)
            , m_lineNo(0)
            , m_errorLevel(eLogMessageLevel::not_defined)
        {
        }

        LogQueueMessage(const std::string& message,
                         time_t timeStamp,
                         const std::string& file,
                         int lineNo,
                         std::thread::id threadID,
                         eLogMessageLevel errorLevel)
            : m_message(message)
            , m_timeStamp(timeStamp)
            , m_file(file)
            , m_lineNo(lineNo)
            , m_threadID(threadID)
            , m_errorLevel(errorLevel)
        {
        }

        LogQueueMessage(const LogQueueMessage& msg)
            : m_message(msg.m_message)
            , m_timeStamp(msg.m_timeStamp)
            , m_file(msg.m_file)
            , m_lineNo(msg.m_lineNo)
            , m_threadID(msg.m_threadID)
            , m_errorLevel(msg.m_errorLevel)
        {
        }

        LogQueueMessage(LogQueueMessage&& msg)
            : m_timeStamp(0)
            , m_lineNo(0)
            , m_errorLevel(eLogMessageLevel::not_defined)
        {
            *this = std::move(msg);
        }

        LogQueueMessage& operator=(const LogQueueMessage& msg)
        {
            if (this != &msg)
            {
                m_message = msg.m_message;
                m_timeStamp = msg.m_timeStamp;
                m_file = msg.m_file;
                m_lineNo = msg.m_lineNo;
                m_threadID = msg.m_threadID;
                m_errorLevel = msg.m_errorLevel;
            }

            return *this;
        }

        LogQueueMessage& operator=(LogQueueMessage&& msg)
        {
            m_message.swap(msg.m_message);
            std::swap(m_timeStamp, msg.m_timeStamp);
            m_file.swap(msg.m_file);
            std::swap(m_lineNo, msg.m_lineNo);
            std::swap(m_threadID, msg.m_threadID);
            std::swap(m_errorLevel, msg.m_errorLevel);
            return *this;
        }

        const std::string& Message() const
        {
            return m_message;
        }

        time_t TimeStamp() const
        {
            return m_timeStamp;
        }

        const std::string& File() const
        {
            return m_file;
        }

        int LineNo() const
        {
            return m_lineNo;
        }

        std::thread::id ThreadID() const
        {
            return m_threadID;
        }

        eLogMessageLevel ErrorLevel() const
        {
            return m_errorLevel;
        }

    private:
        std::string m_message;
        time_t m_timeStamp;
        std::string m_file;
        int m_lineNo;
        std::thread::id m_threadID;
        eLogMessageLevel m_errorLevel;
    };

    mutable std::mutex m_mutex;
    Formatter m_logFormatter;
    std::ofstream m_outputStream;
    const std::string m_softwareVersion;
    const std::string m_logFilePath;
    const std::string m_oldLogFilePath;
    const std::string m_unknownLogMsgLevel;
    threads::MessageQueueThread<int, LogQueueMessage> m_logMsgQueueThread;
    std::map<eLogMessageLevel, std::string> m_logMsgLevelLookup;
    std::set<eLogMessageLevel> m_logMsgFilterSet;

    void SetupLogMsgLevelLookup()
    {
        m_logMsgLevelLookup[eLogMessageLevel::not_defined] = "";
        m_logMsgLevelLookup[eLogMessageLevel::debug]       = "DEBUG";
        m_logMsgLevelLookup[eLogMessageLevel::info]        = "INFO ";
        m_logMsgLevelLookup[eLogMessageLevel::warning]     = "WARN ";
        m_logMsgLevelLookup[eLogMessageLevel::error]       = "ERROR";
        m_logMsgLevelLookup[eLogMessageLevel::fatal]       = "FATAL";
    }

    void RegisterLogQueueMessageId()
    {
        m_logMsgQueueThread.RegisterMessageHandler(LogQueueMessage::MESSAGE_ID
                                                   , std::bind(&DebugLog::MessageHandler
                                                               , this
                                                               , std::placeholders::_1
                                                               , std::placeholders::_2));
    }

    int MessageDecoder(const LogQueueMessage* message, int length)
    {
        if (!message || (length == 0))
        {
            BOOST_THROW_EXCEPTION(xLogMsgHandlerError("invalid message in DebugLog::MessageDecoder"));
        }

        return LogQueueMessage::MESSAGE_ID;
    }

    bool MessageHandler(LogQueueMessage* message, int length)
    {
        if (!message || (length == 0))
        {
            BOOST_THROW_EXCEPTION(xLogMsgHandlerError("invalid message in DebugLog::MessageHandler"));
        }

        // *************************
        // TODO: Handle log message.
        // *************************

        // Make sure message is deleted.
        return true;
    }

    bool IsLogMsgLevelInLookup(eLogMessageLevel logMessageLevel) const
    {
        return (m_logMsgLevelLookup.find(logMessageLevel) != m_logMsgLevelLookup.end());
    }

    const std::string& GetLogMsgLevelAsString(eLogMessageLevel logMessageLevel)
    {
        return IsLogMsgLevelInLookup(logMessageLevel)
               ? m_logMsgLevelLookup[logMessageLevel]
               : m_unknownLogMsgLevel;
    }

    bool IsLogMsgLevelFilterSet(eLogMessageLevel logMessageLevel) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return (m_logMsgFilterSet.find(logMessageLevel) != m_logMsgFilterSet.end());
    }
};

} // namespace log
} // namespace core_lib

#endif // DEBUGLOG_HPP
