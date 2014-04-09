// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
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
 * \file StringUtils.hpp
 * \brief File containing declarations relating various string utilities.
 */

#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include "Exceptions/CustomException.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The string_utils namespace. */
namespace string_utils {

/*!
 * \brief Tidy a string obtained from getline function.
 * \param [IN/OUT] A string obtained using getline.
 *
 * Safely convert string into sensible form due to bug in some implementations
 * of std::getline() (looking at you Emarcadero C++ Builder) where size is
 * greater than pos of null char.
 */
void PackStdString(std::string& line);

/*!
 * \brief Split string bad delimiter exception.
 *
 * This exception class is intended to be thrown by the SplitString function
 * to signify that an error has occured.
 */
class xSplitStringBadDelim: public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xSplitStringBadDelim();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xSplitStringBadDelim(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xSplitStringBadDelim();
};

/*!
 * \brief Split string sub-strings exception.
 *
 * This exception class is intended to be thrown by the SplitString function
 * to signify that an error has occured.
 */
class xSplitStringTooManySubstrings : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xSplitStringTooManySubstrings();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xSplitStringTooManySubstrings(const std::string& message);
    /*! \brief Virtual destructor.*/
    virtual ~xSplitStringTooManySubstrings();
};

/*!
 * \brief Split string options enumeration.
 *
 * This enum controls how the results are formated ater splitting the input
 * string.
 */
enum class eSplitStringResult
{
    /*! \brief Trim the results, removing pre/pro-ceeding spaces. */
    trimmed,
    /*! \brief Do not trim the results. */
    notTrimmed
};

/*!
 * \brief Split a string into two parts given delimiters.
 * \param [OUT] First output substring.
 * \param [OUT] Second output substring.
 * \param [IN] Input string to be split into two.
 * \param [IN] Delimiters to look for to split around.
 * \param [IN] Options to formt resultant substrings.
 *
 * Given an input string and a string containing delimiters the input
 * string is split into two parts either side of the deilimiter string.
 * If none of the delimiters can be found then a xSplitStringBadDelim
 * exception is thrown. If the string will be split into more than two
 * substrings then a xSplitStringTooManySubStrings exception is thrown.
 */
void SplitString(std::string& subStr1, std::string& subStr2,
                 const std::string& toSplit, const std::string& delim,
                 eSplitStringResult option);

/*!
 * \brief Format float options enumeration.
 *
 * This enum controls how the floatint point value is represented as a string.
 */
enum class eFloatStringFormat
{
    /*! \brief Default formating. */
    normal,
    /*! \brief Fixed formatting. */
    fixed,
    /*! \brief Scientific formatting. */
    scientific
};

/*!
 * \brief Convert a floating point value to a string representation.
 * \param [IN] Floating point value to convert to a string.
 * \param [IN] Precision to display the number to as a string.
 * \param [IN] Formatting options.
 * return Resultant string.
 *
 * Convert a single or double precision floating point number to a
 * string representaion.
 */
template <typename T>
std::string FormatFloatString(T&& value, int precision = 15,
                              eFloatStringFormat fsf = eFloatStringFormat::normal)
{
    std::ostringstream ss;

    switch(fsf)
    {
    case eFloatStringFormat::fixed:
        ss << std::fixed;
        break;
    case eFloatStringFormat::scientific:
        ss << std::scientific;
        break;
    case eFloatStringFormat::normal:
    default:
        // do nothing
        break;
    }

    if (precision >= 0)
    {
        ss << std::setprecision(precision);
    }

    ss << value;
    return ss.str();
}

} // namespace string_utils
} // namespace core_lib

#endif // STRINGUTILS_HPP
