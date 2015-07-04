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
 * \file CsvGridCell.h
 * \brief File containing declarations relating the CSVGridCell class.
 */

#ifndef CSVGRIDCELL
#define CSVGRIDCELL

#include "Platform/PlatformDefines.h"

#include <cstdint>
#include <string>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The csv_grid namespace. */
namespace csv_grid {

/*!
 * \brief Class defining a single cell within a row of the grid.
 *
 * This class provides the definition of the interface for a cell within
 * a row in the grid. A cell can be thought of as the value in a given
 * column index within the grid.
 */
class Cell final
{
public:
	/*! \brief Default constructor. */
	Cell() = default;
	/*! \brief Copy constructor. */
	Cell(const Cell&) = default;
#ifdef __USE_EXPLICIT_MOVE__
    /*! \brief Move constructor. */
    Cell(Cell&& cell);
#else
    /*! \brief Move constructor. */
    Cell(Cell&&) = default;
#endif
	/*!
	 * \brief Initializing constructor
     * \param[in] value - The initial value.
	 *
	 * Initialise the cell with a string.
	 */
	explicit Cell(const std::string& value);
	/*!
	 * \brief Initializing constructor
     * \param[in] value - The initial value.
	 *
	 * Initialise the cell with a 32bit integer value.
	 */
	explicit Cell(const int32_t value);
	/*!
	 * \brief Initializing constructor
     * \param[in] value - The initial value.
	 *
	 * Initialise the cell with a 64bit integer value.
	 */
	explicit Cell(const int64_t value);
	/*!
	 * \brief Initializing constructor
     * \param[in] value - The initial value.
	 *
	 * Initialise the cell with a double precision floating point value.
	 */
	explicit Cell(const double value);
	/*!
	 * \brief Initializing constructor
     * \param[in] value - The initial value.
	 *
	 * Initialise the cell with a long double precision floating point value.
	 */
	explicit Cell(const long double value);
	/*! \brief Destructor. */
	~Cell() = default;
	/*! \brief Copy assignment operator. */
    Cell& operator=(const Cell&) = default;
#ifdef __USE_EXPLICIT_MOVE__
    /*! \brief Move assignment operator. */
    Cell& operator=(Cell&& cell);
#else
    /*! \brief Move assignment operator. */
    Cell& operator=(Cell&&) = default;
#endif
	/*!
	 * \brief Value assignment operator.
     * \param[in] rhs - The value to assign.
	 * \return Modified Cell object.
	 *
	 * Assign to a string.
	 */
	Cell& operator=(const std::string& rhs);
	/*!
	 * \brief Value assignment operator.
     * \param[in] rhs - The value to assign.
	 * \return Modified Cell object.
	 *
	 * Assign to a 32bit integer value.
	 */
	Cell& operator=(const int32_t rhs);
	/*!
	 * \brief Value assignment operator.
     * \param[in] rhs - The value to assign.
	 * \return Modified Cell object.
	 *
	 * Assign to a 64bit integer value.
	 */
	Cell& operator=(const int64_t rhs);
	/*!
	 * \brief Value assignment operator.
     * \param[in] rhs - The value to assign.
	 * \return Modified Cell object.
	 *
	 * Assign to a double precision floating point value.
	 */
	Cell& operator=(const double rhs);
	/*!
	 * \brief Value assignment operator.
     * \param[in] rhs - The value to assign.
	 * \return Modified Cell object.
	 *
	 * Assign to a long double precision floating point value.
	 */
	Cell& operator=(const long double rhs);
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
     * \param[in] defval - The default value to return in case of a failure.
	 * \return Cell value as a 32bit integer.
	 *
	 * Convert the cell to a 32bit integer and if this cannot be
	 * performed safely return the default value instead.
	 */
	int32_t ToInt32Def(const int32_t defval) const __NOEXCEPT__;
	/*!
	 * \brief Conversion function
     * \param[in] defval - The default value to return in case of a failure.
	 * \return Cell value as a 64bit integer.
	 *
	 * Convert the cell to a 64bit integer and if this cannot be
	 * performed safely return the default value instead.
	 */
	int64_t ToInt64Def(const int64_t defval) const __NOEXCEPT__;
	/*!
	 * \brief Conversion function
     * \param[in] defval - The default value to return in case of a failure.
	 * \return Cell value as a double.
	 *
	 * Convert the cell to a double and if this cannot be performed
	 * safely return the default value instead.
	 */
	double ToDoubleDef(const double defval) const __NOEXCEPT__;
	/*!
	 * \brief Conversion function
     * \param[in] defval - The default value to return in case of a failure.
	 * \return Cell value as a long double.
	 *
	 * Convert the cell to a long double and if this cannot be performed
	 * safely return the default value instead.
	 */
	long double ToLongDoubleDef(const long double defval) const __NOEXCEPT__;

private:
	/*! \brief The cell's value data memeber. */
	std::string m_value;
};

}// namespace csv_grid
}// namespace core_lib

#endif // CSVGRIDCELL
