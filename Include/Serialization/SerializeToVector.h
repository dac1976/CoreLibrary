// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
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
#include <cstring>
#include <iterator>
#include <type_traits>
#include <algorithm>
#include <span>
#include <boost/throw_exception.hpp>
#include <cereal/types/vector.hpp>
#if defined(USE_FLATBUFFERS)
#include "flatbuffers/flatbuffers.h"
#endif

#define SERIALIZE_TO_STREAM_ARCHIVE(osa, o) osa(CEREAL_NVP(o))
#define DESERIALIZE_FROM_STREAM_ARCHIVE(isa, o) isa(CEREAL_NVP(o))

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The serialize namespace. */
namespace serialize
{

/*! \brief Typedef for char vector. */
using char_vector_t = std::vector<char>;
/*! \brief Typedef for char span. */
using char_span_buf_t = std::span<char>;
/*! \brief Typedef for char const span. */
using char_cspan_buf_t = std::span<const char>;

/*! \brief In archive placeholder struct for serializing POD objects. */
struct CORE_LIBRARY_DLL_SHARED_API raw_iarchive
{
};

/*! \brief Out archive placeholder struct for serializing POD objects. */
struct CORE_LIBRARY_DLL_SHARED_API raw_oarchive
{
};

/*! \brief In archive placeholder struct for serializing Google protobufs. */
struct CORE_LIBRARY_DLL_SHARED_API protobuf_iarchive
{
};

/*! \brief Out archive placeholder struct for serializing Google protobufs. */
struct CORE_LIBRARY_DLL_SHARED_API protobuf_oarchive
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
/*! \brief Typedef to output using Google protocol buffers. */
using out_protobuf_t = protobuf_oarchive;
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
/*! \brief Typedef to input using Google protocol buffers. */
using in_protobuf_t = protobuf_iarchive;

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
     *
     * This overload creates new memory.
     */
    char_vector_t operator()(const T& object) const
    {
        std::stringstream os;
        // Reduce scope of archive to make sure it has
        // flushed its contents to the stream before
        // we try and do anything with it.
        {
            A oa(os);
            // CEREAL_NVP / BOOST_SERIALIZATION_NVP is required to fully support xml archives.
            SERIALIZE_TO_STREAM_ARCHIVE(oa, object);
        }

        char_vector_t charVector{std::istreambuf_iterator<char>(os),
                                 std::istreambuf_iterator<char>()};
        return charVector;
    }

    /*!
     * \brief Function operator
     * \param[in] object - Object to serialize
     * \param[out] result - Char vector containing serialized object
     *
     * This overload uses the memory passed in and resizes if necessary.
     */
    void operator()(const T& object, char_vector_t& result) const
    {
        std::stringstream os;
        // Reduce scope of archive to make sure it has
        // flushed its contents to the stream before
        // we try and do anything with it.
        {
            A oa(os);
            // CEREAL_NVP / BOOST_SERIALIZATION_NVP is required to fully support xml archives.
            SERIALIZE_TO_STREAM_ARCHIVE(oa, object);
        }

        result.assign(std::istreambuf_iterator<char>(os), std::istreambuf_iterator<char>());
    }
};

/*! \brief Serialization to char vector implementation, specialization for POD. */
template <typename T> struct ToCharVectorImpl<T, archives::out_raw_t>
{
    /*!
     * \brief Function operator
     * \param[in] object - Object to serialize
     * \return Char vector containing serialized object
     *
     * This overload creates new memory.
     */
    char_vector_t operator()(const T& object) const
    {
        static_assert(std::is_trivially_copyable<T>::value, "object should be POD");

        auto          begin = reinterpret_cast<char const*>(&object);
        auto          end   = std::next(begin, static_cast<int>(sizeof(T)));
        char_vector_t charVector(begin, end);
        return charVector;
    }

    /*!
     * \brief Function operator
     * \param[in] object - Object to serialize
     * \param[out] result - Char vector containing serialized object
     *
     * This overload uses the memory passed in and resizes if necessary.
     */
    void operator()(const T& object, char_vector_t& result) const
    {
        static_assert(std::is_trivially_copyable<T>::value, "object should be POD");

        auto len = sizeof(T);
        auto begin = reinterpret_cast<char const*>(&object);
        auto end   = std::next(begin, static_cast<int>(len));
        result.resize(len);
        std::copy(begin, end, result.begin());
    }
};

