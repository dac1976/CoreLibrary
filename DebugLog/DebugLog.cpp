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
 * \file DebugLog.cpp
 * \brief File containing definition of DebugLog class.
 */

#include "DebugLog.hpp"
#include <utility>

namespace core_lib {
namespace log {

// ****************************************************************************
// 'class xMsgHandlerError' definition
// ******************************B*********************************************
xLogMsgHandlerError::xLogMsgHandlerError()
	: exceptions::xCustomException("log message handler error")
{
}

xLogMsgHandlerError::xLogMsgHandlerError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xLogMsgHandlerError::~xLogMsgHandlerError()
{
}

// ****************************************************************************
// 'class xInstantiationrError' definition
// ******************************B*********************************************
xInstantiationrError::xInstantiationrError()
	: exceptions::xCustomException("instantiation error")
{
}

xInstantiationrError::xInstantiationrError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xInstantiationrError::~xInstantiationrError()
{
}


// ****************************************************************************
// 'struct DefaultLogFormat' definition
// ****************************************************************************
void DefaultLogFormat::operator() (std::ostream& os
								   , std::time_t timeStamp
								   , const std::string& message
								   , const std::string& logMsgLevel
								   , const std::string& file
								   , int lineNo
								   , const std::thread::id& threadID) const
{
	if (timeStamp != 0)
	{
		// Should use lines below but not necessarily implemented
		// yet in some compilers:
		//     struct std::tm * ptm = std::localtime(&timeStamp);
		//     os << "\t" << std::put_time(ptm,"%F %T");
		// so instead we use...
		std::string time{ctime(&timeStamp)};
		std::replace_if(time.begin(), time.end(),
						[](char c) { return (c == '\n') || (c == '\r'); }, 0);
		os << "< " << time.c_str() << " >";
	}

	os << "< " << message << " >";

	if (logMsgLevel != "")
	{
		os << "< " << logMsgLevel << " >";
	}

	if (file != "")
	{
		os << "< File = " << file << " >";
	}

	if (lineNo >= 0)
	{
		os << "< Line = " << lineNo << " >";
	}

	std::thread::id noThread;

	if (threadID != noThread)
	{
		os << "< Thread ID = " << threadID << " >";
	}

	os << std::endl;
}

} // namespace log
} // namespace core_lib
