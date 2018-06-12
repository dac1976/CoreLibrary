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
 * \file ThreadRunner.h
 * \brief File containing declaration of ThreadRunner class.
 */

#ifndef THREADRUNNER
#define THREADRUNNER

#include <functional>
#include "ThreadBase.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

/*!
 * \brief ThreadRunner class.
 *
 * When writing new code a user is most likely going to derive their own derieved class from
 * ThreadBase but sometimes it may be preferable to have a thread runner object as a member variable
 * of a class that is to have threaded out functionality. In which case ThreadRunner can be used.
 */
class CORE_LIBRARY_DLL_SHARED_API ThreadRunner final : public ThreadBase
{
    /*! \brief Typedef defining functor for virtual thread functions. */
    using thread_function_t = std::function<void()>;

public:
    /*! \brief Default constructor deleted.*/
    ThreadRunner() = delete;
    /*! \brief Copy constructor deleted.*/
    ThreadRunner(const ThreadRunner&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    ThreadRunner& operator=(const ThreadRunner&) = delete;
    /*! \brief Copy constructor deleted.*/
    ThreadRunner(ThreadRunner&&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    ThreadRunner& operator=(ThreadRunner&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] threadFunction - Functor to call in the ThreadFunction method.
     * \param[in] processTerminationConditions - Functor to call in the ProcessTerminationConditions
     * method.
     * \param[in] autoStart - (Optional) Automatically call Start() at end of constructor.
     *
     * The functors should not throw any exceptions.
     */
    ThreadRunner(const thread_function_t& threadFunction,
                 const thread_function_t& processTerminationConditions, bool autoStart = false);
    /*! \brief Destructor.*/
    ~ThreadRunner() override;
    /*!
     * \brief Make this thread sleep for a period of time.
     * \param[in] milliSecs - Time period in milliseconds.
     *
     * This function throws a std::runtime_error exception
     * if thread not fully started and therefore cannot be
     * made to sleep.
     */
    void SleepThreadForTime(unsigned int milliSecs) const;

private:
    /*! \brief Functor to call in the ThreadFunction method. */
    thread_function_t m_threadFunction;
    /*! \brief Functor to call in the ProcessTerminationConditions method. */
    thread_function_t m_processTerminationConditions;

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
    void ThreadIteration() NO_EXCEPT_ override;
    /*!
     * \brief Perform any special termination actions.
     *
     * This function performs no actions in the base class
     * definition but can be overriden in the dervied class
     * to perform any special termination actions that are
     * required after the terminting flag is set but before
     * we call join on our underlying std::thread object.
     */
    void ProcessTerminationConditions() NO_EXCEPT_ override;
};

} // namespace threads
} // namespace core_lib

#endif // THREADRUNNER
