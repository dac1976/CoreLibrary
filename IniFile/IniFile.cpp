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
// 'class IniFile' definition
// ****************************************************************************

IniFile::IniFile(const std::string& iniFilePath)
{
    LoadFile(iniFilePath);
}

void IniFile::LoadFile(const std::string& iniFilePath)
{

    m_changesMade = false;
}

void IniFile::UpdateFile() const
{
    if (!m_changesMade)
    {
        return;
    }

    m_changesMade = false;
}

void IniFile::GetSections(std::vector< std::string >& sections) const
{

}

void IniFile::GetSection(const std::string& section
                         , std::vector< std::pair<std::string, std::string> >& pairs) const
{

}

bool IniFile::SectionExists(const std::string& section) const
{

}

bool IniFile::ValueExists(const std::string& section
                          , const std::string& key) const
{

}

bool IniFile::ReadBool(const std::string& section
                       , const std::string& key
                       , const bool DefaultValue) const
{

}

int IniFile::ReadInteger(const std::string& section
                         , const std::string& key
                         , const int DefaultValue) const
{

}

double IniFile::ReadDouble(const std::string& section
                           , const std::string& key
                           , const double DefaultValue) const
{

}

long double IniFile::ReadLongDouble(const std::string& section
                                    , const std::string& key
                                    , const long double DefaultValue) const
{

}

int64_t IniFile::ReadInteger64(const std::string& section
                               , const std::string& key
                               , const int64_t defaultValue) const
{

}

std::string IniFile::ReadString(const std::string& section
                                , const std::string& key
                                , const std::string& defaultValue) const
{

}

void IniFile::WriteBool(const std::string& section
                        , const std::string& key
                        , const bool value)
{
    WriteValue(section, key, std::to_string(value ? 1 : 0));
}

void IniFile::WriteInteger(const std::string& section
                           , const std::string& key
                           , const int value)
{
    WriteValue(section, key, std::to_string(value));
}

void IniFile::WriteInteger64(const std::string& section
                            , const std::string& key
                            , const int64_t value)
{
    WriteValue(section, key, std::to_string(value));
}

void IniFile::WriteDouble(const std::string& section
                         , const std::string& key
                         , const double value)
{
    std::string strVal;
    string_utils::FormatFloatString(strVal, value);
    WriteValue(section, key, strVal);
}

void IniFile::WriteLongDouble(const std::string& section
                              , const std::string& key
                              , const long double value)
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
    sections_iter sectIt{m_fileMap.find(section)};
    bool found = false;

    if (sectIt != m_fileMap.end())
    {
        kvp_iter kvpIt = sectIt->second.find(key);

        if (kvpIt != sectIt->second.end())
        {
            kvpIt->second = value;
            found = true;
        }
    }

    if (!found)
    {
        (m_fileMap[section])[key] = value;
    }

    m_changesMade = true;
}

void IniFile::WriteValue(const std::string& section
                          , const std::string& key
                          , const std::string& value)
{
    sections_iter sectIt{m_fileMap.find(section)};
    bool found = false;

    if (sectIt != m_fileMap.end())
    {
        kvp_iter kvpIt = sectIt->second.find(key);

        if (kvpIt != sectIt->second.end())
        {
            kvpIt->second = value;
            found = true;
        }
    }

    if (!found)
    {
        (m_fileMap[section])[key] = value;
    }

    m_changesMade = true;
}

void IniFile::EraseSection(const std::string& section)
{
    sections_citer sectIt{m_fileMap.find(section)};

    if (sectIt != m_fileMap.end())
    {
        m_fileMap.erase(sectIt);
    }
}

void IniFile::EraseSections()
{
    m_fileMap.clear();
}

void IniFile::EraseKey(const std::string& section
                        , const std::string& key)
{
    sections_iter sectIt{m_fileMap.find(section)};

    if (sectIt != m_fileMap.end())
    {
        kvp_citer kvpIt{sectIt->second.find(key)};

        if (kvpIt != sectIt->second.end())
        {
            sectIt->second.erase(kvpIt);
        }
    }
}

void IniFile::EraseKeys(const std::string& section)
{
    sections_iter sectIt{m_fileMap.find(section)};

    if (sectIt != m_fileMap.end())
    {
        sectIt->second.clear();
    }
}

IniFile::kvp_citer IniFile::FindKey(const std::string& section
                                    , const std::string& key) const
{
    sections_citer sectIt{m_fileMap.find(section)};

    if (sectIt == m_fileMap.end())
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidSectionError());
    }

    kvp_citer kvpIt{sectIt->second.find(key)};

    if (kvpIt == sectIt->second.end())
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidKeyError());
    }

    return kvpIt;
}

IniFile::kvp_iter IniFile::FindKey(const std::string& section
                                   , const std::string& key)
{
    sections_iter sectIt{m_fileMap.find(section)};

    if (sectIt == m_fileMap.end())
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidSectionError());
    }

    kvp_iter kvpIt{sectIt->second.find(key)};

    if (kvpIt == sectIt->second.end())
    {
        BOOST_THROW_EXCEPTION(xIniFileInvalidKeyError());
    }

    return kvpIt;
}

bool IniFile::FindKeyNoThrow(const std::string& section
                             , const std::string& key
                             , IniFile::kvp_citer& kvpIt) const
{
    bool found{};
    sections_citer sectIt{m_fileMap.find(section)};

    if (sectIt != m_fileMap.end())
    {
        kvpIt = sectIt->second.find(key);

        if (kvpIt != sectIt->second.end())
        {
            found = true;
        }
    }

    return found;
}

bool IniFile::FindKeyNoThrow(const std::string& section
                             , const std::string& key
                             , IniFile::kvp_iter& kvpIt)
{
    bool found{};
    sections_iter sectIt{m_fileMap.find(section)};

    if (sectIt != m_fileMap.end())
    {
        kvpIt = sectIt->second.find(key);

        if (kvpIt != sectIt->second.end())
        {
            found = true;
        }
    }

    return found;
}

} // namespace core_lib
} // namespace ini_file
