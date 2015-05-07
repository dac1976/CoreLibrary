// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2015 Duncan Crutchley
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
 * \file ConcurrentQueue2.hpp
 * \brief File containing concurrent queue declaration.
 */

#ifndef CONCURRENTQUEUE2
#define CONCURRENTQUEUE2

#include <deque>
#include <algorithm>
#include <utility>
#include <memory>
#include <functional>
#include <type_traits>
#include "boost/call_traits.hpp"
#include "ConcurrentQueue.hpp" // for exceptions

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {
 
/*!
 * \brief Single item deleter for queue item.
 *
 * This uses "delete" to destroy the item.
 */
struct SingleItemDeleter
{
	/*!
	 * \brief Function operator.	 
	 * \param[in] p - Item to delete.
	 */
    template <typename P>
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
struct ArrayDeleter
{
	/*!
	 * \brief Function operator.	 
	 * \param[in] p - Item to delete.
	 */
    template <typename P>
    void operator()(P* p) const
	{
		delete[] p;
	}
};
 
 /*!
 * \brief Class defining a concurrent queue.
 *
 * This class implements a fully thread-safe queue, that can be
 * used as single/multiple producer and single/multiple consumer,
 * in any combination. 
 */
template<typename T>
class ConcurrentQueue2 final
{
public:
	/*!
	 * \brief Default constructor.
	 */
	ConcurrentQueue2()
	{ }
	/*! \brief Copy constructor deleted.*/
	ConcurrentQueue2(const ConcurrentQueue2&) = delete;
	/*! \brief Copy assignment operator deleted.*/
	ConcurrentQueue2& operator=(const ConcurrentQueue2&) = delete;
	/*! \brief Destructor.*/
	~ConcurrentQueue2()
	{
		Clear();
	}
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
    /*! \brief Typedef for Push method param type. */
    typedef typename boost::call_traits<T>::param_type param_type_t;
    /*! \brief Typedef for Push method deleter type. */
    typedef typename std::function< void(T&) > deleter_type_t;
    typedef typename boost::call_traits<deleter_type_t>::param_type del_param_type_t;
	/*!
	 * \brief Push an item onto the queue.
     * \param[in] item - Object of type T to push onto queue.
     * \param[in] deleter - Deleter for item.
     *
     * This method takes a template argument to control the deleter
     * function object required to tidy up queue items of type T.
     * This is only required when T is equivalent to an unmanaged raw
     * pointer type., e.g. T is an int*.
     *
     * A valid deleter will have the following form.
     *
     *   struct Deleter
     *   {
     *       template <typename P>
     *       void operator()(P* p)
     *       {
     *           delete p;
     *       }
     *   };
     *
     * This header includes definitions of 2 usable deleter function
     * objects SingleItemDeleter and ArrayDeleter. This can be passed
     * as the deleter in this method if appropriate, else the caller
     * should use the default argument which is suitable for when T
     * is a managed typed such as an RAII class or smart pointer.
     */
    void Push(param_type_t item, del_param_type_t deleter)
	{
		{
			std::lock_guard<std::mutex> lock{m_mutex};
            m_queue.emplace_back(item, deleter);
		}

		m_itemEvent.Signal();
	}
    /*!
     * \brief Push an item onto the queue.
     * \param[in] item - Object of type T to push onto queue.
     *
     * This versio nis when T is a managed type such as an RAII
     * object or smart pointer.
     */
    void Push(param_type_t item)
    {
        Push(item, deleter_type_t());
    }
	/*!
	 * \brief Break out of waiting on a Pop method.
	 *
	 * Useful to force consumer(s) to break out
	 * of wait on Pop etc.
	 */
	void BreakPop()
	{
		m_itemEvent.Signal();
	}
    /*!
	 * \brief Pop an item off the queue if there are any else wait.
	 * \param[out] item - The popped item, only valid if returns true.
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 * \return True if item popped off queue, false otherwise.
	 *
	 * Method will block forever or until an item is placed on the
	 * queue.
	 */
    bool Pop(T& item, deleter_type_t& deleter)
	{
		m_itemEvent.Wait();
        return PopNow(item, deleter);
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
        deleter_type_t deleter;
        return Pop(item, deleter);
    }
	/*!
	 * \brief Pop an item off the queue if there are any else return.
	 * \param[out] item - The popped item, only valid if returns true.
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 * \return True if item popped off queue, false otherwise.
	 */
    bool TryPop(T& item, deleter_type_t& deleter)
	{
        return PopNow(item, deleter);
	}
    /*!
     * \brief Pop an item off the queue if there are any else return.
     * \param[out] item - The popped item, only valid if returns true.
     * \return True if item popped off queue, false otherwise.
     */
    bool TryPop(T& item)
    {
        deleter_type_t deleter;
        return TryPop(item, deleter);
    }
	/*!
	 * \brief Pop an item off the queue.
	 * \param[out] item - The popped item.
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 *
	 * This will throw xQueuePopQueueEmptyError if there are no items
	 * on the queue when called.
	 */
    void TryPopThrow(T& item, deleter_type_t& deleter)
	{
        if (!PopNow(item, deleter))
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}
	}
    /*!
     * \brief Pop an item off the queue.
     * \param[out] item - The popped item.
     *
     * This will throw xQueuePopQueueEmptyError if there are no items
     * on the queue when called.
     */
    void TryPopThrow(T& item)
    {
        deleter_type_t deleter;
        TryPopThrow(item, deleter);
    }
	/*!
	 * \brief Pop an item off the queue but only wait for a given amount of time.
	 * \param[in] timeoutMilliseconds - Amount of time to wait.
	 * \param[out] item - The popped item, only valid if returns true.
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 * \return True if item popped successfully, false if timed out.
	 */
    bool TimedPop(const unsigned int timeoutMilliseconds, T& item
                  , deleter_type_t& deleter)
	{
		bool popSuccess{false};

		if (m_itemEvent.WaitForTime(timeoutMilliseconds))
		{
            popSuccess = PopNow(item, deleter);
		}

		return popSuccess;
	}
    /*!
     * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param[in] timeoutMilliseconds - Amount of time to wait.
     * \param[out] item - The popped item, only valid if returns true.
     * \return True if item popped successfully, false if timed out.
     */
    bool TimedPop(const unsigned int timeoutMilliseconds, T& item)
    {
        deleter_type_t deleter;
        return TimedPop(timeoutMilliseconds, item, deleter);
    }
	/*!
	 * \brief Pop an item off the queue but only wait for a given amount of time.
	 * \param[in] timeoutMilliseconds - Amount of time to wait.
	 * \param[out] item - The popped item.
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 *
	 * If no items have been put onto the queue after the specified amount to time
	 * then a xQueuePopTimeoutError exception is throw.
	 */
    void TimedPopThrow(const unsigned int timeoutMilliseconds, T& item
                       , deleter_type_t& deleter)
	{
		if (!m_itemEvent.WaitForTime(timeoutMilliseconds))
		{
			BOOST_THROW_EXCEPTION(xQueuePopTimeoutError());
		}

        if (!PopNow(item, deleter))
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}
	}
    /*!
     * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param[in] timeoutMilliseconds - Amount of time to wait.
     * \param[out] item - The popped item.
     *
     * If no items have been put onto the queue after the specified amount to time
     * then a xQueuePopTimeoutError exception is throw.
     */
    void TimedPopThrow(const unsigned int timeoutMilliseconds, T& item)
    {
        deleter_type_t deleter;
        TimedPopThrow(timeoutMilliseconds, item, deleter);
    }
	/*!
	 * \brief Steal an item from the back of the queue if there are any else return.
	 * \param[out] item - The stolen item, only valid if returns true.
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 * \return True if item stolen off queue, false otherwise.
	 */
    bool TrySteal(T& item, deleter_type_t& deleter)
	{
        return PopNow(item, deleter, eQueueEnd::back);
	}
    /*!
     * \brief Steal an item from the back of the queue if there are any else return.
     * \param[out] item - The stolen item, only valid if returns true.
     * \return True if item stolen off queue, false otherwise.
     */
    bool TrySteal(T& item)
    {
        deleter_type_t deleter;
        return TrySteal(item, deleter);
    }
	/*!
	 * \brief Steal an item from the back of the queue.
	 * \param[out] item - The stolen item.
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 *
	 * This will throw xQueuePopQueueEmptyError if there are no items
	 * on the queue when called.
	 */
    void TryStealThrow(T& item, deleter_type_t& deleter)
	{
        if (!PopNow(item, deleter, eQueueEnd::back))
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}
	}
    /*!
     * \brief Steal an item from the back of the queue.
     * \param[out] item - The stolen item.
     *
     * This will throw xQueuePopQueueEmptyError if there are no items
     * on the queue when called.
     */
    void TryStealThrow(T& item)
    {
        deleter_type_t deleter;
        TryStealThrow(item, deleter);
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
	const T* Peek(const size_t index) const
	{
		const T* pItem{};
		std::lock_guard<std::mutex> lock{m_mutex};
		
		if (!m_queue.empty() && (index < m_queue.size()))
		{
            const T& item = m_queue[index].item;
			pItem = &item;
		}

		return pItem;
	}
	/*!
	 * \brief Clear the queue.
	 *
	 * This method will clear the queue of its contents and calling
	 * the deleter on each item of the queue if the queue item's 
	 * type is a pointer type.
	 *
	 * The method should only be called when no threads are blocked
	 * on any of the pop methods.
	 */
	void Clear()
	{
		std::lock_guard<std::mutex> lock{m_mutex};

        for (auto& qi : m_queue)
        {
            qi.Delete();
        }
		
		m_queue.clear();
		m_itemEvent.Reset();
	}

private:
	/*! \brief Synchronization mutex. */
	mutable std::mutex m_mutex;
	/*! \brief Synchronization event. */
	SyncEvent m_itemEvent{eNotifyType::signalOneThread
						  , eResetCondition::manualReset
						  , eIntialCondition::notSignalled};
    /*! \brief QueItem wrapper object. */
    struct QueueItem
    {
        T item;
        deleter_type_t deleter;

        QueueItem() = default;
        ~QueueItem() = default;

        QueueItem(param_type_t item
                  , del_param_type_t deleter)
            : item(item_)
            , deleter(deleter_)
        {
        }

        QueueItem(const QueueItem& qi)
            : item(qi.item)
            , deleter(qi.deleter)
        {
        }

        QueueItem(QueueItem&& qi)
        {
            *this = std::move(qi);
        }

        QueueItem& operator=(const QueueItem& qi)
        {
            std::swap(*this, QueueItem(qi));
            return *this;
        }

        QueueItem& operator=(QueueItem&& qi)
        {
            std::swap(item, qi.item);
            std::swap(deleter, qi.deleter);
            return *this;
        }

        void Delete()
        {
            if (qi.deleter)
            {
                qi.deleter(qi.item);
            }
        }
    };
    /*! \brief Typedef for container type. */
    typedef std::deque<QueueItem> container_type;
	/*! \brief Underlying deque container acting as the queue. */
	container_type m_queue;

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
	 * \param[out] item - Item popped off queue
     * \param[out] deleter - The popped item's deleter, only valid if returns true and if deleter not required this will be a null std::function..
	 * \param[in] whichEnd - (Optional) Which end to pop from.
	 * \return True if not empty, false if queue empty.
	 */
    bool PopNow(T& item, deleter_type_t& deleter
			    , const eQueueEnd whichEnd = eQueueEnd::front)
	{
		std::lock_guard<std::mutex> lock{m_mutex};
		const bool isEmpty = m_queue.empty();

		if (!isEmpty)
		{
			if (whichEnd == eQueueEnd::front)
			{
                PopFront(item, deleter);
			}
			else
			{
                PopBack(item, deleter);
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
     * \param[out] deleter - The popped item's deleter.
	 */
    void PopFront(T& item, deleter_type_t& deleter)
	{
        QueueItem& qi = m_queue.front();
        item = std::move(qi.item);
        deleter = std::move(qi.deleter);
		m_queue.pop_front();
	}

	/*!
	 * \brief Pop an item off the back of the queue.
     * \param[out] item - Item popped off back.
     * \param[out] deleter - The popped item's deleter.
	 */
    void PopBack(T& item, deleter_type_t& deleter)
	{
        QueueItem& qi = m_queue.back();
        item = std::move(qi.item);
        deleter = std::move(qi.deleter);
		m_queue.pop_back();
	}
};

} // namespace threads
} // namespace core_lib

#endif // CONCURRENTQUEUE2
