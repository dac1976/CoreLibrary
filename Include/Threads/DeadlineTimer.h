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
 * \file DeadlineTimer.h
 * \brief File containing declaration of DeadlineTimer class.
 */
 
#ifndef DEADLINETIMER_H
#define DEADLINETIMER_H

#include <cstdint>
#include <functional>
#include <thread>
#include "SyncEvent.h"
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"

namespace hgl
{

class DeadlineTimer FINAL_
{
    using callback_t = std::function<void()>;

public:
    DeadlineTimer();
    ~DeadlineTimer();

    void Start(uint32_t timeoutMillisecs, callback_t const& onTimedOut);
    void Cancel();

private:
    void TimeoutThread();

private:
    uint32_t       m_timeoutMillisecs;
    callback_t     m_onTimedOut;
    hgl::SyncEvent m_timeoutEvent;
    std::thread    m_timeoutThread;
};

} // namespace hgl

#endif // DEADLINETIMER_H
