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
 * \file IniFileSectionDetails.cpp
 * \brief File containing definitions relating the IniFile support classes.
 */
 
#include "IniFile/IniFileSectionDetails.h"
 
/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The ini_file namespace. */
namespace ini_file {
/*! \brief The if_private namespace. */
namespace if_private {
 
// ****************************************************************************
// 'class IniFile' support class definitions.
// ****************************************************************************
#ifdef __USE_EXPLICIT_MOVE__
    SectionDetails::SectionDetails(SectionDetails&& section)
    {
        *this = std::move(section);
    }

    SectionDetails& SectionDetails::operator=(SectionDetails&& section)
    {
        std::swap(m_sectIter, section.m_sectIter);
        std::swap(m_keyIters, section.m_keyIters);
        return *this;
    }
#endif

SectionDetails::SectionDetails(const line_iter& sectIter)
	: m_sectIter(sectIter)
{
}

const std::string& SectionDetails::Section() const
{
	return std::dynamic_pointer_cast<SectionLine>(*m_sectIter)->Section();
}

bool SectionDetails::KeyExists(const std::string& key) const
{
	bool found = false;

	for (auto lineIter : m_keyIters)
	{
		std::shared_ptr<KeyLine> keyLine
				= std::dynamic_pointer_cast<KeyLine>(*lineIter);

		if (key == keyLine->Key())
		{
			found = true;
			break;
		}
	}

	return found;
}

void SectionDetails::AddKey(const line_iter& keyIter)
{
	m_keyIters.push_back(keyIter);
}

void SectionDetails::UpdateKey(const std::string& key
										, const std::string& value)
{

	for (auto lineIter : m_keyIters)
	{
		std::shared_ptr<KeyLine> keyLine
				= std::dynamic_pointer_cast<KeyLine>(*lineIter);

		if (key == keyLine->Key())
		{
			keyLine->Value(value);
			break;
		}
	}
}

bool SectionDetails::EraseKey(const std::string& key
									   , line_iter& lineIter)
{
	bool erased = false;

	for (keys_iter keyIter = m_keyIters.begin()
		 ; keyIter != m_keyIters.end()
		 ; ++keyIter)
	{		
		std::shared_ptr<KeyLine> keyLine
				= std::dynamic_pointer_cast<KeyLine>(**keyIter);

		if (keyLine && (key == keyLine->Key()))
		{
			lineIter = *keyIter;
			m_keyIters.erase(keyIter);
			erased = true;
			break;
		}
	}

	return erased;
}

std::string SectionDetails::GetValue(const std::string& key
											  , const std::string& defaultValue) const
{
	std::string value{defaultValue};

	for (auto lineIter : m_keyIters)
	{
		std::shared_ptr<KeyLine> keyLine
				= std::dynamic_pointer_cast<KeyLine>(*lineIter);

		if (keyLine && (key == keyLine->Key()))
		{
			value = keyLine->Value();
			break;
		}
	}

	return value;
}

void SectionDetails::GetKeys(keys_list& keys) const
{
	keys.clear();

	for (auto lineIter : m_keyIters)
	{
		std::shared_ptr<KeyLine> keyLine
				= std::dynamic_pointer_cast<KeyLine>(*lineIter);

		if (keyLine)
		{
			keys.push_back(std::make_pair(keyLine->Key(), keyLine->Value()));
		}
	}
}

line_iter SectionDetails::LineIterator() const
{
	return m_sectIter;
}

} // namespace if_private
} // namespace ini_file
} // namespace core_lib
