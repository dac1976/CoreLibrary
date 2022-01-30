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
 * \file SyncEvent.h
 * \brief File containing declaration of SyncEvent class.
 */

#ifndef SYNCEVENT
#define SYNCEVENT

#include "CoreLibraryDllGlobal.h"
#include <mutex>
#include <functional>
#include <condition_variable>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

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

/*! \brief Enumeration defining units of time for the wait period. */
enum class eWaitTimeUnit
{
    seconds,
    milliseconds,
    microseconds,
    nanoseconds
};


/*!
 * \brief Class defining a thread synchronisation event.
 *
 * This class implemented a thread syncronisation event
 * that is built using a mutex and a condition variable
 * it makes for a neater implementation than using these
 * types of object as is.
 */
class CORE_LIBRARY_DLL_SHARED_API SyncEvent final
{
    using get_condition_t = std::function<bool()>;
    using set_condition_t = std::function<void(bool)>;

public:
    struct Condition
    {
        get_condition_t getCondition;
        set_condition_t setCondition;
    };
    /*!
     * \brief Initialising constructor.
     * \param[in] notifyCondition - Notify type.
     * \param[in] resetCondition - Reset condition.
     * \param[in] initialCondition - Initial condition.
     * \param[in] condition - (Optional) Pointer to a struct
     *                        with a setter and getter for
     *                        an external condition variable
     *                        state. If set to nullptr an
     *                        internal bool is used.
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
    explicit SyncEvent(eNotifyType      notifyCondition  = eNotifyType::signalOneThread,
                       eResetCondition  resetCondition   = eResetCondition::autoReset,
                       eIntialCondition initialCondition = eIntialCondition::notSignalled,
                       Condition*       condition        = nullptr);
    /*! \brief Destructor. */
    ~SyncEvent() = default;
    /*! \brief Copy constructor - disabled. */
    SyncEvent(const SyncEvent&) = delete;
    /*! \brief Copy assignment operator - disabled. */
    SyncEvent& operator=(const SyncEvent&) = delete;
    /*! \brief Move constructor - disabled. */
    SyncEvent(SyncEvent&&) = delete;
    /*! \brief Move assignment operator - disabled. */
    SyncEvent& operator=(SyncEvent&&) = delete;
    /*!
     * \brief Wait for event.
     *
     * Blocking function that waits until underlying condition
     * variable is signalled at which point this function returns.
     *
     * If an external condition argument was specified in the
     * constructor then the getter for that condition is used
     * else we use the internally tracked condition.
     */
    void Wait();
    /*!
     * \brief Wait for event for a period of time.
     * \param[in] period - Number of time units to wait.
     * \param[in] timeUnit - The unit of time associated with the period.
     * \return true if signalled, false if timed out.
     *
     * Blocking function that waits until underlying condition
     * variable is signalled at which point this function returns
     * or if not signalled this function returns after a defined
     * number of milliseconds.
     *
     * If an external condition argument was specified in the
     * constructor then the getter for that condition is used
     * else we use the internally tracked condition.
     */
    bool WaitForTime(unsigned int period, eWaitTimeUnit timeUnit = eWaitTimeUnit::milliseconds);
    /*!
     * \brief Signal event.
     *
     * Call this function to signal the underlying condition variable.
     * If a thread is blocked on a call to Wait or WaitForTime then
     * the waiting function will unblock and return.
     *
     * If an external condition argument was specified in the
     * constructor then the setter for that condition is used
     * else we use the internally tracked condition.
     */
    void Signal();
    /*!
     * \brief Reset event.reset_condition
     *
     * Use this function when SyncEvent created in manual reset mode.
     * Call to reset the signalled state of the event. This should
     * not be called while a thread is blocked on a call to Wait or
     * WaitForTime. This should be called after the event has been
     * signalled and Wait or WaitFor Time has returned and before
     * calling Wait or WaitForTime again.
     *
     * If an external condition argument was specified in the
     * constructor then the setter for that condition is used
     * else we use the internally tracked condition.
     */
    void Reset();

private:
    bool SignalFlag() const;
    void SetSignalFlag(bool signalFlag);

private:
    mutable std::mutex      m_signalMutex;
    std::condition_variable m_signalCondVar;
    bool                    m_signalAllThreads{false};
    bool                    m_autoReset{true};
    bool                    m_signalFlag{false};
    get_condition_t         m_getCondition;
    set_condition_t         m_setCondition;
};

} // namespace threads
} // namespace core_lib

#endif // SYNCEVENT
