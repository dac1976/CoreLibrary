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
 * \file CsvGridRow.h
 * \brief File containing declarations relating the CSVGridRow class.
 */

#ifndef CSVGRIDROW
#define CSVGRIDROW

#include <initializer_list>
#include <ostream>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#ifdef USE_EXPLICIT_MOVE_
#include <utility>
#endif
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/throw_exception.hpp>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"
#include "StringUtils/StringUtils.h"
#include "CsvGridCell.h"
#include "CsvGridCellDouble.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The csv_grid namespace. */
namespace csv_grid
{

/*!
 * \brief Cell format options enumeration.
 *
 * This enumeration is used to control the format of the cells within a row of
 * the CSV grid, in particular whether or not they are surrounded by double qoutes
 * or not.
 */
enum class eCellFormatOptions
{
    /*!
     * \brief All cells are simple and not wrapped in double quotes, e.g. x1,x2,x3. This format is
     * faster than using double-quoted cells.
     */
    simpleCells,
    /*!
     * \brief Cells may contain special, escaped, characters and are in double qoutes, e.g.
     * "x1","x2","x3". This format is slower than simple formatting as care has to be taken with
     * parsing special characters.
     */
    doubleQuotedCells
};

/*! \brief The csv_grid namespace. */
namespace reserver
{

/*! \brief Default container reserver functor. */
template <template <class, class> class C, class T> class ContainerReserver
{
public:
    /*! \brief Typedef to container type. */
    using container_type = C<T, std::allocator<T>>;
    /*!
     * \brief Function operator.
     * \param[in] container - Container to reserve space in.
     * \param[in] size - Number of items to reserve space for.
     *
     * This default functor does nothing as it is intended
     * for containers that do not have a built-in reserve
     * method, such as std::list.
     */
    void operator()(container_type& container, size_t size) const
    {
        (void)container;
        (void)size;

        // Do nothing.
    }
};

/*! \brief Container reserver functor specialisation for vectors. */
template <class T> class ContainerReserver<std::vector, T>
{
public:
    /*! \brief Typedef to container type. */
    using container_type = std::vector<T>;
    /*!
     * \brief Function operator.
     * \param[in] container - Container to reserve space in.
     * \param[in] size - Number of items to reserve space for.
     *
     * This version of the functor calls the vector's built-in
     * reserve method.
     */
    void operator()(container_type& container, size_t size) const
    {
        container.reserve(size);
    }
};

} // namespace reserver

// forward declaration for using in Row class.
template <template <class, class> class C, class T> class TCsvGrid;

/*!
 * \brief Class defining a row of the grid.
 *
 * This class provides the definition of the interface for a row within
 * the grid. A row contains cells and each cell's position represents a column
 * within the grid.
 */
template <template <class, class> class C, class T = Cell> class TRow final
{
public:
    /*! \brief typedef for container type */
    using container_type = C<T, std::allocator<T>>;
    /*! \brief typedef for container type */
    using cell_type = T;
    /*! \brief Friend declaration of CsvGrid so it can have private access to its rows. */
    friend class TCsvGrid<C, T>;
    /*! \brief Default constructor. */
    TRow() = default;
    /*! \brief Copy constructor. */
    TRow(const TRow&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move constructor. */
    TRow(TRow&& row)
    {
        *this = std::move(row);
    }
#else
    /*! \brief Move constructor. */
    TRow(TRow&&) = default;
#endif
    /*!
     * \brief Initializing constructor
     * \param[in] numCols - The initial number of columns.
     *
     * Create the row with an initial number of columns.
     */
    explicit TRow(size_t numCols)
        : m_cells(numCols)
    {
    }
    /*!
     * \brief Initializing constructor
     * \param[in] line - The initial value string - a comma separated line from a CSV file.
     * \param[in] options - The line format.
     *
     * Create the row with an initial value and specify whether cells
     * are wrapped in double quotes in the CSV file.
     */
    TRow(const std::string& line, eCellFormatOptions options)
    {
        LoadRowFromCsvFileLine(line, options);
    }
    /*!
     * \brief Initializer list constructor
     * \param[in] cells - The initial list of cells.
     *
     * Create the row from the given list of cells.
     */
    // cppcheck-suppress noExplicitConstructor
    TRow(std::initializer_list<T> cells)
        : m_cells(cells)
    {
    }
    /*! \brief Destructor. */
    ~TRow() = default;
    /*! \brief Copy assignment operator. */
    TRow& operator=(const TRow&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move assignment operator. */
    TRow& operator=(TRow&& row)
    {
        std::swap(m_cells, row.m_cells);
        return *this;
    }
#else
    /*! \brief Move assignment operator. */
    TRow& operator=(TRow&&) = default;
#endif
    /*!
     * \brief Subscript operator.
     * \param[in] col - A 0-based column index.
     * \return The cell at the given column index.
     *
     * Retrieve the cell at a given column index within a row.
     *
     * \note
     * If the index is out of bounds a std::out_of_range
     * exception is thrown.
     */
    cell_type& operator[](size_t col)
    {
        if (col >= GetSize())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("col out of range"));
        }

        return *std::next(m_cells.begin(), col);
    }
    /*!
     * \brief Const subscript operator.
     * \param[in] col - A 0-based column index.
     * \return The cell at the given column index.
     *
     * Retrieve the cell at a given column index within a row.
     *
     * \note
     * If the index is out of bounds a std::out_of_range
     * exception is thrown.
     */
    const cell_type& operator[](size_t col) const
    {
        if (col >= GetSize())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("col out of range"));
        }

