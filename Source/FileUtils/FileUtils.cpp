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
 * \file FileUtils.cpp
 * \brief File containing definitions relating various file utilities.
 */

#include "FileUtils.h"
#include <algorithm>
#include <iterator>
#include <regex>
#include <stdexcept>
#if BOOST_OS_WINDOWS
#include <Shlobj.h>
#endif
#include <boost/exception/all.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "FileUtils/SelectFileSystemLibrary.hpp"

#if BOOST_OS_WINDOWS
// Link to version.dll using the lib from the Windows SDK.
#if !defined(__MINGW32__) && !defined(__MINGW64__)
#pragma comment(lib, "Shell32")
#endif
#endif // BOOST_OS_WINDOWS

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The file_utils namespace. */
namespace file_utils
{

// ****************************************************************************
// FindFileRecursively definition
// ****************************************************************************
bool FindFileRecursively(const std::wstring& dirPath, const std::wstring& fileName,
                         std::wstring& pathFound, bool includeFileName)
{
    const filesys::recursive_directory_iterator end;

    auto findFunc = [&fileName](const filesys::directory_entry& e)
    { return e.path().filename().wstring() == fileName; };

    const auto it = std::find_if(filesys::recursive_directory_iterator(dirPath), end, findFunc);
    bool       success = false;

    if (it != end)
    {
        pathFound = includeFileName ? it->path().wstring() : it->path().parent_path().wstring();
        success   = true;
    }

    return success;
}

// ****************************************************************************
// FindCommonRootPath definition
// ****************************************************************************
std::wstring FindCommonRootPath(const std::wstring& path1, const std::wstring& path2)
{
    // Convert our path strings to boost::filesystem::path objects.
    filesys::path bfsPath1(path1);
    filesys::path bfsPath2(path2);

    // If they are on different drives then return empty string.
    if (bfsPath1.root_directory() != bfsPath2.root_directory())
    {
        return L"";
    }

    // Define a lambda function to help us out.
    auto FindRoot = [](const filesys::path& p1, const filesys::path& p2)
    {
        auto result = std::mismatch(p1.begin(), p1.end(), p2.begin());

        filesys::path root;

        std::for_each(p2.begin(), result.second, [&root](const filesys::path& p) { root /= p; });

        return filesys::system_complete(root).wstring();
    };

    // Now compute the common root path .
    auto n1 = std::distance(bfsPath1.begin(), bfsPath1.end());
    auto n2 = std::distance(bfsPath2.begin(), bfsPath2.end());

    return (n1 < n2) ? FindRoot(bfsPath1, bfsPath2) : FindRoot(bfsPath2, bfsPath1);
}

// ****************************************************************************
// CopyDirectoryRecursively definition
// ****************************************************************************
void CopyDirectoryRecursively(const std::wstring& source, const std::wstring& target,
                              eCopyDirectoryOptions options)
{
    if (!filesys::is_directory(source) ||
        (filesys::exists(target) && !filesys::is_directory(target)) ||
        !filesys::is_directory(filesys::absolute(target).parent_path()) ||
        (FindCommonRootPath(source, target) == source))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("precondition(s) violated"));
    }

    filesys::path bfsSource(source);
    filesys::path bfsTarget(target);
    filesys::path effectiveTarget = bfsTarget;

    //----------
    effectiveTarget = filesys::system_complete(effectiveTarget);
    //----------

    if (filesys::exists(bfsTarget))
    {
        if (options == eCopyDirectoryOptions::failIfTargetExists)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("target folder already exists"));
        }

        effectiveTarget /= bfsSource.filename();
    }

    if (!filesys::create_directories(effectiveTarget))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to create target directory"));
    }

    filesys::directory_iterator dirItr(bfsSource);
    filesys::directory_iterator endDirItr;

    while (dirItr != endDirItr)
    {
        auto status        = dirItr->symlink_status();
        auto currentTarget = effectiveTarget / dirItr->path().filename();

        if (filesys::is_regular_file(status))
        {
#if defined(USE_STD_FILESYSTEM)
            filesys::copy_file(*dirItr, currentTarget, filesys::copy_options::overwrite_existing);
#else
#if BOOST_VERSION > 107300
            filesys::copy_file(*dirItr, currentTarget, filesys::copy_options::overwrite_existing);
#else
            filesys::copy_file(*dirItr, currentTarget, filesys::copy_option::overwrite_if_exists);
#endif
#endif
        }
        else if (filesys::is_symlink(status))
        {
            filesys::copy_symlink(*dirItr, currentTarget);
        }
        else if (filesys::is_directory(status))
        {
            CopyDirectoryRecursively(dirItr->path().wstring(), effectiveTarget.wstring(), options);
        }

        ++dirItr;
    }
}

