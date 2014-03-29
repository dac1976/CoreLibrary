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
 * \brief Ini file saveerror.
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

class EIniFileInvalidKeyError : public std::runtime_error
{
public:
	EIniFileInvalidKeyError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};
class EIniFileInvalidSectionError : public std::runtime_error
{
public:
	EIniFileInvalidSectionError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};

class IniFile
{
public:
    IniFile();
    
	explicit IniFile(const std::string& IniFilePath);

	~IniFile();

	void LoadFile(const std::string& IniFilePath);

	void UpdateFile() const;

	void GetSections(std::vector< std::string >& Sections) const;

	void GetSection(const std::string& Section, std::vector< std::pair<std::string, std::string> >& pairs) const;

	bool SectionExists(const std::string& Section) const;

	bool ValueExists(const std::string& Section,
					 const std::string& Key) const;

	bool ReadBool(const std::string& Section,
		          const std::string& Key,
				  const bool DefaultValue = false) const;

    int ReadInteger(const std::string& Section,
		            const std::string& Key,
					const int DefaultValue = 0) const;

	double ReadFloat(const std::string& Section,
		             const std::string& Key,
					 const double DefaultValue = 0.0) const;

	std::string ReadString(const std::string& Section,
		                   const std::string& Key,
						   const std::string& DefaultValue = "") const;

	void WriteBool(const std::string& Section,
		           const std::string& Key,
				   const bool Value);

    void WriteInteger(const std::string& Section,
		              const std::string& Key,
					  const int Value);

	void WriteFloat(const std::string& Section,
		            const std::string& Key,
				    const double Value);

	void WriteString(const std::string& Section,
		             const std::string& Key,
				     const std::string& Value);

    void EraseSection(const std::string& Section);

    void EraseSections();

    void DeleteKey(const std::string& Section,
                   const std::string& Key);

    void DeleteKeys(const std::string& Section);

private:

};

} // namespace ini_file
} // namespace core_lib


#endif //INIFILE_HPP
