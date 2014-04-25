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
 * \file IniFile.cpp
 * \brief File containing definitions relating the IniFile class.
 */

#include "../IniFile.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include "../StringUtils.hpp"
#include "boost/algorithm/string/trim.hpp"

namespace core_lib {
namespace ini_file {

// ****************************************************************************
// 'class xIniFileDataConvertError' definition
// ****************************************************************************
xIniFileDataConvertError::xIniFileDataConvertError()
    : exceptions::xCustomException("data convert error")
{
}

xIniFileDataConvertError::xIniFileDataConvertError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xIniFileDataConvertError::~xIniFileDataConvertError()
{
}

// ****************************************************************************
// 'class xIniFileParserError' definition
// ****************************************************************************
xIniFileParserError::xIniFileParserError()
    : exceptions::xCustomException("parser error")
{
}

xIniFileParserError::xIniFileParserError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xIniFileParserError::~xIniFileParserError()
{
}

// ****************************************************************************
// 'class xIniFileSaveError' definition
// ****************************************************************************
xIniFileSaveError::xIniFileSaveError()
    : exceptions::xCustomException("save error")
{
}

xIniFileSaveError::xIniFileSaveError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xIniFileSaveError::~xIniFileSaveError()
{
}

// ****************************************************************************
// 'class xIniFileInvalidKeyError' definition
// ****************************************************************************
xIniFileInvalidKeyError::xIniFileInvalidKeyError()
    : exceptions::xCustomException("invalid key")
{
}

xIniFileInvalidKeyError::xIniFileInvalidKeyError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xIniFileInvalidKeyError::~xIniFileInvalidKeyError()
{
}

// ****************************************************************************
// 'class xIniFileInvalidSectionError' definition
// ****************************************************************************
xIniFileInvalidSectionError::xIniFileInvalidSectionError()
    : exceptions::xCustomException("invalid section")
{
}

xIniFileInvalidSectionError::xIniFileInvalidSectionError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xIniFileInvalidSectionError::~xIniFileInvalidSectionError()
{
}


// ****************************************************************************
// 'class IniFile' support class definitions.
// ****************************************************************************
void IniFile::BlankLine::Print(std::ostream &os) const
{
    os << std::endl;
}

IniFile::CommentLine::CommentLine(const std::string& comment)
    : Line(), m_comment{comment}
{
}

const std::string& IniFile::CommentLine::Comment() const
{
    return m_comment;
}

void IniFile::CommentLine::Print(std::ostream &os) const
{
    os << ";" << m_comment << std::endl;
}

IniFile::SectionLine::SectionLine(const std::string& section)
    : Line(), m_section{section}
{
}

const std::string& IniFile::SectionLine::Section() const
{
    return m_section;
}

void IniFile::SectionLine::Print(std::ostream &os) const
{
    os << "[" << m_section << "]" << std::endl;
}

IniFile::KeyLine::KeyLine(const std::string& key
                          , const std::string& value)
    : Line(), m_key{key}, m_value{value}
{
}

const std::string& IniFile::KeyLine::Key() const
{
    return m_key;
}

const std::string& IniFile::KeyLine::Value() const
{
    return m_value;
}

void IniFile::KeyLine::Value(const std::string& value)
{
    m_value = value;
}

void IniFile::KeyLine::Value(std::string&& value)
{
    m_value = value;
}

void IniFile::KeyLine::Print(std::ostream &os) const
{
    os << m_key << "=" << m_value << std::endl;
}

IniFile::SectionDetails::SectionDetails(const IniFile::line_iter& sectIter)
    : m_sectIter(sectIter)
{
}

const std::string& IniFile::SectionDetails::Section() const
{
    return std::dynamic_pointer_cast<SectionLine>(*m_sectIter)->Section();
}

bool IniFile::SectionDetails::KeyExists(const std::string& key) const
{
    bool found = false;

    for (auto lineIter : m_keyIters)
    {
        std::shared_ptr<KeyLine> keyLine
            = std::dynamic_pointer_cast<KeyLine>(*lineIter);

        if (key.compare(keyLine->Key()) == 0)
        {
            found = true;
            break;
        }
    }

    return found;
}

void IniFile::SectionDetails::AddKey(const IniFile::line_iter& keyIter)
{
    m_keyIters.push_back(keyIter);
}

void IniFile::SectionDetails::UpdateKey(const std::string& key
                                        , const std::string& value)
{

    for (auto lineIter : m_keyIters)
    {
        std::shared_ptr<KeyLine> keyLine
            = std::dynamic_pointer_cast<KeyLine>(*lineIter);

        if (key.compare(keyLine->Key()) == 0)
        {
            keyLine->Value(value);
            break;
        }
    }
}

bool IniFile::SectionDetails::EraseKey(const std::string& key
                                       , IniFile::line_iter& lineIter)
{
    bool erased = false;

    for (keys_iter keyIter = m_keyIters.begin()
         ; keyIter != m_keyIters.end()
         ; ++keyIter)
    {
        std::shared_ptr<KeyLine> keyLine
            = std::dynamic_pointer_cast<KeyLine>(*lineIter);

        if (keyLine && (key.compare(keyLine->Key()) == 0))
        {
            lineIter = *keyIter;
            m_keyIters.erase(keyIter);
            erased = true;
            break;
        }
    }

    return erased;
}

std::string IniFile::SectionDetails::GetValue(const std::string& key
                                              , const std::string& defaultValue) const
{
    std::string value{defaultValue};

    for (auto lineIter : m_keyIters)
    {
        std::shared_ptr<KeyLine> keyLine
            = std::dynamic_pointer_cast<KeyLine>(*lineIter);

        if (keyLine && (key.compare(keyLine->Key()) == 0))
        {
            value = keyLine->Value();
            break;
        }
    }

    return value;
}

void IniFile::SectionDetails::GetKeys(IniFile::keys_list& keys) const
{
    keys.clear();

    for (auto lineIter : m_keyIters)
    {
        std::shared_ptr<KeyLine> keyLine
            = std::dynamic_pointer_cast<KeyLine>(*lineIter);

        if (keyLine)
        {
            keys.push_back(std::make_pair(keyLine->Key(), keyLine->Value()));
        }
    }
}

IniFile::line_iter IniFile::SectionDetails::LineIterator() const
{
    return m_sectIter;
}

// ****************************************************************************
// 'class IniFile' definition
// ****************************************************************************

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
        BOOST_THROW_EXCEPTION(xIniFileParserError("cannot create ifstream"));
    }

