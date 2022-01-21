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
 * \file EventThread.h
 * \brief File containing declaration of EventThread class.
 */
#ifndef EVENTTHREAD_H
#define EVENTTHREAD_H

#include <functional>
#include "Threads/ThreadBase.h"
#include "Threads/SyncEvent.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

/*! \brief Class defining an EventThread that ticks at a given rate and executes a registered
 * callback. */
class CORE_LIBRARY_DLL_SHARED_API EventThread final : public core_lib::threads::ThreadBase
{
    /*! \brief Typedef defining message handler functor.  */
    using event_callback_t = std::function<void()>;

public:
    /*!
     * \brief EventThread constructor.
     * \param[in] eventCallback - Function object to be called when event ticks.
     * \param[in] eventPeriod - Period between event being triggered.
     * \param[in] delayedStart - When true user must called Start() manually.
     * \param[in] timeUnit - The unit of time associated with the period.
     */
    EventThread(event_callback_t const& eventCallback, unsigned int eventPeriod, bool delayedStart,
                eWaitTimeUnit timeUnit = eWaitTimeUnit::milliseconds);

    /*! \brief EventThread destructor. */
    ~EventThread() override;

    /*! \brief Copy constructor deleted.*/
    EventThread(const EventThread&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    EventThread& operator=(const EventThread&) = delete;
    /*! \brief Move constructor deleted.*/
    EventThread(EventThread&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    EventThread& operator=(EventThread&&) = delete;

    /*!
     * \brief Set the time period between ticks of the event.
     * \param[in] eventPeriod - Period between event being triggered.
     * \param[in] timeUnit - The unit of time associated with the period.
     */
    void EventPeriod(unsigned int  eventPeriod,
                     eWaitTimeUnit timeUnit = eWaitTimeUnit::milliseconds);
    /*!
     * \brief Set the time period between ticks of the event.
     * \return Period between event being triggered, in milliseconds.
     */
    unsigned int EventPeriod(eWaitTimeUnit* timeUnit = nullptr) const;

    /*! \brief Forces the thread to tick. */
    void ForceTick();

private:
    /*! \brief Thread iteration function.*/
    void ThreadIteration() NO_EXCEPT_ override;
    /*! \brief Function to process termination conditions.*/
    void ProcessTerminationConditions() NO_EXCEPT_ override;

private:
    /*! \brief Update event.*/
    mutable std::mutex m_eventTickMutex;
    /*! \brief Update event.*/
    core_lib::threads::SyncEvent m_updateEvent{};
    /*! \brief Callback fires on event.*/
    event_callback_t m_eventCallback{};
    /*! \brief Event tick period.*/
    unsigned int m_eventPeriod{0};
    /*! \brief Unit of time for tick period.*/
    eWaitTimeUnit m_timeUnit{eWaitTimeUnit::milliseconds};
};

} // namespace threads
} // namespace core_lib

#endif // EVENTTHREAD_H
