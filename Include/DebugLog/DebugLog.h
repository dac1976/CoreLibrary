// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
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

#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"

#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <unordered_map>
#include <algorithm>
#ifdef USE_EXPLICIT_MOVE_
#include <utility>
#endif
#include <memory>
#include <functional>
#include <type_traits>
#include <stdexcept>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#include <boost/throw_exception.hpp>
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

/*!
 * \brief Default log line formater.
 *
 * This functor formats the log message elements
 * into a single line in the log following default
 * formatting.
 *
 * "Date/Time" | "Level" | "Message" | "File" | "Function" | Line = "..." | Thread ID = "..."
 *
 * If different formatting is required then a user can define their own compatible formatter
 * functor, as long as it has a fully compatible interface with DefaultLogFormat.
 */
struct CORE_LIBRARY_DLL_SHARED_API DefaultLogFormat
{
    /*!
     * \brief Function operator to perform the line formatting.
     * \param[out] os - Output stream to write formatted line to.
     * \param[in] timeStamp -The timestamp.
     * \param[in] message - The actual message.
     * \param[in] logMsgLevel - Log message level.
     * \param[in] file - File where log message was generated.
     * \param[in] function - Function where log message was generated.
     * \param[in] lineNo - Line number where log message was generated.
     * \param[in] threadID - Thread ID fo where log message was generated.
     */
    void operator()(std::ostream& os, std::time_t timeStamp, const std::string& message,
                    const std::string& logMsgLevel, const std::string& file,
                    const std::string& function, int lineNo, const std::thread::id& threadID) const;
};

/*! \brief Static constant defining number of bytes in a mebibyte. */
STATIC_CONSTEXPR_ size_t BYTES_IN_MEBIBYTE{1024 * 1024};

namespace dl_private
{

/*!
 * \brief Log Queue Message class.
 *
 * Message to be placed on message queue to process debug log entry to add to file. This is used
 * internally by the DebugLog class and so is in its own private namespace and is not needed to be
 * used externally to DebugLog.
 */
class CORE_LIBRARY_DLL_SHARED_API LogQueueMessage
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
     * \param[in] function - Function insource file in which message AddLogMessage was called, e.g.
     * BOOST_CURRENT_FUNCTION.
     * \param[in] lineNo - Line number in the source file where AddLogMessage was called, e.g.
     * __LINE__.
     * \param[in] threadID - Thread ID where message was added from.
     * \param[in] errorLevel - Message level.
     */
    LogQueueMessage(const std::string& message, time_t timeStamp, const std::string& file,
                    const std::string& function, int lineNo, const std::thread::id& threadID,
                    eLogMessageLevel errorLevel);
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

private:
    /*! \brief Message string.*/
    std::string m_message{};
    /*! \brief Time stamp.*/
    time_t m_timeStamp{0};
    /*! \brief Source file name.*/
    std::string m_file{};
    /*! \brief Function name.*/
    std::string m_function{};
    /*! \brief Line number in source file.*/
    int m_lineNo{0};
    /*! \brief Thread ID where message originated.*/
    std::thread::id m_threadID{};
    /*! \brief Message error level.*/
    eLogMessageLevel m_errorLevel{eLogMessageLevel::not_defined};
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
template <class Formatter> class DebugLog final
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
     *
     * Create the DebugLog in given folder with given name. A ".txt"
     * extension is automatically appending to log file's name.
     */
    DebugLog(const std::string& softwareVersion, const std::string& logFolderPath,
             const std::string& logName, long maxLogSize = 5 * BYTES_IN_MEBIBYTE)
        :
#ifdef USE_DEFAULT_CONSTRUCTOR_
        m_unknownLogMsgLevel("?")
        ,
#endif
        m_maxLogSize(maxLogSize)
        , m_softwareVersion(softwareVersion)
        , m_logFilePath(logFolderPath)
        , m_oldLogFilePath(logFolderPath)
    {
        m_logFilePath.append(logName);
        m_logFilePath.append(".txt");

        m_oldLogFilePath.append(logName);
        m_oldLogFilePath.append("_old.txt");

#ifdef USE_DEFAULT_CONSTRUCTOR_
        InitialiseLogMessageLevelLookupMap();
#endif
        RegisterLogQueueMessageId();
        OpenOfStream(m_logFilePath, eFileOpenOptions::append_file);
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
        CloseOfStream();
    }
    /*!
     * \brief Instantiate a previously default constructed DebugLog object.
     * \param[in] softwareVersion - Version of software that "owns" the log.
     * \param[in] logFolderPath - Folder path (with trailing slash) where log will be created.
     * \param[in] logName - File name of log file without extension.
     * \param[in] maxLogSize - (Optional) The maximum size for the log file.
     *
     * Instantiate the DebugLog in given folder with given name. A ".txt"
     * extension is automatically appending to log file's name.
     * This method should only be used when constructing a DebugLog
     * using the default constructor.
     *
     * This method throws std::runtime_error exception if object
     * already instantiated.
     */
    void Instantiate(const std::string& softwareVersion, const std::string& logFolderPath,
                     const std::string& logName, long maxLogSize = 5 * BYTES_IN_MEBIBYTE)
    {
        m_maxLogSize = maxLogSize;

        if ((m_softwareVersion != "") || (m_logFilePath != "") || (m_oldLogFilePath != ""))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("DebugLog already instantiated"));
        }

        m_softwareVersion = softwareVersion;
        m_logFilePath     = logFolderPath;
        m_oldLogFilePath  = logFolderPath;

        m_logFilePath.append(logName);
        m_logFilePath.append(".txt");

        m_oldLogFilePath.append(logName);
        m_oldLogFilePath.append("_old.txt");

        RegisterLogQueueMessageId();
        OpenOfStream(m_logFilePath, eFileOpenOptions::append_file);
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
     *
     * Add a simple message to the log without any extra
     * properties set, such as file, line no. etc.
     */
    void AddLogMessage(const std::string& message)
    {
        using std::chrono::system_clock;
        time_t          messageTime = system_clock::to_time_t(system_clock::now());
        std::thread::id noThread;
        m_logMsgQueueThread->Push(dl_private::LogQueueMessage(
            message, messageTime, "", "", -1, noThread, eLogMessageLevel::not_defined));
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
     *
     * Add a message to the log with extra properties set, such as
     * file, line no. etc.
     */
    void AddLogMessage(const std::string& message, const std::string& file,
                       const std::string& function, int lineNo, eLogMessageLevel logMsgLevel)
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
                                                                  logMsgLevel));
        }
    }

