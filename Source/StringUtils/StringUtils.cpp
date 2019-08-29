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
 * \file StringUtils.cpp
 * \brief File containing definitions relating various string utilities.
 */

#include "StringUtils/StringUtils.h"
#include <vector>
#include <locale>
#include <codecvt>
#include <stdexcept>
#include <regex>
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/tokenizer.hpp>
#include <boost/throw_exception.hpp>

namespace core_lib
{
namespace string_utils
{

// ****************************************************************************
// PackStdString definition
// ****************************************************************************
void PackStdString(std::string& line)
{
    size_t pos = line.find_first_of('\0');

    if (pos < std::string::npos)
    {
        std::string correctedLine{line.begin(), line.begin() + static_cast<int>(pos)};
        line.swap(correctedLine);
    }
}

// ****************************************************************************
// SplitString definition
// ****************************************************************************
void SplitString(std::string& subStr1, std::string& subStr2, const std::string& toSplit,
                 const std::string& delimiters, eSplitStringResult option)
{
    std::vector<std::string> splitVec;
    boost::split(splitVec,
                 toSplit,
                 boost::is_any_of(delimiters),
                 option == eSplitStringResult::trimmed ? boost::token_compress_on
                                                       : boost::token_compress_off);

    if (splitVec.size() <= 1U)
    {
        BOOST_THROW_EXCEPTION(
            std::invalid_argument(std::string("cannot find delimiter in string: ") + toSplit));
    }
    else if (splitVec.size() > 2U)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("too many substrings"));
    }

    subStr1 = splitVec[0];
    subStr2 = splitVec[1];

    if (option == eSplitStringResult::trimmed)
    {
        boost::trim(subStr1);
        boost::trim(subStr2);
    }
}

// ****************************************************************************
// RemoveIllegalChars definition
// ****************************************************************************
std::wstring RemoveIllegalChars(const std::wstring& text, const std::wstring& illegalChars,
                                wchar_t replacementChar)
{
    auto textFixed = text;
    auto replacer  = [=](const wchar_t c) { return illegalChars.find(c) != std::wstring::npos; };
    std::replace_if(textFixed.begin(), textFixed.end(), replacer, replacementChar);
    return textFixed;
}

std::string RemoveIllegalChars(const std::string& text, const std::string& illegalChars,
                               char replacementChar)
{
    auto textFixed = text;
    auto replacer  = [=](const char c) { return illegalChars.find(c) != std::string::npos; };
    std::replace_if(textFixed.begin(), textFixed.end(), replacer, replacementChar);
    return textFixed;
}

// ****************************************************************************
// string <-> wstring conversion
// ****************************************************************************

// Utility wrapper to adapt locale-bound facets for wstring/wbuffer convert.
template <typename Facet> struct deletable_facet : Facet
{
    using Facet::Facet;
};

std::wstring StringToWString(const std::string& text)
{
    std::wstring_convert<deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>> conv;
    return conv.from_bytes(text);
}

std::string WStringToString(const std::wstring& text)
{
    std::wstring_convert<deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>> conv;
    return conv.to_bytes(text);
}

// ****************************************************************************
// IsAlphaNumeric definition
// ****************************************************************************
bool IsAlphaNumeric(const std::wstring& text)
{
    auto finder = [=](const wchar_t c) { return std::isalnum(static_cast<int>(c)) == 0; };
    auto count  = std::count_if(text.begin(), text.end(), finder);
    return count == 0;
}

bool IsAlphaNumeric(const std::string& text)
{
    auto finder = [=](const char c) { return std::isalnum(static_cast<int>(c)) == 0; };
    auto count  = std::count_if(text.begin(), text.end(), finder);
    return count == 0;
}

// ****************************************************************************
// Token based string helper definitions.
// ****************************************************************************
std::vector<std::string> TokeniseString(std::string const& text, std::string const& separator,
                                        bool keepEmptyTokens)
{
    using separator_t = boost::char_separator<char>;

    if (keepEmptyTokens)
    {
        separator_t                   sep(separator.c_str(), nullptr, boost::keep_empty_tokens);
        boost::tokenizer<separator_t> tokens(text, sep);
        return {tokens.begin(), tokens.end()};
    }
    else
    {
        separator_t                   sep(separator.c_str());
        boost::tokenizer<separator_t> tokens(text, sep);
        return {tokens.begin(), tokens.end()};
    }
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

} // namespace string_utils
} // namespace core_lib
