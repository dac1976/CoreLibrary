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
 * \file ConcurrentQueue.hpp
 * \brief File containing concurrent queue definition.
 */

#ifndef CONCURRENTQUEUE_HPP
#define CONCURRENTQUEUE_HPP

#include <deque>
#include <algorithm>
#include <utility>
#include "SyncEvent.hpp"
#include "CustomException.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {

/*!
 * \brief Pop timeout exception.
 *
 * This exception class is intended to be thrown by pop functions in
 * the ConcurrentQueue class.
 */
class xQueuePopTimeoutError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xQueuePopTimeoutError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xQueuePopTimeoutError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xQueuePopTimeoutError();
};

/*! Enumeration containing queue configuration options. */
enum class eQueueOptions
{
    /*! \brief Queue items are not auto deleted. */
    noDelete = 0,
    /*! \brief Auto delete queue items in Clear(). */
    autoDelete = 1,
};

/*!
 * \brief Class defining a concurrent queue.
 *
 * This class implements a fully thread-safe queue, that can be
 * used as single/multiple producer and single/multiple consumer,
 * in any combination.
 */
template< typename T
          , eQueueOptions queueOptions = eQueueOptions::autoDelete>
class ConcurrentQueue final
{
public:
    /*!
     * \brief Default constructor.
     *
     * Queue is created to not auto-delete objects.
     */
    ConcurrentQueue()
            : m_autoDelete(queueOptions == eQueueOptions::autoDelete),
              m_itemEvent(eNotifyType::signalOneThread
                          , eResetCondition::manualReset
                          , eIntialCondition::notSignalled)
    {
    }
    /*! \brief Copy constructor deleted.*/
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
    /*! \brief Destructor.*/
    ~ConcurrentQueue()
    {
        Clear();
    }
    /*!
     * \brief Size of the queue.
     * \return The number of items on the queue.
     */
    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
    /*!
     * \brief Is the queue empty.
     * \return True if empty, false otherwise.
     */
    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
    /*!
     * \brief Push an item onto the queue.
     * \param [IN] Pointer to object of type T to push into queue.
     * \param [IN] Number of items of type T pointed to by pItem.
     */
    void Push(T* pItem, size_t size)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push_back(QueueItem(pItem, size));
        }

        m_itemEvent.Signal();
    }
    /*!
     * \brief Push a null item onto the queue.
     * \param [IN] Null pointer f type T.
     *
     * Useful to force a call to Wait or WaitForTime to return.
     */
    void PushNull()
    {
        Push(nullptr, 0);
    }
    /*!
     * \brief Pop an item off the queue if there are any else wait.
     * \param [OUT] Number of items of type T pointed to by returned pointer.
     * \return The popped item.
     *
     * Function will block forever or until an item is placed on the
     * queue.
     */
    T* Pop(size_t* size = nullptr)
    {
        m_itemEvent.Wait();
        size_t tempSize;
        T* pItem = PopNow(tempSize);

        if (size)
            *size = tempSize;

        return pItem;
    }
    /*!
     * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param [IN] Amount of time to wait.
     * \param [OUT] Number of objects of type T pointed to by returned pointer.
     * \return The popped item.
     *
     * If no items have been put onto the queue after the specified amount to time
     * then a nullptr is returned.
     */
    T* TimedPop(unsigned int timeoutMilliseconds, size_t* size = nullptr)
    {
        T* pItem = nullptr;
        size_t tempSize = 0;

        if (m_itemEvent.WaitForTime(timeoutMilliseconds))
            pItem = PopNow(tempSize);

        if (size)
            *size = tempSize;

        return pItem;
    }
    /*!
     * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param [IN] Amount of time to wait.
     * \param [OUT] Number of objects of type T pointed to by returned pointer.
     * \return The popped item.
     *
     * If no items have been put onto the queue after the specified amount to time
     * then a xQueuePopTimeoutError exception is throw.
     */
    T* TimedPopThrow(unsigned int timeoutMilliseconds, size_t* size = nullptr)
    {
        if (!m_itemEvent.WaitForTime(timeoutMilliseconds))
            BOOST_THROW_EXCEPTION(xQueuePopTimeoutError());

        size_t tempSize;
        T* pItem = PopNow(tempSize);

        if (size)
            *size = tempSize;

        return pItem;
    }
    /*!
     * \brief Take a peek at an item at a given index on the queue.
     * \param [IN] Zero-based index of the queue item to peek at.
     * \param [OUT] Number of objects of type T pointed to by returned pointer.
     * \return The peeked item.
     *
     * This function returns nullptr if the index does not exist in the queue.
     * The use of this function can be dangerous if there are multiple consumers.
     * or if there is a single consumer but the function is called from a different
     * thread to the consumer.
     */
    const T* Peek(size_t index, size_t* size = nullptr) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        const T* pItem = nullptr;

        if (m_queue.empty() || (index >= m_queue.size()))
        {
            if (size)
                *size = 0;
        }
        else
        {
            const QueueItem& queueItem = m_queue[index];
            pItem = queueItem.pItem();

            if (size)
                *size = queueItem.Size();
        }        

        return pItem;
    }
    /*!
     * \brief Clear the queue.
     *
     * This function will clear the queue of its contents and if
     * the correct options are set then the items on the queue are
     * deleted.
     *
     * The function should only be called when no threads are blocked
     * on Pop or TimedPop.
     */
    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_autoDelete)
        {
            for (auto& qi : m_queue)
            {
                qi.Release();
            }
        }

        m_queue.clear();
        m_itemEvent.Reset();
    }

