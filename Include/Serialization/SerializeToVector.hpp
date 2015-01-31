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
 * \file SerializeToVector.hpp
 * \brief File containing utilities to serialize objects to byte vectors.
 */

#ifndef SERIALIZETOVECTOR
#define SERIALIZETOVECTOR

#include "SerializationIncludes.hpp"
#include <vector>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The serialize namespace. */
namespace serialize {

/*! \brief Typedef for char vector. */
typedef std::vector<char> char_vector_t;

/*!
 * \brief Serialize an object into a char vector.
 * \param[in] object - A boost serializable object of type T.
 * \return A char vector to receive serialized object.
 */
template <typename T, typename A = eos::portable_oarchive>
char_vector_t ToCharVector(const T& object)
{
    char_vector_t charVector;
	boost::iostreams::filtering_ostream os(boost::iostreams::back_inserter(charVector));
	A oa(os);
	// BOOST_SERIALIZATION_NVP is required to fully support xml_oarchive
	oa << BOOST_SERIALIZATION_NVP(object);
	return charVector;
}

/*!
 * \brief Deserialize a char vector into a corresponding object.
 * \param[in] charVector - A char vector containing a boost serialized object of type T.
 * \return A boost serializable object of type T to receive deserialized vector.
 */
template <typename T, typename A = eos::portable_iarchive>
T ToObject(const char_vector_t& charVector)
{
	boost::iostreams::filtering_istream is(boost::make_iterator_range(charVector));
	A ia(is);
	T object;
	// BOOST_SERIALIZATION_NVP is required to fully support xml_iarchive
	ia >> BOOST_SERIALIZATION_NVP(object);
	return object;
}

} //namespace serialize
} //namespace core_lib

#endif // #define SERIALIZETOVECTOR