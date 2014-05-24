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
 * \file CsvGridRow.hpp
 * \brief File containing declarations relating the CSVGridRow class.
 */

#ifndef CSVGRIDROW_HPP
#define CSVGRIDROW_HPP

#include "CsvGridCell.hpp"
#include "Exceptions/CustomException.hpp"
#include <vector>
#include <initializer_list>
#include <ostream>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The csv_grid namespace. */
namespace csv_grid {

/*!
 * \brief Column index out of range exception.
 *
 * This exception class is intended to be thrown by functions in the CsvGrid
 * class and its child classes when an invalid column index is used.
 */
class xCsvGridColOutOfRangeError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xCsvGridColOutOfRangeError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xCsvGridColOutOfRangeError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xCsvGridColOutOfRangeError();
};

/*!
 * \brief Cell format options enumeration.
 *
 * This enumeration is used to control the format of the cells within a row of
 * the CSV grid, in particular whether or not they are surrounded by double qoutes
 * or not.
 */
enum class eCellFormatOptions
{
    /*! \brief All cells are simple an not wrapped in double quotes, e.g. x1,x2,x3. */
    simpleCells,
    /*! \brief Cells may contain special characters and are in double qoutes, e.g. "x1","x2","x3". */
    doubleQuotedCells
};

// forward declaration for using in Row class.
class CsvGrid;

/*!
 * \brief Class defining a row of the grid.
 *
 * This class provides the definition of the interface for a row within
 * the grid. A row contains cells and each cell's position represents a column
 * within the grid.
 */
class Row final
{
public:
    /*! \brief Friend declaration of CsvGrid so it can have private access to its rows. */
    friend class CsvGrid;

