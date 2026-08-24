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

/*!
 * \file IniFile.cpp
 * \brief File containing definitions relating the IniFile class.
 */

#include "IniFile/IniFile.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/throw_exception.hpp>
#include "StringUtils/StringUtils.h"

namespace core_lib
{
namespace ini_file
{

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
// 'class IniFile' definition
// ****************************************************************************
static bool IsBlankLine(const std::string& line)
{
    return line.empty() || (line == "");
}

static bool IsCommentLine(const std::string& line, char& delimiter, std::string& comment)
{
    if (line.empty())
    {
        return false;
    }

    if (line.front() != ';' && line.front() != '#')
    {
        return false;
    }

    delimiter = line.front();
    comment   = line.substr(1);

    return true;
}

static bool IsSectionLine(const std::string& line, std::string& section)
{
    bool isSection = (line.front() == '[') && (line.back() == ']');

    if (isSection)
    {
        section = line.substr(1, line.size() - 2);
        boost::trim(section);
    }

    return isSection;
}

static bool IsKeyLine(const std::string& line, std::string& key, std::string& value)
{
    auto pos = line.find_first_of('=');

    if (std::string::npos == pos)
    {
        return false;
    }

    key   = boost::trim_copy(line.substr(0, pos));
    value = boost::trim_copy(line.substr(pos + 1, std::string::npos));

    return true;
}

static bool StringToBool(const std::string& text)
{
    const std::string value = boost::algorithm::to_lower_copy(
        boost::algorithm::trim_copy(text));

    if ((value == "1") ||
        (value == "true") ||
        (value == "yes") ||
        (value == "on"))
    {
        return true;
    }

    if ((value == "0") ||
        (value == "false") ||
        (value == "no") ||
        (value == "off"))
    {
        return false;
    }

    BOOST_THROW_EXCEPTION(
        std::runtime_error(
            "failed to convert '" + text + "' to bool"));
}

static int32_t StringToInt32(const std::string& text)
{
    std::size_t pos{};

    const auto value = std::stoi(text, &pos);

    if (pos != text.size())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("invalid int32 value"));
    }

    return static_cast<int32_t>(value);
}

static int64_t StringToInt64(const std::string& text)
{
    std::size_t pos{};

    const auto value = std::stoll(text, &pos);

    if (pos != text.size())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("invalid int64 value"));
    }

    return static_cast<int64_t>(value);
}

static double StringToDouble(const std::string& text)
{
    std::size_t pos{};

    const auto value = std::stod(text, &pos);

    if (pos != text.size())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("invalid double value"));
    }

    return value;
}

static long double StringToLongDouble(const std::string& text)
{
    std::size_t pos{};

    const auto value = std::stold(text, &pos);

    if (pos != text.size())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("invalid long double value"));
    }

    return value;
}

static void RemoveUtf8Bom(std::string& line)
{
    constexpr unsigned char bom0 = 0xEF;
    constexpr unsigned char bom1 = 0xBB;
    constexpr unsigned char bom2 = 0xBF;

    if (line.size() >= 3 &&
        static_cast<unsigned char>(line[0]) == bom0 &&
        static_cast<unsigned char>(line[1]) == bom1 &&
        static_cast<unsigned char>(line[2]) == bom2)
    {
        line.erase(0, 3);
    }
}

static std::runtime_error ParseError(
    size_t line,
    const std::string& message)
{
    std::ostringstream os;
    os << "INI parse error at line " << line << ": " << message;
    return std::runtime_error(os.str());
}

static bool IsEmptyString(const std::string& str)
{
    // Both checks are intentional for compatibility with older
    // C++Builder/Clang STL implementations.
    return str.empty() || (str == "");
}

#ifdef USE_DEFAULT_CONSTRUCTOR_
IniFile::IniFile()
    : m_changesMade(false)
    , m_iniFilePath("config.ini")
{
}
#endif

