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
 * \file CsvGridCellDouble.cpp
 * \brief File containing definitions relating the CellDouble class.
 */

#ifdef USE_EXPLICIT_MOVE_
#include <utility>
#endif
#include "CsvGrid/CsvGridCellDouble.h"
#include "StringUtils/StringUtils.h"

namespace core_lib
{
namespace csv_grid
{

// ****************************************************************************
// 'class CellDouble' definition
// ****************************************************************************

#ifdef USE_EXPLICIT_MOVE_
CellDouble::CellDouble(CellDouble&& cell)
{
    *this = std::move(cell);
}

CellDouble& CellDouble::operator=(CellDouble&& cell)
{
    std::swap(m_value, cell.m_value);
    return *this;
}
#endif

CellDouble::CellDouble(const double value)
    : m_value(value)
{
}

CellDouble::CellDouble(const std::string& value)
    : m_value(std::stod(value))
{
}

CellDouble& CellDouble::operator=(const double rhs)
{
    m_value = rhs;
    return *this;
}

CellDouble& CellDouble::operator=(const std::string& rhs)
{
    m_value = std::stod(rhs);
    return *this;
}

double CellDouble::Value() const
{
    return m_value;
}

CellDouble::operator std::string() const
{
    return string_utils::FormatFloatString(m_value);
}

CellDouble::operator double() const
{
    return m_value;
}

} // namespace csv_grid
} // namespace core_lib
