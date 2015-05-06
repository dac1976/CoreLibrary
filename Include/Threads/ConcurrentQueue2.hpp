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
#include <type_traits>
#include "boost/call_traits.hpp"
#include "ConcurrentQueue.hpp" // for exceptions

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {
 
/*!
 * \brief Default deleter for queue item.
 *
 * This does nothing and assumes the item is managed elsewhere.
 */
struct DefaultDeleter
{
	/*!
	 * \brief Function operator.	
	 * \param[in] p - Item to delete.
	 */
    template <typename T>
	void operator()(T* p)
	{
		(void*)p;
	}
};

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
    template <typename T>
	void operator()(T* p)
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
    template <typename T>
	void operator()(T* p)
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
 *
 * This class is templated and takes a type for the queue item 
 * and a type for the deleter. The deleter type must be for a 
 * function object with following design:
 *
 *     struct MyDeleter
 *     {
 *         template<typename T> 
 *         void operator()(T* p)
 *         {
 *             // Delete p using appropriate technique.
 *         }
 *     };
 */
template<typename T, class D = DefaultDeleter>
class ConcurrentQueue2 final
{
public:
	/*! \brief Typedef for deleter type. */
	typedef D deleter_type;
	/*! \brief Typedef for container type. */
	typedef std::deque<T> container_type;
	/*! \brief Typedef for container size type. */
	typedef typename container_type::size_type size_type;
	/*! \brief Typedef for container value type. */
	typedef typename container_type::value_type value_type;
	/*! \brief Typedef for container param type. */
	typedef typename boost::call_traits<value_type>::param_type param_type;
	
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
	/*!
	 * \brief Push an item onto the queue.
	 * \param[in] item - object of type T to push onto queue.
	 */
	void Push(param_type item)
	{
		{
			std::lock_guard<std::mutex> lock{m_mutex};
			m_queue.push_back(std::move(item));
		}

		m_itemEvent.Signal();
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
	 * \return True if item popped off queue, false otherwise.
	 *
	 * Method will block forever or until an item is placed on the
	 * queue.
	 */
	bool Pop(value_type& item)
	{
		m_itemEvent.Wait();
		return PopNow(item);
	}
	/*!
	 * \brief Pop an item off the queue if there are any else return.
	 * \param[out] item - The popped item, only valid if returns true.
	 * \return True if item popped off queue, false otherwise.
	 */
	bool TryPop(value_type& item)
	{
		return PopNow(item);
	}
	/*!
	 * \brief Pop an item off the queue.
	 * \param[out] item - The popped item.
	 *
	 * This will throw xQueuePopQueueEmptyError if there are no items
	 * on the queue when called.
	 */
	void TryPopThrow(value_type& item)
	{
		if (!PopNow(item))
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}
	}
	/*!
	 * \brief Pop an item off the queue but only wait for a given amount of time.
	 * \param[in] timeoutMilliseconds - Amount of time to wait.
	 * \param[out] item - The popped item, only valid if returns true.
	 * \return True if item popped successfully, false if timed out.
	 */
	bool TimedPop(const unsigned int timeoutMilliseconds, value_type& item)
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
	 * then a xQueuePopTimeoutError exception is throw.
	 */
	void TimedPopThrow(const unsigned int timeoutMilliseconds, value_type& item)
	{
		if (!m_itemEvent.WaitForTime(timeoutMilliseconds))
		{
			BOOST_THROW_EXCEPTION(xQueuePopTimeoutError());
		}

		if (!PopNow(item))
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}
	}
	/*!
	 * \brief Steal an item from the back of the queue if there are any else return.
	 * \param[out] item - The stolen item, only valid if returns true.
	 * \return True if item stolen off queue, false otherwise.
	 */
	bool TrySteal(value_type& item)
	{
		return PopNow(item, eQueueEnd::back);
	}
	/*!
	 * \brief Steal an item from the back of the queue.
	 * \param[out] item - The stolen item.
	 *
	 * This will throw xQueuePopQueueEmptyError if there are no items
	 * on the queue when called.
	 */
	void TryStealThrow(value_type& item)
	{
		if (!PopNow(item, eQueueEnd::back))
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}
	}
	/*!
	 * \brief Take a peek at an item at a given index on the queue.
	 * \param[in] index - Zero-based index of the queue item to peek at.
	 * \return COnst pointer to item on queue being peeked at.
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
			const value_type& item = m_queue[index];
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

		if (std::is_pointer<value_type>::value)
		{
			for (auto& item : m_queue)
			{
				m_deleter(item);
			}
        }
		
		m_queue.clear();
		m_itemEvent.Reset();
	}

private:
	/*! \brief Item deleter. */
	deleter_type m_deleter;
	/*! \brief Null item. */
	value_type m_nullItem;
	/*! \brief Synchronization mutex. */
	mutable std::mutex m_mutex;
	/*! \brief Synchronization event. */
	SyncEvent m_itemEvent{eNotifyType::signalOneThread
						  , eResetCondition::manualReset
						  , eIntialCondition::notSignalled};
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
	 * \param[in] whichEnd - (Optional) Which end to pop from.
	 * \return True if not empty, false if queue empty.
	 */
	bool PopNow(value_type& item
			    , const eQueueEnd whichEnd = eQueueEnd::front)
	{
		std::lock_guard<std::mutex> lock{m_mutex};
		const bool isEmpty = m_queue.empty();

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
	 * \param[out] item - item popped off front.
	 */
	void PopFront(value_type& item)
	{
		item = std::move(m_queue.front());
		m_queue.pop_front();
	}

	/*!
	 * \brief Pop an item off the back of the queue.
	 * \param[out] item - item popped off back.
	 */
	void PopBack(value_type& item)
	{
		item = std::move(m_queue.back());
		m_queue.pop_back();
	}
};

} // namespace threads
} // namespace core_lib

#endif // CONCURRENTQUEUE2
