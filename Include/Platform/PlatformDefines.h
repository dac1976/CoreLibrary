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

#undef USE_EXPLICIT_MOVE_
#undef USE_DEFAULT_CONSTRUCTOR_

#if _WIN32 || _WIN64
#if _WIN64
#define ENV64BIT
#else
#define ENV32BIT
#endif
#else
#if __x86_64__ || __ppc64__
#define ENV64BIT
#else
#define ENV32BIT
#endif
#endif

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 202302L) || __cplusplus >= 202302L)
#define IS_CPP23
#define IS_CPP20
#define IS_CPP17
#define IS_CPP14
#define IS_CPP11
#elif ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L)
#define IS_CPP20
#define IS_CPP17
#define IS_CPP14
#define IS_CPP11
#elif ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#define IS_CPP17
#define IS_CPP14
#define IS_CPP11
#elif ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L) || __cplusplus >= 201402L)
#define IS_CPP14
#define IS_CPP11
#elif ((defined(_MSVC_LANG) && _MSVC_LANG >= 201103L) || __cplusplus >= 201103L)
#define IS_CPP11
#endif

#if __cplusplus >= 201703L
#define CORELIB_ARG_MAYBE_UNUSED [[maybe_unused]]
#define CORELIB_UNUSED_ARG(x)
#else
#define CORELIB_ARG_MAYBE_UNUSED
#define CORELIB_UNUSED_ARG(x) (void)(x);
#endif

#if defined(_MSC_VER)
#if _MSC_VER < 1900
/*! \brief USE_EXPLICIT_MOVE_ definition */
#define USE_EXPLICIT_MOVE_
/*! \brief USE_DEFAULT_CONSTRUCTOR_ definition */
#define USE_DEFAULT_CONSTRUCTOR_
/*! \brief NO_EXCEPT_ definition mapping to nothing*/
#define NO_EXCEPT_
/*! \brief CONSTEXPR_ definition mapping to const*/
#define CONSTEXPR_ const
/*! \brief STATIC_CONSTEXPR_ definition mapping to static const*/
#define STATIC_CONSTEXPR_ static const
/*! \brief TYPENAME_DECL_ definition mapping to nothing*/
#define TYPENAME_DECL_
/*! \brief OVERRIDE_ definition mapping to nothing*/
#define OVERRIDE_
/*! \brief FINAL_ definition mapping to final*/
#define FINAL_
#else
#define NO_EXCEPT_ noexcept
/*! \brief CONSTEXPR_ definition mapping to constexpr*/
#define CONSTEXPR_ constexpr
/*! \brief STATIC_CONSTEXPR_ definition mapping to static constexpr*/
#define STATIC_CONSTEXPR_ static constexpr
/*! \brief TYPENAME_DECL_ definition mapping to typename*/
#define TYPENAME_DECL_ typename
/*! \brief OVERRIDE_ definition mapping to override*/
#define OVERRIDE_ override
/*! \brief FINAL_ definition mapping to final*/
#define FINAL_ final
#endif
#else
#if defined(__clang__) || defined(__GNUC__)
/*! \brief NO_EXCEPT_ definition mapping to noexcept*/
#define NO_EXCEPT_ noexcept
/*! \brief CONSTEXPR_ definition mapping to constexpr*/
#define CONSTEXPR_ constexpr
/*! \brief STATIC_CONSTEXPR_ definition mapping to static constexpr*/
#define STATIC_CONSTEXPR_ static constexpr
/*! \brief TYPENAME_DECL_ definition mapping to typename*/
#define TYPENAME_DECL_ typename
/*! \brief OVERRIDE_ definition mapping to override*/
#define OVERRIDE_ override
/*! \brief FINAL_ definition mapping to final*/
#define FINAL_ final
#else
/*! \brief NO_EXCEPT_ definition mapping to nothing*/
#define NO_EXCEPT_
/*! \brief CONSTEXPR_ definition mapping to const*/
#define CONSTEXPR_ const
/*! \brief STATIC_CONSTEXPR_ definition mapping to static const*/
#define STATIC_CONSTEXPR_ static const
/*! \brief TYPENAME_DECL_ definition mapping to nothing*/
#define TYPENAME_DECL_
/*! \brief OVERRIDE_ definition mapping to nothing*/
#define OVERRIDE_
/*! \brief FINAL_ definition mapping to nothing*/
#define FINAL_
#endif
#endif

#endif // PLATFORMDEFINES