#ifdef USE_EXPLICIT_MOVE_
IniFile::IniFile(IniFile&& ini)
    : m_changesMade(false)
    , m_iniFilePath("config.ini")
{
    *this = std::move(ini);
}

IniFile& IniFile::operator=(IniFile&& ini)
{
    m_changesMade = false;
    std::swap(m_iniFilePath, ini.m_iniFilePath);
    std::swap(m_sectionMap, ini.m_sectionMap);
    std::swap(m_lines, ini.m_lines);
    return *this;
}

#endif

IniFile::IniFile(const std::string& iniFilePath)
{
    LoadFile(iniFilePath);
}

void IniFile::LoadFile(const std::string& iniFilePath)
{
    m_changesMade = false;
    m_iniFilePath = iniFilePath;
    m_sectionMap.clear();
    m_lines.clear();
    std::ifstream iniFile(m_iniFilePath);

    if (!iniFile.is_open() || !iniFile.good())
    {
        // INI file doesn't exist but don't want to sto pus being able to
        // fill out one in memory and create one on disk.
        return;
    }

    std::stringstream iniStream;
    std::copy(std::istreambuf_iterator<char>(iniFile),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(iniStream));
    iniFile.close();

    auto sectIt = m_sectionMap.end();

    std::string line;
    size_t lineNumber{0};

    while (std::getline(iniStream, line))
    {
        ++lineNumber;

        // Gracefully handle UTF-8 with BOM, if we find BOM remove it.
        if (1 == lineNumber)
        {
            RemoveUtf8Bom(line);
        }

        PackStdString(line);
        boost::trim(line);
        std::string str1, str2;
        char commentDelim;

        if (IsBlankLine(line))
        {
            // Remove blank lines on load. We'll put them back between sections
            // upon writing back to disk.l
            //
            // If we wanted to keep blank lines we could do:
            // m_lines.insert(m_lines.end(), std::make_shared<BlankLine>());
        }
        else if (IsCommentLine(line, commentDelim, str1))
        {
            m_lines.insert(m_lines.end(),
                           std::make_shared<if_private::CommentLine>(commentDelim, str1));
        }
        else if (IsSectionLine(line, str1))
        {
            if (str1 == "")
            {
                BOOST_THROW_EXCEPTION(ParseError(lineNumber, "invalid section '" + str1 + "'"));
            }

            if (m_sectionMap.find(str1) != m_sectionMap.end())
            {
                BOOST_THROW_EXCEPTION(ParseError(lineNumber, "duplicate section '" + str1 + "'"));
            }

            auto sectLineIter =
                m_lines.insert(m_lines.end(), std::make_shared<if_private::SectionLine>(str1));

            std::pair<section_iter, bool> result{m_sectionMap.insert(
                std::make_pair(str1, if_private::SectionDetails(sectLineIter)))};

            sectIt = result.first;
        }
        else if (IsKeyLine(line, str1, str2))
        {
            if (IsEmptyString(str1) || (sectIt == m_sectionMap.end()))
            {
                BOOST_THROW_EXCEPTION(ParseError(lineNumber, "invalid key '" + str1 + "'"));
            }

            if (sectIt->second.KeyExists(str1))
            {
                BOOST_THROW_EXCEPTION(ParseError(lineNumber, "duplicate key '" + str1
                                                 + "' in section '" + sectIt->first + "'"));
            }

            auto keyLineIter =
                m_lines.insert(m_lines.end(), std::make_shared<if_private::KeyLine>(str1, str2));

            sectIt->second.AddKey(keyLineIter);
        }
        else
        {
            BOOST_THROW_EXCEPTION(ParseError(lineNumber, "invalid line '" + line + "'"));
        }
    }
}

