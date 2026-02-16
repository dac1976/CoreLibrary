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
 * \file IoContextThreadGroup.cpp
 * \brief File containing definitions relating the IoContextThreadGroup class.
 */

#include "Asio/IoContextThreadGroup.h"
#include <algorithm>
#include <functional>

namespace core_lib
{
namespace asio
{

// ****************************************************************************
// 'class IoContextThreadGroup' definition
// ****************************************************************************
IoContextThreadGroup::IoContextThreadGroup(unsigned int numThreads)
    : m_ioWork(asio_compat::make_work_guard(m_ioService))
{
    const unsigned int numThreadsToUse = std::max(static_cast<unsigned int>(1), numThreads);

    for (unsigned int t = 0; t < numThreadsToUse; ++t)
    {
        auto* svc = &m_ioService;

        m_threadGroup.CreateThread([svc]()
        {
            (void)svc->run();
        });
    }
}

IoContextThreadGroup::~IoContextThreadGroup()
{
    Stop();
}

asio_compat::io_context_t& IoContextThreadGroup::IoContext()
{
    return m_ioContext;
}

void IoContextThreadGroup::Stop()
{
#if (BOOST_VERSION >= 106600)
    // Let run() exit cleanly once queued work drains (optional, but correct).
    // Then stop() to force unblock if needed.
    m_ioWork.reset();
#endif

    if (!m_ioService.stopped())
	{
        m_ioService.stop();
	}

    m_threadGroup.JoinAll();
}

} // namespace asio
} // namespace core_lib
