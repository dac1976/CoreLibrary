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
 * \file DetailedException.h
 * \brief File containing declarations relating to custom exception handling.
 */

#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"
#include <string>
#include <boost/exception/all.hpp>

#ifndef DETAILEDEXCEPTION
#define DETAILEDEXCEPTION

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The exceptions namespace. */
namespace exceptions
{

/*!
 * \brief Exception class from which to define further derived custom exception classes.
 *
 * This class inherits virtually from std::exception and most importantly
 * boost::exception. Inheriting from the latter ensures that we can get
 * extended information when exceptions are thrown using the boost macro
 * BOOST_THROW_EXCEPTION. The extended information contains line, function
 * and file information of where the exception was thrown, as well as
 * providing access to the what() message.
 *
 * To access extended info you can do the following:
 *
\code{.cpp}
try
{
   BOOST_THROW_EXCEPTION(core_lib::exceptions::DetailedException("something has happened"));
}
catch(core_lib::exceptions::DetailedException& e)
{
   std::cerr << boost::diagnostic_information(e);
}
catch(...)
{
   std::cerr << "Unhandled exception!" << std::endl
             << boost::current_exception_diagnostic_information();
}
\endcode
 *
 * This will also work:
 *
\code{.cpp}
try
{
   BOOST_THROW_EXCEPTION(core_lib::exceptions::DetailedException("something has happened"));
}
catch(std::exception& e)
{
   std::cerr << boost::diagnostic_information(e);
}
\endcode
 *
 * And this:
 *
\code{.cpp}
try
{
   BOOST_THROW_EXCEPTION(core_lib::exceptions::DetailedException("something has happened"));
}
catch(boost::exception& e)
{
   std::cerr << boost::diagnostic_information(e);
}
\endcode
 *
 * And lastly this:
 *
\code{.cpp}
try
{
   BOOST_THROW_EXCEPTION(core_lib::exceptions::DetailedException("something has happened"));
}
catch(...)
{
   std::cerr << "Unhandled exception!" << std::endl
             << boost::current_exception_diagnostic_information();
}
\endcode
 *
 * This class is defined inline to make exporting from a DLL
 * containing this class trivial as we are deriving from
 * boost::exception and std::exception.
 */
class DetailedException : public virtual boost::exception, public virtual std::exception
{
public:
    /*! \brief Default constructor. */
    DetailedException() = default;
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit DetailedException(const std::string& message)
        : m_what(message.c_str())
    {
    }
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit DetailedException(const char* message)
        : m_what(message)
    {
    }
    /*! \brief Virtual destructor. */
    ~DetailedException() noexcept override = default;
    /*! \brief Copy constructor. */
    DetailedException(const DetailedException&) = default;
    /*! \brief Copy assignment operator. */
    DetailedException& operator=(const DetailedException&) = default;
    /*! \brief Move constructor. */
    DetailedException(DetailedException&&) = default;
    /*! \brief Move assignment operator. */
    DetailedException& operator=(DetailedException&&) = default;

    char const* what() const NO_EXCEPT_ override
    {
        return m_what.c_str();
    }

private:
    std::string m_what{"exception"};
};

} // namespace exceptions
} // namespace core_lib

#endif // DETAILEDEXCEPTION
