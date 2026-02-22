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
 * \file DebugLog.h
 * \brief File containing declaration of DebugLog class.
 */

#ifndef DEBUGLOG
#define DEBUGLOG

#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
#include <tuple>
#include <unordered_map>
#include <stdexcept>
#include <utility>
#include <memory>
#include <functional>
#include <type_traits>
#include <boost/throw_exception.hpp>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"          
#include "FileUtils/SelectFileSystemLibrary.hpp" 
#include "Threads/MessageQueueThread.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The log namespace. */
namespace log
{

/*! \brief Enumeration containing log message level options. */
enum class eLogMessageLevel
{
    /*! \brief Special value to use when copying mirror log to buffer. */
    copy_mirror_log_to_buf = -2,
    /*! \brief Special value to use when copying log to buffer. */
    copy_log_to_buf = -1,
    /*! \brief No level defined for message. Mapped to default colour in console. */
    not_defined = 0,
    /*! \brief Debug level defined for message. Mapped to cyan colour in console. */
    debug,
    /*! \brief Info level defined for message. Mapped to green colour in console. */
    info,
    /*! \brief Warning level defined for message. Mapped to yellow colour in console. */
    warning,
    /*! \brief Error level defined for message. Mapped to red colour in console. */
    error,
    /*! \brief Fatal level defined for message. Mapped to magenta colour in console. */
    fatal
};

/*! \brief Control where the message gets output to.*/
enum class eMsgTarget
{
    file,
    console,
    both
};

} // namespace log
} // namespace core_lib

/*! \brief The std namespace. */
namespace std
{

/*! \brief Template specialisation of std::hash for enum eLogMessageLevel. */
template <> struct hash<core_lib::log::eLogMessageLevel>
{
    /*! \brief Typedef for argument type. */
    using argument_t = core_lib::log::eLogMessageLevel;
    /*! \brief Typedef for result type. */
    using result_t = std::size_t;
    /*! \brief Typedef for underlying type. */
    using enumType_t = TYPENAME_DECL_ std::underlying_type<argument_t>::type;

    /*!
     * \brief Function operator to perform the hash.
     * \param[in] a - Argument to be hashed.
     * \return The hash value.
     */
    result_t operator()(const argument_t& a) const
    {
        auto                  a2 = static_cast<enumType_t>(a);
        std::hash<enumType_t> h;
        return h(a2);
    }
};

} // namespace std

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The log namespace. */
namespace log
{

extern const char ANSI_DEFAULT[];
extern const char ANSI_RED[];
extern const char ANSI_YELLOW[];
extern const char ANSI_MAGENTA[];
extern const char ANSI_CYAN[];
extern const char ANSI_WHITE[];

/*!
 * \brief Default log line formatter.
 *
 * This functor formats the log message elements
 * into a single line in the log following default
 * formatting.
 *
 * "Date/Time" | "Level" | "Message" | "File" | "Function" | Line = "..." | Thread ID = "..."
 * or
 * "Date/Time" | "Level" | "Message"
 *
 * The first format is used for writing to the log file and the second format is typically
 * intended for writing to the console, when that mode is active.
 *
 * If different formatting is required then a user can define their own compatible formatter
 * functor, as long as it has a fully compatible interface with DefaultLogFormat.
 */
struct CORE_LIBRARY_DLL_SHARED_API DefaultLogFormat FINAL_
{
    /*!
     * \brief Function operator to perform the line formatting, suitable for std::ofstream.
     * \param[out] os - Output stream to write formatted line to.
     * \param[in] timeStamp -The timestamp.
     * \param[in] message - The actual message.
     * \param[in] logMsgLevel - Log message level.
     * \param[in] file - File where log message was generated.
     * \param[in] function - Function where log message was generated.
     * \param[in] lineNo - Line number where log message was generated.
     * \param[in] threadID - Thread ID fo where log message was generated.
     * \param[in] utcTimeStamps - Enable use of UTC timestamps instead of local time.
     * \param[in] tzOffset - Include timezone offset, e.g. +hhmm ... +0100.
     */
    void operator()(std::ostream& os, std::time_t timeStamp, const std::string& message,
                    const std::string& logMsgLevel, const std::string& file,
                    const std::string& function, int lineNo, const std::thread::id& threadID,
                    bool utcTimeStamps, bool tzOffset) const;

