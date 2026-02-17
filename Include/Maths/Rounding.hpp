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

#ifndef ROUNDING_HPP
#define ROUNDING_HPP
//---------------------------------------------------------------------------
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
//---------------------------------------------------------------------------
namespace core_lib
{
namespace maths
{
//---------------------------------------------------------------------------
// Safely cast a floating point type to an integer
// type and avoid incorrect truncation.
template <typename FloatT, typename IntegerT> IntegerT RoundDown(FloatT value)
{
    static_assert(std::is_floating_point<FloatT>::value, "invalid floating point type");
    static_assert(std::is_integral<IntegerT>::value, "invalid integral type");

    return (value < FloatT(0)) ? IntegerT(std::ceil(value) - 0.5)
                               : IntegerT(std::floor(value) + 0.5);
}

//---------------------------------------------------------------------------
// Safely cast a floating point type to an integer
// type and avoid incorrect truncation and rounding to
// the closet integer, either above or below the floating
// point value.
template <typename FloatT, typename IntegerT> IntegerT RoundClosest(FloatT value)
{
    return RoundDown<FloatT, IntegerT>(std::round(value));
}

//---------------------------------------------------------------------------
// Safely round up a floating point type to an integer
// type and avoid incorrect truncation.
template <typename FloatT, typename IntegerT> IntegerT RoundUp(FloatT value)
{
    static_assert(std::is_floating_point<FloatT>::value, "invalid floating point type");
    static_assert(std::is_integral<IntegerT>::value, "invalid integral type");

    return (value < FloatT(0)) ? IntegerT(std::floor(value) - 0.5)
                               : IntegerT(std::ceil(value) + 0.5);
}

//---------------------------------------------------------------------------
// Say you want to round a number to the nearest multiple, for example:
// 794.964 to closest multiple of 50.0, then call this function as:
// RoundToNearestMultiple(794.964, 50.0)
// Whereby the result will be 800.0.
//
// The arg value can be positive or negative. nearestMultiple must be positive.
template <typename FloatT> FloatT RoundToNearestMultiple(FloatT value, FloatT nearestMultiple)
{
    auto temp = static_cast<FloatT>(RoundClosest<FloatT, int32_t>(value / nearestMultiple));
    temp *= nearestMultiple;

    if (nearestMultiple < 1.0)
    {
        return temp;
    }
    else
    {
        auto halfNearest = static_cast<FloatT>(nearestMultiple) / static_cast<FloatT>(2);
        auto remainder   = std::fmod(value, nearestMultiple);

        // Round down.
        if (std::abs(remainder) < halfNearest)
        {
            return static_cast<FloatT>(RoundUp<FloatT, int32_t>(temp));
        }
        // Round up.
        else
        {
            return static_cast<FloatT>(RoundDown<FloatT, int32_t>(temp));
        }
    }
}
//-----------------------------------------------------------------------------
// Checks if an integer value is an exact power of 2.
template <typename UIntType> bool IsPowerOf2(UIntType n)
{
    static_assert(std::is_integral<UIntType>::value, "invalid integral type");
    static_assert(std::is_unsigned<UIntType>::value, "invalid unsigned type");

    return (n > 0) && ((n & (n - 1)) == 0);
}

//-----------------------------------------------------------------------------
// Finds the closest power of 2 equal to or greater than the value of n.
template <typename UIntType> size_t ClosestPowerOf2(UIntType n)
{
    if (IsPowerOf2(n))
    {
        return static_cast<size_t>(n);
    }

    auto powerOf2Below = RoundDown<double, size_t>(std::log2(n));
    auto powerOf2Above = static_cast<size_t>(1) << (powerOf2Below + 1);

    return powerOf2Above;
}

//-----------------------------------------------------------------------------
// Safely compute log 10 without running into NAN values.

template <typename T> T SafeLog10(T value, T minThreshold = std::numeric_limits<T>::min() * T(2))
{
    static_assert(std::is_floating_point<T>::value, "value not floating point");

    // log10(0) is infinity so we'll pick a zero ref point to use instead.
    // Any value passed in below this will be set to the zero ref instead.
    static const auto MIN_LOG10_ZERO_VAL_THRESH = std::numeric_limits<T>::min() * T(2);
    static const auto MIN_LOG10_ZERO_VAL        = std::numeric_limits<T>::min();

    if (minThreshold < MIN_LOG10_ZERO_VAL_THRESH)
    {
        minThreshold = MIN_LOG10_ZERO_VAL;
    }

    if (value < minThreshold)
    {
        value = minThreshold;
    }

    return std::log10(value);
}

//-----------------------------------------------------------------------------
// Safely compute amplitude in decibels without running into NAN values.
template <typename T>
T SafeDecibel(T value, T zeroDbRef, T minThreshold = std::numeric_limits<T>::min() * T(2))
{
    static_assert(std::is_floating_point<T>::value, "value not floating point");

    // log10(0) is infinity so we'll pick a zero ref point to use instead.
    // Any value passed in below this will be set to the zero ref instead.
    static const auto MIN_LOG10_ZERO_VAL_THRESH = std::numeric_limits<T>::min() * T(2);
    static const auto MIN_LOG10_ZERO_VAL        = std::numeric_limits<T>::min();

    if (minThreshold < MIN_LOG10_ZERO_VAL_THRESH)
    {
        minThreshold = MIN_LOG10_ZERO_VAL;
    }

    if (zeroDbRef < minThreshold)
    {
        zeroDbRef = minThreshold;
    }

    if (value < minThreshold)
    {
        value = minThreshold;
    }

    static const auto DB_SCALAR = T(20);

    // dB = 20 * log10(value / zeroDbRef).
    return DB_SCALAR * std::log10(value / zeroDbRef);
}

//-----------------------------------------------------------------------------
// Safely compute decibels as an amplitude without running into NAN values.
template <typename T>
T SafeInvertDecibel(T dB, T zeroValueRef, T minThreshold = std::numeric_limits<T>::min() * T(2))
{
    static_assert(std::is_floating_point<T>::value, "value not floating point");

    // log10(0) is infinity so we'll pick a zero ref point to use instead.
    // Any value passed in below this will be set to the zero ref instead.
    static const auto MIN_LOG10_ZERO_VAL_THRESH = std::numeric_limits<T>::min() * T(2);
    static const auto MIN_LOG10_ZERO_VAL        = std::numeric_limits<T>::min();

    if (minThreshold < MIN_LOG10_ZERO_VAL_THRESH)
    {
        minThreshold = MIN_LOG10_ZERO_VAL;
    }

    if (zeroValueRef < minThreshold)
    {
        zeroValueRef = minThreshold;
    }

    static const auto DB_SCALAR = T(20);
    static const auto LOG_BASE  = T(10);

    // value = 10^(dB / 20) * zeroValueRef
    return std::pow(LOG_BASE, dB / DB_SCALAR) * zeroValueRef;
}
//---------------------------------------------------------------------------
} // namespace maths
} // namespace core_lib
//---------------------------------------------------------------------------
#endif // ROUNDING_HPP
