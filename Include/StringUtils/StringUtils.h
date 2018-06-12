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
 * \file StringUtils.h
 * \brief File containing declarations relating various string utilities.
 */

#ifndef STRINGUTILS
#define STRINGUTILS

#include <string>
#include <sstream>
#include <iomanip>
#include "Exceptions/CustomException.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The string_utils namespace. */
namespace string_utils
{

/*!
 * \brief Tidy a string obtained from getline function.
 * \param[in,out] line - A string obtained using getline.
 *
 * Safely convert string into sensible form due to bug in some implementations
 * of std::getline() (looking at you Embarcadero C++ Builder) where size is
 * greater than pos of null char.
 */
void CORE_LIBRARY_DLL_SHARED_API PackStdString(std::string& line);

/*!
 * \brief Split string bad delimiter exception.
 *
 * This exception class is intended to be thrown by the SplitString function
 * to signify that an error has occured.
 */
class CORE_LIBRARY_DLL_SHARED_API xSplitStringBadDelim : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xSplitStringBadDelim();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xSplitStringBadDelim(const std::string& message);
    /*! \brief Virtual destructor. */
    ~xSplitStringBadDelim() override = default;
    /*! \brief Copy constructor. */
    xSplitStringBadDelim(const xSplitStringBadDelim&) = default;
    /*! \brief Copy assignment operator. */
    xSplitStringBadDelim& operator=(const xSplitStringBadDelim&) = default;
    /*! \brief Move constructor. */
    xSplitStringBadDelim(xSplitStringBadDelim&&) = default;
    /*! \brief Move assignment operator. */
    xSplitStringBadDelim& operator=(xSplitStringBadDelim&&) = default;
};

/*!
 * \brief Split string sub-strings exception.
 *
 * This exception class is intended to be thrown by the SplitString function
 * to signify that an error has occured.
 */
class CORE_LIBRARY_DLL_SHARED_API xSplitStringTooManySubstrings
    : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xSplitStringTooManySubstrings();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xSplitStringTooManySubstrings(const std::string& message);
    /*! \brief Virtual destructor.*/
    ~xSplitStringTooManySubstrings() override = default;
    /*! \brief Copy constructor. */
    xSplitStringTooManySubstrings(const xSplitStringTooManySubstrings&) = default;
    /*! \brief Copy assignment operator. */
    xSplitStringTooManySubstrings& operator=(const xSplitStringTooManySubstrings&) = default;
    /*! \brief Move constructor. */
    xSplitStringTooManySubstrings(xSplitStringTooManySubstrings&&) = default;
    /*! \brief Move assignment operator. */
    xSplitStringTooManySubstrings& operator=(xSplitStringTooManySubstrings&&) = default;
};

/*!
 * \brief Split string options enumeration.
 *
 * This enum controls how the results are formatted ater splitting the input
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
 * \param[out] subStr1 - First output substring.
 * \param[out] subStr2 - Second output substring.
 * \param[in] toSplit - Input string to be split into two.
 * \param[in] delim - Delimiters to look for to split around.
 * \param[in] option - Options to formt resultant substrings.
 *
 * Given an input string and a string containing delimiters the input
 * string is split into two parts either side of the deilimiter string.
 * If none of the delimiters can be found then a xSplitStringBadDelim
 * exception is thrown. If the string will be split into more than two
 * substrings then a xSplitStringTooManySubStrings exception is thrown.
 */
void CORE_LIBRARY_DLL_SHARED_API SplitString(std::string& subStr1, std::string& subStr2,
                                             const std::string& toSplit, const std::string& delim,
                                             const eSplitStringResult option);

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
 * \brief Convert a floating point value to a formatted string representation.
 * \param[in] value - Floating point value to convert to a string.
 * \param[in] precision - Precision to display the number to as a string.
 * \param[in] formatting - Formatting options.
 * return Resultant string.
 *
 * Convert a single or double precision floating point number to a
 * string representaion.
 */
template <typename T>
std::string FormatFloatString(const T value, const int precision = 15,
                              const eFloatStringFormat formatting = eFloatStringFormat::normal)
{
    std::ostringstream ss;

    switch (formatting)
    {
    case eFloatStringFormat::fixed:
        ss << std::fixed;
        break;
    case eFloatStringFormat::scientific:
        ss << std::scientific;
        break;
    case eFloatStringFormat::normal:
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

/*!
 * \brief Return a string with any illegal chars replaced with replacement char.
 * \param[in] text - Source string potentially with illegal chars.
 * \param[in] illegalChars - String containing illegal chars to look for in text.
 * \param[in] replacementChar - String containing replacement.
 * \return Copy of text with ilegal chars replaced.
 */
std::wstring CORE_LIBRARY_DLL_SHARED_API RemoveIllegalChars(
    const std::wstring& text, const std::wstring& illegalChars = L"~#%&*{}\\:<>?/+|\"",
    const wchar_t replacementChar = L'_');

std::string CORE_LIBRARY_DLL_SHARED_API
            RemoveIllegalChars(const std::string& text, const std::string& illegalChars = "~#%&*{}\\:<>?/+|\"",
                               const char replacementChar = '_');

/*!
 * \brief Convert a std::string to std::wstring.
 * \param[in] text - Source string.
 * \return Converted string.
 */
std::wstring CORE_LIBRARY_DLL_SHARED_API StringToWString(const std::string& text);

/*!
 * \brief Convert a std::wstring to std::string.
 * \param[in] text - Source string
 * \return Converted string.
 */
std::string CORE_LIBRARY_DLL_SHARED_API WStringToString(const std::wstring& text);

} // namespace string_utils
} // namespace core_lib

#endif // STRINGUTILS