    /*!
     * \brief Function operator to perform the line formatting, suitable for std::cout.
     * \param[out] os - Output stream to write formatted line to.
     * \param[in] timeStamp -The timestamp.
     * \param[in] message - The actual message.
     * \param[in] logMsgLevel - Log message level.
     * \param[in] utcTimeStamps - Enable use of UTC timestamps instead of local time.
     * \param[in] tzOffset - Include timezone offset, e.g. +hhmm ... +0100.
     * \param[in] colourCode - If set to true then ASCII colour codes are applied to the stream;
     * only use for console output, passed as one of the constants: ANSI_* from above.
     */
    void operator()(std::ostream& os, std::time_t timeStamp, const std::string& message,
                    const std::string& logMsgLevel, bool utcTimeStamps, bool tzOffset,
                    const char* colourCode = nullptr) const;
};

/*! \brief Static constant defining number of bytes in a mebibyte. */
enum eDebugLogSize : size_t
{
    BYTES_IN_MEBIBYTE = 1024 * 1024
};

namespace dl_private
{

/*!
 * \brief Log Queue Message class.
 *
 * Message to be placed on message queue to process debug log entry to add to file. This is used
 * internally by the DebugLog class and so is in its own private namespace and is not needed to
 * be used externally to DebugLog.
 */
class CORE_LIBRARY_DLL_SHARED_API LogQueueMessage FINAL_
{
public:
    /*! \brief Static message ID to register on message queue.*/
    STATIC_CONSTEXPR_ int MESSAGE_ID{1};
    /*! \brief Default constructor.*/
    LogQueueMessage() = default;
    /*!
     * \brief Initialising constructor.
     * \param[in] message - Message to add to log.
     * \param[in] timeStamp - Date/Time stamp for message.
     * \param[in] file - Source file in which message AddLogMessage was called, e.g.
     * std::string(__FILE__).
     * \param[in] function - Function insource file in which message AddLogMessage was called,
     * e.g. BOOST_CURRENT_FUNCTION. \param[in] lineNo - Line number in the source file where
     * AddLogMessage was called, e.g.
     * __LINE__.
     * \param[in] threadID - Thread ID where message was added from.
     * \param[in] errorLevel - Message level.
     * \param[in] msgTarget - Message target, e.g. file, console or both.
     */
    LogQueueMessage(std::string const& message, time_t timeStamp, std::string const& file,
                    std::string const& function, int lineNo, const std::thread::id& threadID,
                    eLogMessageLevel errorLevel, eMsgTarget msgTarget = eMsgTarget::file);
    /*! \brief Copy constructor. */
    LogQueueMessage(const LogQueueMessage&) = default;
    /*! \brief Destructor.*/
    ~LogQueueMessage() = default;
    /*! \brief Copy assignment operator. */
    LogQueueMessage& operator=(const LogQueueMessage&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move constructor. */
    LogQueueMessage(LogQueueMessage&& msg);
    /*! \brief Move assignment operator. */
    LogQueueMessage& operator=(LogQueueMessage&& msg);
#else
    /*! \brief Move constructor. */
    LogQueueMessage(LogQueueMessage&&) = default;
    /*! \brief Move assignment operator. */
    LogQueueMessage& operator=(LogQueueMessage&&) = default;
#endif
    /*!
     * \brief Get message string.
     * \return Message string.
     */
    const std::string& Message() const;
    /*!
     * \brief Get time stamp.
     * \return Time stamp.
     */
    time_t TimeStamp() const;
    /*!
     * \brief Get source file name string.
     * \return File name string.
     */
    const std::string& File() const;
    /*!
     * \brief Get function name string.
     * \return File name string.
     */
    const std::string& Function() const;
    /*!
     * \brief Get source file line number.
     * \return Line number.
     */
    int LineNo() const;
    /*!
     * \brief Get thread ID where message originated.
     * \return Thread ID.
     */
    const std::thread::id& ThreadID() const;
    /*!
     * \brief Get message error level.
     * \return Error level.
     */
    eLogMessageLevel ErrorLevel() const;
    /*!
     * \brief Get message target .
     * \return Message target.
     */
    eMsgTarget MsgTarget() const;

private:
    /*! \brief Message string.*/
    std::string m_message;
    /*! \brief Time stamp.*/
    time_t m_timeStamp{0};
    /*! \brief Source file name.*/
    std::string m_file;
    /*! \brief Function name.*/
    std::string m_function;
    /*! \brief Line number in source file.*/
    int m_lineNo{0};
    /*! \brief Thread ID where message originated.*/
    std::thread::id m_threadID;
    /*! \brief Message error level.*/
    eLogMessageLevel m_errorLevel{eLogMessageLevel::not_defined};
    /*! \brief Message target.*/
    eMsgTarget m_msgTarget{eMsgTarget::file};
};

} // namespace dl_private

/*!
 * \brief DebugLog class.
 *
 * Templated class to perform logging. The class is
 * threaded and thread safe.
 *
 * The template args comprise a formatter type
 * and which should be a function object with same
 * prototype as DefaultLogFormat::operator()().
 *
 * The second arg is optional and controls the size at which
 * the log will close and switch to a new file. Only 2 files
 * ever exist the "log".txt and "log"_old.txt. The default log
 * size is 5MiB.
 */
template <class Formatter> class DebugLog FINAL_
{
public:
/*!
 * \brief Default constructor.
 *
 * Create the DebugLog object in a partial state.
 * This is used by the SingletonHolder to create
 * a singleton DebugLog instance. The user should
 * then call Instantiate to complete the setup of
 * the log, otherwise no output or file will be
 * created.
 */
#ifdef USE_DEFAULT_CONSTRUCTOR_
    DebugLog()
        : m_unknownLogMsgLevel("?")
    {
        InitialiseLogMessageLevelLookupMap();
    }
#else
    DebugLog()                                    = default;
#endif
    /*!
     * \brief Initialisation constructor.
     * \param[in] softwareVersion - Version of software that "owns" the log.
     * \param[in] logFolderPath - Folder path (with trailing slash) where log will be created.
     * \param[in] logName - File name of log file without extension.
     * \param[in] maxLogSize - (Optional) The maximum size for the log file.
     * \param[in] logExt - (Optional) The extension to use for the file, including '.'.
     * \param[in] utcTimeStamps - (Optional) Enable use of UTC timestamps instead of local time.
     * \param[in] tzOffset - (Optional) Include timezone offset, e.g. +hhmm ... +0100.
     * \param[in] mirrorLogFolderPath - (Optional) Folder path (with trailing slash) where a mirror
     *                                  of the log will be created.
     *
     * Create the DebugLog in given folder with given name. A ".txt"
     * extension is automatically appending to log file's name.
     */
    DebugLog(std::string const& softwareVersion, std::string const& logFolderPath,
             std::string const& logName, long maxLogSize = 5 * BYTES_IN_MEBIBYTE,
             std::string const& logExt = ".txt", bool utcTimeStamps = false, bool tzOffset = false,
             std::string const& mirrorLogFolderPath = "")
        :
#ifdef USE_DEFAULT_CONSTRUCTOR_
        m_unknownLogMsgLevel("?")
        ,
#endif
        m_maxLogSize(maxLogSize)
        , m_logExt(logExt)
        , m_utcTimeStamps(utcTimeStamps)
        , m_tzOffset(tzOffset)
        , m_softwareVersion(softwareVersion)
    {
        if (m_logExt.empty() || (m_logExt.front() != '.'))
        {
            m_logExt = ".txt";
        }

        std::tie(m_logFilePath, m_oldLogFilePath) = BuildFilePaths(logFolderPath, logName);

        if (!mirrorLogFolderPath.empty() && (mirrorLogFolderPath != logFolderPath))
        {
            std::tie(m_mirrorLogFilePath, m_oldMirrorLogFilePath) =
                BuildFilePaths(mirrorLogFolderPath, logName);
        }

#ifdef USE_DEFAULT_CONSTRUCTOR_
        InitialiseLogMessageLevelLookupMap();
#endif
        RegisterLogQueueMessageId();

        m_logStatus = OpenOfStream(m_logFilePath, eFileOpenOptions::append_file, m_ofStream);

        if (!m_mirrorLogFilePath.empty())
        {
            m_mirrorLogStatus =
                OpenOfStream(m_mirrorLogFilePath, eFileOpenOptions::append_file, m_ofStreamMirror);
        }
    }
    /*! \brief Copy constructor deleted.*/
    DebugLog(const DebugLog&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    DebugLog& operator=(const DebugLog&) = delete;
    /*! \brief Move constructor deleted.*/
    DebugLog(DebugLog&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    DebugLog& operator=(DebugLog&&) = delete;
    /*! \brief Destructor*/
    ~DebugLog()
    {
        // Manually reset so we process all remaining
        // messages before closing file.
        m_logMsgQueueThread.reset();
        CloseOfStream(m_ofStream);
        CloseOfStream(m_ofStreamMirror);
    }
    /*!
     * \brief Instantiate a previously default constructed DebugLog object.
     * \param[in] softwareVersion - Version of software that "owns" the log.
     * \param[in] logFolderPath - Folder path (with trailing slash) where log will be created.
     * \param[in] logName - File name of log file without extension.
     * \param[in] maxLogSize - (Optional) The maximum size for the log file.
     * \param[in] logExt - (Optional) The extension to use for the file, including '.'.
     * \param[in] utcTimeStamps - (Optional) Enable use of UTC timestamps instead of local time.
     * \param[in] tzOffset - (Optional) Include timezone offset, e.g. +hhmm ... +0100.
     * \param[in] mirrorLogFolderPath - (Optional) Folder path (with trailing slash) where a mirror
     *                                  of the log will be created.
     *
     * Instantiate the DebugLog in given folder with given name. A ".txt"
     * extension is automatically appending to log file's name.
     * This method should only be used when constructing a DebugLog
     * using the default constructor.
     *
     * This method throws xInstantiationError exception if object
     * already instantiated.
     */
    void Instantiate(const std::string& softwareVersion, const std::string& logFolderPath,
                     const std::string& logName, long maxLogSize = 5 * BYTES_IN_MEBIBYTE,
                     std::string const& logExt = ".txt", bool utcTimeStamps = false,
                     bool tzOffset = false, std::string const& mirrorLogFolderPath = "")
    {
        m_maxLogSize = maxLogSize;

        if ((m_softwareVersion != "") || (m_logFilePath != "") || (m_oldLogFilePath != "") ||
            (m_logExt != ""))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("DebugLog already instantiated"));
        }

        m_logExt = logExt;

        if (m_logExt.empty() || (m_logExt.front() != '.'))
        {
            m_logExt = ".txt";
        }

        m_utcTimeStamps   = utcTimeStamps;
        m_tzOffset        = tzOffset;
        m_softwareVersion = softwareVersion;

        std::tie(m_logFilePath, m_oldLogFilePath) = BuildFilePaths(logFolderPath, logName);

        if (!mirrorLogFolderPath.empty() && (mirrorLogFolderPath != logFolderPath))
        {
            std::tie(m_mirrorLogFilePath, m_oldMirrorLogFilePath) =
                BuildFilePaths(mirrorLogFolderPath, logName);
        }

        RegisterLogQueueMessageId();

        m_logStatus = OpenOfStream(m_logFilePath, eFileOpenOptions::append_file, m_ofStream);

        if (!m_mirrorLogFilePath.empty())
        {
            m_mirrorLogStatus =
                OpenOfStream(m_mirrorLogFilePath, eFileOpenOptions::append_file, m_ofStreamMirror);
        }
    }
    /*!
     * \brief Add level to filter.
     * \param[in] logMessageLevel - Message level to filter out of log.
     *
     * You can dynamically filter out log message from appearing
     * in the log file based on adding message levels to the
     * filter set. For example, you may want to filter out
     * messages of type eLogMessageLevel::warning. In this case
     * after calling this function with eLogMessageLevel::warning
     * messages of this type will not appear in the log from this point
     * until you later remove the filter.
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
     * \param[in] logMessageLevel - Message level to remove from filter set.
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
     * \param[in] message - Message to add to log.
     * \param[in] msgTarget - Message target, e.g. file, console or both.
     *
     * Add a simple message to the log without any extra
     * properties set, such as file, line no. etc.
     */
    void AddLogMessage(const std::string& message, eMsgTarget msgTarget = eMsgTarget::file)
    {
        using std::chrono::system_clock;
        time_t          messageTime = system_clock::to_time_t(system_clock::now());
        std::thread::id noThread;
        m_logMsgQueueThread->Push(dl_private::LogQueueMessage(
            message, messageTime, "", "", -1, noThread, eLogMessageLevel::not_defined, msgTarget));
    }

    /*!
     * \brief Add message to the log file.
     * \param[in] message - Message to add to log.
     * \param[in] file - Source file in which message AddLogMessage was called, e.g.
     * std::string(__FILE__).
     * \param[in] function - Function in source file in which message AddLogMessage was called, e.g.
     * BOOST_CURRENT_FUNCTION.
     * \param[in] lineNo - Line number in the source file where AddLogMessage was called, e.g.
     * __LINE__.
     * \param[in] logMsgLevel - Message level.
     * \param[in] msgTarget - Message target, e.g. file, console or both.
     *
     * Add a message to the log with extra properties set, such as
     * file, line no. etc.
     */
    void AddLogMessage(const std::string& message, const std::string& file,
                       const std::string& function, int lineNo, eLogMessageLevel logMsgLevel,
                       eMsgTarget msgTarget = eMsgTarget::file)
    {
        if (!IsLogMsgLevelFilterSet(logMsgLevel))
        {
            using std::chrono::system_clock;
            time_t messageTime = system_clock::to_time_t(system_clock::now());
            m_logMsgQueueThread->Push(dl_private::LogQueueMessage(message,
                                                                  messageTime,
                                                                  file,
                                                                  function,
                                                                  lineNo,
                                                                  std::this_thread::get_id(),
                                                                  logMsgLevel,
                                                                  msgTarget));
        }
    }

    /*!
     * \brief Retrieve current log file path.
     * \return File path as a string.
     */
    std::string const& LogFilePath() const
    {
        return m_logFilePath;
    }

    /*!
     * \brief Retrieve old log file path.
     * \return File path as a string.
     */
    std::string const& OldLogFilePath() const
    {
        return m_oldLogFilePath;
    }

    /*!
     * \brief Retrieve current mirror log file path.
     * \return File path as a string.
     */
    std::string const& MirrorLogFilePath() const
    {
        return m_mirrorLogFilePath;
    }

    /*!
     * \brief Retrieve old mirror log file path.
     * \return File path as a string.
     */
    std::string const& OldMirrorLogFilePath() const
    {
        return m_oldMirrorLogFilePath;
    }

    /*!
     * \brief Log status.
     * \return Boolean denoting if we are currently able to use the log.
     */
    bool LogStatus() const
    {
        std::lock_guard<std::mutex> lock{m_mutex};

        return m_logStatus;
    }

    /*!
     * \brief Mirror log status.
     * \return Boolean denoting if we are currently able to use the log.
     */
    bool MirrorLogStatus() const
    {
        std::lock_guard<std::mutex> lock{m_mutex};

        return m_mirrorLogStatus;
    }

    /*!
     * \brief Safely copy log file into a buffer.
     * \param[in] oldLog - Flag indicating if we are copying current or old log.
     * \param[out] logBuffer - Reference to a vector to contain the log file.
     * \param[in] mirrorLog - Are we copying the mirror log (if one exists).
     */
    void CopyLogToBuffer(bool oldLog, std::vector<char>& logBuffer, bool mirrorLog = false)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        logBuffer.clear();
        m_copyOldLog = oldLog;
        m_logBufPtr  = &logBuffer;

        std::thread::id noThread;
        m_logMsgQueueThread->Push(
            dl_private::LogQueueMessage("",
                                        0,
                                        "",
                                        "",
                                        -1,
                                        noThread,
                                        mirrorLog ? eLogMessageLevel::copy_mirror_log_to_buf
                                                  : eLogMessageLevel::copy_log_to_buf));

        m_logCopyEvent.Wait();
    }

private:
    /*! \brief Build the correct log file paths.*/
    std::pair<std::string, std::string> BuildFilePaths(const std::string& logFolderPath,
                                                       const std::string& logName)
    {
        filesys::path p = filesys::system_complete(logFolderPath);

        std::ostringstream ossCurrLogName;
        ossCurrLogName << logName << m_logExt;

        std::ostringstream ossOldLogName;
        ossOldLogName << logName << "_old" << m_logExt;

        auto currPath = p;
        currPath /= ossCurrLogName.str();
        currPath = filesys::system_complete(currPath);

        auto oldPath = p;
        oldPath /= ossOldLogName.str();
        oldPath = filesys::system_complete(oldPath);

        return std::make_pair(currPath.string(), oldPath.string());
    }
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Initialise lookup map. */
    void InitialiseLogMessageLevelLookupMap()
    {
        m_logMsgLevelLookup.emplace(eLogMessageLevel::not_defined, "");
        m_logMsgLevelLookup.emplace(eLogMessageLevel::debug, "Debug");
        m_logMsgLevelLookup.emplace(eLogMessageLevel::info, "Info");
        m_logMsgLevelLookup.emplace(eLogMessageLevel::warning, "Warning");
        m_logMsgLevelLookup.emplace(eLogMessageLevel::error, "Error");
        m_logMsgLevelLookup.emplace(eLogMessageLevel::fatal, "Fatal");
    }
#endif
    /*! \brief Get the max log size in bytes. */
    long MaxLogSize() const
    {
        return m_maxLogSize;
    }
    /*! \brief Register the log queue message ID. */
    void RegisterLogQueueMessageId()
    {
        m_logMsgQueueThread->RegisterMessageHandler(
            dl_private::LogQueueMessage::MESSAGE_ID,
            std::bind(&DebugLog<Formatter>::MessageHandler, this, std::placeholders::_1));
    }
    /*!
     * \brief Method to decode message ID.
     * \param[in] message - Message to decode.
     * \return Message ID.
     */
    static int MessageDecoder(const dl_private::LogQueueMessage& message)
    {
#if defined(_MSC_VER)
        // The line below is to stop erroneous C4100 compiler warning in MSVC2013.
        (void)message;
#endif
        return message.MESSAGE_ID;
    }
    /*!
     * \brief Method to process message.
     * \param[in] message - Message to process.
     * \return LogQueueMessage reference.
     */
    bool MessageHandler(dl_private::LogQueueMessage& message)
    {
        // If message target is file or both then write to file.
        if (eMsgTarget::console != message.MsgTarget())
        {
            if (eLogMessageLevel::copy_log_to_buf == message.ErrorLevel())
            {
                CopyLogToBuffer(m_logFilePath, m_oldLogFilePath, m_ofStream);
            }
            else if (eLogMessageLevel::copy_mirror_log_to_buf == message.ErrorLevel())
            {
                CopyLogToBuffer(m_mirrorLogFilePath, m_oldMirrorLogFilePath, m_ofStreamMirror);
            }
            else
            {
                CheckLogFileSize(static_cast<long>(message.Message().size()),
                                 m_logFilePath,
                                 m_oldLogFilePath,
                                 m_ofStream);

                CheckLogFileSize(static_cast<long>(message.Message().size()),
                                 m_mirrorLogFilePath,
                                 m_oldMirrorLogFilePath,
                                 m_ofStreamMirror);

                WriteMessageToLog(message, m_ofStream);

                WriteMessageToLog(message, m_ofStreamMirror);

                // Reduce mutex scope.
                {
                    std::lock_guard<std::mutex> lock{m_mutex};

                    m_logStatus       = m_ofStream.good();
                    m_mirrorLogStatus = m_ofStreamMirror.good();
                }
            }
        }

        // If message target is not file only then write to console.
        if (eMsgTarget::file != message.MsgTarget())
        {
            WriteToConsole(message);
        }

        return true;
    }
    /*!
     * \brief Is message level in map.
     * \param[in] logMessageLevel - Message level.
     * \return True if message level is found, false otherwise.
     */
    bool IsLogMsgLevelInLookup(eLogMessageLevel logMessageLevel) const
    {
        return m_logMsgLevelLookup.count(logMessageLevel) > 0;
    }
    /*!
     * \brief Get message level as a string.
     * \param[in] logMessageLevel - Message level.
     * \return Message level string.
     */
    const std::string& GetLogMsgLevelAsString(eLogMessageLevel logMessageLevel) const
    {
        return IsLogMsgLevelInLookup(logMessageLevel)
                   ? m_logMsgLevelLookup.find(logMessageLevel)->second
                   : m_unknownLogMsgLevel;
    }
    /*!
     * \brief Is message level in filter set (no mutex).
     * \param[in] logMessageLevel - Message level.
     * \return True if message level is found, false otherwise.
     */
    bool IsLogMsgLevelFilterSetNoMutex(eLogMessageLevel logMessageLevel) const
    {
        return (m_logMsgFilterSet.find(logMessageLevel) != m_logMsgFilterSet.end());
    }
    /*!
     * \brief Is message level in filter set (with mutex).
     * \param[in] logMessageLevel - Message level.
     * \return True if message level is found, false otherwise.
     */
    bool IsLogMsgLevelFilterSet(eLogMessageLevel logMessageLevel) const
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        return IsLogMsgLevelFilterSetNoMutex(logMessageLevel);
    }
    /*! \brief Enumeration containing file opening options. */
    enum eFileOpenOptions
    {
        /*! \brief Option to truncate file when opened. */
        truncate_file,
        /*! \brief Option to append to file when opened. */
        append_file
    };
    /*!
     * \brief Open file stream.
     * \param[in] filePath - File path.
     * \param[in] fileOptions - File options (truncate or append).
     * \param[in] ofs - File stream to be opened.
     * \return Returns the status of the stream, good or bad as a boolean.
     */
    bool OpenOfStream(const std::string& filePath, eFileOpenOptions fileOptions, std::ofstream& ofs)
    {
        if (ofs.is_open())
        {
            return ofs.good();
        }

        auto path = filesys::system_complete(filesys::path(filePath));
        filesys::create_directories(path.parent_path());

        ofs.open(path.string(),
                 fileOptions == eFileOpenOptions::truncate_file ? std::ofstream::trunc
                                                                : std::ofstream::app);

        using std::chrono::system_clock;
        time_t          messageTime = system_clock::to_time_t(system_clock::now());
        std::thread::id noThread;
        WriteMessageToLog(dl_private::LogQueueMessage("DEBUG LOG STARTED",
                                                      messageTime,
                                                      "",
                                                      "",
                                                      -1,
                                                      noThread,
                                                      eLogMessageLevel::not_defined),
                          ofs);

        if (m_softwareVersion != "")
        {
            std::string message("Software Version ");
            message += m_softwareVersion;
            WriteMessageToLog(
                dl_private::LogQueueMessage(
                    message, messageTime, "", "", -1, noThread, eLogMessageLevel::not_defined),
                ofs);
        }

        return ofs.good();
    }
    /*!
     * \brief Close file stream.
     * \param[in] ofs - File stream to be closes.
     */
    void CloseOfStream(std::ofstream& ofs)
    {
        if (!ofs.is_open())
        {
            return;
        }

        using std::chrono::system_clock;
        time_t          messageTime = system_clock::to_time_t(system_clock::now());
        std::thread::id noThread;
        WriteMessageToLog(dl_private::LogQueueMessage("DEBUG LOG STOPPED",
                                                      messageTime,
                                                      "",
                                                      "",
                                                      -1,
                                                      noThread,
                                                      eLogMessageLevel::not_defined),
                          ofs);
        ofs.close();
    }
    /*!
     * \brief Check size of current log file.
     * \param[in] requiredSpace - Space required in file to write new message.
     * \param[in] ofs - File stream.
     */
    void CheckLogFileSize(long requiredSpace, std::string const& logPath,
                          std::string const& oldLogPath, std::ofstream& ofs)
    {
        if (!ofs.is_open())
        {
            return;
        }

        auto pos = static_cast<long>(ofs.tellp());

        if ((MaxLogSize() - pos) < requiredSpace)
        {
            CloseOfStream(ofs);

#if defined(USE_STD_FILESYSTEM)
            filesys::copy_file(logPath, oldLogPath, filesys::copy_options::overwrite_existing);
#else
#if BOOST_VERSION > 107300
            filesys::copy_file(logPath, oldLogPath, filesys::copy_options::overwrite_existing);
#else
            filesys::copy_file(logPath, oldLogPath, filesys::copy_option::overwrite_if_exists);
#endif
#endif

            OpenOfStream(logPath, eFileOpenOptions::truncate_file, ofs);
        }
    }
    /*!
     * \brief Write log message to file stream.
     * \param[in] logMessage - Log message.
     * \param[in] ofs - File stream.
     */
    void WriteMessageToLog(dl_private::LogQueueMessage const& logMessage, std::ofstream& ofs) const
    {
        if (!ofs.is_open())
        {
            return;
        }

        try
        {
            m_logFormatter(ofs,
                           logMessage.TimeStamp(),
                           logMessage.Message(),
                           GetLogMsgLevelAsString(logMessage.ErrorLevel()),
                           logMessage.File(),
                           logMessage.Function(),
                           logMessage.LineNo(),
                           logMessage.ThreadID(),
                           m_utcTimeStamps,
                           m_tzOffset);
            ofs.flush();
        }
        catch (...)
        {
            // Do nothing.
        }
    }

