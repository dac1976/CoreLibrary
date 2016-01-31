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
 * \file FileUtils.cpp
 * \brief File containing definitions relating various file utilities.
 */

#include "FileUtils/FileUtils.h"
#include <algorithm>
#include <iterator>
#include "boost/filesystem.hpp"

namespace bfs = boost::filesystem;

namespace core_lib {
namespace file_utils {

// ****************************************************************************
// FindCommonRootPath definition
// ****************************************************************************
std::string FindCommonRootPath(const std::string& path1
                               , const std::string& path2)
{
    // Convert our patsh strings to boost::filesystem::path objects.
    bfs::path bfsPath1(path1);
    bfs::path bfsPath2(path2);

    // Define a lambda function to help us out.
    auto FindRoot =
        [](const bfs::path& p1, const bfs::path& p2)
        {
            auto result =
                std::mismatch(p1.begin(), p2.end(), p2.begin());

            bfs::path root;

            std::for_each(p2.begin(), result.second
              , [&root](const bfs::path& p)
                {
                    root /= p;
                });

            return root;
        };

    // Now compute the common root path .
    const size_t n1 = std::distance(bfsPath1.begin(), bfsPath2.end());
    const size_t n2 = std::distance(bfsPath2.begin(), bfsPath2.end());

    return (n1 < n2)
           ? FindRoot(bfsPath1, bfsPath2).string()
           : FindRoot(bfsPath2, bfsPath1).string();
}

// ****************************************************************************
// 'class xCopyDirectoryError' definition
// ****************************************************************************
xCopyDirectoryError::xCopyDirectoryError()
    : exceptions::xCustomException("error copying directory")
{
}

xCopyDirectoryError::xCopyDirectoryError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xCopyDirectoryError::~xCopyDirectoryError()
{
}

// ****************************************************************************
// CopyDirectoryRecursively definition
// ****************************************************************************
void CopyDirectoryRecursively(const std::string& source
                              , const std::string& target
                              , const eCopyDirectoryOptions options)
{
    if (!bfs::is_directory(source)
        || (bfs::exists(target) && !bfs::is_directory(target))
        || !bfs::is_directory(bfs::absolute(target).parent_path())
        || (FindCommonRootPath(source, target) == source))
    {
        BOOST_THROW_EXCEPTION(xCopyDirectoryError("precondition(s) violated"));
    }

    bfs::path bfsSource(source);
    bfs::path bfsTarget(target);
    bfs::path effectiveTarget = bfsTarget;

    if (bfs::exists(bfsTarget))
    {
        if (options == eCopyDirectoryOptions::failIfTargetExists)
        {
            BOOST_THROW_EXCEPTION(xCopyDirectoryError("target folder exists"));
        }

        effectiveTarget /= bfsSource.filename();
    }

    if (!bfs::create_directories(effectiveTarget))
    {
        BOOST_THROW_EXCEPTION(xCopyDirectoryError("failed to create target directory"));
    }

    bfs::directory_iterator dirItr(bfsSource);
    bfs::directory_iterator endDirItr;

    while(dirItr != endDirItr)
    {
        auto status = dirItr->symlink_status();
        auto currentTarget
            = effectiveTarget / dirItr->path().filename();

        if (bfs::regular_file == status.type())
        {
            bfs::copy_file(*dirItr
                           , currentTarget
                           , bfs::copy_option::overwrite_if_exists);
        }
        else if (bfs::symlink_file == status.type())
        {
            bfs::copy_symlink(*dirItr, currentTarget);
        }
        else if (bfs::directory_file == status.type())
        {
            CopyDirectoryRecursively(dirItr->path().string()
                                     , effectiveTarget.string()
                                     , options);
        }

        ++dirItr;
    }
}

} // namespace file_utils
} // namespace core_lib