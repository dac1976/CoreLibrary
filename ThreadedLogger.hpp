#ifndef THREADEDLOGGER_HPP
#define THREADEDLOGGER_HPP
//-----------------------------------------------------------------------------
/*
 NOTES:
 ------

 The ThreadedLogger which is the message logging class can have different formatting
 defined for it. This is done by defining a new format functor in LogFormatters.h.
 There are two format functors already provided but the user can add more if the wish
 to. The only restriction is that the functor's operator() implementation must
 take the same args as the formatter functors already defined.

 We can use either a single log object in an app or have multiple log objects in
 an app. To make managing multiple logs easier the user can use the supplied
 global functions below the ThreadedLogger class declaration. Essentially we
 keep a vector of boost::shared_ptrs to the different log objects and can
 access the different logs using the logs correct index into the vector
 using the previously mentioned functions.

 The ThreadedLogger class supports streaming the log messages to an error console
 as well as writing them to a file. There is a constructor arg for enabling this,
 but it can also be enabled/disabled at any time using SetOutputDebugStringEnabled().
 If the user is using a widestring version of the function OutputDebugString() then
 they need to conditionally define LOG_USE_WIDE_STR in their project.

 The user can associate an error level with a log message (it's one of the args
 passed in with the message string). If specified the error level gets
 appended to the front of the line in the log so the user can see the severity
 of the log message when viewing the file later. However, sometimnes the user
 may not want experimental trace or debug messages appearing in the log
 but do not want to remove them from the code in case they are needed again.
 In this case the user can call AddToFilterSet() to add an error level to the filter,
 whereby any messages with error levels in the filter do not get output to file or
 the error console. The user can remove an error level from the filter set calling
 emoveFromFilterSet().

 Here's an example of how to create and use the ThreadedLogger class in your
 apps (note we make use of typedefs defined below ThreadedLogger class):

 [[[ imagine this is the .h file of your apps main form ]]]

 #include "ThreadedLogger.h"

 class MyForm : public TForm
 {
 __published:

 __private:
 message_log::DefaultLogFormat m_Formatter; 		//use default formatting
 message_log::shared_log_vector_ptr m_LogVector; //we need a vector to hold our log objects
 message_log::shared_log_ptr m_ThreadedLog;      //our app's main log

 void MysteryFunction()
 {
 //do some stuff here...
 }

 __public:
 __fastcall MyForm(TComponent* Owner);
 };

 [[[ imagine this is the .cpp file of your apps main form ]]]

 using namespace std;
 using namespace message_log;

 __fastcall MyForm::MyForm(TComponent* Owner)
 : m_LogVector(new log_vector) //create the vector we'll register to globally hold our log objects
 {
 //first things first work out the path we want for our log...

 string LogFolderPath(AnsiString(ExtractFilePath(Application->ExeName)).c_str());
 AnsiString FileName(ExtractFileName(Application->ExeName));
 FileName = FileName.SubString(1, FileName.LastDelimiter(".") - 1);
 string LogFileName(FileName.c_str());
 string SoftwareVersion = "1.0.0.0";

 //let's create our threaded log object before doing
 //anything else in our app...

 bool StreamDebug = false;
 #if defined(_DEBUG)
 StreamDebug = true;
 #endif
 m_ThreadedLog.reset(new ThreadedLogger(m_Formatter,             //pass a ref to our formatter
 LogFolderPath,           //folder where log will be created inc trailing backslash
 LogFileName,             //name of log file, without extension
 SoftwareVersion,         //a software version string (wil get printed at start of each new log)
 "MY TEST APP",           //name of app so we can ID messages in error console
 StreamDebug,             //turn on streaming to error console
 10 * MEGABYTE_IN_BYTES));//set the size of log, when size is reach log is renamed.OLD and new log created

 //register log object for global use via functions MessageToLog
 //and PerformanceToLog or macros (we must register our vector as
 //well as the log)...

 RegisterLogVectorForGlobalAccess(m_LogVector);
 int LogIndex = RegisterLogForGlobalAccess(m_ThreadedLog);

 //here are some examples of how to print to log...

 //directly:
 m_ThreadedLog->AddLogMessage("Test Message 1",
 std::string(__FILE__),
 std::string(__FUNC__),
 __LINE__
 info);

 //using global functions:
 MessageToLog("Test Message 2",
 std::string(__FILE__),
 std::string(__FUNC__),
 __LINE__
 info);  				//print message to log

 MessageToLog("Test Message 3",
 std::string(__FILE__),
 std::string(__FUNC__),
 __LINE__
 info,
 LogIndex);  	   		//print message to log at index=LogIndex

 //using macros:
 DEBUG_MESSAGE("Test Message: " << 4);   			 //using stream operator to build message string
 DEBUG_MESSAGE("Test Message: " + 5);            	 //using + opertor to build message string
 DEBUG_MESSAGE_L("Test Message: 6", info);       	 //putting message into log with a particular error level
 DEBUG_MESSAGE_LI("Test Message: 7", info, LogIndex); //putting message into log at index=LogIndex with a particular error level
 DEBUG_MESSAGE_MI("Test Message: 8", LogIndex);       //putting message into log at index=LogIndex
 }

 */
