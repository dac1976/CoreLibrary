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
 * \file CsvGrid.cpp
 * \brief File containing definitions relating the CSVGrid class.
 */

#include "../CsvGrid.hpp"
#include <algorithm>
#include <fstream>
#include <utility>
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "../StringUtils.hpp"

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

// ****************************************************************************
// 'class CsvGrid' definition
// ****************************************************************************
CsvGrid::CsvGrid(size_t rows, size_t cols)
{
    if ((rows == 0) || (cols == 0))
    {
        BOOST_THROW_EXCEPTION(xCsvGridDimensionError());
    }

    m_grid.resize(rows, Row(cols));
}

CsvGrid::CsvGrid(const std::string& filename, eCellFormatOptions options)
{
    LoadFromCSVFile(filename, options);
}

CsvGrid::CsvGrid(std::initializer_list<Row> rows)
    : m_grid{rows}
{
}

Row& CsvGrid::operator[](size_t row)
{
    if (row >= m_grid.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
    }

    return m_grid[row];
}

size_t CsvGrid::GetRowCount() const
{
    return m_grid.size();
}

size_t CsvGrid::GetColCount(size_t row) const
{
    if (row >= m_grid.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
    }

    return m_grid[row].GetSize();
}

void CsvGrid::SetRowCount(size_t rows, size_t defaultCols)
{
    m_grid.resize(rows, Row(defaultCols));
}

void CsvGrid::AddRow(size_t cols)
{
    m_grid.push_back(Row(cols));
}

void CsvGrid::AddColumnToAllRows()
{
    for (auto& row : m_grid)
    {
        row.AddColumn();
    }
}

void CsvGrid::InsertRow(size_t row, size_t defaultCols)
{
    if (row >= m_grid.size())
    {
        BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
    }

    m_grid.insert(m_grid.begin() + row, Row(defaultCols));
}

void CsvGrid::InsertColumnInAllRows(size_t col)
{
    for (auto& row : m_grid)
    {
        if (col < row.GetSize())
        {
            row.InsertColumn(col);
        }
    }
}

void CsvGrid::ClearCells()
{
    for (auto& row : m_grid)
    {
        row.ClearCells();
    }
}

void CsvGrid::ResetGrid()
{
    m_grid.clear();
}

void CsvGrid::LoadFromCSVFile(const std::string& filename,
                              eCellFormatOptions options)
{
    std::ifstream csvfile{filename.c_str()};

    if (!csvfile.is_open())
    {
        BOOST_THROW_EXCEPTION(
            xCsvGridCreateFileStreamError(std::string("failed to create file stream for loading: ")
                                          + filename));
    }

    m_grid.clear();

    // read CSV file in a CsvGrid::Row at a time...
    while(csvfile.good())
    {
        std::string line;
        std::getline(csvfile, line);
        string_utils::PackStdString(line);

        //don't add extra row if last row is empty
        if ((csvfile.tellg() == csvfile.gcount())
            || csvfile.eof())
        {
            // Are we done?
            if (line == "")
            {
                break;
            }
        }

        // add new CsvGrid::Row to vector...
        m_grid.push_back(Row(line, options));
    }

    csvfile.close();
}

void CsvGrid::SaveToCsvFile(const std::string& filename) const
{
    // open and check the stream...
    std::ofstream csvfile{filename.c_str(), std::ios::trunc};

    if (!csvfile.is_open())
    {
        BOOST_THROW_EXCEPTION(
            xCsvGridCreateFileStreamError(std::string("failed to create file stream for saving: ")
                                          + filename));
    }

    // output grid to file stream...
    OutputCsvGridToStream(csvfile);

    // close the file stream...
    csvfile.close();
}

void CsvGrid::OutputCsvGridToStream(std::ostream& os) const
{
    size_t row = 0;

    // let's loop over our rows...
    for (auto rowItem : m_grid)
    {
        rowItem.OutputRowToStream(os);

        // add line end if not the last row...
        if (row++ < m_grid.size()-1)
        {
            os << std::endl;
        }
    }

}

// ****************************************************************************
// 'class CsvGrid' ostream operator definition
// ****************************************************************************
std::ostream& operator<< (std::ostream& os, const CsvGrid& csvGrid)
{
    csvGrid.OutputCsvGridToStream(os);
    return os;
}

} // namespace csv_grid
} // namespace core_lib