/*! \brief Serialization to char vector implementation, specialization for Google protocol bufs. */
template <typename T> struct ToCharVectorImpl<T, archives::out_protobuf_t>
{
    /*!
     * \brief Function operator
     * \param[in] object - Object to serialize
     * \return Char vector containing serialized object
     *
     * This overload creates new memory.
     */
    char_vector_t operator()(const T& object) const
    {
        std::stringstream os;

        if (!object.SerializeToOstream(&os))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("failed to serialize protocol buffer"));
        }

        char_vector_t charVector{std::istreambuf_iterator<char>(os),
                                 std::istreambuf_iterator<char>()};
        return charVector;
    }

    /*!
     * \brief Function operator
     * \param[in] object - Object to serialize
     * \param[out] result - Char vector containing serialized object
     *
     * This overload uses the memory passed in and resizes if necessary.
     */
    void operator()(const T& object, char_vector_t& result) const
    {
        std::stringstream os;

        if (!object.SerializeToOstream(&os))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("failed to serialize protocol buffer"));
        }

        result.assign(std::istreambuf_iterator<char>(os), std::istreambuf_iterator<char>());
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
     * \param[in] charSpan - Char span containing serialized object
     * \return Deserialized object
     */
    T operator()(char_cspan_buf_t charSpan) const
    {
        std::stringstream is;
        std::copy(charSpan.begin(), charSpan.end(), std::ostream_iterator<char>(is));
        T object;
        // Reduce scope of archive to make sure it has
        // flushed its contents to the stream before
        // we try and do anything with it.
        {
            A ia(is);
            // CEREAL_NVP / BOOST_SERIALIZATION_NVP is required to fully support xml archives.
            DESERIALIZE_FROM_STREAM_ARCHIVE(ia, object);
        }

        return object;
    }
};

/*! \brief Deserialization to object implementation, specialization for POD. */
template <typename T> struct ToObjectImpl<T, archives::in_raw_t>
{
    /*!
     * \brief Function operator
     * \param[in] charSpan - Char span containing serialized object
     * \return Deserialized object
     */
    T operator()(char_cspan_buf_t charSpan) const
    {
        static_assert(std::is_trivially_copyable<T>::value, "object should be POD");

        if (charSpan.size() != sizeof(T))
        {
            BOOST_THROW_EXCEPTION(std::invalid_argument("buffer to object size mismatch"));
        }

        T object{};
        memcpy(&object, charSpan.data(), charSpan.size());
        return object;
    }
};

/*! \brief Deserialization to object implementation, specialization for Google protocol buffers. */
template <typename T> struct ToObjectImpl<T, archives::in_protobuf_t>
{
    /*!
     * \brief Function operator
     * \param[in] charVector - Char vector containing serialized object
     * \return Deserialized object
     */
    T operator()(char_cspan_buf_t charSpan) const
    {
        std::stringstream is;
        std::copy(charSpan.begin(), charSpan.end(), std::ostream_iterator<char>(is));
        T object;

        if (!object.ParseFromIstream(&is))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("failed to deserialize to protocol buffer"));
        }

        return object;
    }
};

} // namespace impl

/*!
 * \brief Serialize an object into a char vector.
 * \param[in] object - A boost serializable object of type T.
 * \return A char vector to receive serialized object.
 *
 * Convenience function to use for serializing objects to a char vector. Using this function is
 * preferred to directly using ToCharVectorImpl functors.
 *
 * This overload creates new memory.
 */
template <typename T, typename OA = archives::out_port_bin_t>
char_vector_t ToCharVector(const T& object)
{
    return impl::ToCharVectorImpl<T, OA>()(object);
}

/*!
* \brief Serialize an object into a char vector.
* \param[in] object - A boost serializable object of type T.
* \param[out] result -  A char vector to receive serialized object.
*
* Convenience function to use for serializing objects to a char vector. Using this function is
* preferred to directly using ToCharVectorImpl functors.
*
* This overload uses the memory passed in and resizes if necessary.
*/
template <typename T, typename OA = archives::out_port_bin_t>
void ToCharVector(const T& object, char_vector_t& result)
{
    impl::ToCharVectorImpl<T, OA>()(object, result);
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
T ToObject(char_cspan_buf_t charSpan)
{
    return impl::ToObjectImpl<T, IA>()(charSpan);
}

#if defined(USE_FLATBUFFERS)
/*!
* \brief Function to serialize object via flatbuffers
* \param[in] object - Object to serialize (A MyStructT generated by flatbuffers compiler)
* \return Char vector containing serialized object
*
* This overload creates new memory.
*
* Note that flatbuffer "compiler" generates a header containing the structures from
* the schema. It creates 2 instances of each structure one is a high-performance
* view(e.g. MyStruct) and the other is a more traditional C++ struct (MyStructT). The
* high-performance view is typically used for deserialization and the C++ struct is used
* for serialization.This overload is designed to work with the C++ struct version of the data.
* For convenience, with less performance, you'd fill the field1s in the C++ struct and
* then pass it to this function to serialize. For maximum performance, you'd fill
*  the fields in the high-performance view and then use the GetBufferPointer()
* and GetSize() functions to get a pointer and size of the serialized data to send
* directly without copying into a char vector like we do here.
*/
template <typename T, typename PackFunc>
char_vector_t ToCharVectorFlatBuf(const T& object, PackFunc packFunc)
{
    flatbuffers::FlatBufferBuilder builder;

    // packFunc(builder, &object) should return flatbuffers::Offset<SomeTable>
    auto root = packFunc(builder, &object, nullptr);
    builder.Finish(root);

    const auto* p = builder.GetBufferPointer(); // const uint8_t*
    const auto  n = builder.GetSize();          // size_t

    char_vector_t out(n);
    std::memcpy(out.data(), p, n);
    return out;
}
}

