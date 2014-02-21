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
 * \file ThreadGroup.hpp
 * \brief File containing thread group declaration.
 */

#ifndef THREADGROUP_HPP
#define THREADGROUP_HPP

#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include "CustomException.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {

/*!
 * \brief Thread group error.
 *
 * This exception class is intended to be thrown by functions in ThreadGroup
 * class.
 */
class xThreadGroupError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xThreadGroupError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xThreadGroupError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xThreadGroupError();
};

/*!
 * \brief Thread group class.
 *
 * This class implements a thread group similar to boost's
 * implementation but using std::thread instead of boost::thread.
 */
class ThreadGroup final
{
public:
    /*! \brief Default constructor. */
    ThreadGroup();
    /*! \brief Destructor. */
    ~ThreadGroup();
    /*! \brief Copy constructor deleted.*/
    ThreadGroup(const ThreadGroup& threadGroup) = delete;
    /*! \brief Copy assignment operator deleted.*/
    ThreadGroup& operator= (const ThreadGroup& threadGroup) = delete;
    /*!
     * \brief Is current thread in group.
     * \return True if in group, false otherwise.
     */
    bool IsThisThreadIn() const;
    /*!
     * \brief Is given thread in group.
     * \param [IN] Pointer to thread.
     * \return True if in group, false otherwise.
     */
    bool IsThreadIn(std::thread* threadPtr) const;
    /*!
     * \brief Is given thread in group.
     * \param [IN] Thread ID.
     * \return True if in group, false otherwise.
     */
    bool IsThreadIn(const std::thread::id& id) const;
    /*!
     * \brief Create and add thread to group.
     * \param [IN] Thread function to use with created thread.
     * \return Pointer to the created thread.
     */
    template<typename F>
    std::thread* CreateThread(F threadfunction)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::unique_ptr<std::thread> newThread(new std::thread(threadfunction));
        m_threadGroup.push_back(newThread.get());
        return newThread.release();
    }
    /*!
     * \brief Add thread to group.
     * \param [IN] Pointer to thread.
     *
     * Throws xThreadGroupError if called with a thread that
     * already belongs to the thread group.
     */
    void AddThread(std::thread* threadPtr);
    /*!
     * \brief Remove thread from group.
     * \param [IN] Pointer to thread.
     */
    void RemoveThread(std::thread* threadPtr);
    /*!
     * \brief Remove thread from group.
     * \param [IN] Thread ID.
     * \return Pointer to thread.
     *
     * This function returns nullptr if the id cannot be found.
     * Also this function should only be called if the thread
     * for this ID has not been joined else the ID will be invalid.
     */
    std::thread* RemoveThread(const std::thread::id& id);
    /*! \brief Call join on all registered threads.
     *
     * Throws xThreadGroupError if called from one of the
     * threads that are i nthe thread group becuase a thread
     * cannot join itself.
     */
    void JoinAll();
    /*! \brief Get the number of threads registered.
     *  \return Number of threads.
     */
    size_t Size() const;
    /*! \brief Check if we have any threads registered.
     *  \return True if no threads registered, false otherwise.
     */
    bool Empty() const;

private:
    /*! \brief Access mutex for private data. */
    mutable std::mutex m_mutex;
    // Typedefs for thread list.
    typedef std::list<std::thread*> thread_list;
    typedef thread_list::iterator thread_list_iter;
    /*! \brief List containing threads. */
    thread_list m_threadGroup;
     /*!
     * \brief Delete thread object.
     * \param [IN] Pointer to thread.
     */
    static void DeleteThread(std::thread* threadPtr);
    /*!
     * \brief Call join on a thread object.
     * \param [IN] Pointer to thread.
     */
    static void JoinThread(std::thread* threadPtr);
};

} // namespace threads
} // namespace core_lib

#endif // THREADGROUP_HPP
