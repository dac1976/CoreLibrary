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
 * \file IniFile.h
 * \brief File containing declarations relating the IniFile class.
 */

#ifndef INIFILE
#define INIFILE

#include "Platform/PlatformDefines.h"
#include <utility>
#include <map>
#include <cstdint>
#include "Exceptions/CustomException.h"
#include "IniFileSectionDetails.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The ini_file namespace. */
namespace ini_file
{

/*!
 * \brief Ini file data conversion exception.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when a data conversion error eoccurs.
 */
class CORE_LIBRARY_DLL_SHARED_API xIniFileDataConvertError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileDataConvertError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xIniFileDataConvertError(const std::string& message);
    /*! \brief Virtual destructor. */
    ~xIniFileDataConvertError() override = default;
    /*! \brief Copy constructor. */
    xIniFileDataConvertError(const xIniFileDataConvertError&) = default;
    /*! \brief Copy assignment operator. */
    xIniFileDataConvertError& operator=(const xIniFileDataConvertError&) = default;
    /*! \brief Move constructor. */
    xIniFileDataConvertError(xIniFileDataConvertError&&) = default;
    /*! \brief Move assignment operator. */
    xIniFileDataConvertError& operator=(xIniFileDataConvertError&&) = default;
};

/*!
 * \brief Ini file parser error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when a parser error eoccurs.
 */
class CORE_LIBRARY_DLL_SHARED_API xIniFileParserError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileParserError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xIniFileParserError(const std::string& message);
    /*! \brief Virtual destructor. */
    ~xIniFileParserError() override = default;
    /*! \brief Copy constructor. */
    xIniFileParserError(const xIniFileParserError&) = default;
    /*! \brief Copy assignment operator. */
    xIniFileParserError& operator=(const xIniFileParserError&) = default;
    /*! \brief Move constructor. */
    xIniFileParserError(xIniFileParserError&&) = default;
    /*! \brief Move assignment operator. */
    xIniFileParserError& operator=(xIniFileParserError&&) = default;
};

/*!
 * \brief Ini file save error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when a save error eoccurs.
 */
class CORE_LIBRARY_DLL_SHARED_API xIniFileSaveError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileSaveError();
    /*!
     * \brief Initializing constnructor.
     * \param[in] message - A user specified message string.
     */
    explicit xIniFileSaveError(const std::string& message);
    /*! \brief Virtual destructor. */
    ~xIniFileSaveError() override = default;
    /*! \brief Copy constructor. */
    xIniFileSaveError(const xIniFileSaveError&) = default;
    /*! \brief Copy assignment operator. */
    xIniFileSaveError& operator=(const xIniFileSaveError&) = default;
    /*! \brief Move constructor. */
    xIniFileSaveError(xIniFileSaveError&&) = default;
    /*! \brief Move assignment operator. */
    xIniFileSaveError& operator=(xIniFileSaveError&&) = default;
};

/*!
 * \brief Ini file invalid key error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when an invalid key is used occurs.
 */
class CORE_LIBRARY_DLL_SHARED_API xIniFileInvalidKeyError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileInvalidKeyError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xIniFileInvalidKeyError(const std::string& message);
    /*! \brief Virtual destructor. */
    ~xIniFileInvalidKeyError() override = default;
    /*! \brief Copy constructor. */
    xIniFileInvalidKeyError(const xIniFileInvalidKeyError&) = default;
    /*! \brief Copy assignment operator. */
    xIniFileInvalidKeyError& operator=(const xIniFileInvalidKeyError&) = default;
    /*! \brief Move constructor. */
    xIniFileInvalidKeyError(xIniFileInvalidKeyError&&) = default;
    /*! \brief Move assignment operator. */
    xIniFileInvalidKeyError& operator=(xIniFileInvalidKeyError&&) = default;
};

/*!
 * \brief Ini file invalid section error.
 *
 * This exception class is intended to be thrown by functions in the IniFIie
 * class when an invalid section is used occurs.
 */
