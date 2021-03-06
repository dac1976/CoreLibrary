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
 * \file CsvGridMain.h
 * \brief File containing declarations relating the TCsvGrid class.
 */

#ifndef CSVGRIDMAIN
#define CSVGRIDMAIN

#include <fstream>
#include <limits>
#include <cmath>
#include "CsvGridRow.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The csv_grid namespace. */
namespace csv_grid
{

/*! \brief Enumeration controlling how file is saved. */
enum class eSaveToFileOptions
{
    /*! \brief Truncate existing file replacing it with the contents of the grid. */
    truncate,
    /*! \brief Append the contents of the grid to the end of the file if it already exists.*/
    append
};

/*!
 * \brief Grid class with CSV file capabilities.
 *
 * This class provides an implemtnation of an easy-to-use managed grid that
 * can contain strings, integers or floating point data in any combination.
 * Although in its standard setup data is internally stored as a std::string
 * due to the implict text-based nature of a CSV file.
 *
 * The grid can optionally be initialised by loading data from a CSV file. The
 * grid is able to save its contents to CSV file. It can also be streamed to any
 * compatible stream object using the << operator.
 *
 * The grid class handles both rectangular and jagged data and CSV fles. Jagged
 * data contains different number of columns for different rows.
 *
 * The default behaviour is to be able to handle CSV files that may have their
 * cells contained in double quotes. There is a slight performance overhead
 * associated with this because extra parsing is required to tokenize each line.
 * If it is known that the CSV file contains simple data, e.g. no cells in the
 * CSV file are in double quotes, then the user can disable double quote handling,
 * which gives a slight performance increase. With large data sets the performance
 * increase can be significant.
 *
 * \note
 * This class loads the entire CSV file into memory so if the file is particulary
 * large you will crash your program if you do not have enough free memory. However,
 * if you know in advance from what row number you want to start readingdata from a
 * CSV file and how many rows you want to read then you can sepcify these in the
 * LoadFromCSVFile method.
 */
template <template <class, class> class C, class T = Cell> class TCsvGrid final
{
public:
    /*! \brief typedef for row type */
    using row_type = TRow<C, T>;
    /*! \brief typedef for container type */
    using container_type = C<row_type, std::allocator<row_type>>;
    /*! \brief Default constructor. */
    TCsvGrid() = default;
    /*! \brief Copy constructor. */
    TCsvGrid(const TCsvGrid&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move constructor. */
    TCsvGrid(TCsvGrid&& csvGrid)
    {
        *this = std::move(csvGrid);
    }
#else
    /*! \brief Move constructor. */
    TCsvGrid(TCsvGrid&&) = default;
#endif
    /*!
     * \brief Initializing constructor
     * \param[in] rows - The number of rows.
     * \param[in] cols - The number of columns.
     *
     * Create the rectangular grid object with a non-zero number of rows and
     * columns. If rows or columns are 0 then std::out_of_range exception
     * is thrown.
     */
    TCsvGrid(size_t rows, size_t cols)
    {
        if ((rows == 0) || (cols == 0))
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("rows or cols is 0"));
        }

        m_grid.resize(rows, row_type(cols));
    }
    /*!
     * \brief Initializing constructor
     * \param[in] filename - The full path name of the CSV file to load.
     * \param[in] options - Cell formatting options.
     *
     * Create a grid object from a CSV file. If cells are wrapped in double
     * quotes in the CSV file then set options = doubleQuotedCells else set
     * options = simpleCells. Throw a std::runtime_error exception
     * if the file cannot be loaded.
     */
    TCsvGrid(const std::string& filename, eCellFormatOptions options)
    {
        LoadFromCSVFile(filename, options);
    }
    /*!
     * \brief Initializer list constructor
     * \param[in] rows - The initial list of rows.
     *
     * Create the CsvGrid from the given list of rows.
     */
    // cppcheck-suppress noExplicitConstructor
    TCsvGrid(std::initializer_list<row_type> rows)
        : m_grid(rows)
    {
    }
    /*! \brief Ddestructor. */
    ~TCsvGrid() = default;
    /*! \brief Copy assignment operator. */
    TCsvGrid& operator=(const TCsvGrid&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move assignment operator. */
    TCsvGrid& operator=(TCsvGrid&& csvGrid)
    {
        std::swap(m_grid, csvGrid.m_grid);
        return *this;
    }
#else
    /*! \brief Move assignment operator. */
    TCsvGrid& operator=(TCsvGrid&&) = default;
#endif
    /*!
     * \brief Subscript operator.
     * \param[in] row - A 0-based row index.
     * \return The row at the given row index.
     *
     * Retrieve the row at a given row index within a grid.
     *
     * \note If the index is out of bounds a std::out_of_range exception is thrown.
     */
    row_type& operator[](size_t row)
    {
        if (row >= GetRowCount())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("row out of range"));
        }