/*!
* \brief Function to serialize object via flatbuffers
* \param[in] builder - A falttbuffer builder already initialised with the message fields.
* \return Char vector containing serialized object
*
* This overload creates new memory but has best performance regarding creating a message view object
* but the usage is a little more clumsy and less encapsulated.
*
* Example of how to create objectOffset:
*
* flatbuffers::FlatBufferBuilder builder;
*
* auto name_off   = builder.CreateString("I am a test message");
* uint64_t counter  = 666;
* auto values_off = builder.CreateVector(std::vector<double>(100, 666.666));
*
* auto objectOffset = core_lib_test_fb::CreateTestMessage(
*     builder,
*     name_off,
*     counter,
*     values_off
* );
*
* builder.Finish(objectOffset);
*
* auto buf = ToCharVectorFlatBuf(builder);
*/
char_vector_t ToCharVectorFlatBuf(flatbuffers::FlatBufferBuilder builder)
{
    const auto* p = builder.GetBufferPointer(); // const uint8_t*
    const auto  n = builder.GetSize();          // size_t

    char_vector_t out(n);
    std::memcpy(out.data(), p, n);
    return out;
}

/*!
* \brief Function to serialize object via flatbuffers
* \param[in] object - Object to serialize
* \param[out] result - Char vector containing serialized object
*
* This overload uses the memory passed in and resizes if necessary.
*
* Note that flatbuffer "compiler" generates a header containing the structures from
* the schema. It creates 2 instances of each structure one is a high-performance
* view(e.g. MyStruct) and the other is a more traditional C++ struct (MyStructT). The
* high-performance view is typically used for deserialization and the C++ struct is used
* for serialization.This overload is designed to work with the C++ struct version of the data.
* For convenience, with less performance, you'd fill the fields in the C++ struct and
* then pass it to this function to serialize. For maximum performance, you'd fill
*  the fields in the high-performance view and then use the GetBufferPointer()
* and GetSize() functions to get a pointer and size of the serialized data to send
* directly without copying into a char vector like we do here.
*/
template <typename T, typename PackFunc>
void ToCharVectorFlatBuf(const T& object, char_vector_t& out, PackFunc packFunc)
{
    flatbuffers::FlatBufferBuilder builder;

    // packFunc(builder, &object) should return flatbuffers::Offset<SomeTable>
    auto root = packFunc(builder, &object, nullptr);
    builder.Finish(root);

    const auto* p = builder.GetBufferPointer(); // const uint8_t*
    const auto  n = builder.GetSize();          // size_t

    out.resize(n);
    std::memcpy(out.data(), p, n);
}

/*!
* \brief Function to serialize object via flatbuffers
* \param[in] builder - A falttbuffer builder already initialised with the message fields.
* \param[out] out - Char vector to be filled with serialized object
*
* This overload uses the passed in  memory (unless it needs resizing) but has best
* performance regarding creating a message view object but the usage is a little more
* clumsy and less encapsulated.
*
* Example of how to create objectOffset:
*
* flatbuffers::FlatBufferBuilder builder;
*
* auto name_off   = builder.CreateString("I am a test message");
* uint64_t counter  = 666;
* auto values_off = builder.CreateVector(std::vector<double>(100, 666.666));
*
* auto objectOffset = core_lib_test_fb::CreateTestMessage(
*     builder,
*     name_off,
*     counter,
*     values_off
* );
*
* builder.Finish(objectOffset);
* std::vector<char> out;
* ToCharVectorFlatBuf(builder, out);
*/
void ToCharVectorFlatBuf(flatbuffers::FlatBufferBuilder builder, char_vector_t& out)
{
    const auto* p = builder.GetBufferPointer(); // const uint8_t*
    const auto  n = builder.GetSize();          // size_t

    out.resize(n);
    std::memcpy(out.data(), p, n);
}