void IniFile::UpdateFile(const std::string& overridePath) const
{
    if (!m_changesMade && (overridePath == ""))
    {
        return;
    }

    std::ofstream iniFile;

    if (overridePath == "")
    {
        iniFile.open(m_iniFilePath);
    }
    else
    {
        iniFile.open(overridePath);
    }

    if (!iniFile.is_open() || !iniFile.good())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("cannot create ofstream"));
    }

    std::stringstream iniStream;
    size_t            count        = m_lines.size();
    bool              firstSection = true;

    for (const auto& line : m_lines)
    {
        if (std::dynamic_pointer_cast<if_private::SectionLine>(line))
        {
            if (!firstSection)
            {
                if_private::BlankLine blankLine;
                blankLine.Print(iniStream);
            }

            firstSection = false;
        }

        line->Print(iniStream, (--count) > 0);
    }

    std::copy(std::istreambuf_iterator<char>(iniStream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(iniFile));
    iniFile.close();

    if (overridePath == "")
    {
        m_changesMade = false;
    }
}

auto IniFile::GetSections() const -> std::list<std::string>
{
    std::list<std::string> sections;

    std::transform(m_sectionMap.begin(),
                   m_sectionMap.end(),
                   std::back_inserter(sections),
                   [](decltype(m_sectionMap)::value_type const& section) { return section.first; });

    return sections;
}

keys_list IniFile::GetSection(const std::string& section) const
{
    keys_list     keys;
    section_citer sectIt{m_sectionMap.find(section)};

    if (sectIt != m_sectionMap.end())
    {
        sectIt->second.GetKeys(keys);
    }

    return keys;
}

bool IniFile::SectionExists(const std::string& section) const
{
    return m_sectionMap.find(section) != m_sectionMap.end();
}

bool IniFile::KeyExists(const std::string& section, const std::string& key) const
{
    section_citer sectIt{m_sectionMap.find(section)};

    if (sectIt == m_sectionMap.end())
    {
        return false;
    }
    else
    {
        return sectIt->second.KeyExists(key);
    }
}

bool IniFile::ReadBool(const std::string& section, const std::string& key, bool defaultValue) const
{
    return StringToBool(ReadValueString(section, key, std::to_string(defaultValue ? 1 : 0)));
}

int32_t IniFile::ReadInt32(const std::string& section, const std::string& key,
                           int32_t defaultValue) const
{
    return StringToInt32(ReadValueString(section, key, std::to_string(defaultValue)));
}

int64_t IniFile::ReadInt64(const std::string& section, const std::string& key,
                           int64_t defaultValue) const
{
    return StringToInt64(ReadValueString(section, key, std::to_string(defaultValue)));
}

double IniFile::ReadDouble(const std::string& section, const std::string& key,
                           double defaultValue) const
{
    return StringToDouble(ReadValueString(section, key, string_utils::FormatFloatString(defaultValue)));
}

long double IniFile::ReadLongDouble(const std::string& section, const std::string& key,
                                    long double defaultValue) const
{
    return StringToLongDouble(ReadValueString(section, key, string_utils::FormatFloatString(defaultValue, 30)));
}

std::string IniFile::ReadString(const std::string& section, const std::string& key,
                                const std::string& defaultValue) const
{
    return ReadValueString(section, key, defaultValue);
}

std::string IniFile::ReadValueString(const std::string& section, const std::string& key,
                                     const std::string& defaultValue) const
{
    std::string   value{defaultValue};
    section_citer sectIt{m_sectionMap.find(section)};

    if (sectIt != m_sectionMap.end())
    {
        value = sectIt->second.GetValue(key, defaultValue);
    }

    return value;
}

void IniFile::WriteBool(const std::string& section, const std::string& key, bool value)
{
    WriteValueString(section, key, value ? "1" : "0");
}

void IniFile::WriteInt32(const std::string& section, const std::string& key, int32_t value)
{
    WriteValueString(section, key, std::to_string(value));
}

void IniFile::WriteInt64(const std::string& section, const std::string& key, int64_t value)
{
    WriteValueString(section, key, std::to_string(value));
}

void IniFile::WriteDouble(const std::string& section, const std::string& key, double value)
{
    WriteValueString(section, key, string_utils::FormatFloatString(value));
}

