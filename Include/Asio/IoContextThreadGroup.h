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
 * \file IoContextThreadGroup.h
 * \brief File containing declarations relating the IoContextThreadGroup class.
 */

#ifndef IoContextThreadGroup_H
#define IoContextThreadGroup_H

#include "Threads/ThreadGroup.h"
#include "AsioDefines.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{

/*!
 * \brief I/O Context Thread group class.
 *
 * This class implements a thread group mechanism for use by Boost's ASIO I/O context. This allows
 * the I/O context to spread its work load across multiple threads. This class also calls run on the
 * I/O context from each registered thread and also calls stop and joins all threads in its
 * destructor.
 */
class CORE_LIBRARY_DLL_SHARED_API IoContextThreadGroup final
{
public:
    /*!
     * \brief Initialising constuctor.
     * \param[in] numThreads - (Optional) Number of threads to create.
     *
     * If the number of threads is not specified then the value
     * will be assigned using std::thread::hardware_concurrency().
     */
    explicit IoContextThreadGroup(unsigned int numThreads = std::thread::hardware_concurrency());
    /*! \brief Copy constructor deleted.*/
    IoContextThreadGroup(const IoContextThreadGroup&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    IoContextThreadGroup& operator=(const IoContextThreadGroup&) = delete;
    /*! \brief Move constructor deleted.*/
    IoContextThreadGroup(IoContextThreadGroup&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    IoContextThreadGroup& operator=(IoContextThreadGroup&&) = delete;
    /*! \brief Destructor.*/
    ~IoContextThreadGroup();
    /*!
     * \brief Get the I/O context.
     * \return A reference to the I/O context.
     */
    boost_iocontext_t& IoContext();
    /*!
     * \brief Post a function object to be run by one of our threads.
     * \param[in] function - Function to be run by one of our threads.
     */
    template <typename F> void Post(F&& function)
    {
        boost_asio::post(m_ioContext, std::forward<F>(function));
    }

private:
    /*! \brief Boost ASIO I/O context.*/
    boost_iocontext_t m_ioContext;
    /*! \brief Boost ASIO I/O context work guard object.*/
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_ioWorkGuard;
    /*! \brief Our thread group.*/
    threads::ThreadGroup m_threadGroup;
};

} // namespace asio
} // namespace core_lib

#endif // #define IoContextThreadGroup_H
