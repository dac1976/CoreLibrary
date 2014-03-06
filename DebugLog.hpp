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
#include <memory>
#include "boost/filesystem.hpp"
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
    void operator() (std::ostream& os
                     , const std::string& logMsgLevel
                     , std::time_t timeStamp
                     , const std::string& file
                     , int lineNo
                     , const std::thread::id& threadID
                     , const std::string& message) const;
};

static const size_t BYTES_IN_MEBIBYTE = 1024 * 1024;

template<typename Formatter = DefaultLogFormat
         , long MAX_LOG_SIZE = 5 * BYTES_IN_MEBIBYTE>
class DebugLog final
{
public:
    DebugLog(const std::string& softwareVersion,
             const std::string& logFolderPath,
             const std::string& logName)
        : m_softwareVersion(softwareVersion)
        , m_logFilePath(logFolderPath + logName + ".txt")
        , m_oldLogFilePath(logFolderPath + logName + "_old.txt")
        , m_unknownLogMsgLevel(" ?   ")
        , m_logMsgQueueThread(new log_msg_queue(std::bind(&DebugLog::MessageDecoder
                                                          , this
                                                          , std::placeholders::_1
                                                          , std::placeholders::_2)
                                                , threads::eOnDestroyOptions::processRemainingItems))
    {
        SetupLogMsgLevelLookup();
        RegisterLogQueueMessageId();
        OpenOfStream(m_logFilePath, eFileOpenOptions::append_file);
    }

    ~DebugLog()
    {
        // Manually reset so we cprocess all remaining
        // messages before closing file.
        m_logMsgQueueThread.reset();
        CloseOfStream();
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
            m_logMsgQueueThread->Push(new LogQueueMessage(message,
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
    std::ofstream m_ofStream;
    const std::string m_softwareVersion;
    const std::string m_logFilePath;
    const std::string m_oldLogFilePath;
    const std::string m_unknownLogMsgLevel;
    typedef threads::MessageQueueThread<int, LogQueueMessage> log_msg_queue;
    std::unique_ptr<log_msg_queue> m_logMsgQueueThread;
    std::map<eLogMessageLevel, std::string> m_logMsgLevelLookup;
    std::set<eLogMessageLevel> m_logMsgFilterSet;

    void SetupLogMsgLevelLookup()
    {
        m_logMsgLevelLookup[eLogMessageLevel::not_defined] = "";
        m_logMsgLevelLookup[eLogMessageLevel::debug]       = "Debug  ";
        m_logMsgLevelLookup[eLogMessageLevel::info]        = "Info   ";
        m_logMsgLevelLookup[eLogMessageLevel::warning]     = "Warning";
        m_logMsgLevelLookup[eLogMessageLevel::error]       = "Error  ";
        m_logMsgLevelLookup[eLogMessageLevel::fatal]       = "Fatal  ";
    }

    void RegisterLogQueueMessageId()
    {
        m_logMsgQueueThread->RegisterMessageHandler(LogQueueMessage::MESSAGE_ID
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

        CheckLogFileSize(message->Message().size());
        WriteMessageToLog(*message);
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

    enum class eFileOpenOptions
    {
        truncate_file,
        append_file
    };

    void OpenOfStream(const std::string& filePath, eFileOpenOptions fileOptions)
    {
        if (m_ofStream.is_open())
        {
            return;
        }

        if (fileOptions == eFileOpenOptions::truncate_file)
        {
            m_ofStream.open(filePath, std::ofstream::trunc);
        }
        else
        {
            m_ofStream.open(filePath, std::ofstream::app);
        }

        time_t messageTime;
        time(&messageTime);
        std::thread::id dummyID;
        WriteMessageToLog(LogQueueMessage("Debug log started."
                                          , messageTime, "", -1
                                          , dummyID
                                          , eLogMessageLevel::info));

        if (m_softwareVersion != "")
        {
            std::string message("Software Version ");
            message += m_softwareVersion;
            WriteMessageToLog(LogQueueMessage(message, messageTime
                                              , "", -1
                                              , dummyID
                                              , eLogMessageLevel::info));
        }
    }

    void CloseOfStream()
    {
        if (!m_ofStream.is_open())
        {
            return;
        }

        time_t messageTime;
        time(&messageTime);
        std::thread::id dummyID;
        WriteMessageToLog(LogQueueMessage("Debug log stopped."
                                          , messageTime, "", -1
                                          , dummyID
                                          , eLogMessageLevel::info));
        m_ofStream.close();
    }

    void CheckLogFileSize(long requiredSpace)
    {
        if (m_ofStream.is_open())
        {
            return;
        }

        long pos = m_ofStream.tellp();

        if ((MAX_LOG_SIZE - pos) < requiredSpace)
        {
            CloseOfStream();
            boost::filesystem::copy_file(m_logFilePath
                                         , m_oldLogFilePath
                                         , boost::filesystem::copy_option::overwrite_if_exists);
            OpenOfStream(m_logFilePath, eFileOpenOptions::truncate_file);
        }
    }

    void WriteMessageToLog(LogQueueMessage&& logMessage)
    {
        m_logFormatter(m_ofStream
                       , GetLogMsgLevelAsString(logMessage.ErrorLevel())
                       , logMessage.TimeStamp()
                       , logMessage.File()
                       , logMessage.LineNo()
                       , logMessage.ThreadID()
                       , logMessage.Message());
        m_ofStream.flush();
    }
};

} // namespace log
} // namespace core_lib

#endif // DEBUGLOG_HPP
