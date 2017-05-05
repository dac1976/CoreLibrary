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
 * \file PlatformDefines.h
 * \brief File containing platform specific definitions.
 */

#ifndef PLATFORMDEFINES
#define PLATFORMDEFINES

#include "boost/predef.h"

#undef USE_EXPLICIT_MOVE_
#undef USE_DEFAULT_CONSTRUCTOR_

#if BOOST_COMP_MSVC
#if _MSC_VER < 1900
/*! \brief NO_EXCEPT_ definition mapping to nothing*/
#define NO_EXCEPT_
/*! \brief USE_EXPLICIT_MOVE_ definition */
#define USE_EXPLICIT_MOVE_
/*! \brief USE_DEFAULT_CONSTRUCTOR_ definition */
#define USE_DEFAULT_CONSTRUCTOR_
/*! \brief CONSTEXPR_ definition mapping to const*/
#define CONSTEXPR_ const
/*! \brief STATIC_CONSTEXPR_ definition mapping to static const*/
#define STATIC_CONSTEXPR_ static const
#else
#define NO_EXCEPT_ noexcept
/*! \brief CONSTEXPR_ definition mapping to constexpr*/
#define CONSTEXPR_ constexpr
/*! \brief STATIC_CONSTEXPR_ definition mapping to static constexpr*/
#define STATIC_CONSTEXPR_ static constexpr
#endif
/*! \brief TYPENAME_DECL_ definition mapping to nothing*/
#define TYPENAME_DECL_
#else
/*! \brief NO_EXCEPT_ definition mapping to noexcept*/
#define NO_EXCEPT_ noexcept
/*! \brief CONSTEXPR_ definition mapping to constexpr*/
#define CONSTEXPR_ constexpr
/*! \brief STATIC_CONSTEXPR_ definition mapping to static constexpr*/
#define STATIC_CONSTEXPR_ static constexpr
/*! \brief TYPENAME_DECL_ definition mapping to typename*/
#define TYPENAME_DECL_ typename
#endif

#endif // PLATFORMDEFINES
