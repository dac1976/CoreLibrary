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
 * \file CsvGridMain.cpp
 * \brief File containing definitions relating the TCsvGrid class.
 */

#include "CsvGridMain.hpp"
#include "../StringUtils.hpp"

namespace core_lib {
namespace csv_grid {

// ****************************************************************************
// 'class xCsvGridDimensionError' definition
// ****************************************************************************
xCsvGridDimensionError::xCsvGridDimensionError()
	: exceptions::xCustomException("rows and cols must be > 0")
{
}

xCsvGridDimensionError::xCsvGridDimensionError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xCsvGridDimensionError::~xCsvGridDimensionError()
{
}

// ****************************************************************************
// 'class xCsvGridRowOutOfRangeError' definition
// ****************************************************************************
xCsvGridRowOutOfRangeError::xCsvGridRowOutOfRangeError()
	: exceptions::xCustomException("invalid row index")
{
}

xCsvGridRowOutOfRangeError::xCsvGridRowOutOfRangeError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xCsvGridRowOutOfRangeError::~xCsvGridRowOutOfRangeError()
{
}

// ****************************************************************************
// 'class xCsvGridCreateFileStreamError' definition
// ****************************************************************************
xCsvGridCreateFileStreamError::xCsvGridCreateFileStreamError()
	: exceptions::xCustomException("failed to create file stream")
{
}

xCsvGridCreateFileStreamError::xCsvGridCreateFileStreamError(const std::string& message)
	: exceptions::xCustomException(message)
{
}

xCsvGridCreateFileStreamError::~xCsvGridCreateFileStreamError()
{
}

} // namespace csv_grid
} // namespace core_lib
