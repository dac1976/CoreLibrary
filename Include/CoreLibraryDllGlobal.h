// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2015 Duncan Crutchley
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
#ifndef CORELIBRARYDLLGLOBALH
#define CORELIBRARYDLLGLOBALH

#if defined(CORE_LIBRARY_DLL)
    /*! \brief CORE_LIBRARY_DLL_SHARED_API definition mapping to __declspec(dllexport) */
    #define CORE_LIBRARY_DLL_SHARED_API __declspec(dllexport)
    /*! \brief CORE_LIBRARY_DLL_EXTERN definition mapping to nothing */
    #define CORE_LIBRARY_DLL_EXTERN
#else
    #if defined(CORE_LIBRARY_LIB)
        /*! \brief CORE_LIBRARY_DLL_SHARED_API definition mapping to nothing */
        #define CORE_LIBRARY_DLL_SHARED_API
        /*! \brief CORE_LIBRARY_DLL_EXTERN definition mapping to nothing */
        #define CORE_LIBRARY_DLL_EXTERN
    #else
        /*! \brief CORE_LIBRARY_DLL_SHARED_API definition mapping to __declspec(dllimport) */
        #define CORE_LIBRARY_DLL_SHARED_API __declspec(dllimport)
        /*! \brief CORE_LIBRARY_DLL_EXTERN definition mapping to extern */
        #define CORE_LIBRARY_DLL_EXTERN extern
    #endif    
#endif

#endif // CORELIBRARYDLLGLOBALH
