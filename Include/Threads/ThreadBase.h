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
 * \file ThreadBase.h
 * \brief File containing declaration of ThreadBase class.
 */

#ifndef THREADBASE
#define THREADBASE

#include <thread>
#include <mutex>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

/*!
 * \brief Thread base class.
 *
 * This abstract class can be used as a base class for
 * objects that need to be threaded. It neatly wraps all
 * the useful functionality of std::thread in a usable way.
 */
class CORE_LIBRARY_DLL_SHARED_API ThreadBase
{
public:
    /*! \brief Copy constructor deleted.*/
    ThreadBase(const ThreadBase&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    ThreadBase& operator=(const ThreadBase&) = delete;
    /*! \brief Move constructor deleted.*/
    ThreadBase(ThreadBase&&) = default;
    /*! \brief Move assignment operator deleted.*/
    ThreadBase& operator=(ThreadBase&&) = default;
    /*! \brief Destructor.*/
    virtual ~ThreadBase() = default;
    /*!
     * \brief Is thread started.
     * \return Returns true if started, false otherwise.
     */
    bool IsStarted() const;
    /*!
     * \brief Start the thread.
     * \return True if started, false otherwise.
     *
     * Typically called at the end of the derived classes
     * constructor.
     */
    bool Start();
    /*!
     * \brief Safely stop the thread.
     * \return True if started, false otherwise.
     *
     * Typically called at the start of the derived classes
     * destructor.
     */
    bool Stop();
    /*!
     * \brief Get this thread's thread ID.
     * \return Returns thread ID.
     *
     * This function throws a std::runtime_error exception
     * if thread not fully started and so has not got a valid ID.
     */
    std::thread::id ThreadID() const;
    /*!
     * \brief Get the underlying std::thread's native handle.
     * \return Returns native thread handle if supported.
     *
     * This function throws a std::runtime_error exception
     * if thread not fully started and so has not got a handle
     * assigned.
     */
    std::thread::native_handle_type NativeHandle() const;

protected:
    /*! \brief Default constructor.*/
    ThreadBase() = default;
    /*!
     * \brief Set terminating flag.
     * \param[in] terminating - True if terminating, false otherwise.
     */
    void SetTerminating(bool terminating = true);
    /*!
     * \brief Is thread terminating.
     * \return Returns true if terminating, false otherwise.
     */
    bool IsTerminating() const;
    /*!
     * \brief Make this thread sleep for a period of time.
     * \param[in] milliSecs - Time period in milliseconds.
     *
     * This function throws a std::runtime_error exception
     * if thread not fully started and therefore cannot be
     * made to sleep.
     */
    void SleepForTime(unsigned int milliSecs) const;
    /*!
     * \brief Execute a single iteration of the thread.
     *
     * This function is purely virtual and must be defined
     * in the derived class. ThreadBase::Run continually
     * loops until the thread is stopped or destructed so
     * this function will be called each time the Run function
     * loops round. Hence this function can be thought of
     * what needs to be run in a single iteratation of the
     * threads run loop.
     */
    virtual void ThreadIteration() NO_EXCEPT_ = 0;
    /*!
     * \brief Perform any special termination actions.
     *
     * This function performs no actions in the base class
     * definition but can be overriden in the dervied class
     * to perform any special termination actions that are
     * required after the terminting flag is set but before
     * we call join on our underlying std::thread object.
     */
    virtual void ProcessTerminationConditions() NO_EXCEPT_;

private:
    /*!
     * \brief Store thread ID and native handle.
     * \param[in] threadId - Thread ID.
     * \param[in] nativeHandle - Native handle.
     */
    void SetThreadIdAndNativeHandle(const std::thread::id&                 threadId,
                                    const std::thread::native_handle_type& nativeHandle);
    /*!
     * \brief Set started flag.
     * \param[in] started - True if started, false otherwise.
     */
    void SetStarted(bool started = true);
    /*!
     * \brief Run the thread's iterations in a loop.
     *
     * This function loops calling ThreadIteration() to
     * perform a single iterations actions. It stops looping
     * when the thread is termainted.
     */
    void Run();

private:
    /*! \brief Access mutex to protect private data.*/
    mutable std::mutex m_mutex;
    /*! \brief Boolean flag to mark thread as started.*/
    bool m_started{false};
    /*! \brief Boolean flag to mark thread as terminating.*/
    bool m_terminating{false};
    /*! \brief Thread ID of started thread object.*/
    std::thread::id m_threadId{};
    /*! \brief Native thread handle (where supported) of started thread.*/
    std::thread::native_handle_type m_nativeHandle{};
    /*! \brief Underlying std::thread object.*/
    std::thread m_thread{};
};

} // namespace threads
} // namespace core_lib

#endif // THREADBASE
