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

#include <cstdint>
#include <string>
#include <vector>
#include <initializer_list>
#include <ostream>
#include "Exceptions/CustomException.hpp"

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

/*!
 * \brief Class defining a single cell within a row of the grid.
 *
 * This class provides the definition of the interface for a cell within
 * a row in the grid. A cell can be thought of as the value in a given
 * column index within the grid.
 */
class Cell
{
public:
    /*! \brief Default constructor. */
    Cell() = default;
    /*! \brief Copy constructor. */
    Cell(const Cell& rhs) = default;
    /*! \brief Move constructor. */
    Cell(Cell&& rhs) = default;
    /*!
     * \brief Initializing constructor
     * \param [IN] The initial value.
     *
     * Initialise the cell with a string.
     */
    explicit Cell(const std::string& value);
    /*!
     * \brief Initializing constructor
     * \param [IN] The initial value.
     *
     * Initialise the cell with a 32bit integer value.
     */
    explicit Cell(int32_t value);
    /*!
     * \brief Initializing constructor
     * \param [IN] The initial value.
     *
     * Initialise the cell with a 64bit integer value.
     */
    explicit Cell(int64_t value);
    /*!
     * \brief Initializing constructor
     * \param [IN] The initial value.
     *
     * Initialise the cell with a double precision floating point value.
     */
    explicit Cell(double value);
    /*!
     * \brief Initializing constructor
     * \param [IN] The initial value.
     *
     * Initialise the cell with a long double precision floating point value.
     */
    explicit Cell(long double value);
    /*! \brief Destructor. */
    ~Cell() = default;
    /*! \brief Copy assignment operator. */
    Cell& operator=(const Cell& rhs) = default;
    /*! \brief Move assignment operator. */
    Cell& operator=(Cell&& rhs) = default;
    /*!
     * \brief Value assignment operator.
     * \param [IN] The value to assign.
     * \return Modified Cell object.
     *
     * Assign to a string.
     */
    Cell& operator=(const std::string& rhs);
    /*!
     * \brief Value assignment operator.
     * \param [IN] The value to assign.
     * \return Modified Cell object.
     *
     * Assign to a 32bit integer value.
     */
    Cell& operator=(int32_t rhs);
    /*!
     * \brief Value assignment operator.
     * \param [IN] The value to assign.
     * \return Modified Cell object.
     *
     * Assign to a 64bit integer value.
     */
    Cell& operator=(int64_t rhs);
    /*!
     * \brief Value assignment operator.
     * \param [IN] The value to assign.
     * \return Modified Cell object.
     *
     * Assign to a double precision floating point value.
     */
    Cell& operator=(double rhs);
    /*!
     * \brief Value assignment operator.
     * \param [IN] The value to assign.
     * \return Modified Cell object.
     *
     * Assign to a long double precision floating point value.
     */
    Cell& operator=(long double rhs);
    /*!
     * \brief Cast operator.
     * \return Modified Cell object.
     *
     * Cast the cell to a string.
     */
    operator std::string() const;
    /*!
     * \brief Cast operator.
     *
     * Cast the cell to a 32bit integer.
     *
     * \note
     * If the cell's value cannot be cast to a 32bit integer then a
     * boost::bad_lexical_cast exception is thrown.
     */
    operator int32_t() const;
    /*!
     * \brief Cast operator.
     *
     * Cast the cell to a 64bit integer.
     *
     * \note
     * If the cell's value cannot be cast to a 64bit integer then a
     * boost::bad_lexical_cast exception is thrown.
     */
    operator int64_t() const;
    /*!
     * \brief Cast operator.
     *
     * Cast the cell to a double.
     *
     * \note
     * If the cell's value cannot be cast to a double then a
     * boost::bad_lexical_cast exception is thrown.
     */
    operator double() const;
    /*!
     * \brief Cast operator.
     *
     * Cast the cell to a long double.
     *
     * \note
     * If the cell's value cannot be cast to a long double then a
     * boost::bad_lexical_cast exception is thrown.
     */
    operator long double() const;
    /*!
     * \brief Conversion function
     * \param [IN] The default value to return in case of a failure.
     * \return Cell value as a 32bit integer.
     *
     * Convert the cell to a 32bit integer and if this cannot be
     * performed safely return the default value instead.
     */
    int32_t ToInt32Def(int32_t defval) const noexcept;
    /*!
     * \brief Conversion function
     * \param [IN] The default value to return in case of a failure.
     * \return Cell value as a 64bit integer.
     *
     * Convert the cell to a 64bit integer and if this cannot be
     * performed safely return the default value instead.
     */
    int64_t ToInt64Def(int64_t defval) const noexcept;
    /*!
     * \brief Conversion function
     * \param [IN] The default value to return in case of a failure.
     * \return Cell value as a double.
     *
     * Convert the cell to a double and if this cannot be performed
     * safely return the default value instead.
     */
    double ToDoubleDef(double defval) const noexcept;
    /*!
     * \brief Conversion function
     * \param [IN] The default value to return in case of a failure.
     * \return Cell value as a long double.
     *
     * Convert the cell to a long double and if this cannot be performed
     * safely return the default value instead.
     */
    long double ToLongDoubleDef(long double defval) const noexcept;

private:
    /*! \brief The cell's value data memeber. */
    std::string m_value;
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
class Row
{
public:
    /*! \brief Friend declaration of CsvGrid so it can have private access to its rows. */
    friend class CsvGrid;

    /*! \brief Default constructor. */
    Row() = default;
    /*! \brief Copy constructor. */
    Row(const Row& rhs) = default;
    /*! \brief Move constructor. */
    Row(Row&& rhs) = default;
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
    Row& operator=(const Row& rhs) = default;
    /*! \brief Move assignment operator. */
    Row& operator=(Row&& rhs) = default;
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
class CsvGrid
{
public:
    /*! \brief Friend declaration of overloaded operator<< for CsvGrid class. */
    friend std::ostream& operator<< (std::ostream& os, const CsvGrid& csvGrid);

    /*! \brief Default constructor. */
    CsvGrid() = default;
    /*! \brief Copy constructor. */
    CsvGrid(const CsvGrid& rhs) = default;
    /*! \brief Move constructor. */
    CsvGrid(CsvGrid&& rhs) = default;
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
    CsvGrid& operator=(const CsvGrid& rhs) = default;
    /*! \brief Move assignment operator. */
    CsvGrid& operator=(CsvGrid&& rhs) = default;
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
