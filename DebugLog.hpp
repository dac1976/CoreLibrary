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
#include <set>
#include <map>
#include <algorithm>
#include <memory>
#include "MessageQueueThread.hpp"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/filesystem.hpp"

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

/*! \brief Enumeration containing queue configuration options. */
enum class eLogMessageLevel
{
    /*! \brief No level defined for message. */
    not_defined = 0,
    /*! \brief Debug level defined for message. */
    debug,
    /*! \brief Info level defined for message. */
    info,
    /*! \brief Warning level defined for message. */
    warning,
    /*! \brief Error level defined for message. */
    error,
    /*! \brief Fatal level defined for message. */
    fatal
};

/*!
 * \brief Default log line formater.
 *
 * This functor formats the log message elements
 * into a single line in the log following default
 * formatting.
 *
 * < "Level" >< "Date/Time" >< "Message" >< File = "..." >< Line = "..." >< Thread ID = "..." >
 */
struct DefaultLogFormat
{
    void operator() (std::ostream& os
                     , std::time_t timeStamp
                     , const std::string& message
                     , const std::string& logMsgLevel
                     , const std::string& file
                     , int lineNo
                     , const std::thread::id& threadID) const;
};

/*! \brief Static constant defining number of bytes in a mebibyte. */
static const size_t BYTES_IN_MEBIBYTE = 1024 * 1024;

/*!
 * \brief DebugLog class.
 *
 * Templated class to perform logging. The class is
 * threaded and thread safe.
 *
 * The template args comprise the a formatter type
 * and which should be a function object with same
 * prototype as DefaultLogFormat::operator ()(). The
 * second arg is optional and controls the size at which
 * the log will close and switch to a new file. Only 2 files
 * ever exist the <log>.txt and <log>_old.txt. The default log
 * size if 5MiB.
 */
template<typename Formatter /* e.g. DefaultLogFormat*/
         , long MAX_LOG_SIZE = 5 * BYTES_IN_MEBIBYTE>