    std::stringstream iniStream;
    std::copy(std::istreambuf_iterator<char>(iniFile),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(iniStream));
    iniFile.close();

    section_iter sectIt{m_sectionMap.end()};

    while(iniStream.good())
    {
        std::string line;
        std::getline(iniStream, line);
        string_utils::PackStdString(line);
        boost::trim(line);
        std::string str1, str2;

        if (IsBlankLine(line))
        {
            m_lines.insert(m_lines.end()
                           , std::make_shared<BlankLine>());
        }
        else if(IsCommentLine(line, str1))
        {
            m_lines.insert(m_lines.end()
                           , std::make_shared<CommentLine>(str1));
        }
        else if(IsSectionLine(line, str1))
        {
            if (str1.compare("") == 0)
            {
                BOOST_THROW_EXCEPTION(xIniFileParserError("file contains invalid section"));
            }

            if (m_sectionMap.find(str1) != m_sectionMap.end())
            {
                BOOST_THROW_EXCEPTION(xIniFileParserError("file contains duplicate section"));
            }

            line_iter sectLineIter{m_lines.insert(m_lines.end()
                                                  , std::make_shared<SectionLine>(str1))};
            std::pair<section_iter, bool>
                    result{m_sectionMap.insert(std::make_pair(str1, SectionDetails(sectLineIter)))};
            sectIt = result.first;
        }
        else if(IsKeyLine(line, str1, str2))
        {
            if ((str1.compare("") == 0) || (sectIt == m_sectionMap.end()))
            {
                BOOST_THROW_EXCEPTION(xIniFileParserError("file contains invalid key"));
            }

            if (sectIt->second.KeyExists(str1))
            {
                BOOST_THROW_EXCEPTION(xIniFileParserError("file contains duplicate key"));
            }

            line_iter keyLineIter{m_lines.insert(m_lines.end()
                                                  , std::make_shared<KeyLine>(str1, str2))};
            sectIt->second.AddKey(keyLineIter);
        }
        else
        {
            BOOST_THROW_EXCEPTION(xIniFileParserError("file contains invalid line"));
        }
    }
}

