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
 * \file ConcurrentQueue.h
 * \brief File containing concurrent queue declaration.
 */

#ifndef CONCURRENTQUEUE
#define CONCURRENTQUEUE

#include <deque>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <boost/throw_exception.hpp>
#include "SyncEvent.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The threads namespace. */
namespace threads
{

/*!
 * \brief Single item deleter for queue item.
 *
 * This uses "delete" to destroy the item.
 */
template <typename P> struct SingleItemDeleter
{
    /*!
     * \brief Function operator.
     * \param[in] p - Item to delete.
     */
    void operator()(P* p) const
    {
        delete p;
    }
};

/*!
 * \brief Array deleter for queue item.
 *
 * This uses "delete []" to destroy the item.
 */
template <typename P> struct ArrayDeleter
{
    /*!
     * \brief Function operator.
     * \param[in] p - Item to delete.
     */
    void operator()(P* p) const
    {
        delete[] p;
    }
};

/*!
 * \brief Class defining a concurrent queue.
 *
 * This class implements a fully thread-safe queue that can be
 * used with single/multiple producer thread(s) and single/multiple
 * consumer thread(s).
 *
 * It is up to the caller to make sure individual queue items
 * get deallocated correctly. Preferably by using RAII objects
 * as queue items or wrapping  underlying data in std::shared_ptr
 * (using a custom deallocator if necessary - see example custom
 * deleters: SingleItemDeleter and ArrayDeleter).
 *
 * The template T must be a copyable and movable type.
 */
template <typename T> class ConcurrentQueue final
{
public:
    /*!
     * \brief Default constructor.
     */
    ConcurrentQueue() = default;
    /*! \brief Copy constructor deleted.*/
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
    /*! \brief Move constructor deleted.*/
    ConcurrentQueue(ConcurrentQueue&&) = delete;
    /*! \brief Move assignment operator deleted.*/
    ConcurrentQueue& operator=(ConcurrentQueue&&) = delete;
    /*!
     * \brief Destructor
     *
     * If the queue items aren't RAII-like objects and so don't
     * manager their own memory then make sure you call the
     * second Clear method, defined later, that takes a
     * deleter functor to tidy up the memory of each queue item.
     */
    ~ConcurrentQueue() = default;
    /*!
     * \brief Size of the queue.
     * \return The number of items on the queue.
     */
    size_t Size() const
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_queue.size();
    }
    /*!
     * \brief Is the queue empty.
     * \return True if empty, false otherwise.
     */
    bool Empty() const
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_queue.empty();
    }
    /*!
     * \brief Push an item onto the queue.
     * \param[in] item - Object of type T to push onto queue.
     *
     * The arg item must be moveable either implicitly or by
     * passing it to Push wrapped in a std::move() and item
     * will be invalid once this function returns due to
     * being moved onto the internal queue.
     *
     * It is preferred to use this version when performance
     * matters as move semantics are enforced.
     */
    void Push(T&& item)
    {
        {
            std::lock_guard<std::mutex> lock{m_mutex};
            m_queue.emplace_back(std::move(item));
        }

        m_itemEvent.Signal();
    }
    /*!
     * \brief Push an item onto the queue.
     * \param[in] item - Object of type T to push onto queue.
     *
     * Use this version only when you want to push a copy of
     * the item arg onto the innternal queue because maybe
     * you want to use item afterwards.
     *
     * This option is not as efficient as the first version
     * of Push because move semantics are not enforced.
     */
    void Push(const T& item)
    {
        {
            std::lock_guard<std::mutex> lock{m_mutex};
            m_queue.emplace_back(item);
        }

        m_itemEvent.Signal();
    }
    /*!
     * \brief Break out of waiting on a Pop method.
     *
     * Useful to force consumer(s) to break out
     * of wait on Pop etc.
     */
    void BreakPopWait()
    {
        m_itemEvent.Signal();
    }
    /*!
     * \brief Pop an item off the queue if there are any else wait.
     * \param[out] item - The popped item, only valid if returns true.
     * \return True if item popped off queue, false otherwise.
     *
     * Method will block forever or until an item is placed on the
     * queue.
     */
    bool Pop(T& item)
    {
        m_itemEvent.Wait();
        return PopNow(item);
    }
    /*!
     * \brief Pop an item off the queue if there are any else wait.
     * \param[out] item - The popped item, only valid if returns true.
     *
     * Method will block forever or until an item is placed on the
     * queue.
     *
     * This will throw std::runtime_error if there are no items
     * on the queue when called.
     */
    void PopThrow(T& item)
    {
        m_itemEvent.Wait();

        if (!PopNow(item))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("no item to pop"));
        }
    }
    /*!
     * \brief Pop an item off the queue if there are any else return.
     * \param[out] item - The popped item, only valid if returns true.
     * \return True if item popped off queue, false otherwise.
     */
    bool TryPop(T& item)
    {
        return PopNow(item);
    }
    /*!
     * \brief Pop an item off the queue.
     * \param[out] item - The popped item.
     *
     * This will throw std::runtime_error if there are no items
     * on the queue when called.
     */
    void TryPopThrow(T& item)
    {
        if (!PopNow(item))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("no item to pop"));
        }
    }
    /*!
     * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param[in] timeoutMilliseconds - Amount of time to wait.
     * \param[out] item - The popped item, only valid if returns true.
     * \return True if item popped successfully, false if timed out.
     */
    bool TimedPop(unsigned int timeoutMilliseconds, T& item)
    {
        bool popSuccess{false};

        if (m_itemEvent.WaitForTime(timeoutMilliseconds))
        {
            popSuccess = PopNow(item);
        }

        return popSuccess;
    }
    /*!
     * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param[in] timeoutMilliseconds - Amount of time to wait.
     * \param[out] item - The popped item.
     *
     * If no items have been put onto the queue after the specified amount to time
     * then a std::runtime_error exception is throw.
     */
    void TimedPopThrow(unsigned int timeoutMilliseconds, T& item)
    {
        if (!m_itemEvent.WaitForTime(timeoutMilliseconds))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("item event timed out"));
        }

        if (!PopNow(item))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("no item to pop"));
        }
    }
    /*!
     * \brief Steal an item from the back of the queue if there are any else return.
     * \param[out] item - The stolen item, only valid if returns true.
     * \return True if item stolen off queue, false otherwise.
     */
    bool TrySteal(T& item)
    {
        return PopNow(item, eQueueEnd::back);
    }
    /*!
     * \brief Steal an item from the back of the queue.
     * \param[out] item - The stolen item.
     *
     * This will throw std::runtime_error if there are no items
     * on the queue when called.
     */
    void TryStealThrow(T& item)
    {
        if (!PopNow(item, eQueueEnd::back))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("no item to pop"));
        }
    }
    /*!
     * \brief Take a peek at an item at a given index on the queue.
     * \param[in] index - Zero-based index of the queue item to peek at.
     * \return Const pointer to item on queue being peeked at.
     *
     * This method returns a nullptr if the index does not exist in the queue.
     * The use of this method can be dangerous if there are multiple consumers.
     * or if there is a single consumer but the method is called from a different
     * thread to the consumer.
     */
    const T* Peek(size_t index) const
    {
        const T*                    pItem{};
        std::lock_guard<std::mutex> lock{m_mutex};

        if (!m_queue.empty() && (index < m_queue.size()))
        {
            pItem = &m_queue[index];
        }

        return pItem;
    }
    /*!
     * \brief Clear the queue.
     *
     * If this method is called while a thread is blocking on
     * the item event then due to interleving of calls and mutex
     * lock timings this may cause the list to be cleared before
     * a waiting pop function has had a chance to pop anything from
     * the queue. This is safe but what it means is the pop function
     * will not find anything to pop and will either throw or return
     * false to indicate that nothing was popped off the queue.
     */
    void Clear()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_queue.clear();
    }
    /*!
     * \brief Clear the queue.
     * \param[in] deleter - If queue items are not RAII objects that will tidy
     *                      up after themselves then you can pass in a suitable
     *                      deleter to delete each queue item.
     *
     * It is also expected that the queue items are raw pointers.
     *
     * If this method is called while a thread is blocking on
     * the item event then due to interleving of calls and mutex
     * lock timings this may cause the list to be cleared before
     * a waiting pop function has had a chance to pop anything from
     * the queue. This is safe but what it means is the pop function
     * will not find anything to pop and will either throw or return
     * false to indicate that nothing was popped off the queue.
     */
    template <typename F> void Clear(F deleter)
    {
        std::lock_guard<std::mutex> lock{m_mutex};

        for (auto& qi : m_queue)
        {
            deleter(qi);
        }

        m_queue.clear();
    }
    /*! \brief Typedef for container type. */
    using container_type = std::deque<T>;
    /*!
     * \brief Take all items from the queue and return them, thus clearing down the internal queue.
     *
     * If this method is called while a thread is blocking on
     * the item event then due to interleving of calls and mutex
     * lock timings this may cause the list to be cleared before
     * a waiting pop function has had a chance to pop anything from
     * the queue. This is safe but what it means is the pop function
     * will not find anything to pop and will either throw or return
     * false to indicate that nothing was popped off the queue.
     */
    container_type TakeAll()
    {
        container_type q{};

        {
            std::lock_guard<std::mutex> lock{m_mutex};
            q.swap(m_queue);
        }

        return q;
    }

