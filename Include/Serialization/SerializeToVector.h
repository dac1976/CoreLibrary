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
 * \file SerializeToVector.h
 * \brief File containing utilities to serialize objects to byte vectors.
 */

#ifndef SERIALIZETOVECTOR
#define SERIALIZETOVECTOR

#include "CoreLibraryDllGlobal.h"
#include "SerializationIncludes.h"
#include <vector>
#include <sstream>
#include <iterator>
#include <type_traits>
#include <algorithm>
#include <cereal/types/vector.hpp>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The serialize namespace. */
namespace serialize
{

/*! \brief Typedef for char vector. */
using char_vector_t = std::vector<char>;

/*! \brief In archive placeholder struct for serializing POD objects. */
struct CORE_LIBRARY_DLL_SHARED_API raw_iarchive
{
};

/*! \brief Out archive placeholder struct for serializing POD objects. */
struct CORE_LIBRARY_DLL_SHARED_API raw_oarchive
{
};

/*! \brief The archives namespace. */
namespace archives
{

/*! \brief Typedef to output portable binary archive. */
using out_port_bin_t = cereal::PortableBinaryOutputArchive;
/*! \brief Typedef to output binary archive. */
using out_bin_t = cereal::BinaryOutputArchive;
/*! \brief Typedef to output xml archive. */
using out_xml_t = cereal::XMLOutputArchive;
/*! \brief Typedef to output json archive. */
using out_json_t = cereal::JSONOutputArchive;
/*! \brief Typedef to output raw archive. */
using out_raw_t = raw_oarchive;
/*! \brief Typedef to input portable binary archive. */
using in_port_bin_t = cereal::PortableBinaryInputArchive;
/*! \brief Typedef to input binary archive. */
using in_bin_t = cereal::BinaryInputArchive;
/*! \brief Typedef to input xml archive. */
using in_xml_t = cereal::XMLInputArchive;
/*! \brief Typedef to input json archive. */
using in_json_t = cereal::JSONInputArchive;
/*! \brief Typedef to input raw archive. */
using in_raw_t = raw_iarchive;

} // namespace archives

/*! \brief The implementation namespace. */
namespace impl
{

/*!
 * \brief Serialization to char vector implementation.
 *
 * Typically the template argument A is one of the
 * output archive typedefs listed above or a custom
 * output archive that is compatible.
 */
template <typename T, typename A> struct ToCharVectorImpl
{
    /*!
     * \brief Function operator
     * \param[in] object - Object to serialize
     * \return Char vector containing serialized object
     */
    char_vector_t operator()(const T& object) const
    {
        std::stringstream os;
        // Reduce scope of archive to make sure it has
        // flushed its contents to the stream before
        // we try and do anything with it.
        {
            A oa(os);
            // CEREAL_NVP is required to fully support xml archives.
            oa(CEREAL_NVP(object));
        }

        char_vector_t charVector;
        charVector.reserve(os.str().size());
        charVector.assign(std::istreambuf_iterator<char>(os), std::istreambuf_iterator<char>());
        return charVector;
    }
};

/*! \brief Serialization to char vector implementation, specialization for POD. */
template <typename T> struct ToCharVectorImpl<T, archives::out_raw_t>
{
    /*!
     * \brief Function operator
     * \param[in] object - Object to serialize
     * \return Char vector containing serialized object
     */
    char_vector_t operator()(const T& object) const
    {
        char_vector_t charVector;

        if (!std::is_pod<T>::value)
        {
            return charVector;
        }

        auto begin = reinterpret_cast<char const*>(&object);
        auto end   = std::next(begin, static_cast<int>(sizeof(T)));
        std::copy(begin, end, std::back_inserter(charVector));
        return charVector;
    }
};

/*!
 * \brief Deserialization to object implementation.
 *
 * Typically the template argument A is one of the
 * input archive typedefs listed above or a custom
 * input archive that is compatible.
 */
template <typename T, typename A> struct ToObjectImpl
{
    /*!
     * \brief Function operator
     * \param[in] charVector - Char vector containing serialized object
     * \return Deserialized object
     */
    T operator()(const char_vector_t& charVector) const
    {
        std::stringstream is;
        std::copy(charVector.begin(), charVector.end(), std::ostream_iterator<char>(is));
        T object;
        // Reduce scope of archive to make sure it has
        // flushed its contents to the stream before
        // we try and do anything with it.
        {
            A ia(is);
            // CEREAL_NVP is required to fully support xml archives.
            ia(CEREAL_NVP(object));
        }

        return object;
    }
};

/*! \brief Deserialization to object implementation, specialization for POD. */
template <typename T> struct ToObjectImpl<T, archives::in_raw_t>
{
    /*!
     * \brief Function operator
     * \param[in] charVector - Byte vector containing serialized object
     * \return Deserialized object
     */
    T operator()(const char_vector_t& charVector) const
    {
        T object{};

        if (!std::is_pod<T>::value || (charVector.size() != sizeof(T)))
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
 *
 * Convenience function to use for serializing objects to a char vector. Using this function is
 * preferred to directly using ToCharVectorImpl functors.
 */
template <typename T, typename OA = archives::out_port_bin_t>
char_vector_t ToCharVector(const T& object)
{
    return impl::ToCharVectorImpl<T, OA>()(object);
}

/*!
 * \brief Deserialize a char vector into a corresponding object.
 * \param[in] charVector - A char vector containing a boost serialized object of type T.
 * \return A serializable object of type T to receive deserialized vector.
 *
 * Convenience function to use for deserializing a char vector (containing serialized data created
 * using ToCharVector). Using this function is preferred to directly using ToObjectImpl functors.
 */
template <typename T, typename IA = archives::in_port_bin_t>
T ToObject(const char_vector_t& charVector)
{
    return impl::ToObjectImpl<T, IA>()(charVector);
}

} // namespace serialize
} // namespace core_lib

#endif // #define SERIALIZETOVECTOR
