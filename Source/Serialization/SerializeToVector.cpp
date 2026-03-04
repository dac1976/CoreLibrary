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
 * \file SerializeToVector.cpp
 * \brief File containing utilities to serialize objects to byte vectors.
 */
#include "Serialization/SerializeToVector.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The serialize namespace. */
namespace serialize
{

#if defined(USE_FLATBUFFERS)
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
#endif

} // namespace serialize
} // namespace core_lib
