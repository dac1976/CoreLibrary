#ifndef LOGFORMATTERS_HPP
#define LOGFORMATTERS_HPP
//-----------------------------------------------------------------------------
/*
 NOTES:
 ------
 This file contains formatting classes for use with the ThreadedLogger class.

 Add more formatter derived classes as required but each must provide a
 FormatString function.
 */
//-----------------------------------------------------------------------------
#include <vector>
#include <ctime>
//-----------------------------------------------------------------------------
namespace core_lib {
//-----------------------------------------------------------------------------
namespace message_log {
//-----------------------------------------------------------------------------
//abstract formatter base class...
class LogFormatBase
{
public:
    LogFormatBase();
    virtual ~LogFormatBase();

    virtual const char* FormatString(time_t TimeStamp,
                                     const char* Message,   //null terminated
            const char* File,      //null terminated
            const char* Function,  //null terminated
            int LineNo, int ThreadID, const char* ErrorLevel //null terminated
            ) = 0;

protected:
    class TimeAsString
    {
    public:
        TimeAsString();
        const char* FormatString(time_t timeStamp);

    private:
        const size_t DEFAULT_TIME_LEN;
        std::vector<char> m_timeBuffer;
    };

    TimeAsString m_timeAsString;
    std::vector<char> m_buffer;
    std::vector<char>::iterator m_writePos;

    void AddToBuffer(const char* charsToAdd);
    void AddNullTerminatorToBuffer();

private:
    const size_t DEFAULT_LINE_LEN;
};
//-----------------------------------------------------------------------------
//default formatter functor...
class DefaultLogFormat : public LogFormatBase
{
public:
    DefaultLogFormat();
    virtual ~DefaultLogFormat();

    const char* FormatString(time_t TimeStamp, const char* Message, //null terminated
                             const char* File,      //null terminated
                             const char* Function,  //null terminated
                             int LineNo, int ThreadID, const char* ErrorLevel //null terminated
                             );
};
//-----------------------------------------------------------------------------
//a less formal easier to read formatter functor...
class InformalLogFormat : public LogFormatBase
{
public:
    InformalLogFormat();
    virtual ~InformalLogFormat();

    const char* FormatString(time_t TimeStamp, const char* Message, //null terminated
                             const char* File,      //null terminated
                             const char* Function,  //null terminated 
                             int LineNo, int ThreadID, const char* ErrorLevel //null terminated
                             );
};

//-----------------------------------------------------------------------------
//NOTE: Add more functors here for different formatting. Each functor must
//      specify an FormatString() with the same args as the default one above...
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
}//namespace message_log
//-----------------------------------------------------------------------------
} //namespace core_lib
//-----------------------------------------------------------------------------
#endif