        return *std::next(m_grid.begin(), row);
    }
    /*!
     * \brief Const subscript operator.
     * \param[in] row - A 0-based row index.
     * \return The row at the given row index.
     *
     * Retrieve the row at a given row index within a grid.
     *
     * \note If the index is out of bounds a std::out_of_range exception is thrown.
     */
    const row_type& operator[](size_t row) const
    {
        if (row >= GetRowCount())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("row out of range"));
        }

        return *std::next(m_grid.begin(), row);
    }
    /*!
     * \brief Get empty state of grid.
     * \return True if grid is empty.
     */
    bool Empty() const
    {
        return m_grid.empty();
    }
    /*!
     * \brief Get the number of rows.
     * \return The number of rows for this grid.
     */
    size_t GetRowCount() const
    {
        return std::distance(m_grid.begin(), m_grid.end());
    }
    /*!
     * \brief Get the number of columns for a given row.
     * \param[in] row - A 0-based row index.
     * \return The number of columns for this row.
     *
     * \note
     * If the index is out of bounds a std::out_of_range
     * exception is thrown.
     */
    size_t GetColCount(size_t row) const
    {
        if (row >= GetRowCount())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("row out of range"));
        }

        auto n = std::next(m_grid.begin(), row);
        return n->GetSize();
    }
    /*!
     * \brief Resize the grid.
     * \param[in] rows - The number of rows.
     * \param[in] defaultCols - The number of columns for newly created rows.
     *
     * Resize the grid, adding or dropping rows as necessary.
     */
    void SetRowCount(size_t rows, size_t defaultCols = 0)
    {
        m_grid.resize(rows, row_type(defaultCols));
    }
    /*!
     * \brief Add a new row.
     * \param[in] cols - The default number of cells for the new row.
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
    // cppcheck-suppress functionConst
    void AddColumnToAllRows()
    {
        for (auto& row : m_grid)
        {
            row.AddColumn();
        }
    }
    /*!
     * \brief Insert a new row.
     * \param[in] row - The row index at which the new row is to be inserted.
     * \param[in] defaultCols - The default number of columns for the new row.
     *
     * Insert a new row at a given row index in the grid. If the row index is
     * out of range a std::out_of_range exception is thrown.
     */
    void InsertRow(size_t row, size_t defaultCols = 0)
    {
        if (row >= GetRowCount())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("row out of range"));
        }

        m_grid.emplace(std::next(m_grid.begin(), row), defaultCols);
    }
    /*!
     * \brief Insert a new column in all rows.
     * \param[in] col - The column index at which the new column is to be inserted.
     *
     * The column is only inserted if the column index is within range of
     * the row in the grid otherwise a column is not added to the row.
     */
    // cppcheck-suppress functionConst
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
    // cppcheck-suppress functionConst
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
     * \param[in] filename - The full path name of the CSV file to load.
     * \param[in] options - Cell formating options.
     * \param[in] firstRowToLoad - (Optional) First row to load into the grid (zero-based).
     * \param[in] maxNumRowsToLoad - (Optional) Limit number of rows read in to grid.
     *
     * Create a grid object from a CSV file. If cells are wrapped in double
     * quotes in the CSV file then set options = doubleQuotedCells else set
     * options = simpleCells. If the file stream cannot be created or opened
     * the a std::runtime_error exception is thrown.
     */
    void LoadFromCSVFile(const std::string& filename, eCellFormatOptions options,
                         size_t firstRowToLoad   = 0,
                         size_t maxNumRowsToLoad = std::numeric_limits<size_t>::max())
    {
        std::ifstream csvfile{filename.c_str()};

        if (!csvfile.is_open())
        {
            std::string err("failed to create file stream for loading: ");
            err.append(filename);

            BOOST_THROW_EXCEPTION(std::runtime_error(err));
        }

        m_grid.clear();
        size_t      rowCount = 0;
        std::string row;
        bool        rowComplete = false;

        while (csvfile.good() && (m_grid.size() < maxNumRowsToLoad))
        {
            std::string line;
            std::getline(csvfile, line);
            string_utils::PackStdString(line);

            if ((csvfile.tellg() == csvfile.gcount()) || csvfile.eof())
            {
                if (line.compare("") == 0)
                {
                    break;
                }
            }

            // Because a cell may have line breaks within it we have to
            // accumulate lines from the file until we have a complete row.
            // But as we join the lines back to gether to form a complete
            // row we must add back in the new-line char for each line end.
            if (!row.empty())
            {
                row.append("\n");
            }

            row.append(line);

            if (ContainsEndOfRow(row))
            {
                rowComplete = true;
            }

            if ((rowCount >= firstRowToLoad) && rowComplete)
            {
                m_grid.emplace_back(row, options);
            }

            if (rowComplete)
            {
                ++rowCount;
                rowComplete = false;
                row.clear();
            }
        }

        csvfile.close();
    }
    /*!
     * \brief Save the grid to a CSV file.
     * \param[in] filename - The full path name of the CSV file to load.
     * \param[in] option - (Optional) Save to file options: append to or overwrite existing file.
     *
     * Create a CSV file from a grid object. If the file stream cannot
     * be created or opened the a std::runtime_error exception
     * is thrown.
     */
    void SaveToCsvFile(const std::string& filename,
                       eSaveToFileOptions option = eSaveToFileOptions::truncate) const
    {
        std::ofstream csvfile;

        if (option == eSaveToFileOptions::append)
        {
            csvfile.open(filename.c_str(), std::ofstream::app);
        }
        else
        {
            csvfile.open(filename.c_str(), std::ofstream::trunc);
        }

        if (!csvfile.is_open())
        {
            std::string err("failed to create file stream for saving: ");
            err.append(filename);

            BOOST_THROW_EXCEPTION(std::runtime_error(err));
        }

        OutputCsvGridToStream(csvfile);
        csvfile.close();
    }

private:
    /*!
     * \brief Output the grid to a stream object.
     * \param[in] os - The stream object.
     *
     * Write the grid in CSV format to a stream object.
     */
    void OutputCsvGridToStream(std::ostream& os) const
    {
        size_t row = 0;

        for (const auto& rowItem : m_grid)
        {
            rowItem.OutputRowToStream(os);

            if (row++ < GetRowCount() - 1)
            {
                os << std::endl;
            }
        }
    }
    /*!
     * \brief Check if row string contains the actual end of the CSV row.
     * \param[in] row - The row string object.
     * \return True if contains valid row end, false otherwise.
     *
     * If row contains an odd number of " chars then there must be a new line
     * within a cell on the real row i nthe CSV file. If there are 0 or an
     * even number of " chars in row then it must represent a complete row from
     * the CSV file.
     */
    static bool ContainsEndOfRow(const std::string& row)
    {
        std::string::difference_type numQuotes = std::count(row.begin(), row.end(), '"');
        return (numQuotes % 2) == 0;
    }

private:
    /*! \brief The grid row data. */
    container_type m_grid{};
};

} // namespace csv_grid
} // namespace core_lib

#endif // CSVGRIDMAIN
