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

#ifndef SELECTFILESYSTEMLIBRARY_HPP
#define SELECTFILESYSTEMLIBRARY_HPP

//-----------------------------------------------------------------------------
// USING STD::FILESYSTEM
#if defined(USE_STD_FILESYSTEM)

#include <filesystem>

namespace std 
{
namespace filesystem
{

// These functions exist in boost::filesystem but not in std::filesystem.
// We'll map to the closest equivalent function from std::filesystem.

inline path system_complete(const path& p)
{
    return absolute(p);
}

inline path system_complete(const path& p, error_code& ec)
{
    return absolute(p, ec);
}

inline path initial_path()
{
    return current_path();
}

inline path initial_path(error_code& ec)
{
    return current_path(ec);
}

} // filesystem
} // namespace std

namespace filesys = std::filesystem;

//-----------------------------------------------------------------------------
// USING BOOST::FILESYSTEM
#else

#include <boost/filesystem.hpp> 

namespace filesys = boost::filesystem;

#endif // if defined(USE_STD_FILESYSTEM)

#endif // SELECTFILESYSTEMLIBRARY_HPP
