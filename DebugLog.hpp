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

template<typename Formatter,
         typename OutputStream = std::ofstream,
         size_t maxSizeInBytes = 5242880 >
class DebugLog final
{
public:
    enum eLogMessageLevel
    {
        not_defined = 0,
        trace,
        debug,
        performance,
        info,
        warning,
        error,
        fatal
    };

    DebugLog()
        : m_logMsgQueueThread(std::bind(&DebugLog::MessageDecoder
                                        , this
                                        , std::placeholders::_1
                                        , std::placeholders::_2))
    {
        RegisterLogQueueMessageId();
    }

    explicit DebugLog(const std::string& softwareVersion, /*e.g. "1.0.0.1"*/)
        : m_softwareVersion(softwareVersion)
        , m_logMsgQueueThread(std::bind(&DebugLog::MessageDecoder
                                        , this
                                        , std::placeholders::_1
                                        , std::placeholders::_2))
    {
        RegisterLogQueueMessageId();
    }

    DebugLog(const std::string& softwareVersion, /*e.g. "1.0.0.1"*/
             const std::string& logFolderPath,
             const std::string& logName)
        : m_softwareVersion(softwareVersion)
        , m_logFolderPath(logFolderPath)
        , m_logName(logName)
        , m_logMsgQueueThread(std::bind(&DebugLog::MessageDecoder
                                        , this
                                        , std::placeholders::_1
                                        , std::placeholders::_2))
    {
        RegisterLogQueueMessageId();
    }

    ~DebugLog()
    {
    }

private:
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

    Formatter m_logFormatter;
    OutputStream m_outputStream;
    const std::string m_softwareVersion;
    const std::string m_logFolderPath;
    const std::string m_logName;
    core_lib::threads::MessageQueueThread<int, LogQueueMessage> m_logMsgQueueThread;

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
};

} // namespace log
} // namespace core_lib

#endif // DEBUGLOG_HPP