void IniFile::UpdateFile() const
{
    if (!m_changesMade)
    {
        return;
    }

    std::ofstream iniFile(m_iniFilePath);

    if (!iniFile.is_open() || !iniFile.good())
    {
        BOOST_THROW_EXCEPTION(xIniFileSaveError("cannot create ofstream"));
    }

    std::stringstream iniStream;

    for (auto line : m_lines)
    {
        line->Print(iniStream);
    }

    std::copy(std::istreambuf_iterator<char>(iniStream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(iniFile));
    iniFile.close();
    m_changesMade = false;
}

void IniFile::GetSections(std::list< std::string >& sections) const
{
    sections.clear();

    for (auto section : m_sectionMap)
    {
        sections.push_back(section.first);
    }
}

void IniFile::GetSection(const std::string& section
                         , IniFile::keys_list& keys) const
{
    keys.clear();
    section_citer sectIt{m_sectionMap.find(section)};

    if (sectIt != m_sectionMap.end())
    {
        sectIt->second.GetKeys(keys);
    }
}

bool IniFile::SectionExists(const std::string& section) const
{
    return m_sectionMap.find(section) != m_sectionMap.end();
}

bool IniFile::KeyExists(const std::string& section
                        , const std::string& key) const
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

bool IniFile::ReadBool(const std::string& section
                       , const std::string& key
                       , bool defaultValue) const
{
    int value{};

    try
    {
        value = std::stoi(ReadValue(section, key, std::to_string(defaultValue ? 1 : 0)));
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(xIniFileDataConvertError("failed to convert to bool"));
    }

    return value == 1;
}

int32_t IniFile::ReadInteger(const std::string& section
                         , const std::string& key
                         , int32_t defaultValue) const
{
    int32_t value{};

    try
    {
        value = std::stoi(ReadValue(section, key, std::to_string(defaultValue)));
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(xIniFileDataConvertError("failed to convert to int"));
    }

    return value;
}

int64_t IniFile::ReadInteger64(const std::string& section
                               , const std::string& key
                               , int64_t defaultValue) const
{
    int64_t value{};

    try
    {
        value = std::stoll(ReadValue(section, key, std::to_string(defaultValue)));
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(xIniFileDataConvertError("failed to convert to int64_t"));
    }

    return value;
}

double IniFile::ReadDouble(const std::string& section
                           , const std::string& key
                           , double defaultValue) const
{
    std::string defValStr{string_utils::FormatFloatString(defaultValue)};
    double value{};

    try
    {
        value = std::stod(ReadValue(section, key, defValStr));
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(xIniFileDataConvertError("failed to convert to double"));
    }

    return value;
}

long double IniFile::ReadLongDouble(const std::string& section
                                    , const std::string& key
                                    , long double defaultValue) const
{
    std::string defValStr{string_utils::FormatFloatString(defaultValue, 30)};
    long double value{};

    try
    {
        value = std::stold(ReadValue(section, key, defValStr));
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(xIniFileDataConvertError("failed to convert to long double"));
    }

    return value;
}

std::string IniFile::ReadString(const std::string& section
                                , const std::string& key
                                , const std::string& defaultValue) const
{
    return ReadValue(section, key, defaultValue);
}

std::string IniFile::ReadValue(const std::string& section
                                , const std::string& key
                                , const std::string& defaultValue) const
{
    std::string value{defaultValue};
    section_citer sectIt{m_sectionMap.find(section)};

    if (sectIt != m_sectionMap.end())
    {
        value = sectIt->second.GetValue(key, defaultValue);
    }

    return value;
}

std::string IniFile::ReadValue(const std::string& section
                                , const std::string& key
                                , std::string&& defaultValue) const
{
    std::string value{defaultValue};
    section_citer sectIt{m_sectionMap.find(section)};

    if (sectIt != m_sectionMap.end())
    {
        value = sectIt->second.GetValue(key, defaultValue);
    }

    return value;
}

void IniFile::WriteBool(const std::string& section
                        , const std::string& key
                        , bool value)
{
    WriteValue(section, key, std::to_string(value ? 1 : 0));
}

void IniFile::WriteInteger(const std::string& section
                           , const std::string& key
                           , int32_t value)
{
    WriteValue(section, key, std::to_string(value));
}

void IniFile::WriteInteger64(const std::string& section
                            , const std::string& key
                            , int64_t value)
{
    WriteValue(section, key, std::to_string(value));
}

void IniFile::WriteDouble(const std::string& section
                         , const std::string& key
                         , double value)
{
    std::string strVal{string_utils::FormatFloatString(strVal, value)};
    WriteValue(section, key, strVal);
}

void IniFile::WriteLongDouble(const std::string& section
                              , const std::string& key
                              , long double value)
{
    std::string strVal{string_utils::FormatFloatString(value, 30)};
    WriteValue(section, key, strVal);
}

void IniFile::WriteString(const std::string& section
                          , const std::string& key
                          , const std::string& value)
{
    WriteValue(section, key, value);
}

void IniFile::WriteValue(const std::string& section
                          , const std::string& key
                          , std::string&& value)
{
    if (section.compare("") == 0)
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidSectionError("section must be non-empty"));
    }

    if (key.compare("") == 0)
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidKeyError("key must be non-empty"));
    }

    bool addNewKey = false;
    section_iter sectIt{m_sectionMap.find(section)};

    if (sectIt == m_sectionMap.end())
    {
        line_iter secLineIter{m_lines.insert(m_lines.end()
                                             , std::make_shared<SectionLine>(section))};
        std::pair<section_iter, bool>
                result{m_sectionMap.insert(std::make_pair(section
                                                          , SectionDetails(secLineIter)))};
        sectIt = result.first;
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
        line_iter insertPos{sectIt->second.LineIterator()};

        do
        {
            ++insertPos;
        }
        while(!std::dynamic_pointer_cast<SectionLine>(*insertPos));

        line_iter keyLineIter{m_lines.insert(insertPos
                                             , std::make_shared<KeyLine>(key, value))};
        sectIt->second.AddKey(keyLineIter);
    }

    m_changesMade = true;
}

