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
 * \file CoreLibraryDllGlobal.h
 * \brief File containing declaration of DLL import/export control defines.
 */

#ifndef CORELIBRARYDLLGLOBALH
#define CORELIBRARYDLLGLOBALH

#include <boost/predef.h>

#ifdef CORE_LIBRARY_DLL_SHARED_API
	#undef CORE_LIBRARY_DLL_SHARED_API
	#undef USE_CDECL
#endif

#if defined(CORE_LIBRARY_LIB)
    #pragma message("building CoreLibrary as static lib")
    #define CORE_LIBRARY_DLL_SHARED_API
    #define USE_CDECL
#else
	// Windows
	#if BOOST_OS_WINDOWS
		// Windows - Export
		#if defined(CORE_LIBRARY_DLL)
			#if (BOOST_COMP_MSVC || BOOST_COMP_BORLAND)
				#pragma message("CoreLibrary API exporting symbols with __declspec(dllexport)")
				#define CORE_LIBRARY_DLL_SHARED_API __declspec(dllexport)
			#elif (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
				#pragma message("CoreLibrary API exporting symbols with __attribute__((dllexport))")
				#define CORE_LIBRARY_DLL_SHARED_API __attribute__((dllexport))
			#else
				#error Unsupported compiler! Please modify this header to add support.
			#endif
		// Windows - Import
		#else
			#if (BOOST_COMP_MSVC || BOOST_COMP_BORLAND)
				#pragma message("CoreLibrary API importing symbols with __declspec(dllimport)")
				#define CORE_LIBRARY_DLL_SHARED_API __declspec(dllimport)
			#elif (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
				#pragma message("CoreLibrary API importing symbols with __attribute__((dllimport))")
				#define CORE_LIBRARY_DLL_SHARED_API __attribute__((dllimport))
			#else
				#error Unsupported compiler! Please modify this header to add support.
			#endif
		#endif
		// Windows - set __cdecl (or not)
		#ifdef CORE_LIBRARY_DLL_SHARED_API_DO_NOT_USE_CDECL
			#pragma message("CoreLibrary API not using __cdecl")
			#define USE_CDECL
		#else
			#pragma message("CoreLibrary API using __cdecl")
			#define USE_CDECL __cdecl
		#endif
	// Linux
	#elif BOOST_OS_LINUX
		#if (BOOST_COMP_GNUC || BOOST_COMP_CLANG)
			// Linux - Export
			#if defined(CORE_LIBRARY_DLL)
				#pragma message("CoreLibrary API exporting symbols with __attribute__((visibility(\"default\")))")
				#define CORE_LIBRARY_DLL_SHARED_API __attribute__((visibility("default")))
			// Linux - Import
			#else
				#pragma message("CoreLibrary API importing symbols")
				#define CORE_LIBRARY_DLL_SHARED_API
			#endif
		#else
			#error Unsupported compiler! Please modify this header to add support.
		#endif
		// Linux - __cdecl not supported or required
		#define USE_CDECL
	// Unsupported OS
	#else
		#error Unsupported OS!
	#endif
#endif

#endif // CORELIBRARYDLLGLOBALH


