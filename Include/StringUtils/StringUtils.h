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

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <stdexcept>
#include <utility>
#include <map>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"
#if defined(IS_CPP17) && !defined(NO_FROM_CHARS)
#include <charconv>
#include <system_error>
#endif
#if defined(IS_CPP17)
#include <string_view>
#include <type_traits>
#include <numeric>
#endif

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The string_utils namespace. */
namespace string_utils
{

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
 * \return Copy of text with illegal chars replaced.
 */
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API std::string ReplaceIllegalChars(std::string_view text,
                                std::string_view illegalChars    = "~#%&*{}\\:<>?/+|\"",
                                char             replacementChar = '_');

CORE_LIBRARY_DLL_SHARED_API std::wstring ReplaceIllegalChars(std::wstring_view text,
                                 std::wstring_view illegalChars    = L"~#%&*{}\\:<>?/+|\"",
                                 wchar_t           replacementChar = L'_');
#else
CORE_LIBRARY_DLL_SHARED_API std::string ReplaceIllegalChars(const std::string& text,
                                const std::string& illegalChars    = "~#%&*{}\\:<>?/+|\"",
                                char               replacementChar = '_');

CORE_LIBRARY_DLL_SHARED_API std::wstring ReplaceIllegalChars(const std::wstring& text,
                                 const std::wstring& illegalChars    = L"~#%&*{}\\:<>?/+|\"",
                                 wchar_t             replacementChar = L'_');
#endif

/*!
 * \brief Return a flag to indicate whether string contains illegal chars.
 * \param[in] text - Source string potentially with illegal chars.
 * \param[in] illegalChars - String containing illegal chars to look for in text.
 * \return True if contains illegal chars, false otherwise.
 */
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API bool ContainsIllegalChars(std::string_view text,
                          std::string_view illegalChars = "~#%&*{}\\:<>?/+|\"");

CORE_LIBRARY_DLL_SHARED_API bool ContainsIllegalChars(std::wstring_view text,
                          std::wstring_view illegalChars = L"~#%&*{}\\:<>?/+|\"");
#else
CORE_LIBRARY_DLL_SHARED_API bool ContainsIllegalChars(const std::string& text,
                                  const std::string& illegalChars = "~#%&*{}\\:<>?/+|\"");

CORE_LIBRARY_DLL_SHARED_API bool ContainsIllegalChars(const std::wstring& text,
                          const std::wstring& illegalChars = L"~#%&*{}\\:<>?/+|\"");
#endif

/*!
 * \brief Return a flag to indicate whether string contains only alphanumeric chars.
 * \param[in] text - Source string potentially with illegal chars.
 * \return True if contains only alphanumeric chars, false otherwise.
 */
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API bool IsAlphaNumeric(std::string_view text);
CORE_LIBRARY_DLL_SHARED_API bool IsAlphaNumeric(std::wstring_view text);
#else
CORE_LIBRARY_DLL_SHARED_API bool IsAlphaNumeric(const std::string& text);
CORE_LIBRARY_DLL_SHARED_API bool IsAlphaNumeric(const std::wstring& text);
#endif

#if defined(NOT_USING_BOOST_LOCALE)
#if (_MSC_VER >= 1800) || defined(__clang__) || defined(__GNUC__)
/*!
 * \brief Convert a std::string to std::wstring.
 * \param[in] text - Source string.
 * \return Converted string.
 */
CORE_LIBRARY_DLL_SHARED_API std::wstring StringToWString(const std::string& text);

/*!
 * \brief Convert a std::wstring to std::string.
 * \param[in] text - Source string
 * \return Converted string.
 */
CORE_LIBRARY_DLL_SHARED_API std::string WStringToString(const std::wstring& text);
#endif
#else
/*!
 * \brief Convert a std::string to std::wstring.
 * \param[in] text - Source string.
 * \param[in] encoding - Source string encoding, e.g. "ISO-8859-1", "Latin1" etc.
 * \return Converted string.
 */
CORE_LIBRARY_DLL_SHARED_API std::wstring StringToWString(const std::string& text, const std::string& encoding = "ISO-8859-1");

/*!
 * \brief Convert a std::wstring to std::string.
 * \param[in] text - Source string
 * \param[in] encoding - Result string encoding, e.g. "ISO-8859-1", "Latin1" etc.
 * \return Converted string.
 */
CORE_LIBRARY_DLL_SHARED_API std::string WStringToString(const std::wstring& text, const std::string& encoding = "ISO-8859-1");
#endif

/*!
 * \brief Convert a range of data that is convertible to ints to a string of hex values.
 * \param[in] first - Iterator to first item in range
 * \param[in] last - Iterator to end of range
 * \param[in] useUppercase - Use uppercase letters for hex values
 * \param[in] insertSeparator - Insert separator char between each byte value
 * \param[in] separator - Separator char to use, defaults to a space.
 * \return Hex string representing input range.
 */
template <typename Iter>
void MakeHexStream(std::ostream& os, Iter first, Iter last, bool useUppercase, bool insertSeparator,
                   char separator = ' ')
{
    os << std::hex << std::setfill('0');

    if (useUppercase)
    {
        os << std::uppercase;
    }

    while (first != last)
    {
        os << std::setw(2) << static_cast<int>(*first++);

        if (insertSeparator && first != last)
        {
            os << separator;
        }
    }
}

template <typename Iter>
std::string MakeHexString(Iter first, Iter last, bool useUppercase, bool insertSeparator,
                          char separator = ' ')
{
    std::ostringstream ss;

    MakeHexStream(ss, first, last, useUppercase, insertSeparator, separator);

    return ss.str();
}

/*!
 * \brief Convert a single unsigned integral value to a hex string.
 * \param[in] value - Unsigned integer value
 * \param[in] useUppercase - Use uppercase letters for hex values
 * \param[in] insertLeadingSymbol - Insert leading "0x".
 * \param[in] minNumHexDigits - how many hex digits to include, default is to use as many as
 *                              required for integral type T.
 * \return Hex string representing input range.
 */
template <typename T>
void MakeHexStream(std::ostream& os, T value, bool useUppercase = true,
                   bool insertLeadingSymbol = true, size_t minNumHexDigits = 0)
{
    static_assert(std::is_unsigned<T>::value, "T must be an unsigned integral type");

    os << std::hex << std::setfill('0');

    if (insertLeadingSymbol)
    {
        os << "0x";
    }

    if (useUppercase)
    {
        os << std::uppercase;
    }

    if (0 == minNumHexDigits)
    {
        os << std::setw(sizeof(value) * 2);
    }
    else
    {
        os << std::setw(minNumHexDigits);
    }

    os << value;
}

template <typename T>
std::string MakeHexString(T value, bool useUppercase = true, bool insertLeadingSymbol = true,
                          size_t minNumHexDigits = 0)
{
    std::ostringstream ss;

    MakeHexStream(ss, value, useUppercase, insertLeadingSymbol, minNumHexDigits);

    return ss.str();
}

/*!
 * \brief Tokenise a string separated by a separator substring and split it into tokens.
 * \param[in] text - string to tokenise
 * \param[in] separator - separator string
 * \param[in] keepEmptyTokens - keep empty tokens in result vector
 * \return Vector of string tokens, trimmed to remove leading/trailing spaces.
 * The following is supported (when separator = ","):
 *    Field 1,Field 2,Field 3
 */
CORE_LIBRARY_DLL_SHARED_API std::vector<std::string> TokeniseString(std::string const& text, std::string const& separator,
                                 bool keepEmptyTokens = false);

/*!
 * \brief Tokenise a string separated by a separator substring and split it into tokens,
 *        where the tokens can be escaped.
 * \param[in] text - string to tokenise
 * \param[in] separator - separator string
 * \param[in] keepEmptyTokens - keep empty tokens in result vector
 * \return Vector of string tokens, trimmed to remove leading/trailing spaces.
 *
 * The escape char is '\\' and the quote character is '\"'. This fits the CSV definition.
 * The following are supported (when separator = ","):
 *    Field 1,Field 2,Field 3
 *    Field 1,"Field 2, with comma",Field 3
 *    Field 1,Field 2 with \"embedded quote\",Field 3
 *    Field 1, Field 2 with \n new line,Field 3
 *    Field 1, Field 2 with embedded \\ ,Field 3
 */
CORE_LIBRARY_DLL_SHARED_API std::vector<std::string> TokeniseEscapedString(std::string const& text,
                                               std::string const& separator,
                                               bool               keepEmptyTokens = false);

/*!
 * \brief Given a string containing tokens, replace tokens with specific string values.
 * \param[in] text - string to replace tokens in
 * \param[in] tokenMap - map of token strings and the string values to replace the tokens with in
 *                       'text' arg. \return Vector of string tokens.
 * \return Copy of text with tokens replaced.
 */
CORE_LIBRARY_DLL_SHARED_API std::string ReplaceTokens(std::string const&                        text,
                           std::map<std::string, std::string> const& tokenMap);
CORE_LIBRARY_DLL_SHARED_API std::wstring ReplaceTokens(std::wstring const&                         text,
                           std::map<std::wstring, std::wstring> const& tokenMap);

/*!
 * \brief Given a string remove surrounding quotes, either ' or ".
 * \param[in] text - string to remove quotes from.
 * \return Copy of text with quotes removed.
 */
CORE_LIBRARY_DLL_SHARED_API std::string RemoveSurroundingQuotes(std::string const& text);

/*!
 * \brief Given a char array that may or may not have a null terminator convert to std::string.
 * \param[in] text - char array to convert safely
 * \param[in] length - length in bytes of text not inc null char
 * \return String as a std::string
 *
 * Function throws if text is null.
 */
CORE_LIBRARY_DLL_SHARED_API std::string SafeConvertCharArrayToStdString(const char* text, size_t length);

/*!
 * \brief Given a char array that may or may not have a null terminator convert to std::string.
 * \param[in] text - char array to convert safely
 * \return String as a std::string
 */
template <size_t length> std::string SafeConvertCharArrayToStdString(const char (&text)[length])
{
    return SafeConvertCharArrayToStdString(text, length);
}

/*!
 * \brief Given 2 strings that may or may not have a null terminator compare them safely.
 * \param[in] text1 - char array A
 * \param[in] length1 - length in bytes of text1 not inc null char
 * \param[in] text2 - char array B
 * \param[in] length1 - length in bytes of text2 not inc null char
 * \return true if match, false otherwise
 *
 * Function throws if either text1, text2 or both is null.
 */
bool CORE_LIBRARY_DLL_SHARED_API SafeCompareCharArrays(const char* text1, size_t length1, const char* text2, size_t length2);

/*!
 * \brief Given 2 strings that may or may not have a null terminator compare them safely.
 * \param[in] text1 - char array A
 * \param[in] text2 - char array B
 * \return true if match, false otherwise
 */
template <size_t length1, size_t length2>
bool SafeCompareCharArrays(const char (&text1)[length1], const char (&text2)[length2])
{
    return SafeCompareCharArrays(text1, length1, text2, length2);
}

/*!
 * \brief Given 2 strings that may or may not have a null terminator copy
 * one to the other safely.
 * \param[in] dest - destination char array
 * \param[in] destLen - total length in bytes of dest
 * \param[in] src - source char array
 * \param[in] srcLen - length in bytes of src not including null char
 * \param[in] forceNull - force a null to be set in last char may cause truncation
 *
 * This function can result in dest not being null terminated and that is
 * correct behaviour and is an intended side-effect of this function.
 * The dest string will be null terminated if destLen < srcLen or forceNull == true.
 *
 * Function throws if dest is null.
 */
CORE_LIBRARY_DLL_SHARED_API void SafeCopyCharArray(char* dest, size_t destLen, char const* src, size_t srcLen,
                       bool forceNull = false);

/*!
 * \brief Given 2 strings that may or may not have a null terminator copy
 * one to the other safely.
 * \param[in] dest - destination char array
 * \param[in] src - source char array
 * \param[in] forceNull - force a null to be set in last char may cause truncation
 *
 * This function can result in dest not being null terminated and that is
 * correct behaviour and is an intended side-effect of this function.
 * The dest string will be null terminated if destLen < srcLen or forceNull == true.
 */
template <size_t destLen, size_t srcLen>
void SafeCopyCharArray(char (&dest)[destLen], const char (&src)[srcLen], bool forceNull = false)
{
    SafeCopyCharArray(dest, destLen, src, srcLen, forceNull);
}

/*!
 * \brief Given 2 strings that may or may not have a null terminator copy
 * one to the other safely.
 * \param[in] dest - destination char array as a char pointer
 * \param[in] destLen - total length in bytes of dest
 * \param[in] src - source char array
 * \param[in] forceNull - force a null to be set in last char may cause truncation
 *
 * This function can result in dest not being null terminated and that is
 * correct behaviour and is an intended side-effect of this function.
 * The dest string will be null terminated if destLen < srcLen or forceNull == true.
 *
 * Function throws if dest is null.
 */
template <size_t srcLen>
void SafeCopyCharArray(char* dest, size_t destLen, const char (&src)[srcLen],
                       bool forceNull = false)
{
    SafeCopyCharArray(dest, destLen, src, srcLen, forceNull);
}

/*!
 * \brief Given 2 strings that may or may not have a null terminator copy
 * one to the other safely.
 * \param[in] dest - destination char array
 * \param[in] src - source char array
 * \param[in] srcLen - length in bytes of src not including null char
 * \param[in] forceNull - force a null to be set in last char may cause truncation
 *
 * This function can result in dest not being null terminated and that is
 * correct behaviour and is an intended side-effect of this function.
 * The dest string will be null terminated if destLen < srcLen or forceNull == true.
 */
template <size_t destLen>
void SafeCopyCharArray(char (&dest)[destLen], char const* src, size_t srcLen,
                       bool forceNull = false)
{
    SafeCopyCharArray(dest, destLen, src, srcLen, forceNull);
}

/*!
 * \brief Given 2 strings that may or may not have a null terminator copy
 * one to the other safely.
 * \param[in] dest - destination char array
 * \param[in] src - source string
 * \param[in] forceNull - force a null to be set in last char may cause truncation
 *
 * This function can result in dest not being null terminated and that is
 * correct behaviour and is an intended side-effect of this function.
 * The dest string will be null terminated if destLen < srcLen or forceNull == true.
 */
template <size_t destLen>
#if defined(IS_CPP17)
void SafeCopyCharArray(char (&dest)[destLen], std::string_view src, bool forceNull = false)
#else
void SafeCopyCharArray(char (&dest)[destLen], std::string const& src, bool forceNull = false)
#endif
{
    SafeCopyCharArray(dest, destLen, src.data(), src.size(), forceNull);
}

/*!
 * \brief Given 2 strings that may or may not have a null terminator copy
 * one to the other safely.
 * \param[in] dest - destination char array as a char pointer
 * \param[in] destLen - total length in bytes of dest
 * \param[in] src - source string
 * \param[in] forceNull - force a null to be set in last char may cause truncation
 *
 * This function can result in dest not being null terminated and that is
 * correct behaviour and is an intended side-effect of this function.
 * The dest string will be null terminated if destLen < srcLen or forceNull == true.
 *
 * Function throws if dest is null.
 */
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API void SafeCopyCharArray(char* dest, size_t destLen, std::string_view src, bool forceNull = false);
#else
CORE_LIBRARY_DLL_SHARED_API void SafeCopyCharArray(char* dest, size_t destLen, std::string const& src, bool forceNull = false);
#endif

/*!
 * \brief Convert a character array to an integer safely.
 * \param[in] src - source text containing numerical value
 * \param[in] length - total length in bytes of src
 * \return A pair containing the numerical value plus a boolean indicating success.
 */
template <typename TInt>
std::pair<TInt, bool> SafeConvertCharArrayToInt(const char* text, size_t length)
{
    static_assert(std::is_integral<TInt>::value, "TInt must be an integral type");

    std::pair<TInt, bool> result(0, false);

    if ((nullptr == text) || (0 == length))
    {
        return result;
    }

#if defined(IS_CPP17) && !defined(NO_FROM_CHARS)
    auto [ptr, res] = std::from_chars(text, text + length, result.first);
    result.second   = res == std::errc();
#else
    try
    {
        // We'll furst try to convert to a signed long long int (i.e.in64_t)
        // then we'll static case to the proper type from there.
        result.first  = static_cast<TInt>(std::stoll(std::string(text, length)));
        result.second = true;
    }
    catch (...)
    {
        // Do nothing.
    }
#endif

    return result;
}

#if defined(IS_CPP17)
/*!
 * \brief Functor to help with std containers when trying to compare std::string_view with
 * std::string. Such as std::map<std::string, T>::find(XXX).
 * Use like: std::map<std::string, T, StringViewComparator>.
 */
struct CORE_LIBRARY_DLL_SHARED_API StringViewComparator
{
    // Enables heterogeneous lookup.
    using is_transparent = void;

    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return lhs < rhs;
    }

