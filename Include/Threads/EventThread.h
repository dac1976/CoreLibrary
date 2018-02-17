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

/*! \brief Class defining an EventThread that ticks at a given rate and executes a registered callback. */
class EventThread final : public core_lib::threads::ThreadBase
{
	/*! \brief Typedef defining message handler functor.  */
	typedef std::function<void()> event_callback_t; 
	
public:
    /*!
     * \brief EventThread constructor.
     * \param[in] evenCallback - Function object to be called when event ticks.
     * \param[in] eventPeriodMillisecs - Period between event being triggered.
     */
    EventThread(event_callback_t const& evenCallback, unsigned int const eventPeriodMillisecs);

    /*! \brief EventThread destructor. */
    virtual ~EventThread();
	
	/*! \brief Copy constructor deleted.*/
    EventThread(const EventThread&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    EventThread& operator=(const EventThread&) = delete;

private:
	/*! \brief Thread iteration function.*/
    virtual void ThreadIteration() NO_EXCEPT_;
	/*! \brief Function to process termination conditions.*/
    virtual void ProcessTerminationConditions() NO_EXCEPT_;

private:
    /*! \brief Update event.*/
    core_lib::threads::SyncEvent m_updateEvent{};
	/*! \brief Callback fires on event.*/
	event_callback_t m_evenCallback{};
	/*! \brief Event tick period.*/
	unsigned int const  m_eventPeriodMillisecs{};
};

} // namespace threads
} // namespace core_lib

#endif // EVENTTHREAD_H
