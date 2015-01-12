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
 * \file SerializationIncludes.hpp
 * \brief File containing includes for boost serialization.
 */

#ifndef SERIALIZATIONINCLUDES_HPP
#define SERIALIZATIONINCLUDES_HPP

#include "boost/archive/portable_oarchive.hpp"
#include "boost/archive/portable_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "boost/archive/xml_iarchive.hpp"
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/device/back_inserter.hpp"
#include "boost/range/iterator_range.hpp"

namespace boost_arch = boost::archive;

#endif // #define SERIALIZATIONINCLUDES_HPP
