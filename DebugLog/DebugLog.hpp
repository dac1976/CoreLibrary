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
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <functional>
#include <type_traits>
#include "../MessageQueueThread.hpp"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/filesystem.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The log namespace. */
namespace log {

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

} // namespace log
} // namespace core_lib

/*! \brief The std namespace. */
namespace std {
    template <>
    struct hash<core_lib::log::eLogMessageLevel>
    {
        typedef core_lib::log::eLogMessageLevel argument_t;
        typedef std::size_t                     result_t;
        typedef typename std::underlying_type<argument_t>::type enumType_t;

        result_t operator()(const argument_t& a) const
        {
            enumType_t a2 = static_cast<enumType_t>(a);
            std::hash<enumType_t> h;
            return h(a2);
        }
    };
} // namespace std

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
static const size_t BYTES_IN_MEBIBYTE{1024 * 1024};

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
    /*!
     * \brief Default constructor.
     *
     * Create the DebugLog in same folder as application
     * with name log.txt.
     */
    DebugLog()
    {
        RegisterLogQueueMessageId();
        OpenOfStream(m_logFilePath, eFileOpenOptions::append_file);
    }
    /*!
     * \brief Initialisation constructor.
     * \param [IN] Version of software that "owns" the log.
     * \param [IN] Folder path (with trailing slash) where log will be created.
     * \param [IN] File name of log file without extension.
     *
     * Create the DebugLog in given folder with given name. A ".txt"
     * extension is automatically appending to log file's name.
     */
    DebugLog(const std::string& softwareVersion,
             const std::string& logFolderPath,
             const std::string& logName)
        : m_softwareVersion{softwareVersion}
        , m_logFilePath{logFolderPath + logName + ".txt"}
        , m_oldLogFilePath{logFolderPath + logName + "_old.txt"}
    {
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
    /*!
     * \brief Add level to filter.
     * \param [IN] Message level to filter out of log.
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
        std::lock_guard<std::mutex> lock{m_mutex};

        if (!IsLogMsgLevelFilterSetNoMutex(logMessageLevel))
        {
            m_logMsgFilterSet.insert(logMessageLevel);
        }
    }
    /*!
     * \brief Remove level from filter.
     * \param [IN] Message level to remove from filter set.
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
        std::lock_guard<std::mutex> lock{m_mutex};

        if (IsLogMsgLevelFilterSetNoMutex(logMessageLevel))
        {
            m_logMsgFilterSet.insert(logMessageLevel);
        }
    }
    /*!
     * \brief Clear all message levels from filter.
     *
     * Clear the filter set. AFter calling this messages of all
     * levels will once again appear in the log file.
     */
    void ClearLogMsgLevelFilters()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_logMsgFilterSet.clear();
    }
    /*!
     * \brief Add message to the log file.
     * \param [IN] Message to add to log.
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

    /*!
     * \brief Add message to the log file.
     * \param [IN] Message to add to log.
     * \param [IN] Source file in which message AddLogMessage was called, e.g. std::string(__FILE__).
     * \param [IN] Line number in the source file where AddLogMessage was called, e.g. __LINE__.
     * \param [IN] Message level.
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
    /*!
     * \brief Log Queue Message class.
     *
     * Message to be placed on message queue to process
     * debug log entry to add to file.
     */
    class LogQueueMessage
    {
    public:
        /*! \brief Static message ID to register on message queue.*/
        static const int MESSAGE_ID{1};
        /*! \brief Default constructor.*/
        LogQueueMessage() = default;
        /*!
         * \brief Initialising constructor.
         * \param [IN] Message to add to log.
         * \param [IN] Date/Time stamp for message.
         * \param [IN] Source file in which message AddLogMessage was called, e.g. std::string(__FILE__).
         * \param [IN] Line number in the source file where AddLogMessage was called, e.g. __LINE__.
         * \param [IN] Thread ID where message was added from.
         * \param [IN] Message level.
         */
        LogQueueMessage(const std::string& message,
                         time_t timeStamp,
                         const std::string& file,
                         int lineNo,
                         std::thread::id threadID,
                         eLogMessageLevel errorLevel)
            : m_message{message}
            , m_timeStamp{timeStamp}
            , m_file{file}
            , m_lineNo{lineNo}
            , m_threadID{threadID}
            , m_errorLevel{errorLevel}
        {
        }
        /*!
         * \brief Copy constructor.
         * \param [IN] Message to copy.
         */
        LogQueueMessage(const LogQueueMessage&) = default;
        /*!
         * \brief Move constructor.
         * \param [IN] Message to move.
         */
        LogQueueMessage(LogQueueMessage&&) = default;
        /*! \brief Destructor.*/
        ~LogQueueMessage() = default;
        /*!
         * \brief Copy assignment operator.
         * \param [IN] Message to copy.
         * \return LogQueueMessage reference.
         */
        LogQueueMessage& operator=(const LogQueueMessage&) = default;
        /*!
         * \brief Move assignment operator.
         * \param [IN] Message to move.
         * \return LogQueueMessage reference.
         */
        LogQueueMessage& operator=(LogQueueMessage&&) = default;
        /*!
         * \brief Get message string.
         * \return Message string.
         */
        const std::string& Message() const
        {
            return m_message;
        }
        /*!
         * \brief Get time stamp.
         * \return Time stamp.
         */
        time_t TimeStamp() const
        {
            return m_timeStamp;
        }
        /*!
         * \brief Get source file name string.
         * \return File name string.
         */
        const std::string& File() const
        {
            return m_file;
        }
        /*!
         * \brief Get source file line number.
         * \return Line number.
         */
        int LineNo() const
        {
            return m_lineNo;
        }
        /*!
         * \brief Get thread ID where message originated.
         * \return Thread ID.
         */
        const std::thread::id& ThreadID() const
        {
            return m_threadID;
        }
        /*!
         * \brief Get message error level.
         * \return Error level.
         */
        eLogMessageLevel ErrorLevel() const
        {
            return m_errorLevel;
        }

    private:
        /*! \brief Message string.*/
        std::string m_message;
        /*! \brief Time stamp.*/
        time_t m_timeStamp{0};
        /*! \brief Source file name.*/
        std::string m_file;
        /*! \brief Line number in source file.*/
        int m_lineNo{0};
        /*! \brief Thread ID where message originated.*/
        std::thread::id m_threadID;
        /*! \brief Message error level.*/
        eLogMessageLevel m_errorLevel{eLogMessageLevel::not_defined};
    };

    /*! \brief Mutex to lock access.*/
    mutable std::mutex m_mutex;
    /*! \brief Log formatter object.*/
    Formatter m_logFormatter;
    /*! \brief Output file stream.*/
    std::ofstream m_ofStream;
    /*! \brief Software version string.*/
    const std::string m_softwareVersion;
    /*! \brief Path to current log file.*/
    const std::string m_logFilePath{"log.txt"};
    /*! \brief Path to old log file.*/
    const std::string m_oldLogFilePath{"log_old.txt"};
    /*! \brief String for unknown message level.*/
    const std::string m_unknownLogMsgLevel{"?"};
    /*! \brief Typedef for message queue thread.*/
    typedef threads::MessageQueueThread<int, LogQueueMessage> log_msg_queue;
    /*! \brief Unique_ptr holding message queue thread.*/
    std::unique_ptr<log_msg_queue>
        m_logMsgQueueThread{new log_msg_queue(std::bind(&DebugLog::MessageDecoder
                                                        , this
                                                        , std::placeholders::_1
                                                        , std::placeholders::_2)
                                              , threads::eOnDestroyOptions::processRemainingItems)};
    /*! \brief Message level string lookup map.*/
    std::unordered_map<eLogMessageLevel, std::string>
        m_logMsgLevelLookup{{eLogMessageLevel::not_defined, ""}
                            , {eLogMessageLevel::debug, "Debug"}
                            , {eLogMessageLevel::info, "Info"}
                            , {eLogMessageLevel::warning, "Warning"}
                            , {eLogMessageLevel::error, "Error"}
                            , {eLogMessageLevel::fatal, "Fatal"}};
    /*! \brief Message level filter set.*/
    std::set<eLogMessageLevel> m_logMsgFilterSet;

    /*! \brief Register the log queue message ID. */
    void RegisterLogQueueMessageId()
    {
        m_logMsgQueueThread->RegisterMessageHandler(LogQueueMessage::MESSAGE_ID
                                                   , std::bind(&DebugLog::MessageHandler
                                                               , this
                                                               , std::placeholders::_1
                                                               , std::placeholders::_2));
    }
    /*!
     * \brief Method to decode message ID.
     * \param [IN] Message to decode.
     * \param [IN] Message length.
     * \return Message ID.
     */
    int MessageDecoder(const LogQueueMessage* message, int length)
    {
        if (!message || (length == 0))
        {
            BOOST_THROW_EXCEPTION(xLogMsgHandlerError("invalid message in DebugLog::MessageDecoder"));
        }

        return LogQueueMessage::MESSAGE_ID;
    }
    /*!
     * \brief Method to process message.
     * \param [IN] Message to process.
     * \param [IN] Message length.
     * \return LogQueueMessage reference.
     */
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
    /*!
     * \brief Is message level in map.
     * \param [IN] Message level.
     * \return True if message level is found, false otherwise.
     */
    bool IsLogMsgLevelInLookup(eLogMessageLevel logMessageLevel) const
    {
        return m_logMsgLevelLookup.count(logMessageLevel) > 0;
    }
    /*!
     * \brief Get message level as a string.
     * \param [IN] Message level.
     * \return Message level string.
     */
    const std::string& GetLogMsgLevelAsString(eLogMessageLevel logMessageLevel)
    {
        return IsLogMsgLevelInLookup(logMessageLevel)
               ? m_logMsgLevelLookup[logMessageLevel]
               : m_unknownLogMsgLevel;
    }
    /*!
     * \brief Is message level in filter set (no mutex).
     * \param [IN] Message level.
     * \return True if message level is found, false otherwise.
     */
    bool IsLogMsgLevelFilterSetNoMutex(eLogMessageLevel logMessageLevel) const
    {
        return (m_logMsgFilterSet.find(logMessageLevel) != m_logMsgFilterSet.end());
    }
    /*!
     * \brief Is message level in filter set (with mutex).
     * \param [IN] Message level.
     * \return True if message level is found, false otherwise.
     */
    bool IsLogMsgLevelFilterSet(eLogMessageLevel logMessageLevel) const
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        return IsLogMsgLevelFilterSetNoMutex(logMessageLevel);
    }
    /*! \brief Enumeration containing file opening options. */
    enum class eFileOpenOptions
    {
        /*! \brief Option to truncate file when opened. */
        truncate_file,
        /*! \brief Option to append to file when opened. */
        append_file
    };
    /*!
     * \brief Open file stream.
     * \param [IN] File path.
     * \param [IN] FIle options (truncate or append).
     */
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
            m_ofStream.open(filePath, std::ofstream::ate);
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
    /*! \brief Close file stream. */
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
    /*!
     * \brief Check size of current log file.
     * \param [IN] Space required in file to write new message.
     */
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
    /*!
     * \brief Write log message to file stream.
     * \param [IN] Log message (r-value).
     */
    void WriteMessageToLog(LogQueueMessage&& logMessage)
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
    /*!
     * \brief Write log message to file stream.
     * \param [IN] Log message (l-value).
     */
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

#endif // DEBUGLOG_HPP
