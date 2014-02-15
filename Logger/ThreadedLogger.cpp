#include "ThreadedLogger.hpp"
#include <utility>
#include <algorithm>
#include <set>
#include <map>
#include <cstdio>
#include <ctime>
#include "IPC/ConcurrentQueue.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/throw_exception.hpp"

//-----------------------------------------------------------------------------
#ifndef TSS_CLEANUP_IMPLEMENTED
#define TSS_CLEANUP_IMPLEMENTED

extern "C" void tss_cleanup_implemented(void)
{
  // tss_cleanup_implemented needed for boost thread if
  // not using thread safe storage...
}

#endif

//-----------------------------------------------------------------------------
namespace core_lib {
//-----------------------------------------------------------------------------
namespace message_log {
//-----------------------------------------------------------------------------
static const char* UNKNOWN_ERROR_LEVEL_STR = " ?   ";
//-----------------------------------------------------------------------------
class LogQueueMessage
{
public:
	LogQueueMessage();
	LogQueueMessage(const std::string& Message,
					time_t TimeStamp,
					const std::string& File,
					const std::string& Function,
					int LineNo,
					int ThreadID,
					eLogErrorLevel ErrorLevel);
	LogQueueMessage(const LogQueueMessage& Msg);
	LogQueueMessage(LogQueueMessage&& Msg);
	LogQueueMessage& operator=(const LogQueueMessage& Msg);
	LogQueueMessage& operator=(LogQueueMessage&& Msg);

