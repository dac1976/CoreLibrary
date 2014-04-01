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
 * \file IniFile.hpp
 * \brief File containing declarations relating the IniFile class.
 */

/*
    ; I am an opening comment.

    [Section1]
    ; I am a comment in a section.
    Section1_Key1=Section1_Value1
    Section1_Key2=Section1_Value2
    Section1_Key3=Section1_Value3

    [Section2]
    Section2_Key1=Section2_Value1
    Section2_Key2=Section2_Value2
    ; I am also a comment in a section.
    Section2_Key3=Section2_Value3
    ; I am yet another comment in a section.
*/

#ifndef INIFILE_HPP
#define INIFILE_HPP

#include <utility>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "Exceptions/CustomException.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The ini_file namespace. */
namespace ini_file {

/*!
 * \brief Ini file data conversion exception.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when a data conversion error eoccurs.
 */
class xIniFileDataConvertError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileDataConvertError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xIniFileDataConvertError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xIniFileDataConvertError();
};

/*!
 * \brief Ini file parser error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when a parser error eoccurs.
 */
class xIniFileParserError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileParserError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xIniFileParserError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xIniFileParserError();
};

/*!
 * \brief Ini file save error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when a save error eoccurs.
 */
class xIniFileSaveError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileSaveError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xIniFileSaveError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xIniFileSaveError();
};


/*!
 * \brief Ini file invalid key error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when an invalid key is used occurs.
 */
class xIniFileInvalidKeyError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileInvalidKeyError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xIniFileInvalidKeyError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xIniFileInvalidKeyError();
};

/*!
 * \brief Ini file invalid section error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when an invalid section is used occurs.
 */
class xIniFileInvalidSectionError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileInvalidSectionError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xIniFileInvalidSectionError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xIniFileInvalidSectionError();
};

class IniFile
{
public:
    IniFile() = default;

    IniFile(const IniFile&) = default;

    IniFile(IniFile&&) = default;

    explicit IniFile(const std::string& iniFilePath);

    ~IniFile() = default;

    IniFile& operator=(const IniFile&) = default;

    IniFile& operator=(IniFile&&) = default;

    void LoadFile(const std::string& iniFilePath);

    void UpdateFile() const;

    void GetSections(std::vector< std::string >& sections) const;

    void GetSection(const std::string& section
                    , std::vector< std::pair<std::string, std::string> >& pairs) const;

    bool SectionExists(const std::string& section) const;

    bool ValueExists(const std::string& section
                     , const std::string& key) const;

    bool ReadBool(const std::string& section
                  , const std::string& key
                  , const bool defaultValue = false) const;

    int ReadInteger(const std::string& section
                    , const std::string&key
                    , const int defaultValue = 0) const;

    int64_t ReadInteger64(const std::string& section
                          , const std::string& key
                          , const int64_t defaultValue = 0L) const;

    double ReadDouble(const std::string& section
                      , const std::string& key
                      , const double defaultValue = 0.0) const;

    long double ReadLongDouble(const std::string& section
                               , const std::string& key
                               , const long double DefaultValue = 0.0L) const;

    std::string ReadString(const std::string& section
                           , const std::string& key
                           , const std::string& defaultValue = "") const;

    void WriteBool(const std::string& section
                   , const std::string& key
                   , const bool value);

    void WriteInteger(const std::string& section
                      , const std::string& key
                      , const int value);

    void WriteInteger64(const std::string& section
                        , const std::string& key
                        , const int64_t value);

    void WriteDouble(const std::string& section
                     , const std::string& key
                     , const double value);

    void WriteLongDouble(const std::string& section
                         , const std::string& key
                         , const long double value);

    void WriteString(const std::string& section
                     , const std::string& key
                     , const std::string& value);

    void EraseSection(const std::string& section);

    void EraseSections();

    void EraseKey(const std::string& section
                  , const std::string& key);

    void EraseKeys(const std::string& section);

private:
    typedef std::unordered_map<std::string, std::string> key_value_pairs;
    typedef key_value_pairs::iterator kvp_iter;
    typedef key_value_pairs::const_iterator kvp_citer;
    typedef std::unordered_map<std::string, key_value_pairs> sections;
    typedef sections::iterator sections_iter;
    typedef sections::const_iterator sections_citer;
    sections m_fileMap;

    kvp_citer FindKey(const std::string& section
                      , const std::string& key) const;

    kvp_iter FindKey(const std::string& section
                     , const std::string& key);

    bool FindKeyNoThrow(const std::string& section
                        , const std::string& key
                        , kvp_citer& kvpIt) const;

    bool FindKeyNoThrow(const std::string& section
                        , const std::string& key
                        , kvp_iter& kvpIt);
};

} // namespace ini_file
} // namespace core_lib


#endif //INIFILE_HPP