private:
    /*! \brief Class to hold queue item. */
    class QueueItem
    {
    public:
        /*! \brief Default constructor. */
        QueueItem()
            : m_pItem(nullptr), m_size(0)
        { }
        /*!
         * \brief Initialising constuctor.
         * \param [IN] Pointer to actual item.
         * \param [IN] Number of objects of type T pointed to by returned pointer.
         */
        QueueItem(T* pItem, size_t size)
            : m_pItem(pItem), m_size(size)
        {  }
        /*! \brief Copy constructor. */
        QueueItem(const QueueItem& qi)
            : m_pItem(qi.m_item), m_size(qi.m_size)
        {  }
        /*! \brief Move constructor. */
        QueueItem(QueueItem&& qi)
            : m_pItem(nullptr), m_size(0)
        {
            *this = std::move(qi);
        }
        /*! \brief Destructor. */
        ~QueueItem()
        {
            if (m_pItem)
            {
                if (m_size > 1)
                    delete [] m_pItem;
                else
                    delete m_pItem;
            }
        }
        /*! \brief Copy assignment operator. */
        QueueItem& operator=(const QueueItem& qi)
        {
            if (this != &qi)
            {
                m_pItem = qi.m_pItem;
                m_size = qi.m_size;
            }

            return *this;
        }
        /*! \brief Move assignment operator. */
        QueueItem& operator=(QueueItem&& qi)
        {
            std::swap(m_pItem, qi.m_pItem);
            std::swap(m_size, qi.m_size);
            return *this;
        }
        /*! \brief Release member data without deleting memory. */
        void Release()
        {
            m_pItem = 0;
            m_size = 0;
        }
        /*! \brief Get item pointer. */
        T* pItem()
        {
            return m_pItem;
        }
        const T* pItem() const
        {
            return m_pItem;
        }
        /*! \brief Get size in terms of number of items of type T. */
        size_t Size() const
        {
            return m_size;
        }

    private:
        /*! \brief Pointer to item. */
        T* m_pItem;
        /*! \brief Number of objects of type T pointer to by m_item. */
        size_t m_size;
    };
    /*! \brief Synchronization mutex. */
    mutable std::mutex m_mutex;
    /*! \brief Auto-delete items when Clear() is called. */
    const bool m_autoDelete;
    /*! \brief Synchronization event. */
    SyncEvent m_itemEvent;
    /*! \brief Underlying deque container acting as the queue. */
    std::deque<QueueItem> m_queue;

    /*!
     * \brief Pop an item off the queue.
     * \param [OUT] Number of items of type T pointed to by returned pointer.
     * \return The popped item or null if there is a problem.
     */
    T* PopNow(size_t& size)
    {
        T* pItem = nullptr;
        size = 0;

        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (!m_queue.empty())
            {
                QueueItem& queueItem = m_queue.front();
                pItem = queueItem.pItem();
                size = queueItem.Size();
                queueItem.Release();
                m_queue.pop_front();
            }

            if (m_queue.empty())
                m_itemEvent.Reset();
        }

        return pItem;
    }
};

} // namespace threads
} // namespace core_lib

#endif // CONCURRENTQUEUE_HPP
