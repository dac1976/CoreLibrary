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
 * \file CsvGridMain.hpp
 * \brief File containing declarations relating the TCsvGrid class.
 */

#ifndef CSVGRIDMAIN_HPP
#define CSVGRIDMAIN_HPP

#include "CsvGridRow.hpp"
#include <vector>
#include <list>
#include <fstream>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The csv_grid namespace. */
namespace csv_grid {

/*!
 * \brief Grid dimension exception.
 *
 * This exception class is intended to be thrown by functions in the CsvGrid
 * class when invalid grid dimensions are specified.
 */
class xCsvGridDimensionError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xCsvGridDimensionError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xCsvGridDimensionError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xCsvGridDimensionError();
};

/*!
 * \brief Row index out of range exception.
 *
 * This exception class is intended to be thrown by functions in the CsvGrid
 * class when an invalid row idex is used.
 */
class xCsvGridRowOutOfRangeError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xCsvGridRowOutOfRangeError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xCsvGridRowOutOfRangeError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xCsvGridRowOutOfRangeError();
};

/*!
 * \brief File stream creation failure exception.
 *
 *  This exception class is intended to be thrown by functions in the CsvGrid
 *  class when an invalid row idex is used.
 */
class xCsvGridCreateFileStreamError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xCsvGridCreateFileStreamError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xCsvGridCreateFileStreamError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xCsvGridCreateFileStreamError();
};

/*!
 * \brief Grid class with CSV file capabilities.
 *
 * This class provides an implemtnation of an easy-to-use managed grid that
 * can contain strings, integers or floating point data in any combination.
 *
 * The grid can optionally be initialised by loading data from a CSV file. The
 * grid is able to save its contents to CSV file. It can also be streamed to any
 * compatible stream object using the << operator.
 *
 * The grid class handles both rectangular and jagged data and CSV fles. Jagged
 * data contains different number of cells for different rows.
 *
 * The default behaviour is to be able to handle CSV files that may have their
 * cells contained in double quotes. There is a slight performance overhead
 * associated with this because extra parsing is required to tokenise each line.
 * If it is known that the CSV file contains simple data, e.g. no cells in the
 * CSV file are in double quotes, then the user can disable double quote handling,
 * which gives a slight performance increase. With large data sets the performance
 * increase can be significant.
 *
 * \note
 * This class loads the entire CSV file into memory so if the file is particulary
 * large you will crash your program if you do not have enough free memory.
 */