void IniFile::WriteValue(const std::string& section
                          , const std::string& key
                          , const std::string& value)
{
    if (section.compare("") == 0)
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidSectionError("section must be non-empty"));
    }

    if (key.compare("") == 0)
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidKeyError("key must be non-empty"));
    }

    bool addNewKey = false;
    section_iter sectIt{m_sectionMap.find(section)};

    if (sectIt == m_sectionMap.end())
    {
        line_iter secLineIter{m_lines.insert(m_lines.end()
                                             , std::make_shared<SectionLine>(section))};
        std::pair<section_iter, bool>
                result{m_sectionMap.insert(std::make_pair(section
                                                          , SectionDetails(secLineIter)))};
        sectIt = result.first;
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
        line_iter insertPos{sectIt->second.LineIterator()};

        do
        {
            ++insertPos;
        }
        while(!std::dynamic_pointer_cast<SectionLine>(*insertPos));

        line_iter keyLineIter{m_lines.insert(insertPos
                                             , std::make_shared<KeyLine>(key, value))};
        sectIt->second.AddKey(keyLineIter);
    }

    m_changesMade = true;
}

void IniFile::EraseSection(const std::string& section)
{
    section_iter sectIt{m_sectionMap.find(section)};

    if (sectIt != m_sectionMap.end())
    {
        line_iter lineIter{sectIt->second.LineIterator()};
        m_sectionMap.erase(sectIt);

        do
        {
            if (std::dynamic_pointer_cast<SectionLine>(*lineIter)
                || std::dynamic_pointer_cast<KeyLine>(*lineIter))
            {
                lineIter = m_lines.erase(lineIter);
                m_changesMade = true;
            }
            else
            {
                ++lineIter;
            }
        }
        while((lineIter != m_lines.end())
              && !std::dynamic_pointer_cast<SectionLine>(*lineIter));
    }
}

void IniFile::EraseSections()
{
    std::list<std::string> sections;
    GetSections(sections);

    for (auto section : sections)
    {
        EraseSection(section);
    }
}

void IniFile::EraseKey(const std::string& section
                       , const std::string& key)
{
    section_iter sectIt{m_sectionMap.find(section)};

    if (sectIt != m_sectionMap.end())
    {
        line_iter keyLineIter{m_lines.end()};

        if (sectIt->second.EraseKey(key, keyLineIter))
        {
            m_lines.erase(keyLineIter);
            m_changesMade = true;
        }
    }
}

void IniFile::EraseKeys(const std::string& section)
{
    keys_list keys;
    GetSection(section, keys);

    for (auto key : keys)
    {
        EraseKey(section, key.first);
    }
}

bool IniFile::IsBlankLine(const std::string& line) const
{
    return line.compare("") == 0;
}

bool IniFile::IsCommentLine(const std::string& line
                            , std::string& comment) const
{
    bool isComment = line.front() == ';';

    if (isComment)
    {
        comment = line.substr(1, line.size() - 1);
    }

    return isComment;
}

bool IniFile::IsSectionLine(const std::string& line
                            , std::string& section) const
{
    bool isSection
        = (line.front() == '[') && (line.back() == ']');

    if (isSection)
    {
        section = line.substr(1, line.size() - 2);
        boost::trim(section);
    }

    return isSection;
}

bool IniFile::IsKeyLine(const std::string& line
                        , std::string& key
                        , std::string& value) const
{
    bool isKeyLine{true};

    try
    {
        string_utils::SplitString(key, value , line, "="
                                  , string_utils::eSplitStringResult::trimmed);
    }
    catch(...)
    {
        isKeyLine = false;
    }

    return isKeyLine;
}

} // namespace core_lib
} // namespace ini_file