/*!
* \brief Deserialize a char vector into a corresponding object.
* \param[in] charVector - A char vector containing a boost serialized object of type T.
* \param[in] verifyFunc - A function that verifies the flatbuffer data.
* \param[in] getRootFunc - A function that returns a pointer to the root of the flatbuffer data.
* \return A serializable object of type T to receive deserialized vector.
*
* Note that flatbuffer "compiler" generates a header containing the structures from
* the schema. It creates 2 instances of each structure one is a high-performance
* view(e.g. MyStruct) and the other is a more traditional C++ struct (MyStructT). The
* high-performance view is typically used for deserialization and the C++ struct is used
* for serialization. This function returns the unpacked "nice" C++ struct version of the data.
* For maximum performance, you'd use the GetBufferPointer() and GetSize() functions to get a
* pointer and size of the serialized data to pass directly to the flatbuffers verifier
* and then use the GetRoot() function to get a pointer to the deserialized data without
* copying into a char vector like we do here. This function is designed for convenience
* with less performance where you have a char vector containing the flatbuffer data and
* you want to deserialize it into the C++ struct with nice STL types.
*/
template <typename T, typename VerifyFunc, typename GetRootFunc>
T ToObjectFlatBuf(char_cspan_buf_t charSpan, VerifyFunc verifyFunc, GetRootFunc getRootFunc)
{
    const auto* p = reinterpret_cast<const uint8_t*>(charSpan.data());
    const auto  n = charSpan.size();

    flatbuffers::Verifier v(p, n);

    if (!verifyFunc(v))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to verify flatbuffer data"));
    }

    // Most generated getters take ONLY the pointer.
    // If your getRootFunc expects const void*, this still works.
    auto* view = getRootFunc(p);

    T obj;
    view->UnPackTo(&obj);
    return obj; // NRVO/move
}

/*!
* \brief Deserialize a char vector into a corresponding object.
* \param[in] charVector - A char vector containing a boost serialized object of type T.
* \param[out] out - The object to be populated with deserialized data.
* \param[in] verifyFunc - A function that verifies the flatbuffer data.
* \param[in] getRootFunc - A function that returns a pointer to the root of the flatbuffer data.
*
* Note that flatbuffer "compiler" generates a header containing the structures from
* the schema. It creates 2 instances of each structure one is a high-performance
* view(e.g. MyStruct) and the other is a more traditional C++ struct (MyStructT). The
* high-performance view is typically used for deserialization and the C++ struct is used
* for serialization. This function returns the unpacked "nice" C++ struct version of the data.
* For maximum performance, you'd use the GetBufferPointer() and GetSize() functions to get a
* pointer and size of the serialized data to pass directly to the flatbuffers verifier
* and then use the GetRoot() function to get a pointer to the deserialized data without
* copying into a char vector like we do here. This function is designed for convenience
* with less performance where you have a char vector containing the flatbuffer data and
* you want to deserialize it into the C++ struct with nice STL types.
*/
template <typename T, typename VerifyFunc, typename GetRootFunc>
void ToObjectFlatBuf(char_cspan_buf_t charSpan,  T& out, VerifyFunc verifyFunc, GetRootFunc getRootFunc)
{
    const auto* p = reinterpret_cast<const uint8_t*>(charSpan.data());
    const auto  n = charSpan.size();

    flatbuffers::Verifier v(p, n);
    if (!verifyFunc(v)) {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to verify flatbuffer data"));
    }

    auto* view = getRootFunc(p);
    view->UnPackTo(&out);
}

/*!
* \brief Deserialize a char vector into a corresponding object.
* \param[in] charVector - A char vector containing a boost serialized object of type T.
* \return A serializable object of type T to receive deserialized vector.
*
* Note that flatbuffer "compiler" generates a header containing the structures from
* the schema. It creates 2 instances of each structure one is a high-performance
* view(e.g. MyStruct) and the other is a more traditional C++ struct (MyStructT). The
* high-performance view is typically used for deserialization and the C++ struct is used
* for serialization.This overload is designed to work with the high-performance view
* version of the data and returns the view. This gives  the best performance as a copy is
* not made into a char vector and then into a C++ struct. Instead the flatbuffer data is
* verified and then the view is returned which points directly to the deserialized data
* in the original char span.
*/
template <typename Tview, typename VerifyFunc, typename GetRootFunc>
Tview* ToObjectViewFlatBuf(char_cspan_buf_t charSpan, VerifyFunc verifyFunc)
{
    const auto* p = reinterpret_cast<const uint8_t*>(charSpan.data());
    const auto  n = charSpan.size();

    flatbuffers::Verifier v(p, n);

    if (!verifyFunc(v))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to verify flatbuffer data"));
    }

    return flatbuffers::GetRoot<Tview>(p);
}
#endif

} // namespace serialize
} // namespace core_lib

#endif // #define SERIALIZETOVECTOR