    void WriteToConsole(dl_private::LogQueueMessage const& logMessage) const
    {
        try
        {
            const char* colourCode = nullptr;

            switch (logMessage.ErrorLevel())
            {
            case eLogMessageLevel::debug:
                colourCode = ANSI_WHITE;
                break;
            case eLogMessageLevel::info:
                colourCode = ANSI_CYAN;
                break;
            case eLogMessageLevel::warning:
                colourCode = ANSI_YELLOW;
                break;
            case eLogMessageLevel::error:
                colourCode = ANSI_RED;
                break;
            case eLogMessageLevel::fatal:
                colourCode = ANSI_MAGENTA;
                break;
            case eLogMessageLevel::copy_mirror_log_to_buf:
            case eLogMessageLevel::copy_log_to_buf:
            case eLogMessageLevel::not_defined:
            default:
                colourCode = ANSI_DEFAULT;
                break;
            }

            m_logFormatter(std::cout,
                           logMessage.TimeStamp(),
                           logMessage.Message(),
                           GetLogMsgLevelAsString(logMessage.ErrorLevel()),
                           m_utcTimeStamps,
                           m_tzOffset,
                           colourCode);
        }
        catch (...)
        {
            // Do nothing.
        }
    }

    /*! \brief Function to copy log into actual buffer.*/
    void CopyLogToBuffer(std::string const& logPath, std::string const& oldLogPath,
                         std::ofstream& ofs)
    {
        if (nullptr == m_logBufPtr)
        {
            return;
        }

        auto filePath = m_copyOldLog ? oldLogPath : logPath;

        if (!m_copyOldLog)
        {
            ofs.close();
        }

        try
        {
            m_logBufPtr->resize(static_cast<size_t>(filesys::file_size(filePath)));
            std::ifstream ifs(filePath, std::ifstream::binary);
            ifs.read(m_logBufPtr->data(), m_logBufPtr->size());
        }
        catch (...)
        {
            // Do nothing.
        }

        if (!m_copyOldLog)
        {
            ofs.open(logPath, std::ofstream::app);
        }

        m_logCopyEvent.Signal();
    }

private:
    /*! \brief Mutex to lock access.*/
    mutable std::mutex m_mutex;
    /*! \brief Flag controlling which log file to copy to buffer.*/
    bool m_copyOldLog{false};
    /*! \brief Pointer to buffer to copy log into.*/
    std::vector<char>* m_logBufPtr{nullptr};
    /*! \brief Event to signal when log has been copied to buffer.*/
    threads::SyncEvent m_logCopyEvent;
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief String for unknown message level.*/
    std::string m_unknownLogMsgLevel;
    /*! \brief Message level string lookup map.*/
    std::unordered_map<eLogMessageLevel, std::string> m_logMsgLevelLookup;
#else
    /*! \brief String for unknown message level.*/
    std::string m_unknownLogMsgLevel{"?"};
    /*! \brief Message level string lookup map.*/
    std::unordered_map<eLogMessageLevel, std::string> m_logMsgLevelLookup{
        {eLogMessageLevel::not_defined, ""},
        {eLogMessageLevel::debug, "Debug"},
        {eLogMessageLevel::info, "Info"},
        {eLogMessageLevel::warning, "Warning"},
        {eLogMessageLevel::error, "Error"},
        {eLogMessageLevel::fatal, "Fatal"}};
#endif
    /*! \brief Message level filter set.*/
    std::set<eLogMessageLevel> m_logMsgFilterSet;
    /*! \brief Log formatter object.*/
    Formatter m_logFormatter;
    /*! \brief Log file max size.*/
    long m_maxLogSize{5 * BYTES_IN_MEBIBYTE};
    /*! \brief Log file extension.*/
    std::string m_logExt;
    /*! \brief UTC timestamps.*/
    bool m_utcTimeStamps{false};
    /*! \brief Include TZ offset.*/
    bool m_tzOffset{false};
    /*! \brief Output file stream.*/
    std::ofstream m_ofStream;
    /*! \brief Output file stream mirror.*/
    std::ofstream m_ofStreamMirror;
    /*! \brief Software version string.*/
    std::string m_softwareVersion;
    /*! \brief Path to current log file.*/
    std::string m_logFilePath;
    /*! \brief Path to old log file.*/
    std::string m_oldLogFilePath;
    /*! \brief Path to mirror log file.*/
    std::string m_mirrorLogFilePath;
    /*! \brief Path to old mirror log file.*/
    std::string m_oldMirrorLogFilePath;
    /*! \brief Status of log.*/
    bool m_logStatus{false};
    /*! \brief Status of mirror log.*/
    bool m_mirrorLogStatus{false};
    /*! \brief Typedef for message queue thread.*/
    using log_msg_queue = threads::MessageQueueThread<int, dl_private::LogQueueMessage>;
    /*! \brief Unique_ptr holding message queue thread.*/
    std::unique_ptr<log_msg_queue> m_logMsgQueueThread{
        new log_msg_queue(std::bind(&DebugLog<Formatter>::MessageDecoder, std::placeholders::_1),
                          threads::eOnDestroyOptions::processRemainingItems)};
};

/*! \brief Typedef defining our default log's type. */
using default_log_t = core_lib::log::DebugLog<DefaultLogFormat>;

} // namespace log
} // namespace core_lib

#endif // DEBUGLOG
