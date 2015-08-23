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
 * \file CustomException.h
 * \brief File containing declarations relating to custom exception handling.
 */

#include "Platform/PlatformDefines.h"

#include <string>
#include "boost/exception/all.hpp"

#ifndef CUSTOMEXCEPTION
#define CUSTOMEXCEPTION

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The exceptions namespace. */
namespace exceptions {

/*!
 * \brief Exception base class from which to define custom exceptions.
 *
 * This class inherits virtually from std::exception and most importantly
 * boost::exception. Inheriting from the latter ensures that we can get
 * extended information when exceptions are thrown using the boost macro
 * BOOST_THROW_EXCEPTION. The extended information contains line, function
 * and file information of where the exception was thrown, as well as
 * providing access to the what() message.
 *
 * To access extended info do the following:
 *
 * try
 * {
 *     // something throws a n exception derived from
 *     // core_lib::exceptions::xCustomException
 * }
 * catch(core_lib::exceptions::xCustomException& e)
 * {
 *     std::cerr << boost::diagnostic_information(e);
 * }
 * catch(...)
 * {
 *     std::cerr << "Unhandled exception!" << std::endl
 *               << boost::current_exception_diagnostic_information();
 * }
 */
class xCustomException : public virtual boost::exception
		, public virtual std::exception
{
public:
	/*! \brief Default constructor. */
	xCustomException();
	/*!
	 * \brief Initializing constructor.
	 * \param[in] message - A user specified message string.
	 */
	explicit xCustomException(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xCustomException();
	/*! \brief Copy constructor. */
	xCustomException(const xCustomException&) = default;
	/*! \brief Copy assignment operator. */
	xCustomException& operator=(const xCustomException&) = default;
	/*!
	 * \brief Function to get the exception message.
	 * \return The exception message.
	 */
    virtual const char* what() const __NOEXCEPT__ final;

	/*!
	 * \brief Function to get the exception message.
	 * \return The exception message.
	 */
    virtual const std::string& whatStr() const __NOEXCEPT__ final;

protected:
	/*! \brief The exception message. */
	std::string m_message;
};


} // namespace exceptions
} // namespace core_lib

#endif // CUSTOMEXCEPTION
