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
 * \file IoServiceThreadGroup.cpp
 * \brief File containing definitions relating the IoServiceThreadGroup class.
 */

#include "Asio/IoServiceThreadGroup.h"
#include <algorithm>
#include <functional>

namespace core_lib
{
namespace asio
{

// ****************************************************************************
// 'class IoServiceThreadGroup' definition
// ****************************************************************************
IoServiceThreadGroup::IoServiceThreadGroup(unsigned int numThreads)
    : m_ioWork(m_ioService)
{
    unsigned int numThreadsToUse = std::max(static_cast<unsigned int>(1), numThreads);

    for (unsigned int t = 0; t < numThreadsToUse; ++t)
    {
        m_threadGroup.CreateThread(
            std::bind(static_cast<size_t (boost_ioservice_t::*)()>(&boost_ioservice_t::run),
                      std::ref(m_ioService)));
    }
}

IoServiceThreadGroup::~IoServiceThreadGroup()
{
    m_ioService.stop();
    m_threadGroup.JoinAll();
}

boost_ioservice_t& IoServiceThreadGroup::IoService()
{
    return m_ioService;
}

} // namespace asio
} // namespace core_lib