    bool operator()(const std::string& lhs, std::string_view rhs) const
    {
        return lhs < rhs;
    }

    bool operator()(std::string_view lhs, const std::string& rhs) const
    {
        return lhs < rhs;
    }

    bool operator()(std::string_view lhs, std::string_view rhs) const
    {
        return lhs < rhs;
    }
};

/*!
 * \brief Functor to help with std containers when trying to compare std::string_view with
 * std::string for equality. Such as std::unordered_map<std::string, T>::find(XXX).
 * Use like: std::unordered_map<std::string, T, std::hash<std::string>, StringViewEquality>.
 */
struct CORE_LIBRARY_DLL_SHARED_API StringViewEquality
{
    // Enables heterogeneous lookup.
    using is_transparent = void;

    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return lhs == rhs;
    }

    bool operator()(const std::string& lhs, std::string_view rhs) const
    {
        return lhs == rhs;
    }

    bool operator()(std::string_view lhs, const std::string& rhs) const
    {
        return lhs == rhs;
    }

    bool operator()(std::string_view lhs, std::string_view rhs) const
    {
        return lhs == rhs;
    }
};

/*!
 * \brief Fast concatenation of strings, faster than std::stringstream and other methods.
 * \param[in] args - variable number of std::string_view args
 * \return A std::string containing the concatenation of the inputs.
 */