// ****************************************************************************
// MoveDirectoryAndContents definition
// ****************************************************************************
void MoveDirectoryAndContents(const std::wstring& source, const std::wstring& target,
                              eCopyDirectoryOptions options)
{
    if (!filesys::is_directory(source) ||
        (filesys::exists(target) && !filesys::is_directory(target)) ||
        !filesys::is_directory(filesys::absolute(target).parent_path()) ||
        (FindCommonRootPath(source, target) == source))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("precondition(s) violated"));
    }

    filesys::path bfsSource(source);
    filesys::path bfsTarget(target);

    if (filesys::exists(bfsTarget))
    {
        if (options == eCopyDirectoryOptions::failIfTargetExists)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("target folder already exists"));
        }
        else
        {
            filesys::remove_all(bfsTarget);
        }
    }

    filesys::rename(bfsSource, bfsTarget);
}

// ****************************************************************************
// ListDirectoryContents definition
// ****************************************************************************
std::list<std::wstring> ListDirectoryContents(const std::wstring& path,
                                              const std::wstring& extMatch)
{
    std::list<std::wstring> files;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        return files;
    }

    for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
    {
        if (!filesys::is_regular_file(itr->status()))
        {
            continue;
        }

        if ((L"NO-EXT-MATCH" == extMatch) ||
            (boost::to_upper_copy(extMatch) ==
             boost::to_upper_copy(itr->path().extension().wstring())))
        {
            files.push_back(itr->path().filename().wstring());
        }
    }

    return files;
}

std::list<std::string> ListDirectoryContents(const std::string& path, const std::string& extMatch)
{
    std::list<std::string> files;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        return files;
    }

    for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
    {
        if (!filesys::is_regular_file(itr->status()))
        {
            continue;
        }

        if (("NO-EXT-MATCH" == extMatch) ||
            (boost::to_upper_copy(extMatch) ==
             boost::to_upper_copy(itr->path().extension().string())))
        {
            files.push_back(itr->path().filename().string());
        }
    }

    return files;
}

std::list<std::wstring> ListDirectories(std::wstring const& path)
{
    std::list<std::wstring> directories;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        // Directory does not exist
        return directories;
    }
    else
    {
        for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
        {
            if (filesys::is_directory(itr->status()))
            {
                directories.emplace_back(itr->path().filename().wstring());
            }
        }
    }

    return directories;
}

std::list<std::string> ListDirectories(std::string const& path)
{
    std::list<std::string> directories;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        // Directory does not exist
        return directories;
    }
    else
    {
        for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
        {
            if (filesys::is_directory(itr->status()))
            {
                directories.emplace_back(itr->path().filename().string());
            }
        }
    }

    return directories;
}

std::list<std::wstring> ListFilesAndFolders(std::wstring const& path)
{
    std::list<std::wstring> directories;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        // Directory does not exist
        return directories;
    }
    else
    {
        for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
        {
            directories.emplace_back(itr->path().filename().wstring());
        }
    }

    return directories;
}

std::list<std::string> ListFilesAndFolders(std::string const& path)
{
    std::list<std::string> directories;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        // Directory does not exist
        return directories;
    }
    else
    {
        for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
        {
            directories.emplace_back(itr->path().filename().string());
        }
    }

    return directories;
}

std::list<std::wstring> ListDirectorySubfolderPaths(std::wstring const& path)
{
    std::list<std::wstring> directories;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        // Directory does not exist
        return directories;
    }
    else
    {

        for (filesys::recursive_directory_iterator itr(path);
             itr != filesys::recursive_directory_iterator();
             ++itr)
        {
            if (filesys::is_directory(itr->status()))
            {
                directories.emplace_back(itr->path().filename().wstring());
            }
        }
    }

    return directories;
}