class DebugLog final
{
public:
    /*! \brief Default constructor.
     *
     * Create the DebugLog in same folder as application
     * with name log.txt.
     */
    DebugLog()
        : m_logFilePath("log.txt")
        , m_oldLogFilePath("log_old.txt")
        , m_unknownLogMsgLevel("?")
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
    /*! \brief Initialisation constructor.
     * \param Version of software that "owns" the log.
     * \param Folder path (with trailing slash) where log will be created.
     * \param File name of log file without extension.
     *
     * Create the DebugLog in given folder with given name. A ".txt"
     * extension is automatically appending to log file's name.
     */
    DebugLog(const std::string& softwareVersion,
             const std::string& logFolderPath,
             const std::string& logName)
        : m_softwareVersion(softwareVersion)
        , m_logFilePath(logFolderPath + logName + ".txt")
        , m_oldLogFilePath(logFolderPath + logName + "_old.txt")
        , m_unknownLogMsgLevel("?")
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
    /*! \brief Copy constructor deleted.*/
    DebugLog(const DebugLog&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    DebugLog& operator=(const DebugLog&) = delete;
    /*! \brief Destructor*/
    ~DebugLog()
    {
        // Manually reset so we process all remaining
        // messages before closing file.
        m_logMsgQueueThread.reset();
        CloseOfStream();
    }
    /*! \brief Add level to filter.
     * \param Message level to filter out of log.
     *
     * You can dynamically filter out log message from appearing
     * in the log file based on adding message levels to the
     * filter set. For example you may want to filter out
     * messages of type eLogMessageLevel::warning. In this case
     * after calling this function with eLogMessageLevel::warning
     * messages of this type will not appear in the log from this point.
     */
    void AddLogMsgLevelFilter(eLogMessageLevel logMessageLevel)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!IsLogMsgLevelFilterSetNoLock(logMessageLevel))
        {
            m_logMsgFilterSet.insert(logMessageLevel);
        }
    }
    /*! \brief Remove level from filter.
     * \param Message level to remove from filter set.
     *
     * If you want to make sure messages of a given level
     * appear back in the log file after having called
     * AddLogMsgLevelFilter at some point then call this function
     * to remove the message level from the filter set. After calling
     * this function messages of the specified level will once again
     * appear in the log file.
     */
    void RemoveLogMsgLevelFilter(eLogMessageLevel logMessageLevel)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (IsLogMsgLevelFilterSetNoLock(logMessageLevel))
        {
            m_logMsgFilterSet.insert(logMessageLevel);
        }
    }
    /*! \brief Clear all message levels from filter.
     *
     * Clear the filter set. AFter calling this messages of all
     * levels will once again appear in the log file.
     */
    void ClearLogMsgLevelFilters()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_logMsgFilterSet.clear();
    }
    /*! \brief Add message to the log file.
     * \param Message to add to log.
     *
     * Add a simple message to the log without any extra
     * properties set, such as file, line no. etc.
     */
    void AddLogMessage(const std::string& message)
    {
        time_t messageTime;
        time(&messageTime);
        std::thread::id noThread;
        m_logMsgQueueThread->Push(new LogQueueMessage(message,
                                                      messageTime,
                                                      "",
                                                      -1,
                                                      noThread,
                                                      eLogMessageLevel::not_defined));
    }

    /*! \brief Add message to the log file.
     * \param Message to add to log.
     * \param Source file in which message AddLogMessage was called, e.g. std::string(__FILE__).
     * \param Line number in the source file where AddLogMessage was called, e.g. __LINE__.
     * \param Message level.
     *
     * Add a message to the log with extra properties set, such as
     * file, line no. etc.
     */
    void AddLogMessage(const std::string& message
                       , const std::string& file
                       , int lineNo
                       , eLogMessageLevel logMsgLevel)
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
        m_logMsgLevelLookup[eLogMessageLevel::debug]       = "Debug";
        m_logMsgLevelLookup[eLogMessageLevel::info]        = "Info";
        m_logMsgLevelLookup[eLogMessageLevel::warning]     = "Warning";
        m_logMsgLevelLookup[eLogMessageLevel::error]       = "Error";
        m_logMsgLevelLookup[eLogMessageLevel::fatal]       = "Fatal";
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

    bool IsLogMsgLevelFilterSetNoLock(eLogMessageLevel logMessageLevel) const
    {
        return (m_logMsgFilterSet.find(logMessageLevel) != m_logMsgFilterSet.end());
    }

    bool IsLogMsgLevelFilterSet(eLogMessageLevel logMessageLevel) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return (m_logMsgFilterSet.find(logMessageLevel) != m_logMsgFilterSet.end());
    }

    /*! \brief Enumeration containing file opening options. */
    enum class eFileOpenOptions
    {
        /*! \brief Option to truncate file when opened. */
        truncate_file,
        /*! \brief Option to append to file when opened. */
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
        std::thread::id noThread;
        WriteMessageToLog(LogQueueMessage("DEBUG LOG STARTED"
                                          , messageTime, "", -1
                                          , noThread
                                          , eLogMessageLevel::not_defined));

        if (m_softwareVersion != "")
        {
            std::string message("Software Version ");
            message += m_softwareVersion;
            WriteMessageToLog(LogQueueMessage(message, messageTime
                                              , "", -1
                                              , noThread
                                              , eLogMessageLevel::not_defined));
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
        std::thread::id noThread;
        WriteMessageToLog(LogQueueMessage("DEBUG LOG STOPPED"
                                          , messageTime, "", -1
                                          , noThread
                                          , eLogMessageLevel::not_defined));
        m_ofStream.close();
    }

    void CheckLogFileSize(long requiredSpace)
    {
        if (!m_ofStream.is_open())
        {
            return;
        }

        long pos = m_ofStream.tellp();

        if ((MAX_LOG_SIZE - pos) < requiredSpace)
        {
            CloseOfStream();
            boost::filesystem::copy_file(m_logFilePath, m_oldLogFilePath
                                         , boost::filesystem::copy_option::overwrite_if_exists);
            OpenOfStream(m_logFilePath, eFileOpenOptions::truncate_file);
        }
    }

    void WriteMessageToLog(const LogQueueMessage& logMessage)
    {
        m_logFormatter(m_ofStream
                       , logMessage.TimeStamp()
                       , logMessage.Message()
                       , GetLogMsgLevelAsString(logMessage.ErrorLevel())
                       , logMessage.File()
                       , logMessage.LineNo()
                       , logMessage.ThreadID());
        m_ofStream.flush();
    }
};

} // namespace log
} // namespace core_lib

/*!
 * \brief Simple macro to simplify logging.
 * \param [IN] DebugLog object.
 * \param [IN] Object to be used as message in DebugLog.
 */
#define DEBUG_LOG(x, m)           \
    do                            \
    {                             \
        std::ostringstream os;    \
        os << m;    		      \
        x.AddLogMessage(os.str()); \
    } while(false)

/*!
 * \brief Extended macro to simplify logging.
 * \param [IN] DebugLog object.
 * \param [IN] Object to be used as message in DebugLog.
 * \param [IN] Log message level, e.g. info, debug, warning, error etc.
 */
#define DEBUG_LOG_EX(x, m, l)  \
    do                               \
    {                                \
        std::ostringstream os;       \
        os << m;    			     \
        x.AddLogMessage(os.str(), std::string(__FILE__), __LINE__, l); \
    } while(false)

#endif // DEBUGLOG_HPP
