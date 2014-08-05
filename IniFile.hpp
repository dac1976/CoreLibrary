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

/*!
 * \brief Ini file class
 *
 * Here is an example of what a ini file may look like:
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
	/** \brief Default constructor. */
	IniFile() = default;
	/** \brief Copy constructor. */
	IniFile(const IniFile&) = default;
	/** \brief Move constructor. */
	IniFile(IniFile&&) = default;
	/**
	 * \brief INI path based constructor.
	 * \param [IN] Path to INI file.
	 *
	 * Create an INI file object from an INI file path.
	 */
	explicit IniFile(const std::string& iniFilePath);
	/** \brief Destructor. */
	~IniFile() = default;
	/** \brief Copy assignment operator. */
	IniFile& operator=(const IniFile&) = default;
	/** \brief Move assignment operator. */
	IniFile& operator=(IniFile&&) = default;
	/**
	 * \brief Load an INI file.
	 * \param [IN] Path to an INI file.
	 *
	 *  Load from an INI file path.
	 */
	void LoadFile(const std::string& iniFilePath);
	/**
	 * \brief Update the file.
	 * \param [IN] Optional override path to save to alternative location.
	 *
	 * Write settings back to file on disk.
	 */
	void UpdateFile(const std::string& overridePath = "") const;
	/**
	 * \brief Get sections.
	 * \return A list of INI file section names.
	 */
	std::list<std::string> GetSections() const;
	/** \brief Typedef defining the key-value pair list for section entries. */
	typedef std::list<std::pair<std::string, std::string>> keys_list;
	/**
	 *  \brief Get section's key-value pairs.
	 *  \param [IN] section Parameter_Description
	 *  \return A list of key-value pairs for an INI file section.
	 */
	keys_list GetSection(const std::string& section) const;
	/**
	 *  \brief Check if a section exists.
	 *  \param [IN] Section name.
	 *  \return True if section name exists, false otherwise.
	 */
	bool SectionExists(const std::string& section) const;
	/**
	 *  \brief Check if a key exists for a section.
	 *  \param [IN] Section name.
	 *  \param [IN] Key name.
	 *  \return True if key exists, false otherwise.
	 */
	bool KeyExists(const std::string& section
				   , const std::string& key) const;
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section      Parameter_Description
	 *  \param [IN] key          Parameter_Description
	 *  \param [IN] defaultValue Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	bool ReadBool(const std::string& section
				  , const std::string& key
				  , const bool defaultValue = false) const;
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section      Parameter_Description
	 *  \param [IN] key          Parameter_Description
	 *  \param [IN] defaultValue Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	int32_t ReadInteger(const std::string& section
						, const std::string&key
						, const int32_t defaultValue = 0) const;
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section      Parameter_Description
	 *  \param [IN] key          Parameter_Description
	 *  \param [IN] defaultValue Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	int64_t ReadInteger64(const std::string& section
						  , const std::string& key
						  , const int64_t defaultValue = 0L) const;
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section      Parameter_Description
	 *  \param [IN] key          Parameter_Description
	 *  \param [IN] defaultValue Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	double ReadDouble(const std::string& section
					  , const std::string& key
					  , const double defaultValue = 0.0) const;
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section      Parameter_Description
	 *  \param [IN] key          Parameter_Description
	 *  \param [IN] defaultValue Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	long double ReadLongDouble(const std::string& section
							   , const std::string& key
							   , const long double defaultValue = 0.0L) const;
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section      Parameter_Description
	 *  \param [IN] key          Parameter_Description
	 *  \param [IN] defaultValue Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	std::string ReadString(const std::string& section
						   , const std::string& key
						   , const std::string& defaultValue = "") const;
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \param [IN] key     Parameter_Description
	 *  \param [IN] value   Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void WriteBool(const std::string& section
				   , const std::string& key
				   , const bool value);
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \param [IN] key     Parameter_Description
	 *  \param [IN] value   Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void WriteInteger(const std::string& section
					  , const std::string& key
					  , const int32_t value);
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \param [IN] key     Parameter_Description
	 *  \param [IN] value   Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void WriteInteger64(const std::string& section
						, const std::string& key
						, const int64_t value);
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \param [IN] key     Parameter_Description
	 *  \param [IN] value   Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void WriteDouble(const std::string& section
					 , const std::string& key
					 , const double value);
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \param [IN] key     Parameter_Description
	 *  \param [IN] value   Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void WriteLongDouble(const std::string& section
						 , const std::string& key
						 , const long double value);
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \param [IN] key     Parameter_Description
	 *  \param [IN] value   Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void WriteString(const std::string& section
					 , const std::string& key
					 , const std::string& value);
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void EraseSection(const std::string& section);
	/**
	 *  \brief Brief
	 *
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void EraseSections();
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \param [IN] key     Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void EraseKey(const std::string& section
				  , const std::string& key);
	/**
	 *  \brief Brief
	 *
	 *  \param [IN] section Parameter_Description
	 *  \return Return_Description
	 *
	 *  \details Details
	 */
	void EraseKeys(const std::string& section);

