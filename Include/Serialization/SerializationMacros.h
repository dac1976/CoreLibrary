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
#ifndef SERIALIZATIONMACROS_H
#define SERIALIZATIONMACROS_H

//-----------------------------------------------------------------------------
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#define SERIALIZE_TO_ARCHIVE(ar, o) ar(CEREAL_NVP(o))

#define DESERIALIZE_FROM_ARCHIVE(ar, o) ar(CEREAL_NVP(o))

#define FRIEND_PRIVATE_ACCESS friend class cereal::access;

#define CLASS_VERSION(o, v) CEREAL_CLASS_VERSION(o, v)

#define SERIALIZATION_SPLIT_MEMBER

// NOTE:
// In some cases it may be necessary to use the macro:
//     CEREAL_REGISTER_TYPE(YourDerivedClassName)
// in the header where the derived class is declared, after its declaration.
// Then in the serialize template member function use the macro below to
// serialize the base class members by passing in the dereferenced this pointer
//  of the derived object in the 'o' argument, e.g. *this.
// See the web pages below for more specific info:
// https://uscilab.github.io/cereal/inheritance.html
// https://uscilab.github.io/cereal/polymorphism.html
#define SERIALIZE_BASE_OBJECT(ar, o, t) ar(cereal::make_nvp(#t, cereal::base_class<t>(&o)))

#define REGISTER_DERIVED_TYPE(t) CEREAL_REGISTER_TYPE(t)
//-----------------------------------------------------------------------------
/*
 *
----------------
Usage example 1:
----------------

If class has private serialize method and members then must grant friend access.

class MyClass
{
    FRIEND_PRIVATE_ACCESS

public:
    MyClass();
    ~MyClass();

    // Put more public methods here.

private:
    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        SERIALIZE_TO_ARCHIVE(ar, m_value);

        // Serialize more private data members to archive here.
    }

private:
    int m_value;

    // Put more private data here.
};

----------------
Usage example 2:
----------------

If struct has public serialize method and members then does not need private friend access.

struct MyStruct
{
    int value;

    // Put more public data here.

    template <class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        SERIALIZE_TO_ARCHIVE(ar, value);

        // Serialize more private data members to archive here.
    }
};

*/

#endif // SERIALIZATIONMACROS_H