void IniFile::WriteLongDouble(const std::string& section, const std::string& key, long double value)
{
    WriteValueString(section, key, string_utils::FormatFloatString(value, 30));
}

void IniFile::WriteString(const std::string& section, const std::string& key,
                          const std::string& value)
{
    WriteValueString(section, key, value);
}

void IniFile::WriteValueString(const std::string& section, const std::string& key,
                               const std::string& value)
{
    if (section == "")
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("section must be non-empty"));
    }

    if (key == "")
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("key must be non-empty"));
    }

    bool addNewKey = false;
    auto sectIt    = m_sectionMap.find(section);

    if (sectIt == m_sectionMap.end())
    {
        auto secLineIter =
            m_lines.insert(m_lines.end(), std::make_shared<if_private::SectionLine>(section));
        std::pair<section_iter, bool> result{
            m_sectionMap.insert(std::make_pair(section, if_private::SectionDetails(secLineIter)))};
        sectIt    = result.first;
        addNewKey = true;
    }
    else
    {
        if (sectIt->second.KeyExists(key))
        {
            sectIt->second.UpdateKey(key, value);
        }
        else
        {
            addNewKey = true;
        }
    }

    if (addNewKey)
    {
        auto insertPos = sectIt->second.LineIterator();

        do
        {
            ++insertPos;
        } while ((insertPos != m_lines.end()) &&
                 (!std::dynamic_pointer_cast<if_private::SectionLine>(*insertPos)));

        auto keyLineIter =
            m_lines.insert(insertPos, std::make_shared<if_private::KeyLine>(key, value));
        sectIt->second.AddKey(keyLineIter);
    }

    m_changesMade = true;
}

void IniFile::EraseSection(const std::string& section)
{
    auto sectIt = m_sectionMap.find(section);

    if (sectIt != m_sectionMap.end())
    {
        auto lineIter = sectIt->second.LineIterator();
        m_sectionMap.erase(sectIt);

        do
        {
            if (std::dynamic_pointer_cast<if_private::SectionLine>(*lineIter) ||
                std::dynamic_pointer_cast<if_private::KeyLine>(*lineIter))
            {
                lineIter      = m_lines.erase(lineIter);
                m_changesMade = true;
            }
            else
            {
                ++lineIter;
            }
        } while ((lineIter != m_lines.end()) &&
                 !std::dynamic_pointer_cast<if_private::SectionLine>(*lineIter));
    }

    if (m_sectionMap.empty())
    {
        if (!m_lines.empty())
        {
            m_lines.clear();
            m_changesMade = true;
        }
    }
}

void IniFile::EraseSections()
{
    std::list<std::string> sections{GetSections()};

    for (const auto& section : sections)
    {
        EraseSection(section);
    }

    if (m_sectionMap.empty())
    {
        if (!m_lines.empty())
        {
            m_lines.clear();
            m_changesMade = true;
        }
    }
}

void IniFile::EraseKey(const std::string& section, const std::string& key)
{
    auto sectIt = m_sectionMap.find(section);

    if (sectIt != m_sectionMap.end())
    {
        auto keyLineIter = m_lines.end();

        if (sectIt->second.EraseKey(key, keyLineIter))
        {
            m_lines.erase(keyLineIter);
            m_changesMade = true;
        }

        if (sectIt->second.NumKeys() == 0)
        {
            EraseSection(section);
        }
    }

    if (m_sectionMap.empty())
    {
        if (!m_lines.empty())
        {
            m_lines.clear();
            m_changesMade = true;
        }
    }
}

void IniFile::EraseKeys(const std::string& section)
{
    keys_list keys{GetSection(section)};

    for (const auto& key : keys)
    {
        EraseKey(section, key.first);
    }

    if (m_sectionMap.empty())
    {
        if (!m_lines.empty())
        {
            m_lines.clear();
            m_changesMade = true;
        }
    }
}

} // namespace ini_file
} // namespace core_lib
