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
 * \file IniFileLines.cpp
 * \brief File containing definitions relating the IniFile support classes.
 */
 
#include "IniFileLines.hpp"
#include <iostream>
 
/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The ini_file namespace. */
namespace ini_file {
/*! \brief The if_private namespace. */
namespace if_private {
 
// ****************************************************************************
// 'class IniFile' support class definitions.
// ****************************************************************************

void BlankLine::Print(std::ostream &os, bool addLineFeed) const
{
	if (addLineFeed)
	{
		os << std::endl;
	}
}

CommentLine::CommentLine(const std::string& comment)
	: Line(), m_comment{comment}
{
}

const std::string& CommentLine::Comment() const
{
	return m_comment;
}

void CommentLine::Print(std::ostream &os, bool addLineFeed) const
{
	os << ";" << m_comment;

	if (addLineFeed)
	{
		os << std::endl;
	}
}

SectionLine::SectionLine(const std::string& section)
	: Line(), m_section{section}
{
}

const std::string& SectionLine::Section() const
{
	return m_section;
}

void SectionLine::Print(std::ostream &os, bool addLineFeed) const
{
	os << "[" << m_section << "]";

	if (addLineFeed)
	{
		os << std::endl;
	}
}

KeyLine::KeyLine(const std::string& key
						  , const std::string& value)
	: Line(), m_key{key}, m_value{value}
{
}

const std::string& KeyLine::Key() const
{
	return m_key;
}

const std::string& KeyLine::Value() const
{
	return m_value;
}

void KeyLine::Value(const std::string& value)
{
	m_value = value;
}

void KeyLine::Value(std::string&& value)
{
	m_value = value;
}

void KeyLine::Print(std::ostream &os, bool addLineFeed) const
{
	os << m_key << "=" << m_value;

	if (addLineFeed)
	{
		os << std::endl;
	}
}

} // namespace if_private
} // namespace ini_file
} // namespace core_lib