std::list<std::string> ListDirectorySubfolderPaths(std::string const& path)
{
    std::list<std::string> directories;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        // Directory does not exist
        return directories;
    }
    else
    {

        for (filesys::recursive_directory_iterator itr(path);
             itr != filesys::recursive_directory_iterator();
             ++itr)
        {
            if (filesys::is_directory(itr->status()))
            {
                directories.emplace_back(itr->path().filename().string());
            }
        }
    }

    return directories;
}

size_t CountDirectoryContents(const std::wstring& path, const std::wstring& extMatch)
{
    size_t count = 0;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        return count;
    }

    for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
    {
        if (!filesys::is_regular_file(itr->status()))
        {
            continue;
        }

        if ((L"NO-EXT-MATCH" == extMatch) ||
            (boost::to_upper_copy(extMatch) ==
             boost::to_upper_copy(itr->path().extension().wstring())))
        {
            ++count;
        }
    }

    return count;
}

size_t CountDirectoryContents(const std::string& path, const std::string& extMatch)
{
    size_t count = 0;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        return count;
    }

    for (filesys::directory_iterator itr(path); itr != filesys::directory_iterator(); ++itr)
    {
        if (!filesys::is_regular_file(itr->status()))
        {
            continue;
        }

        if (("NO-EXT-MATCH" == extMatch) ||
            (boost::to_upper_copy(extMatch) ==
             boost::to_upper_copy(itr->path().extension().string())))
        {
            ++count;
        }
    }

    return count;
}

size_t CountInSubdirectories(const std::wstring& path, const std::wstring& extMatch)
{
    size_t count = 0;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        return count;
    }
    else
    {
        for (filesys::recursive_directory_iterator itr(path);
             itr != filesys::recursive_directory_iterator();
             ++itr)
        {
            if (filesys::is_directory(itr->status()))
            {
                count += CountDirectoryContents(itr->path().wstring(), extMatch);
            }
        }
    }

    return count;
}

size_t CountInSubdirectories(const std::string& path, const std::string& extMatch)
{
    size_t count = 0;

    if (!filesys::exists(path) || !filesys::is_directory(path))
    {
        return count;
    }
    else
    {
        for (filesys::recursive_directory_iterator itr(path);
             itr != filesys::recursive_directory_iterator();
             ++itr)
        {
            if (filesys::is_directory(itr->status()))
            {
                count += CountDirectoryContents(itr->path().string(), extMatch);
            }
        }
    }

    return count;
}

//-----------------------------------------------------------------------------
bool CheckCifsPath(std::string const& cifsPath, std::string& user, std::string& password,
                   std::string& hostOrIp, std::string& truncatedUrl)
{
    // CIFS URLs for use in software should have the format:
    // <user>:<password>@\\\\<ip address or host>\\<folders>
    // also allow: <user>:<password>@//<ip address or host>/<folders>
    // The <user>:<password>@ portion is optional but if omitted we assume
    // we have guest access and that the shared folder's host is browsable..
    user.clear();
    password.clear();
    std::string url    = cifsPath;
    auto        midPos = url.find_first_of(':');
    auto        endPos = url.find_last_of('@');

    if ((midPos != std::string::npos) && (endPos != std::string::npos))
    {
        user     = url.substr(0, midPos);
        password = url.substr(midPos + 1, endPos - midPos - 1);
        url      = url.substr(endPos + 1, std::string::npos);
    }
    else if (((midPos == std::string::npos) && (endPos != std::string::npos)) ||
             ((midPos != std::string::npos) && (endPos == std::string::npos)))
    {
        return false;
    }

    if ((url.substr(0, 2) != "\\\\") && (url.substr(0, 2) != "//"))
    {
        return false;
    }

    // Remember truncated URL.
    truncatedUrl = url;

    // Extract host/IP address from config URL.
    url = url.substr(2, std::string::npos);

    endPos = url.find_first_of('\\');

    if (endPos == std::string::npos)
    {
        endPos = url.find_first_of('/');

        if (endPos == std::string::npos)
        {
            return false;
        }
    }

    hostOrIp = url.substr(0, endPos);

    return true;
}

