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
 * \brief File containing declarations relating the CsvGrid objects.
 */

#ifndef CSVGRID_HPP
#define CSVGRID_HPP

#include "CsvGrid/CsvGridMain.hpp"
#include <vector>
#include <list>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The csv_grid namespace. */
namespace csv_grid {

/*! \brief Dummy container reserver functor - does nothing. */
template<template<class, class> class C, class T>
struct DummyReserver
{
    typedef T value_type;
    typedef C<value_type, std::allocator<value_type>> container_type;

    void operator() (container_type& /*container*/, size_t /*size*/) const
    {
        // Do nothing.
    }
};

/*! \brief Vector container reserver functor. */
struct VectorReserver
{
    void operator() (std::vector<Cell>& container, size_t size) const
    {
        container.reserve(size);
    }
};

/*! \brief Typedef to CsvGrid object using std::vector as underlying container type. */
typedef TCsvGrid<std::vector, VectorReserver> CsvGridV;
/*! \brief Typedef to RowV object using CSVGridV::row_type. */
typedef CsvGridV::row_type RowV;
/*! \brief Typedef to CsvGrid object using std::list as underlying container type. */
typedef TCsvGrid<std::list, DummyReserver<std::list, Cell>> CsvGridL;
/*! \brief Typedef to RowL object using CSVGridV::row_type. */
typedef CsvGridL::row_type RowL;
/*! \brief Typedef to CsvGrid object using CSVGridL. */
typedef CsvGridL CsvGrid;
/*! \brief Typedef to Row object using CSVGrid::row_type. */
typedef CsvGrid::row_type Row;

}// namespace csv_grid
}// namespace core_lib

#endif // CSVGRID_HPP
