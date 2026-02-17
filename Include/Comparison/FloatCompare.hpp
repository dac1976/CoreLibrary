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

#ifndef FLOAT_COMPARE_HPP
#define FLOAT_COMPARE_HPP

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <type_traits>

namespace core_lib
{
namespace comp
{

template <typename T> struct FloatEpsilon
{
    static T Value()
    {
        throw std::runtime_error("unsupported type");
        return T(0);
    }
};

template <> struct FloatEpsilon<long double>
{
    static long double Value()
    {
        // long double precision is between 18 and 21 d.p.
        return 1.0e-18l;
    }
};

template <> struct FloatEpsilon<double>
{
    static double Value()
    {
        // double precision is between 15 and 18 d.p.
        return 1.0e-15;
    }
};

template <> struct FloatEpsilon<float>
{
    static float Value()
    {
        // float precision is between 6 and 8 d.p.
        return 1.0e-6f;
    }
};

template <typename T> bool float_compare(T a, T b)
{
    static_assert(std::is_floating_point<T>::value, "T is not a floating point type");
    static const auto EPSILON = FloatEpsilon<T>::Value();
    auto              absDiff = std::abs(a - b);

    if (absDiff <= EPSILON)
    {
        return true;
    }

    auto maxVal    = std::max(std::abs(a), std::abs(b));
    auto comparand = EPSILON * maxVal;
    return absDiff <= comparand;
}

} // namespace comp
} // namespace core_lib

#endif // FLOAT_COMPARE_HPP