private:
    /*! \brief Enumeration controlling end of queue to pop from. */
    enum class eQueueEnd
    {
        /*! \brief Pop front of the queue. */
        front,
        /*! \brief Pop back of the queue. */
        back
    };
    /*!
     * \brief Pop an item off the queue.
     * \param[out] item - Item popped off queue.
     * \param[out] whichEnd - WHich end of queue tp pop from.
     * \return True if not empty, false if queue empty.
     */
    bool PopNow(T& item, eQueueEnd whichEnd = eQueueEnd::front)
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        auto                        isEmpty = m_queue.empty();

        if (!isEmpty)
        {
            if (whichEnd == eQueueEnd::front)
            {
                PopFront(item);
            }
            else
            {
                PopBack(item);
            }
        }

        if (m_queue.empty())
        {
            m_itemEvent.Reset();
        }

        return !isEmpty;
    }

    /*!
     * \brief Pop an item off the front of the queue.
     * \param[out] item - Item popped off front.
     */
    void PopFront(T& item)
    {
        item = std::move(m_queue.front());
        m_queue.pop_front();
    }

    /*!
     * \brief Pop an item off the back of the queue.
     * \param[out] item - Item popped off back.
     */
    void PopBack(T& item)
    {
        item = std::move(m_queue.back());
        m_queue.pop_back();
    }

private:
    /*! \brief Synchronization mutex. */
    mutable std::mutex m_mutex;
    /*! \brief Synchronization event. */
    SyncEvent m_itemEvent{
        eNotifyType::signalOneThread, eResetCondition::manualReset, eIntialCondition::notSignalled};
    /*! \brief Underlying deque container acting as the queue. */
    container_type m_queue{};
};

} // namespace threads
} // namespace core_lib

#endif // CONCURRENTQUEUE
