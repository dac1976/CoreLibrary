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
 * \file MemoryUtils.hpp
 * \brief File containing useful memory utilities.
 */
#ifndef MEMORYUTILS_HPP
#define MEMORYUTILS_HPP

#include <algorithm>
#include <iterator>
#include <cstdint>
#include <iterator>
#include <cstring>  
#include <type_traits>
#include "Platform/PlatformDefines.h"
#if defined(IS_CPP20)
#include <bit>	
#include <optional>
#include <span>
#include <cstddef>
#endif

namespace core_lib
{

#if defined(IS_CPP20)
using char_cspan_t = std::span<const char>;
	
template <class T, class F>
T BitCast(const F& src) noexcept
{
    return std::bit_cast<T>(src);
}
#else
template <class T, class F>
typename std::enable_if<sizeof(T) == sizeof(F) && std::is_trivially_copyable<T>::value &&
                            std::is_trivially_copyable<F>::value,
                        T>::type
BitCast(const F& src) noexcept
{
    T dst;
    std::memcpy(&dst, &src, sizeof(T));
    return dst;
}
#endif

#if defined(IS_CPP20)
// The following is safer and better practice for converting a buffer, say received from a
// socket into the underlying structs it represents.
//
// If message is MyHeader + MyPodStruct but stored in a std::vector<char> buffer (or
// std::array<char, N> or {char const*, size_t}), then it is technically safer and
// more correct to do:
//
// auto hdrOpt = TryConvertToPod<MyHeader>(buffer);
// auto payloadOpt = TryConvertToPod<MyPodStruct>(buffer, sizeof(MyHeader));
//
// Rather than what we often do, which is:
//
// auto hdr = *reinterpret_cast<MyHeader const*>(buffer.data());
// auto payload = *reinterpret_cast<MyPodStruct const*>(buffer.data() + sizeof(MyHeader));
template <typename T>
std::optional<T> TryConvertToPod(char_cspan_t buffer, size_t offset = 0) noexcept
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

    if (offset + sizeof(T) > buffer.size())
    {
        return std::nullopt;
    }

    T out;
    std::memcpy(&out, buffer.data() + offset, sizeof(T));
    return out;
}

// This version avoids needing an extra copy outside of this call.
template <typename T> bool TryConvertToPod(T& out, char_cspan_t buffer, size_t offset = 0) noexcept
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

    if (offset + sizeof(T) > buffer.size())
    {
        return false;
    }

    std::memcpy(&out, buffer.data() + offset, sizeof(T));
    return true;
}
#endif

// Fill memory "bytes" with zeroes for a POD object.
template <typename T> void ZeroPodObject(T& o)
{
	static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
	
    // We don't use memset because under certain circumstances it can
    // be optimised out by the compiler and end up not filling the
    // bytes with 0s.
    auto firstByte = reinterpret_cast<char*>(&o);
    auto lastByte  = std::next(firstByte, sizeof(o));
    std::fill(firstByte, lastByte, 0);
}

// Reverse the endianness of arrays of type T, destructive -  will
// overwrite Array argument...
template <typename T> void EndianSwapWholeArray(T* array, size_t length)
{
    if ((array != nullptr) && (length > 0))
    {
        const auto byteCount      = sizeof(T) * length;
        const auto halfByteCount  = byteCount / 2;
        auto*      byteArrayLeft  = reinterpret_cast<uint8_t*>(array);
        auto*      byteArrayRight = &(reinterpret_cast<uint8_t*>(array))[byteCount - 1];

        for (size_t i = 0; i < halfByteCount; ++i, ++byteArrayLeft, --byteArrayRight)
        {
            auto tempChar   = *byteArrayLeft;
            *byteArrayLeft  = *byteArrayRight;
            *byteArrayRight = tempChar;
        }
    }
}

// Reverse the endianness of the items of type T in an array,
// destructive - will overwrite Array argument.
template <typename T> void EndianSwapArrayItems(T* array, size_t length)
{
    if ((array != nullptr) && (length > 0))
    {
        auto* arrayFirst = array;
        auto* arrayLast  = std::next(arrayFirst, length);
        std::for_each(arrayFirst, arrayLast, [](T x) { EndianSwapWholeArray(&x, 1); });
    }
}

// Reverse the endianness of any single item of type T.
template <typename T> T EndianSwapValue(T Value)
{
    EndianSwapWholeArray(&Value, 1);
    return Value;
}

} // namespace core_lib

#endif // MEMORYUTILS_HPP

