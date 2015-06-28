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
 * \file PlatformDefines.h
 * \brief File containing platform specific definitions.
 */
 
#ifndef PLATFORMDEFINES
#define PLATFORMDEFINES

    #undef __USE_EXPLICIT_MOVE__
	#undef __USE_DEFAULT_CONSTRUCTOR__

    #ifdef _MSC_VER
		#if _MSC_VER < 1900
            #define __NOEXCEPT__
            #define __USE_EXPLICIT_MOVE__
			#define __USE_DEFAULT_CONSTRUCTOR__
			#define __CONSTEXPR__ const
		#else
            #define __NOEXCEPT__ noexcept
			#define __CONSTEXPR__ constexpr
		#endif
	#else
        #define __NOEXCEPT__ noexcept
        #define __CONSTEXPR__ constexpr
    #endif
	
#endif // PLATFORMDEFINES

