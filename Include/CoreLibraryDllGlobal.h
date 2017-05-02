// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
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
 * \file CoreLibraryDllGlobal.h
 * \brief File containing declaration of DLL import/export control defines.
 */

#include "boost/predef.h"

#ifndef CORELIBRARYDLLGLOBALH
#define CORELIBRARYDLLGLOBALH

#if BOOST_OS_WINDOWS
#if defined(CORE_LIBRARY_DLL)
#if (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
#define CORE_LIBRARY_DLL_SHARED_API __attribute__((dllexport))
#elif BOOST_COMP_MSVC
#define CORE_LIBRARY_DLL_SHARED_API __declspec(dllexport)
#else
#error Unsupported compiler! Please modify this header to add support.
#endif
#define CORE_LIBRARY_DLL_EXTERN
#else
#if defined(CORE_LIBRARY_LIB)
#define CORE_LIBRARY_DLL_SHARED_API
#define CORE_LIBRARY_DLL_EXTERN
#else
#if (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
#define CORE_LIBRARY_DLL_SHARED_API __attribute__((dllimport))
#else
#define CORE_LIBRARY_DLL_SHARED_API __declspec(dllimport)
#endif
#define CORE_LIBRARY_DLL_EXTERN extern
#endif
#endif
#define CORE_LIBRARY_DLL_PRIVATE_API
#elif BOOST_OS_LINUX
#if (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
#define CORE_LIBRARY_DLL_SHARED_API __attribute__((visibility("default")))
#define CORE_LIBRARY_DLL_PRIVATE_API __attribute__((visibility("hidden")))
#else
#error Unsupported compiler! Please modify this header to add support.
#endif
#define CORE_LIBRARY_DLL_EXTERN
#else
#error Unsupported OS!
#endif

#endif // CORELIBRARYDLLGLOBALH