        return *std::next(m_cells.begin(), col);
    }
    /*!
     * \brief Get empty state of row.
     * \return True if row is empty.
     */
    bool Empty() const
    {
        return m_cells.empty();
    }
    /*!
     * \brief Get the number of columns.
     * \return The number of columns for this row.
     */
    size_t GetSize() const
    {
        return std::distance(m_cells.begin(), m_cells.end());
    }
    /*!
     * \brief Set the number of columns in the row.
     * \param[in] cols - The number of columns to set.
     *
     * If the number of columns are being increased then existing content
     * is preserved and new cells are added at the end of the row forming
     * the extra columns.
     */
    void SetSize(size_t cols)
    {
        m_cells.resize(cols);
    }
    /*!
     * \brief Add a column with the given value.
     * \param[in] value - The cell's value for the new column.
     *
     * The column count is increased by one and the new cell is initialised
     * with the given string.
     */
    template <typename V> void AddColumn(V value)
    {
        m_cells.emplace_back(value);
    }
    /*!
     * \brief Add a column with default value.
     *
     * The column count is increased by one and the new cell is initialised
     * default value from default cell constructor.
     */
    void AddColumn()
    {
        m_cells.emplace_back();
    }
    /*!
     * \brief Insert a new cell.
     * \param[in] col - The column index at which the new cell is to be inserted.
     * \param[in] value - The value to assign to the newly inserted cell.
     *
     * The column count is increased by one and the new cell is initialised
     * with the given string.
     *
     * \note
     * If the index is out of bounds a std::out_of_range
     * exception is thrown.
     */
    template <typename V> void InsertColumn(size_t col, V value)
    {
        if (col >= GetSize())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("col out of range"));
        }

        m_cells.emplace(std::next(m_cells.begin(), col), value);
    }
    /*!
     * \brief Insert a new cell.
     * \param[in] col - The column index at which the new cell is to be inserted.
     *
     * The column count is increased by one and the new cell is initialised
     * with the default cell constructor.
     *
     * \note
     * If the index is out of bounds a std::out_of_range
     * exception is thrown.
     */
    void InsertColumn(size_t col)
    {
        if (col >= GetSize())
        {
            BOOST_THROW_EXCEPTION(std::out_of_range("col out of range"));
        }

        m_cells.emplace(std::next(m_cells.begin(), col));
    }
    /*!
     * \brief Clear the cells' contents.
     *
     * The contents of each column's cell is cleared but the column count
     * remains unchanged.
     */
    void ClearCells()
    {
        std::fill(m_cells.begin(), m_cells.end(), Cell());
    }
    /*!
     * \brief Clear the entire row.
     *
     * The cells are completely removed from the row leaving the column
     * count as 0 afterwards.
     */
    void ResetRow()
    {
        m_cells.clear();
    }

private:
    /*!
     * \brief Load a row from a line in a CSV file.
     * \param[in] line - The line from the CSV file.
     * \param[in] options - The cell format options.
     *
     * Create a row by loading it from a line read in from a CSV file.
     * The options parameter is used to decide how to tokenize the line.
     */
    void LoadRowFromCsvFileLine(const std::string& line, eCellFormatOptions options)
    {
        if (options == eCellFormatOptions::doubleQuotedCells)
        {
            TokenizeLineQuoted(line);
        }
        else
        {
            TokenizeLine(line);
        }
    }
    /*!
     * \brief Write the row's contents to a stream object.
     * \param[in,out] os - The stream object to write to.
     *
     * The row's contents are formatted using CSV formating and output to the
     * stream object.
     */
    void OutputRowToStream(std::ostream& os) const
    {
        // for each row loop over its columns...
        size_t col = 0;

        for (const auto& cellItem : m_cells)
        {
            // let's get our cell value...
            std::string cell{static_cast<std::string>(cellItem)};

            // if string contains quotes, insert an
            // extra quote...
            size_t pos{cell.find('"')};

            while (pos < cell.length())
            {
                cell.insert(pos, "\"");
                pos = cell.find('"', pos + 2);
            }

            // if cell contains ',', '\n' or '\r' wrap it in quotes...
            if (cell.find_first_of(",\r\n") != std::string::npos)
            {
                std::string temp = "\"";
                temp.append(cell);
                temp.append("\"");
                cell.swap(temp);
            }

            // output corrected cell...
            os << cell;

            // add ',' if not last cell on current row...
            if (col++ < GetSize() - 1)
            {
                os << ",";
            }
        }
    }
    /*!
     * \brief Tokenize a row with double quoted cells.
     * \param[in] line - A CSV file's line of text.
     *
     * The comma separated row's tokens are extracted using a boost
     * tokenizer object. This does impact the performance slightly.
     */
    void TokenizeLineQuoted(const std::string& line)
    {
        using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
        Tokenizer                 tokzr{line};
        Tokenizer::const_iterator tokIter{tokzr.begin()};

        while (tokIter != tokzr.end())
        {
            // cppcheck-suppress eraseDereference
            std::string tok{*tokIter++};
            boost::trim(tok);
            m_cells.emplace_back(tok);
        }
    }
    /*!
     * \brief Tokenize a row with simple cells.
     * \param[in] line - A CSV file's line of text.
     *
     * The comma separated row's tokens are extracted using a
     * simple getline based algorithm. This versio ncannot handle
     * double quoted cells.
     */
    void TokenizeLine(const std::string& line)
    {
        std::stringstream line_ss{line};
        std::string       tok;

        while (std::getline(line_ss, tok, ','))
        {
            string_utils::PackStdString(tok);
            boost::trim(tok);
            m_cells.emplace_back(tok);
        }
    }

private:
    /*!  \brief The reservation function to use when initialising the container. */
    reserver::ContainerReserver<C, T> m_reserve{};
    /*!  \brief The row's cells. */
    container_type m_cells{};
};

} // namespace csv_grid
} // namespace core_lib

#endif // CSVGRIDROW