template <typename... Args>
std::enable_if_t<(std::is_same_v<std::string_view, Args> && ...), std::string>
FastConcatenateStrings(Args... args)
{
    // Calculate the total length of all substrings
    size_t totalLength = (args.size() + ... + 0);

    // Reserve the correct amount of bytes
    std::string result;
    result.reserve(totalLength);

    // Append all substrings
    (result += ... += args);

    return result;
}

/*!
 * \brief Fast concatenation of strings, faster than std::stringstream and other methods.
 * \param[in] workspace - string to use as workspace
 * \param[in] args - variable number of std::string_view args
 * \return A std::string const& containing the concatenation of the inputs contained in workspace.
 */
template <typename... Args>
std::enable_if_t<(std::is_same_v<std::string_view, Args> && ...), std::string const&>
FastConcatenateStrings(std::string& workspace, Args... args)
{
    // Calculate the total length of all substrings
    size_t totalLength = (args.size() + ... + 0);

    // Reserve the correct amount of bytes
    workspace.clear();
    workspace.reserve(totalLength);

    // Append all substrings
    (workspace += ... += args);

    return workspace;
}

/*!
 * \brief Concatenation of strings, faster than std::stringstream and other methods.
 * \param[in] args - std::vector of std::string_views to concatenate
 * \return A std::string containing the concatenation of the inputs.
 */
CORE_LIBRARY_DLL_SHARED_API std::string ConcatenateStrings(std::vector<std::string_view> const& args);

/*!
 * \brief Concatenation of strings, faster than std::stringstream and other methods.
 * \param[in] workspace - string to use as workspace
 * \param[in] args - std::vector of std::string_views to concatenate
 * \return A std::string const& containing the concatenation of the inputs contained in workspace.
 */
CORE_LIBRARY_DLL_SHARED_API std::string const& ConcatenateStrings(std::string& workspace,
                                      std::vector<std::string_view> const& args);

// ****************************************************************************
// String vectors
// ****************************************************************************

/*!
 * \brief Quick way to compare vectors of strings reducing memory copying.
 * \param[in] v1 - first vector of strings
 * \param[in] v2 - second vector of strings
 * \return true if vectors contain the same strings regardless of order.
 *
 * Internally uses string_view to avoid needing to copy strings.
 *
 * Vectors are equal if they are the same size and contain the same
 * strings regardless of their order.
 */
CORE_LIBRARY_DLL_SHARED_API bool CompareUnorderedVectors(std::vector<std::string> const& v1,
                                                std::vector<std::string> const& v2);
#endif

} // namespace string_utils
} // namespace core_lib

#endif // STRINGUTILS_H
