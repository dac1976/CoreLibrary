// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014,2015 Duncan Crutchley
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
 * \file PlatformDefines.h
 * \brief File containing platform specific definitions.
 */
 
#ifndef PLATFORMDEFINES
#define PLATFORMDEFINES

#include "boost/predef.h"

#undef __USE_EXPLICIT_MOVE__
#undef __USE_DEFAULT_CONSTRUCTOR__

#if BOOST_COMP_MSVC
    #if _MSC_VER < 1900
        /*! \brief __NOEXCEPT__ definition mapping to nothing*/
        #define __NOEXCEPT__
        /*! \brief __USE_EXPLICIT_MOVE__ definition */
        #define __USE_EXPLICIT_MOVE__
        /*! \brief __USE_EXPLICIT_MOVE__ definition */
        #define __USE_DEFAULT_CONSTRUCTOR__
        /*! \brief __USE_EXPLICIT_MOVE__ definition mapping to static const*/
        #define __CONSTEXPR__ static const
    #else
        #define __NOEXCEPT__ noexcept
        /*! \brief __USE_EXPLICIT_MOVE__ definition mapping to constexpr*/
        #define __CONSTEXPR__ constexpr
    #endif
    /*! \brief __TYPENAME_DECL__ definition mapping to nothing*/
    #define __TYPENAME_DECL__
#else
    /*! \brief __NOEXCEPT__ definition mapping to noexcept*/
    #define __NOEXCEPT__ noexcept
    /*! \brief __USE_EXPLICIT_MOVE__ definition mapping to constexpr*/
    #define __CONSTEXPR__ constexpr
    /*! \brief __TYPENAME_DECL__ definition mapping to typename*/
    #define __TYPENAME_DECL__ typename
#endif
	
#endif // PLATFORMDEFINES

