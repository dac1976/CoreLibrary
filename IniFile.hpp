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
#include <memory>
#include <string>
#include <list>
#include <map>
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
	std::list<std::string> GetSections() const;
    /*! \brief Typedef defining the key-value pair list for section entries. */
	typedef std::list<std::pair<std::string, std::string>> keys_list;
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
    /*! \brief Base class to manage a line in an INI file. */
	class Line
	{
	public:
        /*! \brief Default constructor. */
		Line() = default;
        /*! \brief Copy constructor. */
		Line(const Line&) = default;
        /*! \brief Move constructor. */
        Line(Line&&) = default;
        /*! \brief Virtual destructor. */
		virtual ~Line() = default;
        /*! \brief Copy assignment operator. */
		Line& operator=(const Line&) = default;
        /*! \brief Move assignment operator. */
		Line& operator=(Line&&) = default;
        /*!
         * \brief Virtual Print function
         * \param[in][out] os - Stream to write to.
         * \param[in] addLineFeed - (Optional) add a line feed.
         */
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const = 0;
	};

    /*! \brief Class to manage a blank line in an INI file. */
	class BlankLine : public Line
	{
	public:
        /*! \brief Default constructor. */
		BlankLine() = default;
        /*! \brief Copy constructor. */
		BlankLine(const BlankLine&) = default;
        /*! \brief Move constructor. */
		BlankLine(BlankLine&&) = default;
        /*! \brief Virtual destructor. */
		virtual ~BlankLine() = default;
        /*! \brief Copy assignment operator. */
		BlankLine& operator=(const BlankLine&) = default;
        /*! \brief Move assignment operator. */
		BlankLine& operator=(BlankLine&&) = default;
        /*!
         * \brief Virtual Print function
         * \param[in][out] os - Stream to write to.
         * \param[in] addLineFeed - (Optional) add a line feed.
         */
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;
	};

    /*! \brief Class to manage a commented line in an INI file. */
	class CommentLine : public Line
	{
	public:
        /*! \brief Default constructor. */
		CommentLine() = default;
        /*! \brief Copy constructor. */
		CommentLine(const CommentLine&) = default;
        /*! \brief Move constructor. */
		CommentLine(CommentLine&&) = default;
        /*! \brief Initialising constructor. */
		CommentLine(const std::string& comment);
        /*! \brief Virtual destructor. */
		virtual ~CommentLine() = default;
        /*! \brief Copy assignment operator. */
		CommentLine& operator=(const CommentLine&) = default;
        /*! \brief Move assignment operator. */
		CommentLine& operator=(CommentLine&&) = default;
        /*!
         * \brief Get the comment.
         * \return The comment.
         */
		const std::string& Comment() const;
        /*!
         * \brief Virtual Print function
         * \param[in][out] os - Stream to write to.
         * \param[in] addLineFeed - (Optional) add a line feed.
         */
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

	private:
        /*! \brief The comment. */
		std::string m_comment{};
	};

    /*! \brief Class to manage a section line in an INI file. */
	class SectionLine : public Line
	{
	public:
        /*! \brief Default constructor. */
		SectionLine() = default;
        /*! \brief Copy constructor. */
		SectionLine(const SectionLine&) = default;
        /*! \brief Move constructor. */
		SectionLine(SectionLine&&) = default;
        /*! \brief Initialising constructor. */
		SectionLine(const std::string& section);
        /*! \brief Virtual destructor. */
		virtual ~SectionLine() = default;
        /*! \brief Copy assignment operator. */
		SectionLine& operator=(const SectionLine&) = default;
        /*! \brief Move assignment operator. */
		SectionLine& operator=(SectionLine&&) = default;
        /*!
         * \brief Get the section.
         * \return The section.
         */
		const std::string& Section() const;
        /*!
         * \brief Virtual Print function
         * \param[in][out] os - Stream to write to.
         * \param[in] addLineFeed - (Optional) add a line feed.
         */
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

	private:
        /*! \brief The section. */
		std::string m_section{};
	};

    /*! \brief Class to manage a key line in an INI file. */
	class KeyLine : public Line
	{
	public:
        /*! \brief Default constructor. */
		KeyLine() = default;
        /*! \brief Copy constructor. */
		KeyLine(const KeyLine&) = default;
        /*! \brief Move constructor. */
		KeyLine(KeyLine&&) = default;
        /*! \brief Initialising constructor. */
		KeyLine(const std::string& key, const std::string& value);
        /*! \brief Virtual destructor. */
		virtual ~KeyLine() = default;
        /*! \brief Copy assignment operator. */
		KeyLine& operator=(const KeyLine&) = default;
        /*! \brief Move assignment operator. */
		KeyLine& operator=(KeyLine&&) = default;
        /*!
         * \brief Get the key.
         * \return The key.
         */
		const std::string& Key() const;
        /*!
         * \brief Get the value.
         * \return The value.
         */
		const std::string& Value() const;
        /*!
         * \brief Set the value.
         * \param[in] value - The value by const reference.
         */
		void Value(const std::string& value);
        /*!
         * \brief Set the value.
         * \param[in] value - The value by r-value reference.
         */
		void Value(std::string&& value);
        /*!
         * \brief Virtual Print function
         * \param[in][out] os - Stream to write to.
         * \param[in] addLineFeed - (Optional) add a line feed.
         */
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

	private:
        /*! \brief The key. */
		std::string m_key{};
        /*! \brief The value. */
		std::string m_value{};
	};

    /*! \brief Line list typedef. */
	typedef std::list<std::shared_ptr<Line>> line_list;
    /*! \brief Line list iterator typedef. */
	typedef line_list::iterator line_iter;
    /*! \brief Line list const iterator typedef. */
	typedef line_list::const_iterator line_citer;

    /*! \brief Class to section details. */
	class SectionDetails
	{
	public:
        /*! \brief Default constructor. */
		SectionDetails() = default;
        /*! \brief Copy constructor. */
		SectionDetails(const SectionDetails&) = default;
        /*! \brief Move constructor. */
		SectionDetails(SectionDetails&&) = default;
        /*! \brief Initialising constructor. */
		explicit SectionDetails(const line_iter& sectIter);
		~SectionDetails() = default;
        /*! \brief Copy assignment operator. */
		SectionDetails& operator=(const SectionDetails&) = default;
        /*! \brief Move assignment operator. */
		SectionDetails& operator=(SectionDetails&&) = default;
        /*!
         * \brief Get the section.
         * \return The section.
         */
		const std::string& Section() const;
        /*!
         * \brief Does a key exist
         * \param[in] key - The key.
         * \return True if exists, false otherwise.
         */
		bool KeyExists(const std::string& key) const;
        /*!
         * \brief Add a key to the section.
         * \param[in] keyIter - The key iterator.
         */
		void AddKey(const line_iter& keyIter);
        /*!
         * \brief Update a key's value in the section.
         * \param[in] key - The key.
         * \param[in] value - The value.
         */
		void UpdateKey(const std::string& key
					   , const std::string& value);
        /*!
         * \brief Erase a key value from the section.
         * \param[in] key - The key.
         * \param[out] lineIter - Line iterator to the erased key.
         * \return True if erased, false otherwise.
         */
		bool EraseKey(const std::string& key
					  , IniFile::line_iter& lineIter);
        /*!
         * \brief Get a key's value from the section.
         * \param[in] key - The key.
         * \param[in] defaultValue - The default value to use if key not found.
         * \return The value for the key.
         */
		std::string GetValue(const std::string& key
							 , const std::string& defaultValue = "") const;
        /*!
         * \brief Get a list of keys in the section.
         * \param[out] key - The list of keys.
         */
		void GetKeys(keys_list& keys) const;
        /*!
         * \brief Get the line iterator.
         * \return Line iterator.
         */
		line_iter LineIterator() const;

	private:
        /*! \brief Line iterator. */
		line_iter m_sectIter{};
        /*! \brief Line iterator lists typedef. */
		typedef std::list<line_iter> line_iter_list;
        /*! \brief Line iterator list. */
		line_iter_list m_keyIters{};
        /*! \brief Line iterator list iterator. */
		typedef line_iter_list::iterator keys_iter;
	};

    /*! \brief Changes made flag. */
	mutable bool m_changesMade{false};
    /*! \brief INI file path. */
	std::string m_iniFilePath{"config.ini"};
    /*! \brief Section map typedef. */
	typedef std::map<std::string, SectionDetails> section_map;
    /*! \brief Sectin map. */
	section_map m_sectionMap;
    /*! \brief Section map iterator typedef. */
	typedef section_map::iterator section_iter;
    /*! \brief Section map const iterator typedef. */
	typedef section_map::const_iterator section_citer;
    /*! \brief Line list. */
	line_list m_lines;

    /*!
     * \brief Read value from INI file.
     * \param[in] section - The section.
     * \param[in] key - The key.
     * \param[int] defaultValue - Default value if key not found as const reference.
     * \return The value.
     */
	std::string ReadValue(const std::string& section
						  , const std::string& key
						  , const std::string& defaultValue) const;
    /*!
     * \brief Read value from INI file.
     * \param[in] section - The section.
     * \param[in] key - The key.
     * \param[int] defaultValue - Default value if key not found as r-value reference.
     * \return The value.
     */
	std::string ReadValue(const std::string& section
						  , const std::string& key
						  , std::string&& defaultValue) const;
    /*!
     * \brief Write value to INI file.
     * \param[in] section - The section.
     * \param[in] key - The key.
     * \param[int] value - Value to write as const reference.
     */
	void WriteValue(const std::string& section
					, const std::string& key
					, const std::string& value);
    /*!
     * \brief Write value to INI file.
     * \param[in] section - The section.
     * \param[in] key - The key.
     * \param[int] value - Value to write as r-value reference.
     */
    void WriteValue(const std::string& section
                    , const std::string& key
                    , std::string&& value);
};

} // namespace ini_file
} // namespace core_lib


#endif //INIFILE_HPP
