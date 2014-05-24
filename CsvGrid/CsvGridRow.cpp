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
 * \file CsvGridRow.cpp
 * \brief File containing definitions relating the CsvGridRow class.
 */

#include "CsvGridRow.hpp"
#include "../StringUtils.hpp"
#include <algorithm>
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string/trim.hpp"

namespace core_lib {
namespace csv_grid {

// ****************************************************************************
// 'class xRowOutOfRangeError' definition
// ****************************************************************************
xCsvGridColOutOfRangeError::xCsvGridColOutOfRangeError()
    : exceptions::xCustomException("invalid column index")
{
}

xCsvGridColOutOfRangeError::xCsvGridColOutOfRangeError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xCsvGridColOutOfRangeError::~xCsvGridColOutOfRangeError()
{
}

// ****************************************************************************
// 'class Row' definition
// ****************************************************************************

Row::Row(size_t numCols)
    : m_cells{numCols}
{
}

Row::Row(std::initializer_list<Cell> cells)
    : m_cells{cells}
{
}

Row::Row(std::initializer_list<std::string> cells)
{
    m_cells.reserve(cells.size());

    for (auto cell : cells)
    {
        m_cells.push_back(Cell(cell));
    }
}

Row::Row(std::initializer_list<int32_t> cells)
{
    m_cells.reserve(cells.size());

    for (auto cell : cells)
    {
        m_cells.push_back(Cell(cell));
    }
}

Row::Row(std::initializer_list<int64_t> cells)
{
    m_cells.reserve(cells.size());

    for (auto cell : cells)
    {
        m_cells.push_back(Cell(cell));
    }
}

Row::Row(std::initializer_list<double> cells)
{
    m_cells.reserve(cells.size());

    for (auto cell : cells)
    {
        m_cells.push_back(Cell(cell));
    }
}

Row::Row(std::initializer_list<long double> cells)
{
    m_cells.reserve(cells.size());

    for (auto cell : cells)
    {
        m_cells.push_back(Cell(cell));
    }
}

Row::Row(const std::string& line, eCellFormatOptions options)
{
    LoadRowFromCsvFileLine(line, options);
}

Cell& Row::operator[](size_t col)
{
    if (col >= m_cells.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridColOutOfRangeError());
    }

    return m_cells[col];
}

size_t Row::GetSize() const
{
    return m_cells.size();
}

void Row::SetSize(size_t cols)
{
    m_cells.resize(cols);
}

void Row::AddColumn(const std::string& value)
{
    m_cells.push_back(Cell(value));
}

void Row::AddColumn(int32_t value)
{
    m_cells.push_back(Cell(value));
}

void Row::AddColumn(int64_t value)
{
    m_cells.push_back(Cell(value));
}

void Row::AddColumn(double value)
{
    m_cells.push_back(Cell(value));
}

void Row::AddColumn(long double value)
{
    m_cells.push_back(Cell(value));
}

void Row::InsertColumn(size_t col, const std::string& value)
{
    if (col >= m_cells.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridColOutOfRangeError());
    }

    m_cells.insert(m_cells.begin() + col, Cell(value));
}

void Row::InsertColumn(size_t col, int32_t value)
{
    if (col >= m_cells.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridColOutOfRangeError());
    }

    m_cells.insert(m_cells.begin() + col, Cell(value));
}

void Row::InsertColumn(size_t col, int64_t value)
{
    if (col >= m_cells.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridColOutOfRangeError());
    }

    m_cells.insert(m_cells.begin() + col, Cell(value));
}

void Row::InsertColumn(size_t col, double value)
{
    if (col >= m_cells.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridColOutOfRangeError());
    }

    m_cells.insert(m_cells.begin() + col, Cell(value));
}

void Row::InsertColumn(size_t col, long double value)
{
    if (col >= m_cells.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridColOutOfRangeError());
    }

    m_cells.insert(m_cells.begin() + col, Cell(value));
}

void Row::ClearCells()
{
    std::fill(m_cells.begin(), m_cells.end(), Cell());
}

void Row::ResetRow()
{
    m_cells.clear();
}

void Row::LoadRowFromCsvFileLine(const std::string& line,
                                 eCellFormatOptions options)
{
    // add row to grid...
    if (options == eCellFormatOptions::doubleQuotedCells)
    {
        TokenizeLineQuoted(line);
    }
    else
    {
        TokenizeLine(line);
    }
}

void Row::OutputRowToStream(std::ostream& os) const
{
    // for each row loop over its columns...
    size_t col{};

    for (auto cellItem : m_cells)
    {
        // let's get our cell value...
        std::string cell = m_cells[col];

        // if string contains quotes, insert an
        // extra quote...
        size_t pos{cell.find('"')};

        while (pos != std::string::npos)
        {
            pos = cell.find('"', pos);
            cell.insert(pos, "\"");
            ++pos;
        }

        // if cell contains ',', '\n' or '\r' wrap it in quotes...
        if (cell.find_first_of(",\r\n") != std::string::npos)
        {
            cell = "\"" + cell + "\"";
        }

        // output corrected cell...
        os << cell;

        // add ',' if not last cell on current row...
        if (col++ < m_cells.size() - 1)
        {
            os << ",";
        }
    }
}

void Row::TokenizeLineQuoted(const std::string& line)
{
    typedef boost::tokenizer< boost::escaped_list_separator<char> >
            Tokenizer;

    // prepare tokenizer...
    Tokenizer tokzr{line};
    Tokenizer::const_iterator tokIter{tokzr.begin()};

    // loop over columns and trim leading and trailing
    // spaces before updating cell contents...
    while (tokIter != tokzr.end())
    {
        // trim token...
        std::string tok{*tokIter++};
        boost::trim(tok);
        // add new column...
        m_cells.push_back(Cell{tok});
    }
}

void Row::TokenizeLine(const std::string& line)
{
    // loop over columns and trim leading and trailing
    // spaces before updating cell contents...
    std::stringstream line_ss{line};
    std::string tok;

    while (std::getline(line_ss, tok, ','))
    {
        // tidy and trim token...
        string_utils::PackStdString(tok);
        boost::trim(tok);

        // add new column...
        m_cells.push_back(Cell{tok});
    }
}

} // namespace csv_grid
} // namespace core_lib
