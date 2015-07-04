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
 * \file CsvGridMain.h
 * \brief File containing declarations relating the TCsvGrid class.
 */

#ifndef CSVGRIDMAIN
#define CSVGRIDMAIN

#include "Platform/PlatformDefines.h"

#include <fstream>
#include <limits>
#ifdef __USE_EXPLICIT_MOVE__
    #include <utility>
#endif
#include "CsvGridRow.h"

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
	 * \param[in] message - A user specified message string.
	 */
	explicit xCsvGridDimensionError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xCsvGridDimensionError();
	/*! \brief Copy constructor. */
    xCsvGridDimensionError(const xCsvGridDimensionError&) = default;
	/*! \brief Copy assignment operator. */
    xCsvGridDimensionError& operator=(const xCsvGridDimensionError&) = default;
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
	 * \param[in] message - A user specified message string.
	 */
	explicit xCsvGridRowOutOfRangeError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xCsvGridRowOutOfRangeError();
	/*! \brief Copy constructor. */
    xCsvGridRowOutOfRangeError(const xCsvGridRowOutOfRangeError&) = default;
	/*! \brief Copy assignment operator. */
    xCsvGridRowOutOfRangeError& operator=(const xCsvGridRowOutOfRangeError&) = default;
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
	 * \param[in] message - A user specified message string.
	 */
	explicit xCsvGridCreateFileStreamError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xCsvGridCreateFileStreamError();
	/*! \brief Copy constructor. */
    xCsvGridCreateFileStreamError(const xCsvGridCreateFileStreamError&) = default;
	/*! \brief Copy assignment operator. */
    xCsvGridCreateFileStreamError& operator=(const xCsvGridCreateFileStreamError&) = default;
};

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
template<template<class, class> class C, class T = Cell>
class TCsvGrid final
{
public:
	/*! \brief typedef for row type */
	typedef TRow<C, T> row_type;
	/*! \brief typedef for container type */
	typedef C<row_type, std::allocator<row_type>> container_type;
	/*! \brief Default constructor. */
	TCsvGrid() = default;
	/*! \brief Copy constructor. */
	TCsvGrid(const TCsvGrid&) = default;
#ifdef __USE_EXPLICIT_MOVE__
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
	 * columns. If rows or columns are 0 then xCsvGridDimensionError exception
	 * is thrown.
	 */
	TCsvGrid(const size_t rows, const size_t cols)
	{
		if ((rows == 0) || (cols == 0))
		{
			BOOST_THROW_EXCEPTION(xCsvGridDimensionError());
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
	 * options = simpleCells. Throw a xCsvGridCreateFileStreamError exceptions
	 * if the file cannot be loaded.
	 */
	TCsvGrid(const std::string& filename, const eCellFormatOptions options)
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
		: m_grid{rows}
	{
	}
	/*! \brief Ddestructor. */
	~TCsvGrid() = default;
	/*! \brief Copy assignment operator. */
	TCsvGrid& operator=(const TCsvGrid&) = default;
#ifdef __USE_EXPLICIT_MOVE__
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
	 * \note If the index is out of bounds a xCsvGridRowOutOfRangeError exception is thrown.
	 */
	row_type& operator[](const size_t row)
	{
		if (row >= GetRowCount())
		{
			BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
		}

		return *std::next(m_grid.begin(), row);
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
	 * If the index is out of bounds a xCsvGridRowOutOfRangeError
	 * exception is thrown.
	 */
	size_t GetColCount(const size_t row) const
	{
		if (row >= GetRowCount())
		{
			BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
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
	void SetRowCount(const size_t rows, const size_t defaultCols = 0)
	{
		m_grid.resize(rows, row_type(defaultCols));
	}
	/*!
	 * \brief Add a new row.
	 * \param[in] cols - The default number of cells for the new row.
	 *
	 * Resize the grid, adding a new row with the given number of cells.
	 */
	void AddRow(const size_t cols = 0)
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
	 * out of range a xCsvGridRowOutOfRangeError exception is thrown.
	 */
	void InsertRow(const size_t row, const size_t defaultCols = 0)
	{
		if (row >= GetRowCount())
		{
			BOOST_THROW_EXCEPTION(xCsvGridRowOutOfRangeError());
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
    void InsertColumnInAllRows(const size_t col)
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
	 * the a xCsvGridCreateFileStreamError exception is thrown.
	 */
	void LoadFromCSVFile(const std::string& filename
						 , const eCellFormatOptions options
						 , const size_t firstRowToLoad = 0
						 , const size_t maxNumRowsToLoad = std::numeric_limits<size_t>::max())
	{
		std::ifstream csvfile{filename.c_str()};

		if (!csvfile.is_open())
		{
			BOOST_THROW_EXCEPTION(
						xCsvGridCreateFileStreamError(std::string("failed to create file stream for loading: ")
													  + filename));
		}

		m_grid.clear();
		size_t rowCount = 0;

		while(csvfile.good() && (m_grid.size() < maxNumRowsToLoad))
		{
			std::string line;
			std::getline(csvfile, line);
			string_utils::PackStdString(line);

			if ((csvfile.tellg() == csvfile.gcount())
				|| csvfile.eof())
			{
				if (line == "")
				{
					break;
				}
			}

			if (rowCount >= firstRowToLoad)
			{
				m_grid.emplace_back(line, options);
			}

			++rowCount;
		}

		csvfile.close();
	}
	/*!
	 * \brief Save the grid to a CSV file.
	 * \param[in] filename - The full path name of the CSV file to load.
	 * \param[in] option - (Optional) Save to file options: append to or overwrite existing file.
	 *
	 * Create a CSV file from a grid object. If the file stream cannot
	 * be created or opened the a xCsvGridCreateFileStreamError exception
	 * is thrown.
	 */
	void SaveToCsvFile(const std::string& filename
					   , const eSaveToFileOptions option = eSaveToFileOptions::truncate) const
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
			BOOST_THROW_EXCEPTION(
						xCsvGridCreateFileStreamError(std::string("failed to create file stream for saving: ")
													  + filename));
		}

		OutputCsvGridToStream(csvfile);
		csvfile.close();
	}

private:
	/*! \brief The grid row data. */
	container_type m_grid;
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

			if (row++ < GetRowCount()-1)
			{
				os << std::endl;
			}
		}

	}
};

}// namespace csv_grid
}// namespace core_lib

#endif // CSVGRIDMAIN
