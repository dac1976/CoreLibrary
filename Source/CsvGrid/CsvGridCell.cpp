// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
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
 * \brief File containing definitions relating the Cell class.
 */

#ifdef USE_EXPLICIT_MOVE_
#include <utility>
#endif
#include "CsvGrid/CsvGridCell.h"
#include "StringUtils/StringUtils.h"
#include "boost/algorithm/string/trim.hpp"

namespace core_lib
{
namespace csv_grid
{

// ****************************************************************************
// 'class Cell' definition
// ****************************************************************************

#ifdef USE_EXPLICIT_MOVE_
Cell::Cell(Cell&& cell)
{
    *this = std::move(cell);
}

Cell& Cell::operator=(Cell&& cell)
{
    m_value.swap(cell.m_value);
    return *this;
}
#endif

Cell::Cell(const std::string& data)
    : m_value(data)
{
}

Cell::Cell(const int32_t value)
    : m_value(std::to_string(value))
{
}

Cell::Cell(const int64_t value)
    : m_value(std::to_string(value))
{
}

Cell::Cell(const float value)
    : m_value(string_utils::FormatFloatString(value))
{
}

Cell::Cell(const double value)
    : m_value(string_utils::FormatFloatString(value))
{
}

Cell::Cell(const long double value)
    : m_value(string_utils::FormatFloatString(value, 30))
{
}

Cell& Cell::operator=(const std::string& rhs)
{
    m_value = rhs;
    return *this;
}

Cell& Cell::operator=(const int32_t rhs)
{
    m_value = std::to_string(rhs);
    return *this;
}

Cell& Cell::operator=(const int64_t rhs)
{
    m_value = std::to_string(rhs);
    return *this;
}

Cell& Cell::operator=(const float rhs)
{
    m_value = string_utils::FormatFloatString(rhs);
    return *this;
}

Cell& Cell::operator=(const double rhs)
{
    m_value = string_utils::FormatFloatString(rhs);
    return *this;
}

Cell& Cell::operator=(const long double rhs)
{
    m_value = string_utils::FormatFloatString(rhs, 30);
    return *this;
}

std::string Cell::Value() const
{
    return m_value;
}

Cell::operator std::string() const
{
    return m_value;
}

Cell::operator int32_t() const
{
    return std::stoi(boost::trim_copy(m_value));
}

Cell::operator int64_t() const
{
    return std::stoll(boost::trim_copy(m_value));
}

Cell::operator float() const
{
    return std::stof(boost::trim_copy(m_value));
}

Cell::operator double() const
{
    return std::stod(boost::trim_copy(m_value));
}

Cell::operator long double() const
{
    return std::stold(boost::trim_copy(m_value));
}

int32_t Cell::ToInt32Def(const int32_t defval) const NO_EXCEPT_
{
    int32_t val;

    try
    {
        val = std::stoi(boost::trim_copy(m_value));
    }
    catch (...)
    {
        val = defval;
    }

    return val;
}

int64_t Cell::ToInt64Def(const int64_t defval) const NO_EXCEPT_
{
    int64_t val;

    try
    {
        val = std::stoll(boost::trim_copy(m_value));
    }
    catch (...)
    {
        val = defval;
    }

    return val;
}

float Cell::ToFloatDef(const float defval) const NO_EXCEPT_
{
    float val;

    try
    {
        val = std::stof(boost::trim_copy(m_value));
    }
    catch (...)
    {
        val = defval;
    }

    return val;
}

double Cell::ToDoubleDef(const double defval) const NO_EXCEPT_
{
    double val;

    try
    {
        val = std::stod(boost::trim_copy(m_value));
    }
    catch (...)
    {
        val = defval;
    }

    return val;
}

long double Cell::ToLongDoubleDef(const long double defval) const NO_EXCEPT_
{
    long double val;

    try
    {
        val = std::stold(boost::trim_copy(m_value));
    }
    catch (...)
    {
        val = defval;
    }

    return val;
}

} // namespace csv_grid
} // namespace core_lib
