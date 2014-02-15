/*-----------------------------------------------------------------------------
This is a thread-safe (mutex protected) class for loading, creating, modifying and saving
ini files. It has a similar public interface and functionality to TMemIniFile found
in C++ Builder. The main difference is this class is portable and thread-safe, whereas
TMemIniFile uses UnicodeString, a C++ Builder specific class, and is not thread-safe. This
class uses standard C++, STL and boost objects.

The ini file format should be of the form:
	[Section1]
	Section1_Key1=Section1_Value1
	Section1_Key2=Section1_Value2
	Section1_Key3=Section1_Value3

	[Section2]
	Section2_Key1=Section2_Value1
	Section2_Key2=Section2_Value2
	Section2_Key3=Section2_Value3

Section headings are (non-empty) strings that are unique within the ini file.

Keys are (non-empty) strings that are unique within their section.

Values can be boolean (1 or 0), integer (-2,147,483,648 to +2,147,483,647), double precision floating
point (to 18 significant figures) or strings.

Non-fatal formatting errors are handled gracefully by the parser and skipped over hence
the memory copy of the ini file held in class IniFile will only contain the legal entries.
This means if you save the ini file back down it will have the errors stripped from it.

The parser tolerates spaces either side of '=' and in section titles, e.g. The following
are both legal:
	[Section1]
	Section1_Key1=Section1_Value1

	[ Section1 ]
	Section1_Key2 = Section1_Value2

Unnecessary whitespace and spaces are removed when parsing the file so when saved back down
these will have been stripped from the file, e.g.

	[ Section1 ]
	Section1_Key2 = Section1_Value2

will become:

	[Section1]
	Section1_Key1=Section1_Value1

Ini files also have a standard comment delimiter. This IniFile class also handles comment
lines within the INI file and will preserve them upon saving the file back down. Comment
lines in an INI file must start with ';'.  Below is an example:

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

-----------------------------------------------------------------------------*/
#pragma once
//-----------------------------------------------------------------------------
#include <string>
#include <vector>
#include <exception>
#include <boost/shared_ptr.hpp>
//-----------------------------------------------------------------------------
namespace hgl {
//-----------------------------------------------------------------------------
namespace ini_file {
//-----------------------------------------------------------------------------
//exception classes...
class EIniFileDataConvertError : public std::runtime_error
{
public:
	EIniFileDataConvertError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};
//-----------------------------------------------------------------------------
class EIniFileParserError : public std::runtime_error
{
public:
	EIniFileParserError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};
//-----------------------------------------------------------------------------
class EIniFileSaveError : public std::runtime_error
{
public:
	EIniFileSaveError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};
//-----------------------------------------------------------------------------
class EIniFileInvalidKeyError : public std::runtime_error
{
public:
	EIniFileInvalidKeyError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};
//-----------------------------------------------------------------------------
class EIniFileInvalidSectionError : public std::runtime_error
{
public:
	EIniFileInvalidSectionError(const std::string& Message)
		: std::runtime_error(Message)
	{ }
};
//-----------------------------------------------------------------------------
class IniFile
{
public:
	//constructor, throws standard exceptions and EIniFileParserError...
	explicit IniFile(const std::string& IniFilePath);

	//constructor, throws std::bad_alloc, does NOT read the ini file.
    enum DoNotReadIni_t{ DoNotReadIni };
	IniFile(const std::string& IniFilePath, DoNotReadIni_t);

	//destructor...
	virtual ~IniFile();

	//read ini file, throws EIniFileParserError if error occurs...
	void ReadFile() const;

	//save ini file to disk, throws EIniFileSaveError if error occurs...
	void UpdateFile() const;

	//get a list of the Sections...
	void ReadSections(std::vector< std::string >& Sections) const;

	//get list of Keys for this Section...
	void ReadSection(const std::string& Section,
		             std::vector< std::string >& Keys) const;

	//get list of Values for this Section (in 1-to-1 correspondence
	//with Keys)...
	void ReadSectionValues(const std::string& Section,
						   std::vector< std::string >& Values) const;

	//Does a section exist in the in file?
	bool SectionExists(const std::string& Section) const;

	//Does a key-value pair exist for a section in the in file?
	bool ValueExists(const std::string& Section,
					 const std::string& Key) const;

	//Functions to get Values based on a Section, Key and specific type.
	//If cannot find the section/key combination then the default value
	//given by the arg DefaultValue is returned...
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

	//Functions to set Values based on a Section, Key and specific type,
    //can throw EIniFileInvalidKeyError, EIniFileDataConvertError and
	//EIniFileInvalidSectionError. Note: Section and/or Key are created
	//if they do not already exist...
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

    //delete a Section...
    void EraseSection(const std::string& Section);

    //delete all Sections...
    void EraseSections();

    //delete a Key (and its Value) in a Section...
    void DeleteKey(const std::string& Section,
                   const std::string& Key);

    //delete all Keys (and its Value) in a Section...
    void DeleteKeys(const std::string& Section);

private:
	//object to hold all our private data...
    class IniFilePImpl;
	boost::shared_ptr<IniFilePImpl> m_PImpl;
};

//typedef for a shared_ptr of the correct type...
typedef boost::shared_ptr< IniFile > ini_file_ptr;

//-----------------------------------------------------------------------------
} //namespace ini_file
using ini_file::IniFile;
using ini_file::EIniFileDataConvertError;
using ini_file::EIniFileParserError;
using ini_file::EIniFileSaveError;
using ini_file::EIniFileInvalidKeyError;
using ini_file::EIniFileInvalidSectionError;
//-----------------------------------------------------------------------------
} //namespace hgl
//-----------------------------------------------------------------------------
namespace hgl_ini_file = hgl::ini_file;
//-----------------------------------------------------------------------------
