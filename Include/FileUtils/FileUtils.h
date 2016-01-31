// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2016 Duncan Crutchley
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
 * \file FileUtils.h
 * \brief File containing declarations relating various file utilities.
 */

#ifndef FILEUTILS
#define FILEUTILS

#include <string>
#include "Exceptions/CustomException.h"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The file_utils namespace. */
namespace file_utils {

/*!
 * \brief Find the common root path of 2 paths.
 * \param[in] path1 - First path.
 * \param[in] path2 - Second path.
 *
 * Based on an example given in Learning Boost C++ Libraries
 * by Arindam Mukherjee.
 */
std::string CORE_LIBRARY_DLL_SHARED_API FindCommonRootPath(const std::string& path1
                                                           , const std::string& path2);

/*!
 * \brief Copying a directory error exception.
 *
 * This exception class is intended to be thrown by the CopyDirectoryRecursively
 * function to signify that an error has occured.
 */
class CORE_LIBRARY_DLL_SHARED_API xCopyDirectoryError: public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xCopyDirectoryError();
    /*!
     * \brief Initializing constructor.
     * \param[in] message - A user specified message string.
     */
    explicit xCopyDirectoryError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xCopyDirectoryError();
    /*! \brief Copy constructor. */
    xCopyDirectoryError(const xCopyDirectoryError&) = default;
    /*! \brief Copy assignment operator. */
    xCopyDirectoryError& operator=(const xCopyDirectoryError&) = default;
};

/*! \brief Copy directory options enumeration. */
enum class eCopyDirectoryOptions
{
    /*! \brief Do not continue copying if target directory exists. */
    failIfTargetExists,
    /*! \brief Continue copying even if target directory exists. */
    continueIfTargetExists
};

/*!
 * \brief Recursively copy one directory and its contents to another location.
 * \param[in] source - Source path fro mwhere we will copy data.
 * \param[in] target - Target path to where we will place the copied data.
 * \param[in] options - Control how to handle exsiting target locations.
 *
 * Based on an example given in Learning Boost C++ Libraries
 * by Arindam Mukherjee.
 *
 * Preconditions that are enforced by this function are:
 * 1. The source path is a directory.
 * 2. The target path is a directory and may or may not exist.
 * 3. The parent of the target path is a directory.
 * 4. The target path is not a subdirectory of the source path.
 *
 * Throws xCopyDirectoryError is an error occurs. If target folder
 * already exists then the source is copying into a folder within
 * the target folder with the same name as the srouce folder.
 */
void CORE_LIBRARY_DLL_SHARED_API CopyDirectoryRecursively(const std::string& source
                                                          , const std::string& target
                                                          , const eCopyDirectoryOptions options
                                                                = eCopyDirectoryOptions::continueIfTargetExists);

} // namespace file_utils
} // namespace core_lib

#endif // FILEUTILS
