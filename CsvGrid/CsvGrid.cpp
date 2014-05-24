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
#include "../StringUtils.hpp"
#include <fstream>

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