//-----------------------------------------------------------------------------
#include <memory>
#include <vector>
#include <sstream>//for our logging macros at the bottom of this file
#include <string>
#include <boost/current_function.hpp>
#include "Logger/LogFormatters.hpp"
#include "Logger/MessageLogErrors.hpp"
//-----------------------------------------------------------------------------
namespace core_lib {
//-----------------------------------------------------------------------------
namespace message_log {
//-----------------------------------------------------------------------------
class ThreadedLogger
{
public:
    static const size_t MEGABYTE_IN_BYTES;

    //constructor...
    ThreadedLogger(LogFormatBase& Formatter, /*log formatter object*/
                   const std::string& LogFolderPath, /*folder path with trailing backslash*/
                   const std::string& LogFileName, /*no extension*/
                   const std::string& SoftwareVersion, /*e.g. "1.0.0.1"*/
                   const std::string& StreamIDString, /*e.g. "HAWK CONTROLLER"*/
                   bool EnableOutputDebugString = false, /*stream output to console as well as file */
                   size_t MaxSizeInBytes = (5 * MEGABYTE_IN_BYTES));/*max log file size in bytes*/
    //destructor...
    ~ThreadedLogger();
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
    void AddLogMessage(const std::string& Message, const std::string& File = "", /*e.g. std::string(__FILE__)*/
                       const std::string& Function = "", /*e.g. std::string(__FUNC__)*/
                       int LineNo = -1, /*e.g.  __LINE__*/
                       eLogErrorLevel ErrorLevel = not_defined);

private:
    //member variables...
    class ThreadedLoggerPImpl;
    std::shared_ptr<ThreadedLoggerPImpl> m_PImpl;