class CORE_LIBRARY_DLL_SHARED_API xIniFileInvalidSectionError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xIniFileInvalidSectionError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xIniFileInvalidSectionError(const std::string& message);
    /*! \brief Virtual destructor. */
    ~xIniFileInvalidSectionError() override = default;
    /*! \brief Copy constructor. */
    xIniFileInvalidSectionError(const xIniFileInvalidSectionError&) = default;
    /*! \brief Copy assignment operator. */
    xIniFileInvalidSectionError& operator=(const xIniFileInvalidSectionError&) = default;
    /*! \brief Move constructor. */
    xIniFileInvalidSectionError(xIniFileInvalidSectionError&&) = default;
    /*! \brief Move assignment operator. */
    xIniFileInvalidSectionError& operator=(xIniFileInvalidSectionError&&) = default;
};

/*!
 * \brief Ini file class
 *
 * Here is an example of what an INI file could look like:
\verbatim
; I am an opening comment.

[Section1]
; I am a comment in a section.
Section1_Key1=Section1_Value1
Section1_Key2=Section1_Value2
Section1_Key3 = Section1_Value3

[Section2]
Section2_Key1=Section2_Value1
Section2_Key2 = Section2_Value2
; I am also a comment in a section.
Section2_Key3=Section2_Value3
; I am yet another comment in a section.
\endverbatim
 *
 * Please note that the supported comment delimiter is ';'.
 * Also note that blank lines and comments are preserved
 * when loading an ini file. However, leading and trailing
 * whitespace in sections, key or value items are removed.
 *
 * Only flat heirarchy INI files are supported; nested
 * sections are not supported.
 */
