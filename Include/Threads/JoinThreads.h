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
 * \file JoinThreads.h
 * \brief File containing multi-thread joiner declaration.
 */

#ifndef JOINTHREADS
#define JOINTHREADS

#include <thread>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

/*!
 * \brief Class to manage joining multiple threads held as objects in an STL container.
 *
 * You can specify the container type in the template argument.
\code
std::vector<std::thread> myThreads(2);
JoinThreads<std::vector> joiner(myThreads);
\endcode
 */
template <template <class, class> class C> class JoinThreads final
{
public:
    /*! \brief typedef for container type */
    using container_type = C<std::thread, std::allocator<std::thread>>;
    /*!
     * \brief Initialisation constructor.
     * \param[in] threads - Containr of std::threads.
     */
    explicit JoinThreads(container_type& threads)
        : m_threads(threads)
    {
    }
    /*! \brief Copy constructor deleted.*/
    JoinThreads(const JoinThreads&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    JoinThreads& operator=(const JoinThreads&) = delete;
    /*! \brief Move constructor deleted.*/
    JoinThreads(JoinThreads&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    JoinThreads& operator=(JoinThreads&&) = delete;
    /*! \brief Destructor- joins the threads.*/
    ~JoinThreads()
    {
        for (auto& t : m_threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }

private:
    /*! \brief Container of threads.*/
    container_type& m_threads;
};

/*!
 * \brief Class to manage joining multiple threads held as pointers in an STL container.
 *
 * You can specify the container type in the template argument.
\code
std::vector<std::thread*> myThreads(2);
JoinThreads<std::vector> joiner(myThreads);
\endcode
 */
template <template <class, class> class C> class JoinThreadsP final
{
public:
    /*! \brief typedef for container type */
    using container_type = C<std::thread*, std::allocator<std::thread*>>;
    /*!
     * \brief Initialisation constructor.
     * \param[in] threads - Containr of std::threads.
     */
    explicit JoinThreadsP(container_type& threads)
        : m_threads(threads)
    {
    }
    /*! \brief Copy constructor deleted.*/
    JoinThreadsP(const JoinThreadsP&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    JoinThreadsP& operator=(const JoinThreadsP&) = delete;
    /*! \brief Move constructor deleted.*/
    JoinThreadsP(JoinThreadsP&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    JoinThreadsP& operator=(JoinThreadsP&&) = delete;
    /*! \brief Destructor- joins the threads.*/
    ~JoinThreadsP()
    {
        for (auto& t : m_threads)
        {
            if (t && t->joinable())
            {
                t->join();
            }
        }
    }

private:
    /*! \brief Container of threads.*/
    container_type& m_threads;
};

} // namespace threads
} // namespace core_lib

#endif // JOINTHREADS
