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

#ifndef INIFILE_HPP
#define INIFILE_HPP

#include <utility>
#include <map>
#include <cstdint>
#include "Exceptions/CustomException.hpp"
#include "IniFile/IniFileSectionDetails.hpp"

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
	 * \param[in] message - A user specifed message string.
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
	 * \param[in] message - A user specifed message string.
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
	 * \param[in] message - A user specifed message string.
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
	 * \param[in] message - A user specifed message string.
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
	 * \param[in] message - A user specifed message string.
	 */
	explicit xIniFileInvalidSectionError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xIniFileInvalidSectionError();
};

/*!
 * \brief Ini file class
 *
 * Here is an example of what a ini file could look like:
 *
 *    ; I am an opening comment.
 *
 *    [Section1]
 *    ; I am a comment in a section.
 *    Section1_Key1=Section1_Value1
 *    Section1_Key2=Section1_Value2
 *    Section1_Key3=Section1_Value3
 *
 *    [Section2]
 *    Section2_Key1=Section2_Value1
 *    Section2_Key2=Section2_Value2
 *    ; I am also a comment in a section.
 *    Section2_Key3=Section2_Value3
 *    ; I am yet another comment in a section.
 *
 * Please note that the supported comment delimiter is ';'.
 * Also note that blank lines and comments are preserved
 * when loading an ini file. However, leading and trailing
 * whitespace in section, key or value items are removed.
 */
class IniFile final
{
public:
	/*! \brief Default constructor. */
	IniFile() = default;
	/*! \brief Copy constructor. */
	IniFile(const IniFile&) = default;
	/*! \brief Move constructor. */
	IniFile(IniFile&&) = default;
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
	/*! \brief Move assignment operator. */
	IniFile& operator=(IniFile&&) = default;
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
    auto GetSections() -> std::list<std::string> const;
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
	bool KeyExists(const std::string& section
				   , const std::string& key) const;
	/*!
	 * \brief Read boolean value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] defaultValue - Default value if key not found.
	 * \return Returns the value stored in the INI file.
	 */
	bool ReadBool(const std::string& section
				  , const std::string& key
				  , const bool defaultValue = false) const;
	/*!
	 * \brief Read 32 bit integer value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] defaultValue - Default value if key not found.
	 * \return Returns the value stored in the INI file.
	 */
	int32_t ReadInteger(const std::string& section
						, const std::string&key
						, const int32_t defaultValue = 0) const;
	/*!
	 * \brief Read 64 bit integer value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] defaultValue - Default value if key not found.
	 * \return Returns the value stored in the INI file.
	 */
	int64_t ReadInteger64(const std::string& section
						  , const std::string& key
						  , const int64_t defaultValue = 0L) const;
	/*!
	 * \brief Read double value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] defaultValue - Default value if key not found.
	 * \return Returns the value stored in the INI file.
	 */
	double ReadDouble(const std::string& section
					  , const std::string& key
					  , const double defaultValue = 0.0) const;
	/*!
	 * \brief Read long double value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] defaultValue - Default value if key not found.
	 * \return Returns the value stored in the INI file.
	 */
	long double ReadLongDouble(const std::string& section
							   , const std::string& key
							   , const long double defaultValue = 0.0L) const;
	/*!
	 * \brief Read string value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] defaultValue - Default value if key not found.
	 * \return Returns the value stored in the INI file.
	 */
	std::string ReadString(const std::string& section
						   , const std::string& key
						   , const std::string& defaultValue = "") const;
	/*!
	 * \brief Write boolean value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] value - Value to write to file.
	 */
	void WriteBool(const std::string& section
				   , const std::string& key
				   , const bool value);
	/*!
	 * \brief Write 32 bit integer value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] value - Value to write to file.
	 */
	void WriteInteger(const std::string& section
					  , const std::string& key
					  , const int32_t value);
	/*!
	 * \brief Write 64 bit integer value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] value - Value to write to file.
	 */
	void WriteInteger64(const std::string& section
						, const std::string& key
						, const int64_t value);
	/*!
	 * \brief Write double value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] value - Value to write to file.
	 */
	void WriteDouble(const std::string& section
					 , const std::string& key
					 , const double value);
	/*!
	 * \brief Write long double value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] value - Value to write to file.
	 */
	void WriteLongDouble(const std::string& section
						 , const std::string& key
						 , const long double value);
	/*!
	 * \brief Write string value.
	 * \param[in] section - Section name.
	 * \param[in] key - Key name.
	 * \param[in] value - Value to write to file.
	 */
	void WriteString(const std::string& section
					 , const std::string& key
					 , const std::string& value);
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
	void EraseKey(const std::string& section
				  , const std::string& key);
	/*!
	 * \brief Erase all keys in a section.
	 * \param[in] section - Section name.
	 */
	void EraseKeys(const std::string& section);

private:

	/*! \brief Changes made flag. */
	mutable bool m_changesMade{false};
	/*! \brief INI file path. */
	std::string m_iniFilePath{"config.ini"};
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
	std::string ReadValue(const std::string& section
						  , const std::string& key
						  , const std::string& defaultValue) const;
	/*!
	 * \brief Read value from INI file.
	 * \param[in] section - The section.
	 * \param[in] key - The key.
	 * \param[in] defaultValue - Default value if key not found as r-value reference.
	 * \return The value.
	 */
	std::string ReadValue(const std::string& section
						  , const std::string& key
						  , std::string&& defaultValue) const;
	/*!
	 * \brief Write value to INI file.
	 * \param[in] section - The section.
	 * \param[in] key - The key.
	 * \param[in] value - Value to write as const reference.
	 */
	void WriteValue(const std::string& section
					, const std::string& key
					, const std::string& value);
	/*!
	 * \brief Write value to INI file.
	 * \param[in] section - The section.
	 * \param[in] key - The key.
	 * \param[in] value - Value to write as r-value reference.
	 */
	void WriteValue(const std::string& section
					, const std::string& key
					, std::string&& value);
};

} // namespace ini_file
} // namespace core_lib


#endif //INIFILE_HPP