private:
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
        return message.MESSAGE_ID;
    }
    /*!
     * \brief Method to process message.
     * \param[in] message - Message to process.
     * \return LogQueueMessage reference.
     */
    bool MessageHandler(dl_private::LogQueueMessage& message)
    {
        CheckLogFileSize(static_cast<long>(message.Message().size()));
        WriteMessageToLog(std::forward<dl_private::LogQueueMessage>(message));
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
     */
    void OpenOfStream(const std::string& filePath, eFileOpenOptions fileOptions)
    {
        if (m_ofStream.is_open())
        {
            return;
        }

        m_ofStream.open(filePath,
                        fileOptions == eFileOpenOptions::truncate_file ? std::ofstream::trunc
                                                                       : std::ofstream::app);

        using std::chrono::system_clock;
        time_t          messageTime = system_clock::to_time_t(system_clock::now());
        std::thread::id noThread;
        WriteMessageToLog(dl_private::LogQueueMessage(
            "DEBUG LOG STARTED", messageTime, "", "", -1, noThread, eLogMessageLevel::not_defined));

        if (m_softwareVersion != "")
        {
            std::string message("Software Version ");
            message += m_softwareVersion;
            WriteMessageToLog(dl_private::LogQueueMessage(
                message, messageTime, "", "", -1, noThread, eLogMessageLevel::not_defined));
        }
    }
    /*! \brief Close file stream. */
    void CloseOfStream()
    {
        if (!m_ofStream.is_open())
        {
            return;
        }

        using std::chrono::system_clock;
        time_t          messageTime = system_clock::to_time_t(system_clock::now());
        std::thread::id noThread;
        WriteMessageToLog(dl_private::LogQueueMessage(
            "DEBUG LOG STOPPED", messageTime, "", "", -1, noThread, eLogMessageLevel::not_defined));
        m_ofStream.close();
    }
    /*!
     * \brief Check size of current log file.
     * \param[in] requiredSpace - Space required in file to write new message.
     */
    void CheckLogFileSize(long requiredSpace)
    {
        if (!m_ofStream.is_open())
        {
            return;
        }

        auto pos = static_cast<long>(m_ofStream.tellp());

        if ((MaxLogSize() - pos) < requiredSpace)
        {
            CloseOfStream();
            boost::filesystem::copy_file(m_logFilePath,
                                         m_oldLogFilePath,
                                         boost::filesystem::copy_option::overwrite_if_exists);
            OpenOfStream(m_logFilePath, eFileOpenOptions::truncate_file);
        }
    }
    /*!
     * \brief Write log message to file stream.
     * \param[in] logMessage - Log message, pefectly forwarded.
     */
    void WriteMessageToLog(dl_private::LogQueueMessage&& logMessage)
    {
        m_logFormatter(m_ofStream,
                       logMessage.TimeStamp(),
                       logMessage.Message(),
                       GetLogMsgLevelAsString(logMessage.ErrorLevel()),
                       logMessage.File(),
                       logMessage.Function(),
                       logMessage.LineNo(),
                       logMessage.ThreadID());
        m_ofStream.flush();
    }

private:
    /*! \brief Mutex to lock access.*/
    mutable std::mutex m_mutex;
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
    std::set<eLogMessageLevel> m_logMsgFilterSet{};
    /*! \brief Log formatter object.*/
    Formatter m_logFormatter{};
    /*! \brief Log file max size.*/
    long m_maxLogSize{5 * BYTES_IN_MEBIBYTE};
    /*! \brief Output file stream.*/
    std::ofstream m_ofStream{};
    /*! \brief Software version string.*/
    std::string m_softwareVersion{};
    /*! \brief Path to current log file.*/
    std::string m_logFilePath{};
    /*! \brief Path to old log file.*/
    std::string m_oldLogFilePath{};
    /*! \brief Typedef for message queue thread.*/
    using log_msg_queue = threads::MessageQueueThread<int, dl_private::LogQueueMessage>;
    /*! \brief Unique_ptr holding message queue thread.*/
    std::unique_ptr<log_msg_queue> m_logMsgQueueThread{
        new log_msg_queue(std::bind(&DebugLog<Formatter>::MessageDecoder, std::placeholders::_1),
                          threads::eOnDestroyOptions::processRemainingItems)};
};

} // namespace log
} // namespace core_lib

#endif // DEBUGLOG
