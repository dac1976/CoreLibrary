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
 * \file CsvGrid.h
 * \brief File containing declarations relating the CsvGrid objects.
 */

#ifndef CSVGRID
#define CSVGRID

#include <vector>
#include <list>
#include "CsvGridMain.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The csv_grid namespace. */
namespace csv_grid
{

/*! \brief Typedef to CsvGrid object using std::vector as underlying container type. More efficient
 * when grid has a fixed size.*/
using CsvGridV = TCsvGrid<std::vector>;
/*! \brief Typedef to RowV object using CSVGridV::row_type. */
using RowV = CsvGridV::row_type;
/*! \brief Typedef to CsvGrid object using std::list as underlying container type. More efficient
 * when grid will be dynamically resized.*/
using CsvGridL = TCsvGrid<std::list>;
/*! \brief Typedef to RowL object using CSVGridL::row_type. */
using RowL = CsvGridL::row_type;
/*! \brief Typedef our default general use grid object to CSVGridL. */
using CsvGrid = CsvGridL;
/*! \brief Typedef to Row object using CSVGrid::row_type. */
using Row = CsvGrid::row_type;

/*! \brief Typedef to CsvGrid object using std::vector as underlying container type. More efficient
 * when grid has a fixed size and only contains numerical data (stored as double type).*/
using CsvGridVD = TCsvGrid<std::vector, CellDouble>;
/*! \brief Typedef to RowV object using CSVGridV::row_type. */
using RowV = CsvGridV::row_type;
/*! \brief Typedef to CsvGrid object using std::list as underlying container type. More efficient
 * when grid will be dynamically resized and only contains numerical data (stored as double type).*/
using CsvGridLD = TCsvGrid<std::list, CellDouble>;
/*! \brief Typedef to RowL object using CSVGridL::row_type. */
using RowL = CsvGridL::row_type;
/*! \brief Typedef for grid when cells only contain double precision data. */
using CsvGridD = CsvGridLD;
/*! \brief Typedef to Row object using CSVGrid::row_type. */
using RowD = CsvGridD::row_type;

} // namespace csv_grid
} // namespace core_lib

#endif // CSVGRID
