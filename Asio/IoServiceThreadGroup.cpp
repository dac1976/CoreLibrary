// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
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
 * \file IoServiceThreadGroup.cpp
 * \brief File containing definitions relating the IoServiceThreadGroup class.
 */

#include "IoServiceThreadGroup.hpp"
#include <algorithm>
#include <functional>

namespace core_lib {
namespace asio {

IoServiceThreadGroup::IoServiceThreadGroup(boost::asio::io_service& ioService,
                                           unsigned int numThreads)
    : m_ioService(ioService), m_ioWork(ioService)
{
    numThreads = std::max(1U, numThreads);

    for (unsigned int t = 0; t < numThreads; ++t)
    {
        m_threadGroup.CreateThread(std::bind(&IoServiceThreadGroup::RunIoService, this));
    }
}

IoServiceThreadGroup::~IoServiceThreadGroup()
{
    m_ioService.stop();
    m_threadGroup.JoinAll();
}

void IoServiceThreadGroup::RunIoService()
{
    m_ioService.run();
}

} //namespace asio
} //namespace core_lib
