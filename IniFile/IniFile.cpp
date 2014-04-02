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
#include "../StringUtils.hpp"

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
// 'class IniFile::KeyValuePair' definition
// ****************************************************************************
IniFile::KeyValuePair::KeyValuePair(const std::string& key, const std::string& value)
    : m_kvp{key, value}
{
}

bool IniFile::KeyValuePair::operator==(const std::string& key) const
{
    return m_kvp.first == key;
}

const std::string& IniFile::KeyValuePair::Key() const
{
    return m_kvp.first;
}

const std::string& IniFile::KeyValuePair::Value() const
{
    return m_kvp.second;
}

void IniFile::KeyValuePair::Value(const std::string& value)
{
    m_kvp.second = value;
}

bool IniFile::KeyValuePair::IsComment() const
{
    return m_kvp.first.empty() && (m_kvp.second.front() == ';');
}

bool IniFile::KeyValuePair::IsBlank() const
{
    return m_kvp.first.empty() && m_kvp.second.empty();
}

// ****************************************************************************
// 'class IniFile::Section' definition
// ****************************************************************************
IniFile::Section::Section(const std::string& name)
    : m_name{name}
{
}

bool IniFile::Section::operator==(const std::string& name) const
{
    return m_name == name;
}

const std::string& IniFile::Section::Name() const
{
    return m_name;
}

void IniFile::Section::UpdateKey(const std::string& key
                                 , const std::string& value)
{
    std::list<KeyValuePair>::iterator
        kvpIt(std::find(m_kvps.begin(), m_kvps.end(), key));

    if (kvpIt == m_kvps.end())
    {
        m_kvps.push_back(KeyValuePair(key, value));
    }
    else
    {
        kvpIt->Value(value);
    }
}

void IniFile::Section::UpdateKey(const std::string& key
                                 , std::string&& value)
{
    std::list<KeyValuePair>::iterator
        kvpIt(std::find(m_kvps.begin(), m_kvps.end(), key));

    if (kvpIt == m_kvps.end())
    {
        m_kvps.push_back(KeyValuePair(key, value));
    }
    else
    {
        kvpIt->Value(value);
    }
}

void IniFile::Section::EraseKey(const std::string& key)
{
    std::list<KeyValuePair>::iterator
        kvpIt(std::find(m_kvps.begin(), m_kvps.end(), key));

    if (kvpIt != m_kvps.end())
    {
        m_kvps.erase(kvpIt);
    }
}

void IniFile::Section::EraseKeys()
{
    m_kvps.clear();
}

bool IniFile::Section::KeyExists(const std::string& key) const
{
    return std::find(m_kvps.begin(), m_kvps.end(), key)
           != m_kvps.end();
}

std::string IniFile::Section::GetKey(const std::string& key
                                     , const std::string& defaultValue) const
{
    std::string value{defaultValue};
    std::list<KeyValuePair>::const_iterator
        kvpIt(std::find(m_kvps.begin(), m_kvps.end(), key));

    if (kvpIt != m_kvps.end())
    {
        value = kvpIt->Value();
    }

    return value;
}

void IniFile::Section::GetKeys(std::list<std::pair<std::string
                                                   , std::string>>& pairs) const
{
    pairs.clear();

    for (auto kvp : m_kvps)
    {
        pairs.push_back(std::make_pair(kvp.Key(), kvp.Value()));
    }
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
    //TODO: Implement this method.
    m_changesMade = false;
}

void IniFile::UpdateFile() const
{
    if (!m_changesMade)
    {
        return;
    }

    //TODO: Implement this method.

    m_changesMade = false;
}

void IniFile::GetSections(std::list< std::string >& sections) const
{
    sections.clear();

    for (auto section : m_sections)
    {
        sections.push_back(section.Name());
    }
}

void IniFile::GetSection(const std::string& section
                         , std::list< std::pair<std::string, std::string> >& pairs) const
{
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt == m_sections.end())
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidSectionError());
    }
    else
    {
        secIt->GetKeys(pairs);
    }
}

bool IniFile::SectionExists(const std::string& section) const
{
    return std::find(m_sections.begin(), m_sections.end(), section)
           != m_sections.end();
}

bool IniFile::KeyExists(const std::string& section
                        , const std::string& key) const
{
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt == m_sections.end())
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidSectionError());
    }

    return secIt->KeyExists(key);
}

bool IniFile::ReadBool(const std::string& section
                       , const std::string& key
                       , bool defaultValue) const
{
    bool value{defaultValue};
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        std::string strVal{secIt->GetKey(key, std::to_string(defaultValue ? 1 : 0))};
        value = strVal == "1";
    }

    return value;
}

int IniFile::ReadInteger(const std::string& section
                         , const std::string& key
                         , int defaultValue) const
{
    int value{defaultValue};
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        std::string strVal{secIt->GetKey(key, std::to_string(defaultValue))};
        value = std::stoi(strVal);
    }

    return value;
}

int64_t IniFile::ReadInteger64(const std::string& section
                               , const std::string& key
                               , int64_t defaultValue) const
{
    int64_t value{defaultValue};
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        std::string strVal{secIt->GetKey(key, std::to_string(defaultValue))};
        value = std::stoll(strVal);
    }

    return value;
}

double IniFile::ReadDouble(const std::string& section
                           , const std::string& key
                           , double defaultValue) const
{
    double value{defaultValue};
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        std::string strVal{secIt->GetKey(key, std::to_string(defaultValue))};
        value = std::stod(strVal);
    }

    return value;
}

long double IniFile::ReadLongDouble(const std::string& section
                                    , const std::string& key
                                    , long double defaultValue) const
{
    long double value{defaultValue};
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        std::string strVal{secIt->GetKey(key, std::to_string(defaultValue))};
        value = std::stold(strVal);
    }

    return value;
}

std::string IniFile::ReadString(const std::string& section
                                , const std::string& key
                                , const std::string& defaultValue) const
{
    std::string value{defaultValue};
    std::list<Section>::const_iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        value = secIt->GetKey(key, defaultValue);
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
                           , int value)
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
    std::string strVal;
    string_utils::FormatFloatString(strVal, value);
    WriteValue(section, key, strVal);
}

void IniFile::WriteLongDouble(const std::string& section
                              , const std::string& key
                              , long double value)
{
    std::string strVal;
    string_utils::FormatFloatString(strVal, value, 30);
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
    std::list<Section>::iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt == m_sections.end())
    {
        secIt->UpdateKey(key, value);
    }
    else
    {
        m_sections.push_back(Section(section));
        m_sections.back().UpdateKey(key, value);
    }

    m_changesMade = true;
}

void IniFile::WriteValue(const std::string& section
                          , const std::string& key
                          , const std::string& value)
{
    std::list<Section>::iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt == m_sections.end())
    {
        secIt->UpdateKey(key, value);
    }
    else
    {
        m_sections.push_back(Section(section));
        m_sections.back().UpdateKey(key, value);
    }

    m_changesMade = true;
}

void IniFile::EraseSection(const std::string& section)
{
    std::list<Section>::iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        m_sections.erase(secIt);
    }
}

void IniFile::EraseSections()
{
    m_sections.clear();
}

void IniFile::EraseKey(const std::string& section
                       , const std::string& key)
{
    std::list<Section>::iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        secIt->EraseKey(key);
    }
}

void IniFile::EraseKeys(const std::string& section)
{
    std::list<Section>::iterator
        secIt(std::find(m_sections.begin(), m_sections.end(), section));

    if (secIt != m_sections.end())
    {
        secIt->EraseKeys();
    }
}

} // namespace core_lib
} // namespace ini_file
