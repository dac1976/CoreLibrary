// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
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
#include <list>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The file_utils namespace. */
namespace file_utils
{

/*!
 * \brief Find the common root path of 2 paths.
 * \param[in] dirPath - Directory to search recusively.
 * \param[in] fileName - Name of file to search for.
 * \param[out] pathFound - Full path to the file.
 * \param[in] includeFileName - Flag to indicate if pathFound should include name of file.
 * \return True if found, false otherwise.
 */
bool CORE_LIBRARY_DLL_SHARED_API FindFileRecursively(const std::wstring& dirPath,
                                                     const std::wstring& fileName,
                                                     std::wstring&       pathFound,
                                                     bool                includeFileName = true);

/*!
 * \brief Find the common root path of 2 paths.
 * \param[in] path1 - First path.
 * \param[in] path2 - Second path.
 *
 * Based on an example given in Learning Boost C++ Libraries
 * by Arindam Mukherjee.
 */
std::wstring CORE_LIBRARY_DLL_SHARED_API FindCommonRootPath(const std::wstring& path1,
                                                            const std::wstring& path2);

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
 * \param[in] source - Source path from where we will copy data.
 * \param[in] target - Target path to where we will place the copied data.
 * \param[in] options - Control how to handle exsiting target locations.
 *
 * Based on an example given in Learning Boost C++ Libraries
 * by Arindam Mukherjee.
 *
 * Preconditions that are enforced by this function are:
 * 1. The source path must be a directory.
 * 2. The target path must be a directory and may or may not exist.
 * 3. The parent of the target path must be a directory.
 * 4. The target path must not be a subdirectory of the source path.
 *
 * Throws std::runtime_error if an error occurs. If target folder
 * already exists then the source is copied into a folder within
 * the target folder with the same name as the srouce folder.
 */
void CORE_LIBRARY_DLL_SHARED_API CopyDirectoryRecursively(
    const std::wstring& source, const std::wstring& target,
    eCopyDirectoryOptions options = eCopyDirectoryOptions::continueIfTargetExists);

/*!
 * \brief List of regular files in the specified directory.
 * \param[in] path - Directory path.
 * \param[in] extMatch - (Optional) If specified then should be the file extension to filter by.
 * \return List of files in the directory.
 *
 * Throws boost::filesystem_error if a problem occurs.
 */
std::list<std::wstring> CORE_LIBRARY_DLL_SHARED_API
                        ListDirectoryContents(const std::wstring& path, const std::wstring& extMatch = L"");

/*!
 * \brief List of immediate sub-directories in the specified directory.
 * \param[in] path - Directory path.
 * \return List of sub-directories in the directory.
 *
 * Throws boost::filesystem_error if a problem occurs.
 */
std::list<std::wstring> CORE_LIBRARY_DLL_SHARED_API ListSubDirectories(const std::wstring& path);

} // namespace file_utils
} // namespace core_lib

#endif // FILEUTILS
