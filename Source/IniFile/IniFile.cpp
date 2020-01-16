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
 * \file IniFile.cpp
 * \brief File containing definitions relating the IniFile class.
 */

#include "IniFile/IniFile.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <boost/algorithm/string/trim.hpp>
#include <boost/throw_exception.hpp>
#include "StringUtils/StringUtils.h"

namespace core_lib
{
namespace ini_file
{

// ****************************************************************************
// 'class IniFile' definition
// ****************************************************************************
namespace
{

static bool IsBlankLine(const std::string& line)
{
    return line == "";
}

static bool IsCommentLine(const std::string& line, std::string& comment)
{
    bool isComment = line.front() == ';';

    if (isComment)
    {
        comment = line.substr(1, line.size() - 1);
    }

    return isComment;
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
    bool isKeyLine{true};

    try
    {
        string_utils::SplitString(key, value, line, "=", string_utils::eSplitStringResult::trimmed);
    }
    catch (...)
    {
        isKeyLine = false;
    }

    return isKeyLine;
}

} // namespace

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
        // fill out one in memeory and create one on disk.
        return;
    }

    std::stringstream iniStream;
    std::copy(std::istreambuf_iterator<char>(iniFile),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(iniStream));
    iniFile.close();

    auto sectIt = m_sectionMap.end();

    while (iniStream.good())
    {
        std::string line;
        std::getline(iniStream, line);
        string_utils::PackStdString(line);
        boost::trim(line);
        std::string str1, str2;

        if (IsBlankLine(line))
        {
            m_lines.insert(m_lines.end(), std::make_shared<if_private::BlankLine>());
        }
        else if (IsCommentLine(line, str1))
        {
            m_lines.insert(m_lines.end(), std::make_shared<if_private::CommentLine>(str1));
        }
        else if (IsSectionLine(line, str1))
        {
            if (str1 == "")
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("file contains invalid section"));
            }

            if (m_sectionMap.find(str1) != m_sectionMap.end())
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("file contains duplicate section"));
            }

            auto sectLineIter =
                m_lines.insert(m_lines.end(), std::make_shared<if_private::SectionLine>(str1));
            std::pair<section_iter, bool> result{m_sectionMap.insert(
                std::make_pair(str1, if_private::SectionDetails(sectLineIter)))};
            sectIt = result.first;
        }
        else if (IsKeyLine(line, str1, str2))
        {
            if ((str1 == "") || (sectIt == m_sectionMap.end()))
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("file contains invalid key"));
            }

            if (sectIt->second.KeyExists(str1))
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("file contains duplicate key"));
            }

            auto keyLineIter =
                m_lines.insert(m_lines.end(), std::make_shared<if_private::KeyLine>(str1, str2));
            sectIt->second.AddKey(keyLineIter);
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("file contains invalid line"));
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
    size_t            count = m_lines.size();

    for (const auto& line : m_lines)
    {
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

    for (const auto& section : m_sectionMap)
    {
        sections.push_back(section.first);
    }

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
    int value{};

    try
    {
        value = std::stoi(ReadValueString(section, key, std::to_string(defaultValue ? 1 : 0)));
    }
    catch (...)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to convert to bool"));
    }

    return value == 1;
}

int32_t IniFile::ReadInt32(const std::string& section, const std::string& key,
                           int32_t defaultValue) const
{
    int32_t value{};

    try
    {
        value = std::stoi(ReadValueString(section, key, std::to_string(defaultValue)));
    }
    catch (...)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to convert to int"));
    }

    return value;
}

int64_t IniFile::ReadInt64(const std::string& section, const std::string& key,
                           int64_t defaultValue) const
{
    int64_t value{};

    try
    {
        value = std::stoll(ReadValueString(section, key, std::to_string(defaultValue)));
    }
    catch (...)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to convert to int64_t"));
    }

    return value;
}

double IniFile::ReadDouble(const std::string& section, const std::string& key,
                           double defaultValue) const
{
    double value{};

    try
    {
        value =
            std::stod(ReadValueString(section, key, string_utils::FormatFloatString(defaultValue)));
    }
    catch (...)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to convert to double"));
    }

    return value;
}

long double IniFile::ReadLongDouble(const std::string& section, const std::string& key,
                                    long double defaultValue) const
{
    long double value{};

    try
    {
        value = std::stold(
            ReadValueString(section, key, string_utils::FormatFloatString(defaultValue, 30)));
    }
    catch (...)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to convert to long double"));
    }

    return value;
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
    WriteValueString(section, key, std::to_string(value ? 1 : 0));
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
