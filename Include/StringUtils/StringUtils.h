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
#include <map>
#include <vector>
#include <limits>
#include <cmath>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"

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
 * If none of the delimiters can be found then a std::invalid_argument
 * exception is thrown. If the string will be split into more than two
 * substrings then a std::runtime_error exception is thrown.
 */
void CORE_LIBRARY_DLL_SHARED_API SplitString(std::string& subStr1, std::string& subStr2,
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
                              eFloatStringFormat formatting = eFloatStringFormat::normal)
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
 * \brief Convert a floating point value to a string representation using the most suitable
 * formatting. \param[in] value - Floating point value to convert to a string. \param[in]
 * decimalPlaces - Number of decimal places to present after the decimal point. return Resultant
 * string.
 *
 * Convert a single or double precision floating point number to a
 * string representaion.
 */
template <typename T> std::string AutoFormatFloatString(const T value, int decimalPlaces = 1)
{
    std::string formattedValue;

    if (decimalPlaces < 0)
    {
        decimalPlaces = 0;
    }

    const T min    = std::pow(T(10), T(-1) * decimalPlaces);
    const T absVal = std::fabs(value);

    if (absVal < std::numeric_limits<T>::epsilon()) // 0.0 case
    {
        formattedValue = FormatFloatString(value, 1 + decimalPlaces);
    }
    else if ((absVal < min) || (absVal >= T(100000))) // very small and very big values
    {
        formattedValue = FormatFloatString(value, decimalPlaces, eFloatStringFormat::scientific);
    }
    else // everything in between
    {
        if (absVal < T(1))
        {
            formattedValue = FormatFloatString(value, decimalPlaces);
        }
        else if (absVal < T(10))
        {
            formattedValue = FormatFloatString(value, 1 + decimalPlaces);
        }
        else if (absVal < T(100))
        {
            formattedValue = FormatFloatString(value, 2 + decimalPlaces);
        }
        else if (absVal < T(1000))
        {
            formattedValue = FormatFloatString(value, 3 + decimalPlaces);
        }
        else if (absVal < T(10000))
        {
            formattedValue = FormatFloatString(value, 4 + decimalPlaces);
        }
        else
        {
            formattedValue = FormatFloatString(value, 5 + decimalPlaces);
        }
    }

    return formattedValue;
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
    wchar_t replacementChar = L'_');

std::string CORE_LIBRARY_DLL_SHARED_API
            RemoveIllegalChars(const std::string& text, const std::string& illegalChars = "~#%&*{}\\:<>?/+|\"",
                               char replacementChar = '_');

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

/*!
 * \brief Return a flag to indicate whether string contains only alphanumeric chars.
 * \param[in] text - Source string potentially with illegal chars.
 * \return True if contains only alphanumeric chars, false otherwise.
 */
bool CORE_LIBRARY_DLL_SHARED_API IsAlphaNumeric(const std::wstring& text);

bool CORE_LIBRARY_DLL_SHARED_API IsAlphaNumeric(const std::string& text);

/*!
 * \brief Convert a range of data that is convertible to ints to a string of hex values.
 * \param[in] first - Iterator to first item in range
 * \param[in] last - Iterator to end of range
 * \param[in] useUppercase - Use uppercase letters for hex values
 * \param[in] insertSpaces - Insert spaces between each byte value
 * \return Hex string representing input range.
 */
template <typename Iter>
std::string MakeHexString(Iter first, Iter last, bool useUppercase, bool insertSpaces)
{
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    if (useUppercase)
    {
        ss << std::uppercase;
    }

    while (first != last)
    {
        ss << std::setw(2) << static_cast<int>(*first++);

        if (insertSpaces && first != last)
        {
            ss << " ";
        }
    }

    return ss.str();
}

/*!
 * \brief Tokenise a string separated by a separator substring and split it into tokens.
 * \param[in] text - string to tokenise
 * \param[in] separator - separator string
 * \param[in] keepEmptyTokens - keep empty tokens in result vector
 * \return Vector of string tokens.
 */
std::vector<std::string> CORE_LIBRARY_DLL_SHARED_API TokeniseString(std::string const& text,
                                                                    std::string const& separator,
                                                                    bool keepEmptyTokens = false);

/*!
 * \brief Gieven a string containing tokens, replace tokens with specific string values.
 * \param[in] text - string to replace tokens in
 * \param[in] tokenMap - map of token strings and the string values to replace the tokens with in
 *                       'text' arg. \return Vector of string tokens.
 * \return Copy of text with tokens replaced.
 */
std::string CORE_LIBRARY_DLL_SHARED_API
            ReplaceTokens(std::string const& text, std::map<std::string, std::string> const& tokenMap);

} // namespace string_utils
} // namespace core_lib

#endif // STRINGUTILS
