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
 * \file IniFileSectionDetails.h
 * \brief File containing declarations relating the IniFile support classes.
 */

#include "Platform/PlatformDefines.h"
#include "IniFileLines.h"

#ifndef INIFILESECTIONDETAILS
#define INIFILESECTIONDETAILS

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The ini_file namespace. */
namespace ini_file {
/*! \brief The if_private namespace. */
namespace if_private {

/*! \brief Class to section details. */
class CORE_LIBRARY_DLL_SHARED_API SectionDetails
{
public:
	/*! \brief Default constructor. */
	SectionDetails() = default;
	/*! \brief Copy constructor. */
	SectionDetails(const SectionDetails&) = default;
	/*! \brief Initialising constructor. */
	explicit SectionDetails(const line_iter& sectIter);
    /*! \brief Destructor. */
	~SectionDetails() = default;
	/*! \brief Copy assignment operator. */
	SectionDetails& operator=(const SectionDetails&) = default;
#ifdef USE_EXPLICIT_MOVE_
    /*! \brief Move constructor. */
    SectionDetails(SectionDetails&& section);
    /*! \brief Move assignment operator. */
    SectionDetails& operator=(SectionDetails&& section);
#else
    /*! \brief Move constructor. */
    SectionDetails(SectionDetails&&) = default;
    /*! \brief Move assignment operator. */
    SectionDetails& operator=(SectionDetails&&) = default;
#endif
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
				  , line_iter& lineIter);
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
	 * \param[out] keys - The list of keys.
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


} // namespace if_private
} // namespace ini_file
} // namespace core_lib


#endif //INIFILESECTIONDETAILS
