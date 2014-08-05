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
 * \file IoServiceThreadGroup.hpp
 * \brief File containing declarations relating the IoServiceThreadGroup class.
 */

#ifndef IOSERVICETHREADGROUP_HPP
#define IOSERVICETHREADGROUP_HPP

#include "../ThreadGroup.hpp"
#include <boost/asio.hpp>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {

/*!
 * \brief I/O Service Thread group class.
 *
 * This class implements a thread group mechanism for
 * use by Boost's ASIO I/O service. This allows the
 * I/O Service to spread its work load across multiple
 * threads. This class also calls run on the I/O service
 * from each registered thread and also calls stop and joins
 * all threads in its destructor.
 */
class IoServiceThreadGroup final
{
public:
    /*!
     * \brief Initialising constuctor.
     * \param [IN] (Optional) Number of threads to create.
     *
     * If the number of threads is not specified then the value
     * will be assigned using std::thread::hardware_concurrency().
     */
    explicit IoServiceThreadGroup(const unsigned int numThreads
                                      = std::thread::hardware_concurrency());
    /*! \brief Copy constructor deleted.*/
    IoServiceThreadGroup(const IoServiceThreadGroup& ) = delete;
    /*! \brief Copy assignment operator deleted.*/
    IoServiceThreadGroup& operator=(const IoServiceThreadGroup& ) = delete;
    /*! \brief Destructor.*/
    ~IoServiceThreadGroup();
    /*!
     * \brief Get the I/O service.
     * \return A reference to the I/O service.
     */
    boost::asio::io_service& IoService();

private:
    /*! \brief Boost ASIO I/O service.*/
    boost::asio::io_service m_ioService;
    /*! \brief Boost ASIO I/O service work object.*/
    boost::asio::io_service::work m_ioWork;
    /*! \brief Our thread group.*/
    threads::ThreadGroup m_threadGroup;
};

} //namespace asio
} //namespace core_lib

#endif // #define IOSERVICETHREADGROUP_HPP
