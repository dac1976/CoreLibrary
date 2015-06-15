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
 * \file CsvGrid.h
 * \brief File containing declarations relating the CsvGrid objects.
 */

#ifndef CSVGRID
#define CSVGRID
#include <vector>

#include "CsvGridMain.h"
#include <list>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The csv_grid namespace. */
namespace csv_grid {

/*! \brief Typedef to CsvGrid object using std::vector as underlying container type. */
typedef TCsvGrid<std::vector> CsvGridV;
/*! \brief Typedef to RowV object using CSVGridV::row_type. */
typedef CsvGridV::row_type RowV;
/*! \brief Typedef to CsvGrid object using std::list as underlying container type. */
typedef TCsvGrid<std::list> CsvGridL;
/*! \brief Typedef to RowL object using CSVGridL::row_type. */
typedef CsvGridL::row_type RowL;
/*! \brief Typedef our default CsvGrid object to CSVGridL. */
typedef CsvGridL CsvGrid;
/*! \brief Typedef to Row object using CSVGrid::row_type. */
typedef CsvGrid::row_type Row;

}// namespace csv_grid
}// namespace core_lib

#endif // CSVGRID