    //disbale copying...
    ThreadedLogger(const ThreadedLogger&);
    ThreadedLogger& operator=(const ThreadedLogger);
};
//-----------------------------------------------------------------------------
// GLOBAL ACCESS BITS AND PIECES
//-----------------------------------------------------------------------------
typedef boost::shared_ptr<ThreadedLogger> shared_log_ptr;
typedef std::vector<shared_log_ptr> log_ptr_vector;
typedef boost::shared_ptr<log_ptr_vector> shared_log_vector_ptr;
//-----------------------------------------------------------------------------
//THIS FUNCTION MUST BE CALLED FIRST TO REGISTER A VECTOR TO HOLD OUR LOGS
//EVEN IF YOU ONLY WANT TO USE ONE LOG BECUASE THE GLOBAL HELPER FUNCTIONS
//ARE DESIGNED TO HELP WHEN YOU WANT MULTIPLE LOGS IN YOUR APP...
//
//register a vector to hold our logs...
void RegisterLogVectorForGlobalAccess(shared_log_vector_ptr& LogVector);
//-----------------------------------------------------------------------------
//global function to add log to global vector of our app's threaded log
//objects so we can implicitly access it when we call DebugMessage below to
//print debug to it and returns logs index in the vector (-1 if there is an error)...
int RegisterLogForGlobalAccess(shared_log_ptr& ThreadedLog);
//-----------------------------------------------------------------------------
//call to reset global log vector...
void ResetGlobalLogVector();
//-----------------------------------------------------------------------------
//enable/disable streaming messages to console as well as file for a
//particular log...
void SetOutputDebugStringEnabled(bool Enable, size_t LogIndex = 0);
//-----------------------------------------------------------------------------
//add error levels to the global log's filter set to stop messages
//of those levels being output for a particular log...
void AddToFilterSet(eLogErrorLevel ErrorLevel, size_t LogIndex = 0);
//-----------------------------------------------------------------------------
//remove error levels from the global log's filter set to cause messages
//of those levels to be output for a particular log...
void RemoveFromFilterSet(eLogErrorLevel ErrorLevel, size_t LogIndex = 0);
//-----------------------------------------------------------------------------
//Global function to write debug (with error level) using global log object
//held in g_ThreadedLog if it has been created for a particular log...
void MessageToLog(const std::string& Message, const std::string& File, /* = std::string(__FILE__)*/
                  const std::string& Function, /* = std::string(__FUNC__)*/
                  int LineNo, /* = __LINE__*/
                  eLogErrorLevel ErrorLevel = not_defined, size_t LogIndex = 0);
//-----------------------------------------------------------------------------
}// namespace message_log
//-----------------------------------------------------------------------------
} // namespace core_lib
//-----------------------------------------------------------------------------
// macro equivalents of MessageToLog function...
//-----------------------------------------------------------------------------
#if defined (DO_NOT_PERFORM_ANY_LOGGING)
#define DEBUG_MESSAGE(m)                0
#define DEBUG_MESSAGE_L(m, l)           0
#define DEBUG_MESSAGE_LI(m, l, i)       0
#define DEBUG_MESSAGE_EL(m, e, l)       0
#define DEBUG_MESSAGE_I(m, i)           0
#else
//print a string to the default log (the log must have still been setup using
//RegisterLogVectorForGlobalAccess and RegisterLogForGlobalAccess)...
#define DEBUG_MESSAGE(m)  do {                                                     \
								   std::ostringstream os;                              \
								   os << m;    				                           \
								   core_lib::message_log::MessageToLog(os.str(),                 \
															 std::string(__FILE__),    \
															 std::string(BOOST_CURRENT_FUNCTION), \
															 __LINE__,                 \
															 core_lib::message_log::::not_defined, \
															 0);                       \
							   } while(false)
//print a string and error level (from enum eLogErrorLevel)
//to the default log (the log must have still been setup using
//RegisterLogVectorForGlobalAccess and RegisterLogForGlobalAccess)...
#define DEBUG_MESSAGE_L(m, l) do { 								                    \
									   std::ostringstream os;                           \
									   os << m;    				                        \
									   core_lib::message_log::::MessageToLog(os.str(),              \
																 std::string(__FILE__), \
																 std::string(BOOST_CURRENT_FUNCTION), \
																 __LINE__,              \
																 l,                     \
																 0);                    \
								  } while(false)
//print a string and error level to a particular log, as
//setup using the RegisterLogVectorForGlobalAccess and
//RegisterLogForGlobalAccess functions defined above...
#define DEBUG_MESSAGE_LI(m, l, i) do { 								                    \
										   std::ostringstream os;                           \
										   os << m;    				                        \
										   core_lib::message_log::::MessageToLog(os.str(),              \
																	 std::string(__FILE__), \
																	 std::string(BOOST_CURRENT_FUNCTION), \
																	 __LINE__,              \
																	 l,                     \
																	 i);                    \
									  } while(false)
//print a string, event (ignored by ThreadedLogger, macro defined for legacy
//compatibility) and error level (from enum eLogErrorLevel) to the default log
//(the log must have still been setup using RegisterLogVectorForGlobalAccess
//and RegisterLogForGlobalAccess)...
#define DEBUG_MESSAGE_EL(m, e, l)    do { 								                    \
											   std::ostringstream os;                           \
											   os << m;    				                        \
											   core_lib::message_log::::MessageToLog(os.str(),              \
																		 std::string(__FILE__), \
																		 std::string(BOOST_CURRENT_FUNCTION), \
																		 __LINE__,              \
																		 l,                     \
																		 0);                    \
										  } while(false)

//print a string to a particular log as setup using the
//RegisterLogVectorForGlobalAccess and RegisterLogForGlobalAccess
//functions defined above...
#define DEBUG_MESSAGE_I(m, i) do { 						                               \
										std::ostringstream os;                             \
										os << m;    				                       \
										core_lib::message_log::::MessageToLog(os.str(),                \
																  std::string(__FILE__),   \
																  std::string(BOOST_CURRENT_FUNCTION),   \
																  __LINE__,                \
																  core_lib::message_log::::not_defined,\
																  i);                      \
									  } while(false)
#endif
//-----------------------------------------------------------------------------
#endif
