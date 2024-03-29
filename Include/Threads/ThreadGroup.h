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
 * \file ThreadGroup.h
 * \brief File containing thread group declaration.
 */

#ifndef THREADGROUP
#define THREADGROUP

#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include "CoreLibraryDllGlobal.h"
#include "Platform/PlatformDefines.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

/*!
 * \brief Thread group class.
 *
 * This class implements a thread group similar to boost's example
 * implementation but using std::thread instead of boost::thread.
 *
 * It is the responsibility of the caller of ~ThreadGroup() to make
 * sure that all threads held in the thread group have been joined.
 * This can be done by calling ThreadGroup::JoinAll().
 */
class CORE_LIBRARY_DLL_SHARED_API ThreadGroup final
{
public:
    /*! \brief Default constructor. */
    ThreadGroup() = default;
    /*! \brief Destructor. */
    ~ThreadGroup();
    /*! \brief Copy constructor deleted.*/
    ThreadGroup(const ThreadGroup&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    ThreadGroup& operator=(const ThreadGroup&) = delete;
    /*! \brief Move constructor deleted.*/
    ThreadGroup(ThreadGroup&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    ThreadGroup& operator=(ThreadGroup&&) = delete;
    /*!
     * \brief Is current thread in group.
     * \return True if in group, false otherwise.
     */
    bool IsThisThreadIn() const;
    /*!
     * \brief Is given thread in group.
     * \param[in] threadPtr - Pointer to thread.
     * \return True if in group, false otherwise.
     */
    bool IsThreadIn(std::thread* threadPtr) const;
    /*!
     * \brief Is given thread in group.
     * \param[in] id - Thread ID.
     * \return True if in group, false otherwise.
     */
    bool IsThreadIn(const std::thread::id& id) const;
    /*!
     * \brief Create and add thread to group.
     * \param[in] threadfunction - Thread function to use with created thread.
     * \return Pointer to the created thread.
     */
    template <typename F> std::thread* CreateThread(F&& threadfunction)
    {
        std::lock_guard<std::mutex>  lock{m_mutex};
        std::unique_ptr<std::thread> newThread{new std::thread(std::forward<F>(threadfunction))};
        m_threadGroup.push_back(newThread.get());
        return newThread.release();
    }
    /*!
     * \brief Add thread to group.
     * \param[in] threadPtr - Pointer to thread.
     *
     * Throws std::runtime_error if called with a thread that
     * already belongs to the thread group.
     */
    void AddThread(std::thread* threadPtr);
    /*!
     * \brief Remove thread from group.
     * \param[in] threadPtr - Pointer to thread.
     */
    void RemoveThread(std::thread* threadPtr);
    /*!
     * \brief Remove thread from group.
     * \param[in] id - Thread ID.
     * \return Pointer to thread.
     *
     * This function returns nullptr if the id cannot be found.
     * Also this function should only be called if the thread
     * for this ID has not been joined else the ID will be invalid.
     */
    std::thread* RemoveThread(const std::thread::id& id);
    /*!
     * \brief Call join on all registered threads.
     * \return True if join successful, false otherwise.
     */
    bool JoinAll();
    /*! \brief Get the number of threads registered.
     *  \return Number of threads.
     */
    size_t Size() const;
    /*! \brief Check if we have any threads registered.
     *  \return True if no threads registered, false otherwise.
     */
    bool Empty() const;
	/*! \brief Delete all threads and clear list.
     */
    void Clear();

private:
    /*! \brief Access mutex for private data. */
    mutable std::mutex m_mutex;
    /*! \brief Typedef for thread list type. */
    using thread_list = std::list<std::thread*>;
    /*! \brief Typedef for thread list iterator type. */
    using thread_list_iter = thread_list::iterator;
    /*! \brief List containing threads. */
    thread_list m_threadGroup;
    /*!
     * \brief Is current thread in group (no mutex).
     * \return True if in group, false otherwise.
     */
    bool IsThisThreadInNoMutex() const;
    /*!
     * \brief Is given thread in group (no mutex).
     * \param[in] id - Thread ID.
     * \return True if in group, false otherwise.
     */
    bool IsThreadInNoMutex(const std::thread::id& id) const;
    /*!
     * \brief Delete thread object.
     * \param[in] threadPtr - Pointer to thread.
     */
    static void DeleteThread(std::thread* threadPtr);
};

} // namespace threads
} // namespace core_lib

#endif // THREADGROUP