//-----------------------------------------------------------------------------
bool IsValidCifsPath(std::string const& cifsPath)
{
    auto cifsPathCopy = cifsPath;

    // Remove user name and password element if necessary.
    auto atPos = cifsPathCopy.find('@');

    if (atPos != std::string::npos)
    {
        cifsPathCopy = cifsPathCopy.substr(atPos + 1, std::string::npos);
    }

    std::regex expr("\\/\\/[a-zA-Z0-9\\.\\-_]{1,}(\\/[a-zA-Z0-9\\-_]{1,}){1,}[\\$]{0,1}|"
                    "\\\\\\\\[a-zA-Z0-9\\.\\-_]{1,}(\\\\[a-zA-Z0-9\\-_]{1,}){1,}[\\$]{0,1}");

    return std::regex_match(cifsPathCopy, expr);
}

//-----------------------------------------------------------------------------
bool CifsPathExists(std::string const& cifsPath)
{
    auto cifsPathCopy = cifsPath;

    // Remove user name and password element if necessary.
    auto atPos = cifsPathCopy.find('@');

    if (atPos != std::string::npos)
    {
        cifsPathCopy = cifsPathCopy.substr(atPos + 1, std::string::npos);
    }

    auto path = filesys::system_complete(cifsPathCopy);
    return filesys::exists(path);
}

//-----------------------------------------------------------------------------

#if BOOST_OS_LINUX
void GrantReadWriteAccessToAll(std::string const& filePath)
{
    std::ostringstream ssCmd;
    ssCmd << "chmod a+rw " << filePath;

    auto returnCode = std::system(ssCmd.str().c_str());

    if (0 != returnCode)
    {
        std::ostringstream ssErrMsg;
        ssErrMsg << "Error setting: " << ssCmd.str() << ", return code: " << returnCode;

        BOOST_THROW_EXCEPTION(std::runtime_error(ssErrMsg.str()));
    }
}
#endif

//----------------------------------------------------------------------------
#if BOOST_OS_WINDOWS
std::string GetLocalAppDataPath()
{
    char szFolderPath[MAX_PATH];

    if (!SHGetSpecialFolderPathA(NULL, szFolderPath, CSIDL_LOCAL_APPDATA, FALSE))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to get value of CSIDL_LOCAL_APPDATA"));
    }

    return szFolderPath;
}

std::wstring GetLocalAppDataPathW()
{
    wchar_t wszFolderPath[MAX_PATH];

    if (!SHGetSpecialFolderPathW(NULL, wszFolderPath, CSIDL_LOCAL_APPDATA, FALSE))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to get value of CSIDL_LOCAL_APPDATA"));
    }

    return wszFolderPath;
}
#else
std::string GetLocalAppDataPath()
{
    std::string folderPath;

    const char* xdgDir = std::getenv("XDG_CONFIG_HOME");

    if (xdgDir && xdgDir[0] != '\0')
    {
        folderPath = filesys::system_complete(filesys::path(xdgDir)).make_preferred().string();
    }
    else
    {
        // Fallback to ~/.config
        const char* homeDir = std::getenv("HOME");

        if (homeDir && homeDir[0] != '\0')
        {
            folderPath = filesys::system_complete(filesys::path(homeDir) / ".config")
                             .make_preferred()
                             .string();
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("failed to get value of local app data path"));
        }
    }

    return folderPath;
}

std::wstring GetLocalAppDataPathW()
{
    std::wstring folderPath;

    const char* xdgDir = std::getenv("XDG_CONFIG_HOME");

    if (xdgDir && xdgDir[0] != '\0')
    {
        folderPath = filesys::system_complete(filesys::path(xdgDir)).make_preferred().wstring();
    }
    else
    {
        // Fallback to ~/.config
        const char* homeDir = std::getenv("HOME");

        if (homeDir && homeDir[0] != '\0')
        {
            folderPath = filesys::system_complete(filesys::path(homeDir) / ".config")
                             .make_preferred()
                             .wstring();
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("failed to get value of local app data path"));
        }
    }

    return folderPath;
}
#endif

} // namespace file_utils
} // namespace core_lib
