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
 * \file MemoryUtils.h
 * \brief File containing memory utils declaration.
 */
#ifndef MEMORYUTILS_H
#define MEMORYUTILS_H

#include <algorithm>
#include <iterator>

namespace hgl
{

/*!
 * \brief Function to zero fill bytes of a POD object.
 * \param[in] o - Reference to POD object to zero fill.
 */
template <typename T> void ZeroPodObject(T& o)
{
    // We don't use memset because under certain circumstances it can
    // be optimised out by the compiler and end up not filling the
    // bytes with 0s.
    auto firstByte = reinterpret_cast<char*>(&o);
    auto lastByte  = std::next(firstByte, static_cast<int>(sizeof(o)));
    std::fill(firstByte, lastByte, 0);
}

} // namespace hgl

#endif // MEMORYUTILS_H