private:
	class Line
	{
	public:
		Line() = default;
		Line(const Line&) = default;
		Line(Line&&) = default;
		virtual ~Line() = default;
		Line& operator=(const Line&) = default;
		Line& operator=(Line&&) = default;
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const = 0;
	};

	class BlankLine : public Line
	{
	public:
		BlankLine() = default;
		BlankLine(const BlankLine&) = default;
		BlankLine(BlankLine&&) = default;
		virtual ~BlankLine() = default;
		BlankLine& operator=(const BlankLine&) = default;
		BlankLine& operator=(BlankLine&&) = default;
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;
	};

	class CommentLine : public Line
	{
	public:
		CommentLine() = default;
		CommentLine(const CommentLine&) = default;
		CommentLine(CommentLine&&) = default;
		CommentLine(const std::string& comment);
		virtual ~CommentLine() = default;
		CommentLine& operator=(const CommentLine&) = default;
		CommentLine& operator=(CommentLine&&) = default;
		const std::string& Comment() const;
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

	private:
		std::string m_comment{};
	};

	class SectionLine : public Line
	{
	public:
		SectionLine() = default;
		SectionLine(const SectionLine&) = default;
		SectionLine(SectionLine&&) = default;
		SectionLine(const std::string& section);
		virtual ~SectionLine() = default;
		SectionLine& operator=(const SectionLine&) = default;
		SectionLine& operator=(SectionLine&&) = default;
		const std::string& Section() const;
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

	private:
		std::string m_section{};
	};

	class KeyLine : public Line
	{
	public:
		KeyLine() = default;
		KeyLine(const KeyLine&) = default;
		KeyLine(KeyLine&&) = default;
		KeyLine(const std::string& key, const std::string& value);
		virtual ~KeyLine() = default;
		KeyLine& operator=(const KeyLine&) = default;
		KeyLine& operator=(KeyLine&&) = default;
		const std::string& Key() const;
		const std::string& Value() const;
		void Value(const std::string& value);
		void Value(std::string&& value);
		virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

	private:
		std::string m_key{};
		std::string m_value{};
	};

	typedef std::list<std::shared_ptr<Line>> line_list;
	typedef line_list::iterator line_iter;
	typedef line_list::const_iterator line_citer;

	class SectionDetails
	{
	public:
		SectionDetails() = default;
		SectionDetails(const SectionDetails&) = default;
		SectionDetails(SectionDetails&&) = default;
		explicit SectionDetails(const line_iter& sectIter);
		~SectionDetails() = default;
		SectionDetails& operator=(const SectionDetails&) = default;
		SectionDetails& operator=(SectionDetails&&) = default;
		const std::string& Section() const;
		bool KeyExists(const std::string& key) const;
		void AddKey(const line_iter& keyIter);
		void UpdateKey(const std::string& key
					   , const std::string& value);
		bool EraseKey(const std::string& key
					  , IniFile::line_iter& lineIter);
		std::string GetValue(const std::string& key
							 , const std::string& defaultValue = "") const;
		void GetKeys(keys_list& keys) const;
		line_iter LineIterator() const;

	private:
		line_iter m_sectIter{};
		typedef std::list<line_iter> line_iter_list;
		line_iter_list m_keyIters{};
		typedef line_iter_list::iterator keys_iter;
	};

	mutable bool m_changesMade{false};
	std::string m_iniFilePath{"config.ini"};
	typedef std::map<std::string, SectionDetails> section_map;
	section_map m_sectionMap;
	typedef section_map::iterator section_iter;
	typedef section_map::const_iterator section_citer;
	line_list m_lines;


	std::string ReadValue(const std::string& section
						  , const std::string& key
						  , const std::string& defaultValue) const;
	std::string ReadValue(const std::string& section
						  , const std::string& key
						  , std::string&& defaultValue) const;
	void WriteValue(const std::string& section
					, const std::string& key
					, std::string&& value);
	void WriteValue(const std::string& section
					, const std::string& key
					, const std::string& value);
};

} // namespace ini_file
} // namespace core_lib


#endif //INIFILE_HPP
