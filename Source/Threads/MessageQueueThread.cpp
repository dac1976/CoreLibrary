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
 * \file MessageQueueThread.cpp
 * \brief File containing definition of MessageQueueThread class.
 */

#include "../../Include/Threads/MessageQueueThread.hpp"

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xMsgHandlerError' definition
// ******************************B**********************************************
xMsgHandlerError::xMsgHandlerError()
	: exceptions::xCustomException("message handler error")
{
}

xMsgHandlerError::xMsgHandlerError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xMsgHandlerError::~xMsgHandlerError()
{
}

} // namespace threads
} // namespace core_lib