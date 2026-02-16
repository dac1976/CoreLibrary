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
//-----------------------------------------------------------------------------
// This code was originally part of the above referenced open source library
// and the author has given HGL Dynamics permission to make minor modifications
// to the code for use in their software.
//
// The author has also agreed to new licensing terms as described below.
//
// An exemption to the LGPL license has been given by the author, Duncan
// Crutchley, so that HGL Dynamics Ltd can freely use this code within their
// commercial software.
//-----------------------------------------------------------------------------

/*!
 * \file FileUtils.h
 * \brief File containing declarations relating various file utilities.
 */

#ifndef FILEUTILS
#define FILEUTILS

#include <string>
#include <list>
#include <set>
#include <boost/predef.h>
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
bool CORE_LIBRARY_DLL_SHARED_API FindFileRecursively(const std::wstring& dirPath, const std::wstring& fileName,
                         std::wstring& pathFound, bool includeFileName = true);

/*!
 * \brief Find the common root path of 2 paths.
 * \param[in] path1 - First path.
 * \param[in] path2 - Second path.
 *
 * Based on an example given in Learning Boost C++ Libraries
 * by Arindam Mukherjee.
 */
std::wstring CORE_LIBRARY_DLL_SHARED_API FindCommonRootPath(const std::wstring& path1, const std::wstring& path2);

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
 * \param[in] options - Control how to handle existing target locations.
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
 * the target folder with the same name as the source folder.
 */
void CORE_LIBRARY_DLL_SHARED_API CopyDirectoryRecursively(
    const std::wstring& source, const std::wstring& target,
    eCopyDirectoryOptions options = eCopyDirectoryOptions::continueIfTargetExists);

/*!
 * \brief Move (by renaming) one directory and its contents to another location.
 * \param[in] source - Source path from where we will copy data.
 * \param[in] target - Target path to where we will place the copied data.
 * \param[in] options - Control how to handle existing target locations.
 *
 * Preconditions that are enforced by this function are:
 * 1. The source path must be a directory.
 * 2. The target path must be a directory and may or may not exist.
 * 3. The parent of the target path must be a directory.
 * 4. The target path must not be a subdirectory of the source path.
 * 5. The source and target must be on the same drive.
 *
 * Throws std::runtime_error if an error occurs. If target folder
 * already exists then the source is copied into a folder within
 * the target folder with the same name as the source folder.
 */
void CORE_LIBRARY_DLL_SHARED_API MoveDirectoryAndContents(
    const std::wstring& source, const std::wstring& target,
    eCopyDirectoryOptions options = eCopyDirectoryOptions::continueIfTargetExists);

/*!
 * \brief List of regular files in the specified directory.
 * \param[in] path - Directory path.
 * \param[in] extMatch - (Optional) If specified then should be the file extension to filter by.
 * \return List of files in the directory.
 */
std::list<std::wstring> CORE_LIBRARY_DLL_SHARED_API ListDirectoryContents(const std::wstring& path,
                                              const std::wstring& extMatch = L"NO-EXT-MATCH");
std::list<std::string>  CORE_LIBRARY_DLL_SHARED_API ListDirectoryContents(const std::string& path,
                                              const std::string& extMatch = "NO-EXT-MATCH");

/*!
 * \brief List of subdirectories in the specified directory.
 * \param[in] path - Directory path.
 * \return List of subdrectories.
 */
std::list<std::wstring> CORE_LIBRARY_DLL_SHARED_API ListDirectories(std::wstring const& path);
std::list<std::string>  CORE_LIBRARY_DLL_SHARED_API ListDirectories(std::string const& path);

std::list<std::wstring> CORE_LIBRARY_DLL_SHARED_API ListDirectorySubfolderPaths(std::wstring const& path);
std::list<std::string>  CORE_LIBRARY_DLL_SHARED_API ListDirectorySubfolderPaths(std::string const& path);

std::list<std::wstring> CORE_LIBRARY_DLL_SHARED_API ListFilesAndFolders(std::wstring const& path);
std::list<std::string>  CORE_LIBRARY_DLL_SHARED_API ListFilesAndFolders(std::string const& path);

/*!
 * \brief Count regular files in the specified directory.
 * \param[in] path - Directory path.
 * \param[in] extMatch - (Optional) If specified then should be the file extension to filter by.
 * \return Count of files in the directory.
 */
size_t CORE_LIBRARY_DLL_SHARED_API CountDirectoryContents(const std::wstring& path,
                              const std::wstring& extMatch = L"NO-EXT-MATCH");
size_t CORE_LIBRARY_DLL_SHARED_API CountDirectoryContents(const std::string& path,
                              const std::string& extMatch = "NO-EXT-MATCH");

/*!
 * \brief Count files of given extension in subdirectories in the specified directory.
 * \param[in] path - Directory path.
 * \return Count of files.
 */
size_t CORE_LIBRARY_DLL_SHARED_API CountInSubdirectories(const std::wstring& path,
                             const std::wstring& extMatch = L"NO-EXT-MATCH");
size_t CORE_LIBRARY_DLL_SHARED_API CountInSubdirectories(const std::string& path, const std::string& extMatch = "NO-EXT-MATCH");

/*!
 * \brief Check a CIFS Path/URL for correctness.
 * \param[in] cifsPath - CIFS URL of the form:
 *                       "<user>:<password>@\\\\<ip address or host>\\<folders>"
 *                       or "<user>:<password>@//<ip address or host>/<folders>".
 * \param[out] user - if contained in CIFS path the user name.
 * \param[out] password - if contained in CIFS path the password.
 * \param[out] hostOrIp - the host or IP address portion of the CIFS URL.
 * \param[out] truncatedUrl - the truncated URL with user name and password removed.
 * \return True if URL seems valid, false otherwise.
 */
bool CORE_LIBRARY_DLL_SHARED_API CheckCifsPath(std::string const& cifsPath, std::string& user, std::string& password,
                   std::string& hostOrIp, std::string& truncatedUrl);

/*!
 * \brief Check a CIFS Path/URL for correctness.
 * \param[in] cifsPath - CIFS URL of the form:
 *                       "<user>:<password>@\\\\<ip address or host>\\<folders>"
 *                       or "<user>:<password>@//<ip address or host>/<folders>".
 * \return True if URL seems valid, false otherwise.
 */
bool CORE_LIBRARY_DLL_SHARED_API IsValidCifsPath(std::string const& cifsPath);

/*!
 * \brief Check a CIFS Path/URL exists and is accessible.
 * \param[in] cifsPath - CIFS URL of the form:
 *                       "\\\\<ip address or host>\\<folders>"
 *                       or "//<ip address or host>/<folders>".
 * \return True if URL seems valid, false otherwise.
 *
 * NOTE: This only works when used from a PC that has access to the share path
 *        without needing the a user name or password to access it.
 */
bool CORE_LIBRARY_DLL_SHARED_API CifsPathExists(std::string const& cifsPath);

#if BOOST_OS_LINUX
/*!
 * \brief Make a file fully accessible for read and write to all.
 * \param[in] filePath - path to a file
 *
 * Throws if fails.
 */
void CORE_LIBRARY_DLL_SHARED_API GrantReadWriteAccessToAll(std::string const& filePath);
#endif

/*!
 * \brief Get the local app data path from Window API.
 * \return the path to local app data for current user account
 */
std::string  CORE_LIBRARY_DLL_SHARED_API GetLocalAppDataPath();
std::wstring CORE_LIBRARY_DLL_SHARED_API GetLocalAppDataPathW();

} // namespace file_utils
} // namespace core_lib

#endif // FILEUTILS
