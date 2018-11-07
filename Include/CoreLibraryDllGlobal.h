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
 * \file CoreLibraryDllGlobal.h
 * \brief File containing declaration of DLL import/export control defines.
 */

#ifndef CORELIBRARYDLLGLOBALH
#define CORELIBRARYDLLGLOBALH

#include <boost/predef.h>

#if BOOST_OS_WINDOWS
#if defined(CORE_LIBRARY_DLL)
#if (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
#define CORE_LIBRARY_DLL_SHARED_API __attribute__((dllexport))
#elif defined(_MSC_VER)
#define CORE_LIBRARY_DLL_SHARED_API __declspec(dllexport)
#else
#error Unsupported compiler! Please modify this header to add support.
#endif
#else
#if defined(CORE_LIBRARY_LIB)
#define CORE_LIBRARY_DLL_SHARED_API
#else
#if (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
#define CORE_LIBRARY_DLL_SHARED_API __attribute__((dllimport))
#else
#define CORE_LIBRARY_DLL_SHARED_API __declspec(dllimport)
#endif
#endif
#endif
#elif BOOST_OS_LINUX
#if (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
#define CORE_LIBRARY_DLL_SHARED_API __attribute__((visibility("default")))
#else
#error Unsupported compiler! Please modify this header to add support.
#endif
#else
#error Unsupported OS!
#endif

#endif // CORELIBRARYDLLGLOBALH
