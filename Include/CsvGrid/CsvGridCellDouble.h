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
 * \file CsvGridCellDouble.h
 * \brief File containing declarations relating the CellDouble class.
 */

#ifndef CSVGRIDCELLDOUBLE
#define CSVGRIDCELLDOUBLE

#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"
#include <string>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The csv_grid namespace. */
namespace csv_grid
{

/*!
 * \brief Class defining a single cell within a row of the grid.
 *
 * This class provides the definition of the interface for a cell within
 * a row in the grid. A cell can be thought of as the value in a given
 * column index within the grid. This version is specialised for double
 * precision numerical data.
 */
class CORE_LIBRARY_DLL_SHARED_API CellDouble final
{
public:
    /*! \brief Default constructor. */
    CellDouble() = default;
    /*! \brief Copy constructor. */
    CellDouble(const CellDouble&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move constructor. */
    CellDouble(CellDouble&& cell);
#else
    /*! \brief Move constructor. */
    CellDouble(CellDouble&&) = default;
#endif
    /*!
     * \brief Initializing constructor
     * \param[in] value - The initial value.
     *
     * Initialise the cell with a double.
     */
    explicit CellDouble(const double value);
    /*!
     * \brief Initializing constructor
     * \param[in] value - The initial value.
     *
     * Initialise the cell with a string.
     */
    explicit CellDouble(const std::string& value);
    /*! \brief Destructor. */
    ~CellDouble() = default;
    /*! \brief Copy assignment operator. */
    CellDouble& operator=(const CellDouble&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move assignment operator. */
    CellDouble& operator=(CellDouble&& cell);
#else
    /*! \brief Move assignment operator. */
    CellDouble& operator=(CellDouble&&) = default;
#endif
    /*!
     * \brief Value assignment operator.
     * \param[in] rhs - The value to assign.
     * \return Modified Cell object.
     *
     * Assign to a double precision floating point value.
     */
    CellDouble& operator=(const double rhs);
    /*!
     * \brief Value assignment operator.
     * \param[in] rhs - The value to assign.
     * \return Modified Cell object.
     *
     * Assign to a string.
     */
    CellDouble& operator=(const std::string& rhs);
    /*!
     * \brief Value method.
     * \return Get the underlying double value of the Cell.
     */
    double Value() const;
    /*!
     * \brief Cast operator.
     *
     * Cast the cell to a string.
     */
    operator std::string() const;
    /*!
     * \brief Cast operator.
     *
     * Cast the cell to a double.
     */
    operator double() const;

private:
    /*! \brief The cell's value data memeber. */
    double m_value{0.0};
};

} // namespace csv_grid
} // namespace core_lib

#endif // CSVGRIDCELLDOUBLE
