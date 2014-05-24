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
 * \file CsvGrid.hpp
 * \brief File containing declarations relating the CSVGrid class.
 */

#ifndef CSVGRID_HPP
#define CSVGRID_HPP

#include "CsvGrid/CsvGridRow.hpp"

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
class CsvGrid final
{
public:
    /*! \brief Friend declaration of overloaded operator<< for CsvGrid class. */
    friend std::ostream& operator<< (std::ostream& os, const CsvGrid& csvGrid);

    /*! \brief Default constructor. */
    CsvGrid() = default;
    /*! \brief Copy constructor. */
    CsvGrid(const CsvGrid&) = default;
    /*! \brief Move constructor. */
    CsvGrid(CsvGrid&&) = default;
    /*!
     * \brief Initializing constructor
     * \param [IN] The number of rows.
     * \param [IN] The number of columns.
     *
     * Create the rectangular grid object with a non-zero number of rows and
     * columns. If rows or columns are 0 then xCsvGridDimensionError exception
     * is thrown.
     */
    CsvGrid(size_t rows, size_t cols);
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
    CsvGrid(const std::string& filename, eCellFormatOptions options);
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of Rows.
     *
     * Create the CsvGrid from the given list of rows.
     */
    CsvGrid(std::initializer_list<Row> rows);
    /*! \brief Ddestructor. */
    ~CsvGrid() = default;
    /*! \brief Copy assignment operator. */
    CsvGrid& operator=(const CsvGrid&) = default;
    /*! \brief Move assignment operator. */
    CsvGrid& operator=(CsvGrid&&) = default;
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
    Row& operator[](size_t row);
    /*!
     * \brief Get the number of rows.
     * \return The number of rows for this grid.
     */
    size_t GetRowCount() const;
    /*!
     * \brief Get the number of columns for a given row.
     * \param [IN] A 0-based row index.
     * \return The number of columns for this row.
     *
     * \note
     * If the index is out of bounds a xCsvGridRowOutOfRangeError
     * exception is thrown.
     */
    size_t GetColCount(size_t row) const;
    /*!
     * \brief Resize the grid.
     * \param [IN] The number of rows.
     * \param [IN] The number of columns for newly created rows.
     *
     * Resize the grid, adding or dropping rows as necessary.
     */
    void SetRowCount(size_t rows, size_t defaultCols = 0);
    /*!
     * \brief Add a new row.
     * \param [IN] The default number of cells for the new row.
     *
     * Resize the grid, adding a new row with the given number of cells.
     */
    void AddRow(size_t cols = 0);
    /*!
     * \brief Add a column to each row.
     *
     * Resize the grid, adding a new column to each row.
     */
    void AddColumnToAllRows();
    /*!
     * \brief Insert a new row.
     * \param [IN] The row index at which the new row is to be inserted.
     * \param [IN] The default number of columns for the new row.
     *
     * Insert a new row at a given row index in the grid. If the row index is
     * out of range a xCsvGridRowOutOfRangeError exception is thrown.
     */
    void InsertRow(size_t row, size_t defaultCols = 0);
    /*!
     * \brief Insert a new column in all rows.
     * \param [IN] The column index at which the new column is to be inserted.
     *
     * The column is only inserted if the column index is within range of
     * the row in the grid otherwise a column is not added to the row.
     */
    void InsertColumnInAllRows(size_t col);
    /*!
    * \brief Clear the contents of all cells.
    *
    * The contents of each cell in the grid is cleared but the row and
    * column counts remain unchanged.
    */
    void ClearCells();
    /*!
    * \brief Clear the entire grid.
    *
    * All rows are removed from the grid leaving a row count of 0 afterwards.
    */
    void ResetGrid();
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
    void LoadFromCSVFile(const std::string& filename, eCellFormatOptions options);
    /*!
     * \brief Save the grid to a CSV file.
     * \param [IN] The full path name of the CSV file to load.
     *
     * Create a CSV file from a grid object. If the file stream cannot
     * be created or opened the a xCsvGridCreateFileStreamError exception
     * is thrown.
     */
    void SaveToCsvFile(const std::string& filename) const;

private:
    /*! \brief The grid row data. */
    std::vector<Row> m_grid;

    /*!
     * \brief Output the grid to a stream object.
     * \param [IN] The stream object.
     *
     * Write the grid in CSV format to a stream object.
     */
    void OutputCsvGridToStream(std::ostream& os) const;
};

/*!
 * \brief Output a CsvGrid object to a stream object.
 * \param [IN/OUT] The stream object to write to.
 * \param [IN] The grid object.
 * \return The modified stream object.
 *
 * Write the grid object in CSV format to a stream object.
 */
std::ostream& operator<< (std::ostream &os, const CsvGrid &csvGrid);

}// namespace csv_grid
}// namespace core_lib

#endif // CSVGRID_HPP
