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
#include <type_traits>
#include <algorithm>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The serialize namespace. */
namespace serialize {

/*! \brief Typedef for char vector. */
typedef std::vector<char> char_vector_t;

/*! \brief In archive placeholder struct for non-boost serialized POD objects. */
struct raw_iarchive
{
};

/*! \brief Out archive placeholder struct for non-boost serialized POD objects. */
struct raw_oarchive
{
};

/*! \brief The archives namespace. */
namespace archives {

typedef eos::portable_oarchive      out_port_bin_t;
typedef boost_arch::binary_oarchive out_bin_t;
typedef boost_arch::xml_oarchive    out_xml_t;
typedef boost_arch::text_oarchive   out_txt_t;
typedef raw_oarchive                out_raw_t;

typedef eos::portable_iarchive      in_port_bin_t;
typedef boost_arch::binary_iarchive in_bin_t;
typedef boost_arch::xml_iarchive    in_xml_t;
typedef boost_arch::text_iarchive   in_txt_t;
typedef raw_iarchive                in_raw_t;

} // namespace archives

/*! \brief The impl namespace. */
namespace impl
{

/*! \brief Serialization to char vector impl. */
template <typename T, typename A>
struct ToCharVectorImpl
{
    char_vector_t operator()(const T& object) const
    {
        char_vector_t charVector;
        boost::iostreams::filtering_ostream os(boost::iostreams::back_inserter(charVector));
        A oa(os);
        // BOOST_SERIALIZATION_NVP is required to fully support xml_oarchive
        oa << BOOST_SERIALIZATION_NVP(object);
        return charVector;
    }
};

/*! \brief Serialization to char vector impl, specialization for POD. */
template <typename T>
struct ToCharVectorImpl<T, archives::out_raw_t>
{
    char_vector_t operator()(const T& object) const
    {
        char_vector_t messageBuffer;

        if (!std::is_pod<T>::value)
        {
            return messageBuffer;
        }

        const char* begin = reinterpret_cast<const char*>(&object);
        const char* end = begin + sizeof(T);

        std::copy(begin, end, std::back_inserter(messageBuffer));

        return messageBuffer;
    }
};

/*! \brief Deserialization to object impl. */
template <typename T, typename A>
struct ToObjectImpl
{
    T operator()(const char_vector_t& charVector) const
    {
        boost::iostreams::filtering_istream is(boost::make_iterator_range(charVector));
        A ia(is);
        T object;
        // BOOST_SERIALIZATION_NVP is required to fully support xml_iarchive
        ia >> BOOST_SERIALIZATION_NVP(object);
        return object;
    }
};

/*! \brief Deserialization to object impl, specialization for POD. */
template <typename T>
struct ToObjectImpl<T, archives::in_raw_t>
{
    T operator()(const char_vector_t& charVector) const
    {
        T object;

        if (!std::is_pod<T>::value
            || (charVector.size() != sizeof(T)))
        {
            return object;
        }

        memcpy(&object, charVector.data(), charVector.size());

        return object;
    }
};

}

/*!
 * \brief Serialize an object into a char vector.
 * \param[in] object - A boost serializable object of type T.
 * \return A char vector to receive serialized object.
 */
template <typename T, typename OA = archives::out_port_bin_t>
char_vector_t ToCharVector(const T& object)
{
    return impl::ToCharVectorImpl<T, OA>()(object);
}

/*!
 * \brief Deserialize a char vector into a corresponding object.
 * \param[in] charVector - A char vector containing a boost serialized object of type T.
 * \return A boost serializable object of type T to receive deserialized vector.
 */
template <typename T, typename IA = archives::in_port_bin_t>
T ToObject(const char_vector_t& charVector)
{
    return impl::ToObjectImpl<T, IA>()(charVector);
}

} //namespace serialize
} //namespace core_lib

#endif // #define SERIALIZETOVECTOR
