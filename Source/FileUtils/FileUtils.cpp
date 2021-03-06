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

#include "FileUtils/FileUtils.h"
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/throw_exception.hpp>

namespace bfs = boost::filesystem;

namespace core_lib
{
namespace file_utils
{

// ****************************************************************************
// FindFileRecursively definition
// ****************************************************************************
bool FindFileRecursively(const std::wstring& dirPath, const std::wstring& fileName,
                         std::wstring& pathFound, const bool includeFileName)
{
    const bfs::recursive_directory_iterator end;

    auto findFunc = [&fileName](const bfs::directory_entry& e) {
        return e.path().filename().wstring() == fileName;
    };

    const auto it      = std::find_if(bfs::recursive_directory_iterator(dirPath), end, findFunc);
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
    bfs::path bfsPath1(path1);
    bfs::path bfsPath2(path2);

    // If they are on different drives then return empty string.
    if (bfsPath1.root_directory() != bfsPath2.root_directory())
    {
        return L"";
    }

    // Define a lambda function to help us out.
    auto FindRoot = [](const bfs::path& p1, const bfs::path& p2) {
        auto result = std::mismatch(p1.begin(), p1.end(), p2.begin());

        bfs::path root;

        std::for_each(p2.begin(), result.second, [&root](const bfs::path& p) { root /= p; });

        return bfs::system_complete(root).wstring();
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
                              const eCopyDirectoryOptions options)
{
    if (!bfs::is_directory(source) || (bfs::exists(target) && !bfs::is_directory(target)) ||
        !bfs::is_directory(bfs::absolute(target).parent_path()) ||
        (FindCommonRootPath(source, target) == source))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("precondition(s) violated"));
    }

    bfs::path bfsSource(source);
    bfs::path bfsTarget(target);
    bfs::path effectiveTarget = bfsTarget;

    if (bfs::exists(bfsTarget))
    {
        if (options == eCopyDirectoryOptions::failIfTargetExists)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("target folder already exists"));
        }

        effectiveTarget /= bfsSource.filename();
    }

    if (!bfs::create_directories(effectiveTarget))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("failed to create target directory"));
    }

    bfs::directory_iterator dirItr(bfsSource);
    bfs::directory_iterator endDirItr;

    while (dirItr != endDirItr)
    {
        auto status        = dirItr->symlink_status();
        auto currentTarget = effectiveTarget / dirItr->path().filename();

        if (bfs::regular_file == status.type())
        {
            bfs::copy_file(*dirItr, currentTarget, bfs::copy_option::overwrite_if_exists);
        }
        else if (bfs::symlink_file == status.type())
        {
            bfs::copy_symlink(*dirItr, currentTarget);
        }
        else if (bfs::directory_file == status.type())
        {
            CopyDirectoryRecursively(dirItr->path().wstring(), effectiveTarget.wstring(), options);
        }

        ++dirItr;
    }
}

// ****************************************************************************
// ListDirectoryContents definition
// ****************************************************************************
std::list<std::wstring> ListDirectoryContents(const std::wstring& path,
                                              const std::wstring& extMatch)
{
    std::list<std::wstring> files;

    if (!bfs::exists(path) || !bfs::is_directory(path))
    {
        return files;
    }

    for (const auto& entry : bfs::directory_iterator(path))
    {
        if (bfs::regular_file != entry.status().type())
        {
            continue;
        }

        if (extMatch.empty() || (boost::to_upper_copy(extMatch) ==
                                 boost::to_upper_copy(entry.path().extension().wstring())))
        {
            files.push_back(entry.path().filename().wstring());
        }
    }

    return files;
}

// ****************************************************************************
// ListSubDirectories definition
// ****************************************************************************
std::list<std::wstring> ListSubDirectories(const std::wstring& path)
{
    std::list<std::wstring> files;

    if (!bfs::exists(path) || !bfs::is_directory(path))
    {
        return files;
    }

    for (const auto& entry : bfs::directory_iterator(path))
    {
        if (bfs::directory_file != entry.status().type())
        {
            continue;
        }

        files.push_back(entry.path().filename().wstring());
    }

    return files;
}

} // namespace file_utils
} // namespace core_lib