    /*! \brief Default constructor. */
    Row() = default;
    /*! \brief Copy constructor. */
    Row(const Row&) = default;
    /*! \brief Move constructor. */
    Row(Row&&) = default;
    /*!
     * \brief Initializing constructor
     * \param [IN] The initial number of columns.
     *
     * Create the row with an initial number of columns.
     */
    explicit Row(size_t numCols);
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of Cells.
     *
     * Create the row from the given list of Cells.
     */
    Row(std::initializer_list<Cell> cells);
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of strings.
     *
     * Create the row from the given list of strings.
     */
    Row(std::initializer_list<std::string> cells);
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of int32_ts.
     *
     * Create the row from the given list of int32_ts.
     */
    Row(std::initializer_list<int32_t> cells);
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of int64_ts.
     *
     * Create the row from the given list of int64_ts.
     */
    Row(std::initializer_list<int64_t> cells);
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of doubles.
     *
     * Create the row from the given list of doubles.
     */
    Row(std::initializer_list<double> cells);
    /*!
     * \brief Initializer list constructor
     * \param [IN] The initial list of long doubles.
     *
     * Create the row from the given list of long doubles.
     */
    Row(std::initializer_list<long double> cells);
    /*! \brief Destructor. */
    ~Row() = default;
    /*! \brief Copy assignment operator. */
    Row& operator=(const Row&) = default;
    /*! \brief Move assignment operator. */
    Row& operator=(Row&&) = default;
    /*!
     * \brief Subscript operator.
     * \param [IN] A 0-based column index.
     * \return The cell at the given column index.
     *
     * Retrieve the cell at a given column index within a row.
     *
     * \note
     * If the index is out of bounds a xCsvGridColOutOfRangeError
     * exception is thrown.
     */
    Cell& operator[](size_t col);
    /*!
     * \brief Get the number of columns.
     * \return The number of columns for this row.
     */
    size_t GetSize() const ;
    /*!
    * \brief Set the number of columns in the row.
    * \param [IN] The number of columns to set.
    *
    * If the number of columns are being increased then existing content
    * is preserved and new cells are added at the end of the row forming
    * the extra columns.
    */
    void SetSize(size_t cols);
    /*!
    * \brief Add a column with the given value.
    * \param [IN] The cell's value for the new column.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given string.
    */
    void AddColumn(const std::string& value = "");
    /*!
    * \brief Add a column with the given value.
    * \param [IN] The cell's value for the new column.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given 32bit integer value.
    */
    void AddColumn(int32_t value);
    /*!
    * \brief Add a column with the given value.
    * \param [IN] The cell's value for the new column.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given 64bit integer value.
    */
    void AddColumn(int64_t value);
    /*!
    * \brief Add a column with the given value.
    * \param [IN] The cell's value for the new column.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given double precesision floating point value.
    */
    void AddColumn(double value);
    /*!
    * \brief Add a column with the given value.
    * \param [IN] The cell's value for the new column.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given long double precesision floating point value.
    */
    void AddColumn(long double value);
    /*!
    * \brief Insert a new cell.
    * \param [IN] The column index at which the new cell is to be inserted.
    * \param [IN] The value to assign to the newly inserted cell.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given string.
    */
    void InsertColumn(size_t col, const std::string& value = "");
    /*!
    * \brief Insert a new cell.
    * \param [IN] The column index at which the new cell is to be inserted.
    * \param [IN] The value to assign to the newly inserted cell.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given 32bit integer.
    */
    void InsertColumn(size_t col, int32_t value);
    /*!
    * \brief Insert a new cell.
    * \param [IN] The column index at which the new cell is to be inserted.
    * \param [IN] The value to assign to the newly inserted cell.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given 64bit integer.
    */
    void InsertColumn(size_t col, int64_t value);
    /*!
    * \brief Insert a new cell.
    * \param [IN] The column index at which the new cell is to be inserted.
    * \param [IN] The value to assign to the newly inserted cell.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given double precision floating point value.
    */
    void InsertColumn(size_t col, double value);
    /*!
    * \brief Insert a new cell.
    * \param [IN] The column index at which the new cell is to be inserted.
    * \param [IN] The value to assign to the newly inserted cell.
    *
    * The column count is increased by one and the new cell is initialised
    * with the given long double precision floating point value.
    */
    void InsertColumn(size_t col, long double value);
    /*!
    * \brief Clear the cells' contents.
    *
    * The contents of each column's cell is cleared but the column count
    * remains unchanged.
    */
    void ClearCells();
    /*!
    * \brief Clear the entire row.
    *
    * The cells are completely removed from the row leaving the column
    * count as 0 afterwards.
    */
    void ResetRow();

private:
    /*!  \brief The row's cells. */
    std::vector<Cell> m_cells;

    /*!
     * \brief Initializing constructor
     * \param [IN] The initial value string.
     * \param [IN] The line format.
     *
     * Create the row with an initial value and specify whether cells
     * are wrapped in double quotesin the CSV file.
     */
    Row(const std::string& line, eCellFormatOptions options);
    /*!
    * \brief Load a row from a line in a CSV file.
    * \param [IN] The line from the CSV file.
    * \param [IN] The cell format options.
    *
    * Create a row by loading it from a line read in from a CSV file.
    * The options parameter is used to decide how to tokenize the line.
    */
    void LoadRowFromCsvFileLine(const std::string& line,
                                eCellFormatOptions options);
    /*!
    * \brief Write the row's contents to a stream object.
    * \param [IN/OUT] The stream object to write to.
    *
    * The row's contents are formated using CSV formating and output to the
    * stream object.
    */
    void OutputRowToStream(std::ostream& os) const;
    /*!
    * \brief Tokenize a row with double quoted cells.
    * \param [IN] A CSV file's line of text.
    *
    * The comma separated row's tokens are extracted using a boost
    * tokenizer object. This does impact the performance slightly.
    */
    void TokenizeLineQuoted(const std::string& line);
    /*!
    * \brief Tokenize a row with simple cells.
    * \param [IN] A CSV file's line of text.
    *
    * The comma separated row's tokens are extracted using a
    * simple getline based algorithm. This versio ncannot handle
    * double quoted cells.
    */
    void TokenizeLine(const std::string& line);
};

}// namespace csv_grid
}// namespace core_lib

#endif // CSVGRIDROW_HPP