	const char* Message() const;
	time_t TimeStamp() const;
	const char* File() const;
	const char* Function() const;
	int LineNo() const;
	int ThreadID() const;
	eLogErrorLevel ErrorLevel() const;

private:
	std::vector<char> m_Message;
	time_t m_TimeStamp;
	std::vector<char> m_File;
	std::vector<char> m_Function;
	int m_LineNo;
	int m_ThreadID;
	eLogErrorLevel m_ErrorLevel;
};

//-----------------------------------------------------------------------------
LogQueueMessage::LogQueueMessage()
	:  m_Message(1, 0),//make sure char buffers have null char...
	   m_TimeStamp(0),
	   m_File(1, 0),//make sure char buffers have null char...
	   m_Function(1, 0),//make sure char buffers have null char...
	   m_LineNo(-1),
	   m_ThreadID(-1),
	   m_ErrorLevel(not_defined)
{
}
//-----------------------------------------------------------------------------
LogQueueMessage::LogQueueMessage(const std::string& Message,
								 time_t TimeStamp,
								 const std::string& File,
								 const std::string& Function,
								 int LineNo,
								 int ThreadID,
								 eLogErrorLevel ErrorLevel)
	 : m_Message(Message.size() + 1, 0),//make sure char buffers have null char...
	   m_TimeStamp(TimeStamp),
	   m_File(File.size() + 1, 0),//make sure char buffers have null char...
	   m_Function(Function.size() + 1, 0),//make sure char buffers have null char...
	   m_LineNo(LineNo),
	   m_ThreadID(ThreadID),
	   m_ErrorLevel(ErrorLevel)
{
	std::copy(Message.begin(), Message.end(), m_Message.begin());
	std::copy(File.begin(), File.end(), m_File.begin());
	std::copy(Function.begin(), Function.end(), m_Function.begin());
}
//-----------------------------------------------------------------------------
LogQueueMessage::LogQueueMessage(const LogQueueMessage& Msg)
    : m_Message(Msg.m_Message),
	  m_TimeStamp(Msg.m_TimeStamp),
	  m_File(Msg.m_File),
	  m_Function(Msg.m_Function),
	  m_LineNo(Msg.m_LineNo),
	  m_ThreadID(Msg.m_ThreadID),
	  m_ErrorLevel(Msg.m_ErrorLevel)
{
}
//-----------------------------------------------------------------------------
LogQueueMessage::LogQueueMessage(LogQueueMessage&& Msg)
	:  m_Message(1, 0),//make sure char buffers have null char...
	   m_TimeStamp(0),
	   m_File(1, 0),//make sure char buffers have null char...
	   m_Function(1, 0),//make sure char buffers have null char...
	   m_LineNo(-1),
	   m_ThreadID(-1),
	   m_ErrorLevel(not_defined)
{
    *this = std::move(Msg);
}
//-----------------------------------------------------------------------------
LogQueueMessage& LogQueueMessage::operator=(const LogQueueMessage& Msg)
{
	if (this != &Msg)
	{
		m_Message = Msg.m_Message;
		m_TimeStamp = Msg.m_TimeStamp;
		m_File = Msg.m_File;
		m_Function = Msg.m_Function;
		m_LineNo = Msg.m_LineNo;
		m_ThreadID = Msg.m_ThreadID;
		m_ErrorLevel = Msg.m_ErrorLevel;
	}

	return *this;
}
//-----------------------------------------------------------------------------
LogQueueMessage& LogQueueMessage::operator=(LogQueueMessage&& Msg)
{
	m_Message.swap(Msg.m_Message);
	std::swap(m_TimeStamp, Msg.m_TimeStamp);
	m_File.swap(Msg.m_File);
	m_Function.swap(Msg.m_Function);
	std::swap(m_LineNo, Msg.m_LineNo);
	std::swap(m_ThreadID, Msg.m_ThreadID);
	std::swap(m_ErrorLevel, Msg.m_ErrorLevel);
	return *this;
}
//-----------------------------------------------------------------------------
const char* LogQueueMessage::Message() const
{
	//vector always has at least one null char...
	return &m_Message.front();
}
//-----------------------------------------------------------------------------
time_t LogQueueMessage::TimeStamp() const
{
	return m_TimeStamp;
}
//-----------------------------------------------------------------------------
const char* LogQueueMessage::File() const
{
	//vector always has at least one null char...
	return &m_File.front();
}
//-----------------------------------------------------------------------------
const char* LogQueueMessage::Function() const
{
	//vector always has at least one null char...
	return &m_Function.front();
}
//-----------------------------------------------------------------------------
int LogQueueMessage::LineNo() const
{
	return m_LineNo;
}
//-----------------------------------------------------------------------------
int LogQueueMessage::ThreadID() const
{
	return m_ThreadID;
}
//-----------------------------------------------------------------------------
eLogErrorLevel LogQueueMessage::ErrorLevel() const
{
	return m_ErrorLevel;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ThreadedLogger::ThreadedLoggerPImpl
{
public:
    //constructor...
	ThreadedLoggerPImpl(LogFormatBase& Formatter,       /*log formatter object*/
				  const std::string& LogFolderPath,		/*folder path with trailing backslash*/
				  const std::string& LogFileName,		/*no extension*/
				  const std::string& SoftwareVersion,	/*e.g. "1.0.0.1"*/
				  const std::string& StreamIDString,	/*e.g. "HAWK CONTROLLER"*/
				  bool EnableOutputDebugString,	        /*stream output to console as well as file */
				  size_t MaxSizeInBytes);               /*max log file size in bytes*/
    //destructor...
	~ThreadedLoggerPImpl();
	//this is an option to (in "real-time") enable or disable
	//streaming debug messages to error console...
	void SetOutputDebugStringEnabled(bool EnableStreaming);
    //add error levels to the internal filter set to stop messages
    //of those levels being output...
	void AddToFilterSet(eLogErrorLevel ErrorLevel);
    //remove error levels from the internal filter set to cause messages
	//of those levels to be output...
	void RemoveFromFilterSet(eLogErrorLevel ErrorLevel);
	//add a message to the log...
	void AddLogMessage(const std::string& Message,
					   const std::string& File,     	/*e.g. std::string(__FILE__)*/
					   const std::string& Function, 	/*e.g. std::string(__FUNC__)*/
					   int LineNo,           	        /*e.g.  __LINE__*/
					   eLogErrorLevel ErrorLevel);

private:
	//member variables...
	mutable std::mutex m_Mutex;
	std::thread m_Thread;
	bool m_Running;
	FILE* m_ofFileStream;
	LogFormatBase& m_Formatter;
	const std::string m_LogFilePathCurrent;
	const std::string m_LogFilePathOld;
	const std::string m_SoftwareVersion;
	const std::string m_StreamIDString;
	const size_t m_MaxSizeInBytes;
	bool m_StreamingEnabled;
	std::map< eLogErrorLevel, std::string > m_ErrorLevelLookup;
	std::set< eLogErrorLevel > m_MessageFilterSet;
	core_lib::ipc::ConcurrentQueue< LogQueueMessage > m_LogMessageQueue;

	//disbale copying...
	ThreadedLoggerPImpl(const ThreadedLoggerPImpl&);
	ThreadedLoggerPImpl& operator= (const ThreadedLoggerPImpl);

	//stop the thread...
	void TerminateThread();
	//clear message queue writing messages
    //to file...
	void ClearMessageQueue();
    //set running state...
	void SetRunning(bool Running);
    //get running state...
	bool GetRunning() const;
    //are we outputting messages to output
    //console...
	bool GetOutputDebugStringEnabled() const;
    //what is out stream ID to append to the
    //message...
	void AppendOutputDebugStringStreamID(std::string& stubToAppendTo) const;
	//open file stram and initialise log...
	void OpenFileOutStream(const std::string& FilePath,
						   bool Truncate = false);
	//close the file stream object...
	void CloseFileStream();
    //finish the log and close file...
	void CloseAndFinaliseFile();
    //check log size and open new log if requred...
	void CheckLogSize();
	//process message queue...
	void ProcessLogMessageQueue();
    //write log message to file...
	void WriteMessageToLog(const char* LogMessage,
						   bool CheckSize = true);
    //write log message to output console...
	void WriteMessageToOutputDebugString(const char* LogMessage);
	//is error level known about...
	bool IsInLevelStringLookup(eLogErrorLevel ErrorLevel) const;
	//get a valid error level string...
	const char* GetErrorLevelAsString(eLogErrorLevel ErrorLevel);
    //build a log file entry...
	const char* BuildLine(LogQueueMessage&& LogMessage);
	//is error level in filter set...
	bool IsInFilterSet(eLogErrorLevel ErrorLevel) const;
};
//-----------------------------------------------------------------------------
ThreadedLogger::ThreadedLoggerPImpl::ThreadedLoggerPImpl(LogFormatBase& Formatter,
							 const std::string& LogFolderPath,
							 const std::string& LogFileName,
							 const std::string& SoftwareVersion,
							 const std::string& StreamIDString,
							 bool EnableOutputDebugString,
							 size_t MaxSizeInBytes)
	: m_Formatter(Formatter),
	  m_LogFilePathCurrent(LogFolderPath + LogFileName + ".txt"),
	  m_LogFilePathOld(LogFolderPath + LogFileName + "_old.txt"),
	  m_SoftwareVersion(SoftwareVersion),
	  m_StreamIDString(StreamIDString),
	  m_MaxSizeInBytes(MaxSizeInBytes),
	  m_StreamingEnabled(EnableOutputDebugString),
	  m_Running(true),
	  m_LogMessageQueue(cqoAutoDelete),
	  m_ofFileStream(NULL)
{
	//build error level map...
	m_ErrorLevelLookup[not_defined] = "";
	m_ErrorLevelLookup[trace]       = "TRACE";
	m_ErrorLevelLookup[debug]       = "DEBUG";
	m_ErrorLevelLookup[performance] = "PERF ";
	m_ErrorLevelLookup[info]        = "INFO ";
	m_ErrorLevelLookup[warning]     = "WARN ";
	m_ErrorLevelLookup[error]       = "ERROR";
	m_ErrorLevelLookup[fatal]       = "FATAL";

	//open file stream...
	OpenFileOutStream(m_LogFilePathCurrent);

	//create thread object binding it to our
	//ProcessLogMessageQueue() function...
	m_Thread = boost::thread(&ThreadedLogger::ThreadedLoggerPImpl::ProcessLogMessageQueue, this);
}
//-----------------------------------------------------------------------------
ThreadedLogger::ThreadedLoggerPImpl::~ThreadedLoggerPImpl()
{
	//terminate thread...
	TerminateThread();

	//clear message queue now thread is guaranteed
	//to have finished...
	ClearMessageQueue();

	//close the log's file stream...
	CloseAndFinaliseFile();
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::SetOutputDebugStringEnabled(bool EnableStreaming)
{
	boost::lock_guard<boost::mutex> Lock(m_Mutex);
	m_StreamingEnabled = EnableStreaming;
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::AddToFilterSet(eLogErrorLevel ErrorLevel)
{
	if (!IsInFilterSet(ErrorLevel))
	{
		boost::lock_guard<boost::mutex> Lock(m_Mutex);
		m_MessageFilterSet.insert(ErrorLevel);
	}
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::RemoveFromFilterSet(eLogErrorLevel ErrorLevel)
{
	if (IsInFilterSet(ErrorLevel))
	{
		boost::lock_guard<boost::mutex> Lock(m_Mutex);
		m_MessageFilterSet.erase(ErrorLevel);
	}
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::AddLogMessage(const std::string& Message,
								  const std::string& File,
								  const std::string& Function,
								  int LineNo,
								  eLogErrorLevel ErrorLevel)
{
	//only add message to  queue if not filtering it out...
	if (!IsInFilterSet(ErrorLevel))
	{
		//get message time...
		time_t MessageTime;
		time(&MessageTime);

		//build log message, put message on thread queue...
		m_LogMessageQueue.Push(new LogQueueMessage(Message,
												   MessageTime,
												   File,
												   Function,
												   LineNo,
												   static_cast<int>(GetCurrentThreadId()),
												   ErrorLevel));
	}
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::TerminateThread()
{
	//make sure we stop looping in thread function...
	SetRunning(false);

	//put null message on queue so we break out of waiting...
	m_LogMessageQueue.Push(m_NullQueueMsg());

	//wait for thread to finish...
	m_Thread.join();
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::ClearMessageQueue()
{
	//clear log message queue before finishing...
	while(m_LogMessageQueue.Size() > 0)
	{
		boost::scoped_ptr<LogQueueMessage> LogMessage(m_LogMessageQueue.Pop());

		if (LogMessage)
			WriteMessageToLog(BuildLine(std::move(*LogMessage)));
	}
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::SetRunning(bool Running)
{
	boost::lock_guard<boost::mutex> Lock(m_Mutex);
	m_Running = Running;
}
//-----------------------------------------------------------------------------
bool ThreadedLogger::ThreadedLoggerPImpl::GetRunning() const
{
	boost::lock_guard<boost::mutex> Lock(m_Mutex);
	return m_Running;
}
//-----------------------------------------------------------------------------
bool ThreadedLogger::ThreadedLoggerPImpl::GetOutputDebugStringEnabled() const
{
	boost::lock_guard<boost::mutex> Lock(m_Mutex);
	return m_StreamingEnabled;
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::AppendOutputDebugStringStreamID(std::string& stubToAppendTo) const
{
	boost::lock_guard<boost::mutex> Lock(m_Mutex);
	stubToAppendTo += m_StreamIDString;
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::OpenFileOutStream(const std::string& FilePath,
									  bool Truncate)
{
	//return if already exists...
	if (m_ofFileStream)
		return;

	//open filestream in correct mode (append or truncate)...
	if (Truncate)
		m_ofFileStream = fopen(FilePath.c_str(), "w");
	else
		m_ofFileStream = fopen(FilePath.c_str(), "a");

	//local variables...
	eLogErrorLevel ErrorLevel = info;
	time_t MessageTime;

	//generate time stamp...
	time(&MessageTime);

	//add opening line to log...
	WriteMessageToLog(BuildLine(LogQueueMessage("Threaded message log started.",
												MessageTime,
												"",//File
												"",//Function
												-1,//Line No.
												-1,//Thread ID
												info)), false);

	//update message string...
	std::string Message("Software Version ");
	Message += m_SoftwareVersion;

	//add software version string...
	WriteMessageToLog(BuildLine(LogQueueMessage(Message,
												MessageTime,
												"",//File
												"",//Function
												-1,//Line No.
												-1,//Thread ID
												ErrorLevel)), false);
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::CloseFileStream()
{
	fclose(m_ofFileStream);
	m_ofFileStream = NULL;
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::CloseAndFinaliseFile()
{
	//return if doesn't exist...
	if (!m_ofFileStream)
		return;

	//local variables...
	eLogErrorLevel ErrorLevel = info;
	time_t MessageTime;

	//generate time stamp...
	time(&MessageTime);

	//add closing line to log...
	WriteMessageToLog(BuildLine(LogQueueMessage("Threaded message log stopped.",
												MessageTime,
												"",//File
												"",//Function
												-1,//Line No.
												-1,//Thread ID
												ErrorLevel)), false);

	//close stream...
	CloseFileStream();
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::ProcessLogMessageQueue()
{
	try
	{
		while(GetRunning())
		{
			try
			{
				boost::scoped_ptr<LogQueueMessage> LogMessage(m_LogMessageQueue.Pop());

				if (LogMessage)
					WriteMessageToLog(BuildLine(std::move(*LogMessage)));
			}
			catch (boost::thread_interrupted&)
			{
				throw;
			}
			catch (...)
			{
				//can't log here.. so just swallow the exception sadly
			}
		}
	}
    catch (...)
    {
        //almost certainly a boost::thread_interrupted but why discriminate?
	}
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::CheckLogSize()
{
	//return if doesn't exist...
	if (!m_ofFileStream)
		return;

	//what's the current position...
	long currSize = ftell(m_ofFileStream);

	//do we need to switch to a new log file?
	if ((currSize >= static_cast<long>(m_MaxSizeInBytes)) || (currSize < 0))
	{
		//close log and create new log...
		CloseAndFinaliseFile();

	#if !defined(LOG_USE_WIDE_STR)
		//delete existing old file...
		::DeleteFile(m_LogFilePathOld.c_str());

		//rename this version to have .old extension...
		::MoveFile(m_LogFilePathCurrent.c_str(), m_LogFilePathOld.c_str());
	#else
		//make wide string...
		std::wstring OldPathWStr(m_LogFilePathOld.length(), L' ');
		std::copy(m_LogFilePathOld.begin(), m_LogFilePathOld.end(), OldPathWStr.begin());

		//delete existing old file...
		::DeleteFile(OldPathWStr.c_str());

		//make wide string...
		std::wstring CurrPathWStr(m_LogFilePathCurrent.length(), L' ');
		std::copy(m_LogFilePathCurrent.begin(), m_LogFilePathCurrent.end(), CurrPathWStr.begin());

		//rename this version to have .old extension...
		::MoveFile(CurrPathWStr.c_str(), OldPathWStr.c_str());
	#endif

		//recreate current log...
		OpenFileOutStream(m_LogFilePathCurrent, true);
	}
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::WriteMessageToLog(const char* LogMessage,
									                        bool CheckSize)
{
	//stream to debug console...
	WriteMessageToOutputDebugString(LogMessage);

	//check log size...
	if (CheckSize)
		CheckLogSize();

	//write to file if open...
	if (!m_ofFileStream)
		return;

	//write buffer to file stream...
	fputs(LogMessage, m_ofFileStream);
	fputs("\r\n", m_ofFileStream);

	//flush stream straight away (don't wait
	//for close)...
	fflush(m_ofFileStream);
}
//-----------------------------------------------------------------------------
void ThreadedLogger::ThreadedLoggerPImpl::WriteMessageToOutputDebugString(const char* LogMessage)
{
	if (GetOutputDebugStringEnabled())
	{
		std::string StreamIDString = "STREAM ID: ";
		AppendOutputDebugStringStreamID(StreamIDString);
		StreamIDString += ": ";
        StreamIDString += LogMessage;

	#if !defined(LOG_USE_WIDE_STR)
		//stream to error console...
		OutputDebugString(StreamIDString.c_str());
	#else
		//make wide string...
		std::wstring StreamIDWStr(StreamIDString.length(), L' ');
		copy(StreamIDString.begin(), StreamIDString.end(), StreamIDWStr.begin());
		//stream to error console...
		OutputDebugString(StreamIDWStr.c_str());
	#endif
	}
}
//-----------------------------------------------------------------------------
bool ThreadedLogger::ThreadedLoggerPImpl::IsInLevelStringLookup(eLogErrorLevel ErrorLevel) const
{
	return (m_ErrorLevelLookup.find(ErrorLevel) != m_ErrorLevelLookup.end());
}
//-----------------------------------------------------------------------------
const char* ThreadedLogger::ThreadedLoggerPImpl::GetErrorLevelAsString(eLogErrorLevel ErrorLevel)
{
	return IsInLevelStringLookup(ErrorLevel)
		   ? m_ErrorLevelLookup[ErrorLevel].c_str()
		   : UNKNOWN_ERROR_LEVEL_STR;
}
//-----------------------------------------------------------------------------
const char* ThreadedLogger::ThreadedLoggerPImpl::BuildLine(LogQueueMessage&& LogMessage)
{
	//use formatter to build log entry and return result...
	return m_Formatter.FormatString(LogMessage.TimeStamp(),
									LogMessage.Message(),
									LogMessage.File(),
									LogMessage.Function(),
									LogMessage.LineNo(),
									LogMessage.ThreadID(),
									GetErrorLevelAsString(LogMessage.ErrorLevel()));
}
//-----------------------------------------------------------------------------
bool ThreadedLogger::ThreadedLoggerPImpl::IsInFilterSet(eLogErrorLevel ErrorLevel) const
{
	boost::lock_guard<boost::mutex> Lock(m_Mutex);
	return (m_MessageFilterSet.find(ErrorLevel) != m_MessageFilterSet.end());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ThreadedLogger::ThreadedLogger(LogFormatBase& Formatter,                   
			  const std::string& LogFolderPath,				
			  const std::string& LogFileName,				
			  const std::string& SoftwareVersion,			
			  const std::string& StreamIDString,			
			  bool EnableOutputDebugString,	        
			  size_t MaxSizeInBytes)
	: m_PImpl(boost::make_shared< ThreadedLoggerPImpl >(Formatter, LogFolderPath, LogFileName,
													   SoftwareVersion, StreamIDString,
													    EnableOutputDebugString, MaxSizeInBytes))
{
}
//-----------------------------------------------------------------------------
ThreadedLogger::~ThreadedLogger()
{
}
//-----------------------------------------------------------------------------
void ThreadedLogger::SetOutputDebugStringEnabled(bool EnableStreaming)
{
	m_PImpl->SetOutputDebugStringEnabled(EnableStreaming);
}
//-----------------------------------------------------------------------------
void ThreadedLogger::AddToFilterSet(eLogErrorLevel ErrorLevel)
{
	m_PImpl->AddToFilterSet(ErrorLevel);
}
//-----------------------------------------------------------------------------
void ThreadedLogger::RemoveFromFilterSet(eLogErrorLevel ErrorLevel)
{
	m_PImpl->RemoveFromFilterSet(ErrorLevel);
}
//-----------------------------------------------------------------------------
void ThreadedLogger::AddLogMessage(const std::string& Message,
				   const std::string& File,     	
				   const std::string& Function, 	
				   int LineNo,           	       
				   eLogErrorLevel ErrorLevel)
{
	m_PImpl->AddLogMessage(Message, File, Function, LineNo, ErrorLevel);
}
//-----------------------------------------------------------------------------
// GLOBAL ACCESS BITS AND PIECES...
//-----------------------------------------------------------------------------
static shared_log_vector_ptr g_LogVectorPtr;
//-----------------------------------------------------------------------------
void RegisterLogVectorForGlobalAccess(shared_log_vector_ptr& LogVector)
{
	g_LogVectorPtr = LogVector;
}
//-----------------------------------------------------------------------------
int RegisterLogForGlobalAccess(shared_log_ptr& ThreadedLog)
{
	int Index = -1;
	if (g_LogVectorPtr)
	{
		g_LogVectorPtr->push_back(ThreadedLog);
		Index = g_LogVectorPtr->size() - 1;
	}
	//return this log's index in the log vector...
	return Index;
}
//-----------------------------------------------------------------------------
void ResetGlobalLogVector()
{
	g_LogVectorPtr.reset();
}
//-----------------------------------------------------------------------------
void SetOutputDebugStringEnabled(bool Enable,
								 size_t LogIndex)
{
	if (g_LogVectorPtr &&
		(LogIndex < g_LogVectorPtr->size()))
	{
		(*g_LogVectorPtr)[LogIndex]->SetOutputDebugStringEnabled(Enable);
	}
}
//-----------------------------------------------------------------------------
void AddToFilterSet(eLogErrorLevel ErrorLevel,
					size_t LogIndex)
{
	if (g_LogVectorPtr &&
		(LogIndex < g_LogVectorPtr->size()))
	{
		(*g_LogVectorPtr)[LogIndex]->AddToFilterSet(ErrorLevel);
	}
}
//-----------------------------------------------------------------------------
void RemoveFromFilterSet(eLogErrorLevel ErrorLevel,
						 size_t LogIndex)
{
	if (g_LogVectorPtr &&
		(LogIndex < g_LogVectorPtr->size()))
	{
		(*g_LogVectorPtr)[LogIndex]->RemoveFromFilterSet(ErrorLevel);
	}
}
//-----------------------------------------------------------------------------
void MessageToLog(const std::string& Message,
				  const std::string& File,
				  const std::string& Function,
				  int LineNo,
				  eLogErrorLevel ErrorLevel,
				  size_t LogIndex)
{
	if (g_LogVectorPtr &&
		(LogIndex < g_LogVectorPtr->size()))
	{
		(*g_LogVectorPtr)[LogIndex]->AddLogMessage(Message,
												File,
												Function,
												LineNo,
											    ErrorLevel);
	}
}
//-----------------------------------------------------------------------------
} // namespace message_log
//-----------------------------------------------------------------------------
} // namespace core_lib
//-----------------------------------------------------------------------------
