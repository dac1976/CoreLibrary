// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2015 Duncan Crutchley
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
 * \file ConcurrentQueue.cpp
 * \brief File containing concurrent queue definition.
 */

#include "Threads/ConcurrentQueue.h"

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xQueuePopTimeoutError' definition
// ****************************************************************************
xQueuePopTimeoutError::xQueuePopTimeoutError()
	: exceptions::xCustomException("pop timeout")
{
}

xQueuePopTimeoutError::xQueuePopTimeoutError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xQueuePopTimeoutError::~xQueuePopTimeoutError()
{
}

// ****************************************************************************
// 'class xQueuePopQueueEmptyError' definition
// ****************************************************************************
xQueuePopQueueEmptyError::xQueuePopQueueEmptyError()
	: exceptions::xCustomException("pop queue empty")
{
}

xQueuePopQueueEmptyError::xQueuePopQueueEmptyError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xQueuePopQueueEmptyError::~xQueuePopQueueEmptyError()
{
}

} // namespace threads
} // namespace core_lib
