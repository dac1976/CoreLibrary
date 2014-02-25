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
#include <string>
#include <fstream>
#include <set>
#include <map>
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

/*! \brief The message namespace. */
namespace message {

class LogQueueMessage
{
public:
    static const int MESSAGE_ID = 1;

    LogQueueMessage();
    LogQueueMessage(const std::string& Message,
                    time_t TimeStamp,
                    const std::string& File,
                    const std::string& Function,
                    int LineNo,
                    std::thread::id ThreadID,
                    eLogMessageLevel ErrorLevel);
    LogQueueMessage(const LogQueueMessage& Msg);
    LogQueueMessage(LogQueueMessage&& Msg);
    LogQueueMessage& operator=(const LogQueueMessage& Msg);
    LogQueueMessage& operator=(LogQueueMessage&& Msg);

    const std::string& Message() const;
    time_t TimeStamp() const;
    const std::string& File() const;
    const std::string& Function() const;
    int LineNo() const;
    std::thread::id ThreadID() const;
    eLogMessageLevel ErrorLevel() const;

private:
    std::string m_message;
    time_t m_timeStamp;
    std::string m_file;
    std::string m_function;
    int m_lineNo;
    std::thread::id m_threadID;
    eLogMessageLevel m_errorLevel;
};

} // namespace message

static const size_t BYTES_IN_MEBIBYTE = 1024 * 1024;

template<typename Formatter,
         typename OutputStream = std::ofstream,
         size_t maxSizeInBytes = 5 * BYTES_IN_MEBIBYTE >
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
                       , const std::string& function = "" /*e.g. std::string(__FUNC__)*/
                       , int lineNo = -1 /*e.g.  __LINE__*/
                       , eLogMessageLevel logMsgLevel = eLogMessageLevel::not_defined)
    {
        if (!IsLogMsgLevelFilterSet(logMsgLevel))
        {
            time_t messageTime;
            time(&messageTime);
            m_logMsgQueueThread.Push(new message::LogQueueMessage(message,
                                                                  messageTime,
                                                                  file,
                                                                  function,
                                                                  lineNo,
                                                                  std::this_thread::get_id(),
                                                                  logMsgLevel));
        }
    }

private:
    mutable std::mutex m_mutex;
    Formatter m_logFormatter;
    OutputStream m_outputStream;
    const std::string m_softwareVersion;
    const std::string m_logFilePath;
    const std::string m_oldLogFilePath;
    const std::string m_unknownLogMsgLevel;
    threads::MessageQueueThread<int, message::LogQueueMessage> m_logMsgQueueThread;
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
        m_logMsgQueueThread.RegisterMessageHandler(message::LogQueueMessage::MESSAGE_ID
                                                   , std::bind(&DebugLog::MessageHandler
                                                               , this
                                                               , std::placeholders::_1
                                                               , std::placeholders::_2));
    }

    int MessageDecoder(const message::LogQueueMessage* message, int length)
    {
        if (!message || (length == 0))
        {
            BOOST_THROW_EXCEPTION(xLogMsgHandlerError("invalid message in DebugLog::MessageDecoder"));
        }

        return message::LogQueueMessage::MESSAGE_ID;
    }

    bool MessageHandler(message::LogQueueMessage* message, int length)
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
