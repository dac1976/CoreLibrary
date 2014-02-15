//-----------------------------------------------------------------------------
#include "LogFormatters.hpp"
#include <cstring>
#include <cstdio>
#include <algorithm>
//-----------------------------------------------------------------------------
namespace core_lib {
//-----------------------------------------------------------------------------
namespace message_log {
//-----------------------------------------------------------------------------
static bool IsNewLineOrCarraigeReturnChar(char c)
{
    return (c == '\n') || (c == '\r');
}
//-----------------------------------------------------------------------------
LogFormatBase::TimeAsString::TimeAsString()
        : //fixed buf length of 26 chars as per c standard for ctime
        DEFAULT_TIME_LEN(26),
          m_timeBuffer(DEFAULT_TIME_LEN, 0)
{
}
//-----------------------------------------------------------------------------
const char* LogFormatBase::TimeAsString::FormatString(time_t timeStamp)
{
    //copy into string...
    strncpy(&m_timeBuffer.front(), ctime(&timeStamp),
            std::min(m_timeBuffer.size(), DEFAULT_TIME_LEN));

    //if no errors remove the annoying penultimate '\n' in
    //the time char string converting it to null...
    std::replace_if(m_timeBuffer.begin(), m_timeBuffer.end(),
            IsNewLineOrCarraigeReturnChar, 0);

    return &m_timeBuffer.front();
}
//-----------------------------------------------------------------------------
LogFormatBase::LogFormatBase()
        : //default fixed line length should be long enough for most scenarios
        DEFAULT_LINE_LEN(1024)
{
    //reserve some space but not fully create it yet...
    m_buffer.reserve(DEFAULT_LINE_LEN);

    //initialise write pos, at this point m_buffer.begin()
    //and m_buffer.end() are the same...
    m_writePos = m_buffer.begin();
}
//-----------------------------------------------------------------------------
LogFormatBase::~LogFormatBase()
{
}
//-----------------------------------------------------------------------------
void LogFormatBase::AddToBuffer(const char* charsToAdd)
{
    if (charsToAdd)
    {
        const size_t strLen = strlen(charsToAdd);

        //do we have space already?
        if (m_writePos + strLen <= m_buffer.end())
        {
            //already have memory allocated for vector so
            //copy into it in the correct position...
            std::copy(charsToAdd, charsToAdd + strLen, m_writePos);

            //increment write position for next time...
            m_writePos += strLen;
        }
        else
        {
            //work out how much to insert and how much to copy...
            const size_t numToCopy = m_buffer.end() - m_writePos;
            const size_t numToInsert = strLen - numToCopy;

            if (numToCopy > 0)
            {
                //already have memory allocated for vector so
                //copy into it in the correct position...
                std::copy(charsToAdd, charsToAdd + numToCopy, m_writePos);

                //increment write position for next time...
                m_writePos += numToCopy;
            }

            if (numToInsert > 0)
            {
                //this will only create new memory once a line
                //length goes beyond DEFAULT_LINE_LEN (= 1KB),
                //which is the initial reserved amount...
                m_buffer.insert(m_writePos, charsToAdd + numToCopy,
                        charsToAdd + strLen);

                //increment write position for next time...
                m_writePos = m_buffer.end();
            }
        }
    }
}
//-----------------------------------------------------------------------------
void LogFormatBase::AddNullTerminatorToBuffer()
{
    //do we have space already?
    if (m_writePos < m_buffer.end())
    {
        //already have memory allocated for vector so
        //set value in the write pos to 0...
        *m_writePos = 0;
    }
    else
    {
        //this will only create new memory once a line
        //length goes beyond DEFAULT_LINE_LEN (= 1KB),
        //which is the initial reserved ammount, so
        //just insert a 0 at the write pos...
        m_writePos = m_buffer.insert(m_writePos, 0);
    }

    //increment write position for next time...
    ++m_writePos;

}
//-----------------------------------------------------------------------------
DefaultLogFormat::DefaultLogFormat()
        : LogFormatBase()
{
}
//-----------------------------------------------------------------------------
DefaultLogFormat::~DefaultLogFormat()
{
}
//-----------------------------------------------------------------------------
const char* DefaultLogFormat::FormatString(time_t TimeStamp,
                                           const char* Message,
                                           const char* File,
                                           const char* Function, int LineNo,
                                           int ThreadID, const char* ErrorLevel)
{
    //set write position iterator to beginning of vector...
    m_writePos = m_buffer.begin();

    //add error level...
    if (strcmp(ErrorLevel, "") != 0)
    {
        //add to buffer...
        AddToBuffer("<LEVEL: ");
        AddToBuffer(ErrorLevel);
        AddToBuffer("> ");
    }

    //add time portion...
    AddToBuffer("<TIME: ");
    AddToBuffer(m_timeAsString.FormatString(TimeStamp));
    AddToBuffer("> ");

    //add message portion...
    AddToBuffer("<MESSAGE: \"");
    AddToBuffer(Message);
    AddToBuffer("\">");

    //add file path portion...
    if (strcmp(File, "") != 0)
    {
        AddToBuffer(" <FILE: \"");
        AddToBuffer(File);
        AddToBuffer("\">");
    }

    //add function name portion...
    if (strcmp(Function, "") != 0)
    {
        AddToBuffer(" <FUNCTION: \"");
        AddToBuffer(Function);
        AddToBuffer("\">");
    }

    //temp array for numbers...
    char tempValue[32];

    //add line number portion...
    if (LineNo != -1)
    {
        AddToBuffer(" <LINE: ");

        if (sprintf(tempValue, "%d", LineNo) > 0) AddToBuffer(tempValue);
        else AddToBuffer("!!!CANNOT CONVERT LINE NO. TO STRING!!!");

        AddToBuffer(">");
    }

    //add thread ID portion...
    if (ThreadID != -1)
    {
        AddToBuffer(" <THREAD ID: ");

        if (sprintf(tempValue, "%d", ThreadID) > 0) AddToBuffer(tempValue);
        else AddToBuffer("!!!CANNOT CONVERT THREAD ID TO STRING!!!");

        AddToBuffer(">");
    }

    //null terminate our char buffer...
    AddNullTerminatorToBuffer();

    //return line...
    return &m_buffer[0];
}
//-----------------------------------------------------------------------------
InformalLogFormat::InformalLogFormat()
        : LogFormatBase()
{
}
//-----------------------------------------------------------------------------
InformalLogFormat::~InformalLogFormat()
{
}
//-----------------------------------------------------------------------------
const char* InformalLogFormat::FormatString(time_t TimeStamp,
                                            const char* Message,
                                            const char* File,
                                            const char* Function, int LineNo,
                                            int ThreadID,
                                            const char* ErrorLevel)
{
    //set write position iterator to beginning of vector...
    m_writePos = m_buffer.begin();

    //add error level...
    if (strcmp(ErrorLevel, "") != 0)
    {
        AddToBuffer(ErrorLevel);
        AddToBuffer("    ");
    }

    //add time portion...
    AddToBuffer(m_timeAsString.FormatString(TimeStamp));
    AddToBuffer("    ");

    //add message portion...
    AddToBuffer("\"");
    AddToBuffer(Message);
    AddToBuffer("\"    (");
    bool commaRequired = false;

    //add file path portion...
    if (strcmp(File, "") != 0)
    {
        AddToBuffer("\"");
        AddToBuffer(File);
        AddToBuffer("\"");
        commaRequired = true;
    }

    //add function name portion...
    if (strcmp(Function, "") != 0)
    {
        if (commaRequired) AddToBuffer(", \"");
        else AddToBuffer("\"");

        AddToBuffer(Function);
        AddToBuffer("\"");
        commaRequired = true;
    }

    //temp array for numbers...
    char tempValue[32];

    //add line number portion...
    if (LineNo != -1)
    {
        if (commaRequired) AddToBuffer(", LINE = ");
        else AddToBuffer("LINE = ");

        if (sprintf(tempValue, "%d", LineNo) > 0) AddToBuffer(tempValue);
        else AddToBuffer("!!!CANNOT CONVERT LINE NO. TO STRING!!!");

        commaRequired = true;
    }

    //add thread ID portion...
    if (ThreadID != -1)
    {
        if (commaRequired) AddToBuffer(", THREAD ID = ");
        else AddToBuffer("THREAD ID = ");

        if (sprintf(tempValue, "%d", ThreadID) > 0) AddToBuffer(tempValue);
        else AddToBuffer("!!!CANNOT CONVERT THREAD ID TO STRING!!!");
    }

    //close bracket...
    AddToBuffer(")");

    //null terminate our char buffer...
    AddNullTerminatorToBuffer();

    //return line...
    return &m_buffer[0];
}
//-----------------------------------------------------------------------------
}//namespace message_log
//-----------------------------------------------------------------------------
} //namespace core_lib
//-----------------------------------------------------------------------------
