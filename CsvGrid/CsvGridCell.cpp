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
 * \file CsvGridCell.cpp
 * \brief File containing definitions relating the CSVGridCell class.
 */

#include "CsvGridCell.hpp"
#include "../StringUtils.hpp"

namespace core_lib {
namespace csv_grid {

// ****************************************************************************
// 'class Cell' definition
// ****************************************************************************
Cell::Cell(const std::string& data)
    : m_value{data}
{
}

Cell::Cell(int32_t value)
    : m_value{std::to_string(value)}
{
}

Cell::Cell(int64_t value)
    : m_value{std::to_string(value)}
{
}

Cell::Cell(double value)
{
    m_value = string_utils::FormatFloatString(value);
}

Cell::Cell(long double value)
{
    m_value = string_utils::FormatFloatString(value, 30);
}

Cell& Cell::operator=(const std::string& rhs)
{
    m_value = rhs;
    return *this;
}

Cell& Cell::operator=(int32_t rhs)
{
    m_value = std::to_string(rhs);
    return *this;
}

Cell& Cell::operator=(int64_t rhs)
{
    m_value = std::to_string(rhs);
    return *this;
}

Cell& Cell::operator=(double rhs)
{
    m_value = string_utils::FormatFloatString(rhs);
    return *this;
}

Cell& Cell::operator=(long double rhs)
{
    m_value = string_utils::FormatFloatString(rhs, 30);
    return *this;
}

Cell::operator std::string() const
{
    return m_value;
}

Cell::operator int32_t() const
{
    return std::stoi(m_value);
}

Cell::operator int64_t() const
{
    return std::stoll(m_value);
}

Cell::operator double() const
{
    return std::stod(m_value);
}

Cell::operator long double() const
{
    return std::stold(m_value);
}

int32_t Cell::ToInt32Def(int32_t defval) const noexcept
{
    int32_t val;

    try
    {
        val = std::stoi(m_value);
    }
    catch(...)
    {
        val = defval;
    }

    return val;
}

int64_t Cell::ToInt64Def(int64_t defval) const noexcept
{
    int64_t val;

    try
    {
        val = std::stoll(m_value);
    }
    catch(...)
    {
        val = defval;
    }

    return val;
}


double Cell::ToDoubleDef(double defval) const noexcept
{
    double val;

    try
    {
        val = std::stod(m_value);
    }
    catch(...)
    {
        val = defval;
    }

    return val;
}

long double Cell::ToLongDoubleDef(long double defval) const noexcept
{
    long double val;

    try
    {
        val = std::stold(m_value);
    }
    catch(...)
    {
        val = defval;
    }

    return val;
}

} // namespace csv_grid
} // namespace core_lib