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
 * \file SyncEvent.hpp
 * \brief File containing declaration of SyncEvent class.
 */

#ifndef SYNCEVENT_HPP
#define SYNCEVENT_HPP

#include <mutex>
#include <condition_variable>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {

/*! \brief Enumeration defining reset mechanism for event. */
enum class eResetCondition
{
    manualReset,
    autoReset
};

/*! \brief Enumeration defining notify mechanism for event. */
enum class eNotifyType
{
    signalOneThread,
    signalAllThreads
};

/*! \brief Enumeration defining intial state of event. */
enum class eIntialCondition
{
    notSignalled,
    signalled
};

/*!
 * \brief Class defining a thread synchronisation event.
 *
 * This class implemented a thread syncronisation event
 * that is built using a mutex and a condition variable
 * it makes for a neater package than using these constructs
 * bare.
 */
class SyncEvent final
{
public:
    /*!
     * \brief Default constructor.
     *
     * Create the SyncEvent in auto-reset mode, signal
     * one thread end initially not signalled.
     */
    SyncEvent();
    /*!
     * \brief Intialising constructor.
     * \param [IN] Notify type.
     * \param [IN] Reset condition.
     * \param [IN] Initial condition.
     *
     * Create the SyncEvent setting whether auto-
     * or manual reset is to be used. Also setting
     * whether when signalled it notifies all waiting
     * threads or just one. Can also set the initial
     * condition as signalled or not signalled.
     *
     * If notifyCondition == eNotifyType::signalAllThreads
     * then eResetCondition == eResetCondition::manualReset.
     */
    SyncEvent(eNotifyType notifyCondition
              , eResetCondition resetCondition
              , eIntialCondition initialCondition);
    /*! \brief Destructor. */
    ~SyncEvent();
    /*! \brief Copy constructor - disabled. */
    SyncEvent(const SyncEvent&) = delete;
    /*! \brief Copy assignment operator - disabled. */
    SyncEvent& operator=(const SyncEvent&) = delete;
    /*!
     * \brief Wait for event.
     *
     * Blocking function that waits until underlying condition
     * variable is signalled at which point this function returns.
     */
    void Wait();
    /*!
     * \brief Wait for event for a period of time.
     * \param [IN] Number of milliseconds to wait.
     * \return true if signalled, false if timed out.
     *
     * Blocking function that waits until underlying condition
     * variable is signalled at which point this function returns
     * or if not signalled this function returns after a defined
     * number of milliseconds.
     */
    bool WaitForTime(size_t milliseconds);
    /*!
     * \brief Signal event.
     *
     * Call this function to signal the underlying condition variable.
     * If a thread is blocked on a call to Wait or WaitForTime then
     * the waiting function will unblock and return.
     */
    void Signal();
    /*!
     * \brief Reset event.reset_condition
     *
     * Use this function when SYncEvent created in maunal reset mode.
     * Call to reset the signalled state of the event. This should
     * not be called while athread is blocked on a call to Wait or
     * WaitForTime. This should be called after the event has been
     * signalled and Wait or WaitFor Time has returned and before
     * calling Wait or WaitForTime again.
     */
    void Reset();

private:
    /*! \brief Mutex to lock access to members. */
    mutable std::mutex m_signalMutex;
    /*! \brief Condition vairable to perform the waiting and signalling. */
    std::condition_variable m_signalCondVar;
    /*! \brief Signal type flag. */
    const bool m_signalAllThreads;
    /*! \brief Auto-reset flag. */
    const bool m_autoReset;
    /*! \brief Signal flag. */
    bool m_signalFlag;
};

} // namespace threads
} // namespace core_lib

#endif //SYNCEVENT_HPP