class CORE_LIBRARY_DLL_SHARED_API IniFile final
{
public:
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Default constructor. */
    IniFile();
#else
    /*! \brief Default constructor. */
    IniFile() = default;
#endif
    /*! \brief Copy constructor. */
    IniFile(const IniFile&) = default;
    /*!
     * \brief INI path based constructor.
     * \param[in] iniFilePath - Path to INI file.
     *
     * Create an INI file object from an INI file path.
     */
    explicit IniFile(const std::string& iniFilePath);
    /*! \brief Destructor. */
    ~IniFile() = default;
    /*! \brief Copy assignment operator. */
    IniFile& operator=(const IniFile&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move constructor. */
    IniFile(IniFile&& ini);
    /*! \brief Move assignment operator. */
    IniFile& operator=(IniFile&& ini);
#else
    /*! \brief Move constructor. */
    IniFile(IniFile&&) = default;
    /*! \brief Move assignment operator. */
    IniFile& operator=(IniFile&&) = default;
#endif
    /*!
     * \brief Load an INI file.
     * \param[in] iniFilePath - Path to an INI file.
     *
     *  Load from an INI file path.
     */
    void LoadFile(const std::string& iniFilePath);
    /*!
     * \brief Update the file.
     * \param[in] overridePath - Optional override path to save to alternative location.
     *
     * Write settings back to file on disk.
     */
    void UpdateFile(const std::string& overridePath = "") const;
    /*!
     * \brief Get sections.
     * \return A list of INI file section names.
     */
    std::list<std::string> GetSections() const;
    /*!
     * \brief Get section's key-value pairs.
     * \param[in] section - section Parameter_Description
     * \return A list of key-value pairs for an INI file section.
     */
    keys_list GetSection(const std::string& section) const;
    /*!
     * \brief Check if a section exists.
     * \param[in] section - Section name.
     * \return True if section name exists, false otherwise.
     */
    bool SectionExists(const std::string& section) const;
    /*!
     * \brief Check if a key exists for a section.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \return True if key exists, false otherwise.
     */
    bool KeyExists(const std::string& section, const std::string& key) const;
    /*!
     * \brief Read boolean value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] defaultValue - Default value if key not found.
     * \return Returns the value stored in the INI file.
     */
    bool ReadBool(const std::string& section, const std::string& key,
                  const bool defaultValue = false) const;
    /*!
     * \brief Read 32 bit integer value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] defaultValue - Default value if key not found.
     * \return Returns the value stored in the INI file.
     */
    int32_t ReadInt32(const std::string& section, const std::string& key,
                      const int32_t defaultValue = 0) const;
    /*!
     * \brief Read 64 bit integer value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] defaultValue - Default value if key not found.
     * \return Returns the value stored in the INI file.
     */
    int64_t ReadInt64(const std::string& section, const std::string& key,
                      const int64_t defaultValue = 0L) const;
    /*!
     * \brief Read double value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] defaultValue - Default value if key not found.
     * \return Returns the value stored in the INI file.
     */
    double ReadDouble(const std::string& section, const std::string& key,
                      const double defaultValue = 0.0) const;
    /*!
     * \brief Read long double value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] defaultValue - Default value if key not found.
     * \return Returns the value stored in the INI file.
     */
    long double ReadLongDouble(const std::string& section, const std::string& key,
                               const long double defaultValue = 0.0L) const;
    /*!
     * \brief Read string value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] defaultValue - Default value if key not found.
     * \return Returns the value stored in the INI file.
     */
    std::string ReadString(const std::string& section, const std::string& key,
                           const std::string& defaultValue = "") const;

    /*!
     * \brief Write boolean value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] value - Value to write to file.
     */
    void WriteBool(const std::string& section, const std::string& key, const bool value);
    /*!
     * \brief Write 32 bit integer value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] value - Value to write to file.
     */
    void WriteInt32(const std::string& section, const std::string& key, const int32_t value);
    /*!
     * \brief Write 64 bit integer value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] value - Value to write to file.
     */
    void WriteInt64(const std::string& section, const std::string& key, const int64_t value);
    /*!
     * \brief Write double value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] value - Value to write to file.
     */
    void WriteDouble(const std::string& section, const std::string& key, const double value);
    /*!
     * \brief Write long double value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] value - Value to write to file.
     */
    void WriteLongDouble(const std::string& section, const std::string& key,
                         const long double value);
    /*!
     * \brief Write string value.
     * \param[in] section - Section name.
     * \param[in] key - Key name.
     * \param[in] value - Value to write to file.
     */
    void WriteString(const std::string& section, const std::string& key, const std::string& value);
    /*!
     * \brief Erase a whole section.
     * \param[in] section - Section name.
     */
    void EraseSection(const std::string& section);
    /*!
     * \brief Erase all the sections.
     */
    void EraseSections();
    /*!
     * \brief Erase a key within a section.
     * \param[in] section - Section name.
     * \param[in] key - Section name.
     */
    void EraseKey(const std::string& section, const std::string& key);
    /*!
     * \brief Erase all keys in a section.
     * \param[in] section - Section name.
     */
    void EraseKeys(const std::string& section);

private:
#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Changes made flag. */
    mutable bool m_changesMade;
    /*! \brief INI file path. */
    std::string m_iniFilePath;
#else
    /*! \brief Changes made flag. */
    mutable bool m_changesMade{false};
    /*! \brief INI file path. */
    std::string m_iniFilePath{"config.ini"};
#endif
    /*! \brief Section map typedef. */
    typedef std::map<std::string, if_private::SectionDetails> section_map;
    /*! \brief Sectin map. */
    section_map m_sectionMap;
    /*! \brief Section map iterator typedef. */
    typedef section_map::iterator section_iter;
    /*! \brief Section map const iterator typedef. */
    typedef section_map::const_iterator section_citer;
    /*! \brief Line list. */
    if_private::line_list m_lines;

    /*!
     * \brief Read value from INI file.
     * \param[in] section - The section.
     * \param[in] key - The key.
     * \param[in] defaultValue - Default value if key not found as const reference.
     * \return The value.
     */
    std::string ReadValueString(const std::string& section, const std::string& key,
                                const std::string& defaultValue) const;
    /*!
     * \brief Write value to INI file.
     * \param[in] section - The section.
     * \param[in] key - The key.
     * \param[in] value - Value to write as const reference.
     */
    void WriteValueString(const std::string& section, const std::string& key,
                          const std::string& value);
};

} // namespace ini_file
} // namespace core_lib

#endif // INIFILE
