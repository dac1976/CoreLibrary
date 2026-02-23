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

#include "StringUtils/StringUtils.h"
#if defined(NOT_USING_BOOST_LOCALE)
#if (_MSC_VER >= 1800) || defined(__clang__) || defined(__GNUC__)
#include <locale>
#include <codecvt>
#endif
#else
#include <boost/locale.hpp>
#endif
#include <cctype>
#include <regex>
#include <cstring>
#if defined(IS_CPP20)
#include <ranges>
#endif
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The string_utils namespace. */
namespace string_utils
{

// ****************************************************************************
// ReplaceIllegalChars definition
// ****************************************************************************
#if defined(IS_CPP17)
std::string ReplaceIllegalChars(std::string_view text, std::string_view illegalChars,
                                char replacementChar)
#else
std::string  ReplaceIllegalChars(const std::string& text, const std::string& illegalChars,
                                 char replacementChar)
#endif
{
    std::string textFixed(text.begin(), text.end());
    auto        replacer = [=](const char c) { return illegalChars.find(c) != std::string::npos; };
    std::replace_if(textFixed.begin(), textFixed.end(), replacer, replacementChar);
    return textFixed;
}

#if defined(IS_CPP17)
std::wstring ReplaceIllegalChars(std::wstring_view text, std::wstring_view illegalChars,
                                 wchar_t replacementChar)
#else
std::wstring ReplaceIllegalChars(const std::wstring& text, const std::wstring& illegalChars,
                                 wchar_t replacementChar)
#endif
{
    std::wstring textFixed(text.begin(), text.end());
    auto replacer = [=](const wchar_t c) { return illegalChars.find(c) != std::wstring::npos; };
    std::replace_if(textFixed.begin(), textFixed.end(), replacer, replacementChar);
    return textFixed;
}

// ****************************************************************************
// ContainsIllegalChars definition
// ****************************************************************************
#if defined(IS_CPP17)
bool ContainsIllegalChars(std::string_view text, std::string_view illegalChars)
#else
bool         ContainsIllegalChars(const std::string& text, const std::string& illegalChars)
#endif
{
    auto finder = [=](const char c) { return illegalChars.find(c) != std::string::npos; };
    auto count  = std::count_if(text.begin(), text.end(), finder);
    return count > 0;
}

#if defined(IS_CPP17)
bool ContainsIllegalChars(std::wstring_view text, std::wstring_view illegalChars)
#else
bool         ContainsIllegalChars(const std::wstring& text, const std::wstring& illegalChars)
#endif
{
    auto finder = [=](const wchar_t c) { return illegalChars.find(c) != std::wstring::npos; };
    auto count  = std::count_if(text.begin(), text.end(), finder);
    return count > 0;
}

// ****************************************************************************
// IsAlphaNumeric definition
// ****************************************************************************
#if defined(IS_CPP17)
bool IsAlphaNumeric(std::string_view text)
#else
bool         IsAlphaNumeric(const std::string& text)
#endif
{
    auto finder = [=](const char c) { return std::isalnum(static_cast<int>(c)) == 0; };
    auto count  = std::count_if(text.begin(), text.end(), finder);
    return count == 0;
}

#if defined(IS_CPP17)
bool IsAlphaNumeric(std::wstring_view text)
#else
bool         IsAlphaNumeric(const std::wstring& text)
#endif
{
    auto finder = [=](const wchar_t c) { return std::isalnum(static_cast<int>(c)) == 0; };
    auto count  = std::count_if(text.begin(), text.end(), finder);
    return count == 0;
}

// ****************************************************************************
// string <-> wstring conversion
// ****************************************************************************

#if defined(NOT_USING_BOOST_LOCALE)
#if (_MSC_VER >= 1800) || defined(__clang__) || defined(__GNUC__)
// Utility wrapper to adapt locale-bound facets for wstring/wbuffer convert.
template <typename Facet> struct deletable_facet : Facet
{
    using Facet::Facet;
};

std::wstring StringToWString(const std::string& text)
{
#if (_MSC_VER == 1800)
    std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> conv;
#else
    std::wstring_convert<deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>> conv;
#endif
    return conv.from_bytes(text);
}

std::string WStringToString(const std::wstring& text)
{
#if (_MSC_VER == 1800)
    std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> conv;
#else
    std::wstring_convert<deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>> conv;
#endif
    return conv.to_bytes(text);
}
#endif
#else
std::wstring StringToWString(const std::string& text, const std::string& encoding)
{
    return boost::locale::conv::to_utf<wchar_t>(text, encoding);
}

std::string WStringToString(const std::wstring& text, const std::string& encoding)
{
    return boost::locale::conv::from_utf(text, encoding);
}
#endif

// ****************************************************************************
// Tokenisation helper functions definitions
// ****************************************************************************

std::vector<std::string> TokeniseString(std::string const& text, std::string const& separator,
                                        bool keepEmptyTokens)
{
    using separator_t = boost::char_separator<char>;
    std::vector<std::string> tokens;

    if (keepEmptyTokens)
    {
        separator_t                   sep(separator.c_str(), nullptr, boost::keep_empty_tokens);
        boost::tokenizer<separator_t> tokzr(text, sep);

        for (boost::tokenizer<separator_t>::iterator itr = tokzr.begin(); itr != tokzr.end(); ++itr)
        {
            std::string token = *itr;
            boost::trim(token);
            tokens.emplace_back(token);
        }
    }
    else
    {
        separator_t                   sep(separator.c_str());
        boost::tokenizer<separator_t> tokzr(text, sep);

        for (boost::tokenizer<separator_t>::iterator itr = tokzr.begin(); itr != tokzr.end(); ++itr)
        {
            std::string token = *itr;
            boost::trim(token);
            tokens.emplace_back(token);
        }
    }

    return tokens;
}

std::vector<std::string> TokeniseEscapedString(std::string const& text,
                                               std::string const& separator, bool keepEmptyTokens)
{
    using separator_t = boost::escaped_list_separator<char>;

    std::vector<std::string>      tokens;
    separator_t                   sep("\\", separator, "\"");
    boost::tokenizer<separator_t> tok(text, sep);

    for (boost::tokenizer<separator_t>::iterator itr = tok.begin(); itr != tok.end(); ++itr)
    {
        std::string name = *itr;

        if (!name.empty() || keepEmptyTokens)
        {
            boost::trim(name);
            tokens.emplace_back(name);
        }
    }

    return tokens;
}

std::string ReplaceTokens(std::string const&                        text,
                          std::map<std::string, std::string> const& tokenMap)
{
    auto textCopy = text;

    for (auto const& token : tokenMap)
    {
        textCopy = std::regex_replace(textCopy, std::regex(token.first), token.second);
    }

    return textCopy;
}

std::wstring ReplaceTokens(std::wstring const&                         text,
                           std::map<std::wstring, std::wstring> const& tokenMap)
{
    auto textCopy = text;

    for (auto const& token : tokenMap)
    {
        textCopy = std::regex_replace(textCopy, std::wregex(token.first), token.second);
    }

    return textCopy;
}

// ****************************************************************************
// RemoveSurroundingQuotes definition
// ****************************************************************************

std::string RemoveSurroundingQuotes(std::string const& text)
{
    std::string textCopy = text;

    if (!textCopy.empty())
    {
        if ((textCopy.front() == '\"') || (textCopy.front() == '\''))
        {
            textCopy = textCopy.substr(1, std::string::npos);
        }

        if ((textCopy.back() == '\"') || (textCopy.back() == '\''))
        {
            textCopy.resize(textCopy.size() - 1);
        }
    }

    return textCopy;
}

// ****************************************************************************
// SafeConvertCharArrayToStdString definition
// ****************************************************************************

std::string SafeConvertCharArrayToStdString(const char* text, size_t length)
{
    if (nullptr == text)
    {
        throw std::invalid_argument("text is null");
    }

    auto first = text;
    auto last  = std::next(first, length);
    auto temp  = std::string(first, last);
    auto pos   = temp.find_first_of('\0');
    temp       = temp.substr(0, pos);
    return temp;
}

// ****************************************************************************
// SafeCompareCharArrays definition
// ****************************************************************************

bool SafeCompareCharArrays(const char* text1, size_t length1, const char* text2, size_t length2)
{
    if (nullptr == text1)
    {
        throw std::invalid_argument("text1 is null");
    }

    if (nullptr == text2)
    {
        throw std::invalid_argument("text2 is null");
    }

    // Adjust length to the first NULL within [text1, text1 + length1), if any.
    if (const void* p = std::memchr(text1, '\0', length1))
    {
        length1 = static_cast<const char*>(p) - text1;
    }

    // Adjust length to the first NULL within [text, text + length), if any.
    if (const void* p = std::memchr(text2, '\0', length2))
    {
        length2 = static_cast<const char*>(p) - text2;
    }

    if (length1 != length2)
    {
        return false;
    }

    return std::memcmp(text1, text2, length1) == 0;
}

// ****************************************************************************
// SafeCopyCharArray definitions
// ****************************************************************************

void SafeCopyCharArray(char* dest, size_t destLen, char const* src, size_t srcLen, bool forceNull)
{
    if (nullptr == dest)
    {
        throw std::invalid_argument("dest is null");
    }

    if (nullptr == src)
    {
        return;
    }

    auto length = std::min(destLen, srcLen);
    std::copy(src, std::next(src, length), dest);

    if (srcLen < destLen)
    {
        dest[srcLen] = 0;
    }
    else if (forceNull)
    {
        dest[destLen - 1] = 0;
    }
}

#if defined(IS_CPP17)
void SafeCopyCharArray(char* dest, size_t destLen, std::string_view src, bool forceNull)
#else
void SafeCopyCharArray(char* dest, size_t destLen, std::string const& src, bool forceNull)
#endif
{
    SafeCopyCharArray(dest, destLen, src.data(), src.size(), forceNull);
}

// ****************************************************************************
// Concatenate definition
// ****************************************************************************
#if defined(IS_CPP17)
std::string ConcatenateStrings(std::vector<std::string_view> const& args)
{
    // Calculate the total length of all substrings
    size_t totalLength =
        std::accumulate(args.begin(),
                        args.end(),
                        size_t(0),
                        [](size_t sum, std::string_view str) { return sum + str.size(); });

    // Reserve the correct amount of bytes
    std::string result;
    result.reserve(totalLength);

    // Append all substrings
    for (auto& str : args)
    {
        result += str;
    }

    return result;
}

std::string const& ConcatenateStrings(std::string&                         workspace,
                                      std::vector<std::string_view> const& args)
{
    // Calculate the total length of all substrings
    size_t totalLength =
        std::accumulate(args.begin(),
                        args.end(),
                        size_t(0),
                        [](size_t sum, std::string_view str) { return sum + str.size(); });

    // Reserve the correct amount of bytes
    workspace.clear();
    workspace.reserve(totalLength);

    // Append all substrings
    for (auto& str : args)
    {
        workspace += str;
    }

    return workspace;
}

// ****************************************************************************
// String vectors
// ****************************************************************************
bool CompareUnorderedVectors(std::vector<std::string> const& v1, std::vector<std::string> const& v2)
{
    if (v1.size() != v2.size())
    {
        return false;
    }

    std::vector<std::string_view> v1View(v1.begin(), v1.end());
    std::vector<std::string_view> v2View(v2.begin(), v2.end());

    std::sort(v1View.begin(), v1View.end());
    std::sort(v2View.begin(), v2View.end());

#if defined(IS_CPP20)
    return std::ranges::equal(v1View, v2View);
#else
    return v1View == v2View;
#endif
}
#endif // (defined(HGL_CPP17) || defined(IS_CPP20))

} // namespace string_utils
} // namespace core_lib

