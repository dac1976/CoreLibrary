// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014,2015 Duncan Crutchley
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
 * \file IniFileLines.h
 * \brief File containing declarations relating the IniFile support classes.
 */

#ifndef INIFILELINES
#define INIFILELINES

#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"
#include <string>
#include <list>
#include <memory>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The ini_file namespace. */
namespace ini_file {

/*! \brief Typedef defining the key-value pair list for section entries. */
typedef std::list<std::pair<std::string, std::string>> keys_list;

/*! \brief The if_private namespace. */
namespace if_private {

/*! \brief Base class to manage a line in an INI file. */
class CORE_LIBRARY_DLL_SHARED_API Line
{
public:   
	/*!
	 * \brief Virtual Print function
	 * \param[in,out] os - Stream to write to.
	 * \param[in] addLineFeed - (Optional) add a line feed.
	 */
	virtual void Print(std::ostream &os, const bool addLineFeed = true) const = 0;
};

/*! \brief Class to manage a blank line in an INI file. */
class CORE_LIBRARY_DLL_SHARED_API BlankLine : public Line
{
public:
	/*! \brief Default constructor. */
	BlankLine() = default;
	/*! \brief Copy constructor. */
	BlankLine(const BlankLine&) = default;
	/*! \brief Virtual destructor. */
	virtual ~BlankLine() = default;
	/*! \brief Copy assignment operator. */
	BlankLine& operator=(const BlankLine&) = default;
#ifdef __USE_EXPLICIT_MOVE__
    /*! \brief Move constructor. */
    BlankLine(BlankLine&& line);
    /*! \brief Move assignment operator. */
    BlankLine& operator=(BlankLine&& line);
#else
    /*! \brief Move constructor. */
    BlankLine(BlankLine&&) = default;
	/*! \brief Move assignment operator. */
	BlankLine& operator=(BlankLine&&) = default;
#endif
	/*!
	 * \brief Virtual Print function
	 * \param[in,out] os - Stream to write to.
	 * \param[in] addLineFeed - (Optional) add a line feed.
	 */
	virtual void Print(std::ostream &os, const bool addLineFeed = true) const;
};

/*! \brief Class to manage a commented line in an INI file. */
class CORE_LIBRARY_DLL_SHARED_API CommentLine : public Line
{
public:
	/*! \brief Default constructor. */
	CommentLine() = default;
	/*! \brief Copy constructor. */
	CommentLine(const CommentLine&) = default;	
	/*! \brief Initialising constructor. */
	explicit CommentLine(const std::string& comment);
	/*! \brief Virtual destructor. */
	virtual ~CommentLine() = default;
	/*! \brief Copy assignment operator. */
    CommentLine& operator=(const CommentLine&) = default;
#ifdef __USE_EXPLICIT_MOVE__
    /*! \brief Move constructor. */
    CommentLine(CommentLine&& line);
    /*! \brief Move assignment operator. */
    CommentLine& operator=(CommentLine&& line);
#else
    /*! \brief Move constructor. */
    CommentLine(CommentLine&&) = default;
    /*! \brief Move assignment operator. */
    CommentLine& operator=(CommentLine&&) = default;
#endif
	/*!
	 * \brief Get the comment.
	 * \return The comment.
	 */
	const std::string& Comment() const;
	/*!
	 * \brief Virtual Print function
	 * \param[in,out] os - Stream to write to.
	 * \param[in] addLineFeed - (Optional) add a line feed.
	 */
	virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

private:
	/*! \brief The comment. */
	std::string m_comment{};
};

/*! \brief Class to manage a section line in an INI file. */
class CORE_LIBRARY_DLL_SHARED_API SectionLine : public Line
{
public:
	/*! \brief Default constructor. */
	SectionLine() = default;
	/*! \brief Copy constructor. */
    SectionLine(const SectionLine&) = default;
	/*! \brief Initialising constructor. */
	explicit SectionLine(const std::string& section);
	/*! \brief Virtual destructor. */
	virtual ~SectionLine() = default;
	/*! \brief Copy assignment operator. */
    SectionLine& operator=(const SectionLine&) = default;
#ifdef __USE_EXPLICIT_MOVE__
    /*! \brief Move constructor. */
    SectionLine(SectionLine&& line);
    /*! \brief Move assignment operator. */
    SectionLine& operator=(SectionLine&& line);
#else
    /*! \brief Move constructor. */
    SectionLine(SectionLine&&) = default;
    /*! \brief Move assignment operator. */
    SectionLine& operator=(SectionLine&&) = default;
#endif
	/*!
	 * \brief Get the section.
	 * \return The section.
	 */
	const std::string& Section() const;
	/*!
	 * \brief Virtual Print function
	 * \param[in,out] os - Stream to write to.
	 * \param[in] addLineFeed - (Optional) add a line feed.
	 */
	virtual void Print(std::ostream &os, const bool addLineFeed = true) const;

private:
	/*! \brief The section. */
	std::string m_section{};
};

/*! \brief Class to manage a key line in an INI file. */
class CORE_LIBRARY_DLL_SHARED_API KeyLine : public Line
{
public:
	/*! \brief Default constructor. */
	KeyLine() = default;
	/*! \brief Copy constructor. */
    KeyLine(const KeyLine&) = default;
	/*! \brief Initialising constructor. */
	KeyLine(const std::string& key, const std::string& value);
	/*! \brief Virtual destructor. */
	virtual ~KeyLine() = default;
	/*! \brief Copy assignment operator. */
    KeyLine& operator=(const KeyLine&) = default;
#ifdef __USE_EXPLICIT_MOVE__
    /*! \brief Move constructor. */
    KeyLine(KeyLine&& line);
    /*! \brief Move assignment operator. */
    KeyLine& operator=(KeyLine&& line);
#else
    /*! \brief Move constructor. */
    KeyLine(KeyLine&&) = default;
    /*! \brief Move assignment operator. */
    KeyLine& operator=(KeyLine&&) = default;
#endif
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
	 * \param[in,out] os - Stream to write to.
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
typedef std::list< std::shared_ptr<Line> > line_list;
/*! \brief Line list iterator typedef. */
typedef line_list::iterator line_iter;
/*! \brief Line list const iterator typedef. */
typedef line_list::const_iterator line_citer;

} // namespace if_private
} // namespace ini_file
} // namespace core_lib


#endif //INIFILELINES