template<template<class, class> class C>
class TCsvGrid final
{
public:
    /*! \brief typedef for row type */
    typedef TRow<C> row_type;
    /*! \brief typedef for container type */
    typedef C<row_type, std::allocator<row_type>> container_type;
    /*! \brief Default constructor. */
    TCsvGrid() = default;
    /*! \brief Copy constructor. */
    TCsvGrid(const TCsvGrid&) = default;
    /*! \brief Move constructor. */
    TCsvGrid(TCsvGrid&&) = default;
    /*!
     * \brief Initializing constructor
     * \param [IN] The number of rows.
     * \param [IN] The number of columns.
     *
     * Create the rectangular grid object with a non-zero number of rows and
     * columns. If rows or columns are 0 then xCsvGridDimensionError exception
     * is thrown.
     */
    TCsvGrid(size_t rows, size_t cols)
    {
        if ((rows == 0) || (cols == 0))
        {
            BOOST_THROW_EXCEPTION(xCsvGridDimensionError());
        }

        m_grid.resize(rows, row_type(cols));
    }
    /*!
     * \brief Initializing constructor
     * \param [IN] The full path name of the CSV file to load.
     * \param [IN] Cell formating options.
     *
     * Create a grid object from a CSV file. If cells are wrapped in double
     * quotes in the CSV file then set options = doubleQuotedCells else set
     * options = simpleCells. Throw a xCsvGridCreateFileStreamError exceptions
     * if the file cannot be loaded.
     */
    TCsvGrid(const std::string& filename, eCellFormatOptions options)
    {
        LoadFromCSVFile(filename, options);
    }
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of Rows.
     *
     * Create the CsvGrid from the given list of rows.
     */
    TCsvGrid(std::initializer_list<row_type> rows)
        : m_grid{rows}
    {
    }
    /*! \brief Ddestructor. */
    ~TCsvGrid() = default;
    /*! \brief Copy assignment operator. */
    TCsvGrid& operator=(const TCsvGrid&) = default;
    /*! \brief Move assignment operator. */
    TCsvGrid& operator=(TCsvGrid&&) = default;
    /*!
     * \brief Subscript operator.
     * \param [IN] A 0-based row index.
     * \return The row at the given row index.
     *
     * Retrieve the row at a given row index within a grid.
     *
     * \note
     * If the index is out of bounds a xCsvGridRowOutOfRangeError
     * exception is thrown.
     */
    row_type& operator[](size_t row)
    {
        if (row >= m_grid.size())
        {
            BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
        }

        return m_grid[row];
    }
    /*!
     * \brief Get the number of rows.
     * \return The number of rows for this grid.
     */
    size_t GetRowCount() const
    {
        return m_grid.size();
    }
    /*!
     * \brief Get the number of columns for a given row.
     * \param [IN] A 0-based row index.
     * \return The number of columns for this row.
     *
     * \note
     * If the index is out of bounds a xCsvGridRowOutOfRangeError
     * exception is thrown.
     */
    size_t GetColCount(size_t row) const
    {
        if (row >= m_grid.size())
        {
            BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
        }

        return m_grid[row].GetSize();
    }
    /*!
     * \brief Resize the grid.
     * \param [IN] The number of rows.
     * \param [IN] The number of columns for newly created rows.
     *
     * Resize the grid, adding or dropping rows as necessary.
     */
    void SetRowCount(size_t rows, size_t defaultCols = 0)
    {
        m_grid.resize(rows, row_type(defaultCols));
    }
    /*!
     * \brief Add a new row.
     * \param [IN] The default number of cells for the new row.
     *
     * Resize the grid, adding a new row with the given number of cells.
     */
    void AddRow(size_t cols = 0)
    {
        m_grid.emplace_back(cols);
    }
    /*!
     * \brief Add a column to each row.
     *
     * Resize the grid, adding a new column to each row.
     */
    void AddColumnToAllRows()
    {
        for (auto& row : m_grid)
        {
            row.AddColumn();
        }
    }
    /*!
     * \brief Insert a new row.
     * \param [IN] The row index at which the new row is to be inserted.
     * \param [IN] The default number of columns for the new row.
     *
     * Insert a new row at a given row index in the grid. If the row index is
     * out of range a xCsvGridRowOutOfRangeError exception is thrown.
     */
    void InsertRow(size_t row, size_t defaultCols = 0)
    {
        if (row >= m_grid.size())
        {
            BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
        }

        m_grid.emplace(m_grid.begin() + row, defaultCols);
    }
    /*!
     * \brief Insert a new column in all rows.
     * \param [IN] The column index at which the new column is to be inserted.
     *
     * The column is only inserted if the column index is within range of
     * the row in the grid otherwise a column is not added to the row.
     */
    void InsertColumnInAllRows(size_t col)
    {
        for (auto& row : m_grid)
        {
            if (col < row.GetSize())
            {
                row.InsertColumn(col);
            }
        }
    }
    /*!
    * \brief Clear the contents of all cells.
    *
    * The contents of each cell in the grid is cleared but the row and
    * column counts remain unchanged.
    */
    void ClearCells()
    {
        for (auto& row : m_grid)
        {
            row.ClearCells();
        }
    }
    /*!
    * \brief Clear the entire grid.
    *
    * All rows are removed from the grid leaving a row count of 0 afterwards.
    */
    void ResetGrid()
    {
        m_grid.clear();
    }
    /*!
     * \brief Load a csv file into the grid.
     * \param [IN] The full path name of the CSV file to load.
     * \param [IN] Cell formating options.
     *
     * Create a grid object from a CSV file. If cells are wrapped in double
     * quotes in the CSV file then set options = doubleQuotedCells else set
     * options = simpleCells. If the file stream cannot be created or opened
     * the a xCsvGridCreateFileStreamError exception is thrown.
     */
    void LoadFromCSVFile(const std::string& filename, eCellFormatOptions options)
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
            m_grid.emplace_back(line, options);
        }

        csvfile.close();
    }
    /*!
     * \brief Save the grid to a CSV file.
     * \param [IN] The full path name of the CSV file to load.
     *
     * Create a CSV file from a grid object. If the file stream cannot
     * be created or opened the a xCsvGridCreateFileStreamError exception
     * is thrown.
     */
    void SaveToCsvFile(const std::string& filename) const
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

private:
    /*! \brief The grid row data. */
    container_type m_grid;

    /*!
     * \brief Output the grid to a stream object.
     * \param [IN] The stream object.
     *
     * Write the grid in CSV format to a stream object.
     */
    void OutputCsvGridToStream(std::ostream& os) const
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
};

}// namespace csv_grid
}// namespace core_lib

#endif // CSVGRIDMAIN_HPP
