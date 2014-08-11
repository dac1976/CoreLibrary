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
 * \brief File containing concurrent queue declaration.
 */

#ifndef CONCURRENTQUEUE_HPP
#define CONCURRENTQUEUE_HPP

#include <deque>
#include <algorithm>
#include <utility>
#include "SyncEvent.hpp"
#include "Exceptions/CustomException.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {

/*!
 * \brief Pop timeout exception.
 *
 * This exception class is intended to be thrown by pop methods in
 * the ConcurrentQueue class.
 */
class xQueuePopTimeoutError : public exceptions::xCustomException
{
public:
	/*! \brief Default constructor. */
	xQueuePopTimeoutError();
	/*!
	 * \brief Initializing constructor.
     * \param[in] A user specifed message string.
	 */
	explicit xQueuePopTimeoutError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xQueuePopTimeoutError();
};

/*!
 * \brief Pop queue empty exception.
 *
 * This exception class is intended to be thrown by pop methods in
 * the ConcurrentQueue class.
 */
class xQueuePopQueueEmptyError : public exceptions::xCustomException
{
public:
	/*! \brief Default constructor. */
	xQueuePopQueueEmptyError();
	/*!
	 * \brief Initializing constructor.
     * \param[in] A user specifed message string.
	 */
	explicit xQueuePopQueueEmptyError(const std::string& message);
	/*! \brief Virtual destructor. */
	virtual ~xQueuePopQueueEmptyError();
};

/*! \brief Enumeration containing queue configuration options. */
enum class eQueueOptions
{
	/*! \brief Queue items are not auto deleted. */
	noDelete = 0,
	/*! \brief Auto delete queue items in Clear(). */
	autoDelete = 1,
};

/*! \brief The cq_private namespace. */
namespace cq_private {

/*! \brief Class to hold queue item. */
template <typename T>
class QueueItem final
{
public:
    /*! \brief Default constructor. */
    QueueItem() = default;
    /*!
     * \brief Initialising constuctor.
     * \param[in] Pointer to an item.
     *
     * Items created with this constructor will
     * be deleted with delete. And their size will
     * be stored as the special value of -1.
     */
    QueueItem(T* pItem)
        : m_pItem{pItem}, m_size{-1}
    {  }
    /*!
     * \brief Initialising constuctor.
     * \param[in] Pointer to an array of items.
     * \param[in] Number of objects of type T pointed to by returned pointer.
     *
     * Items created with this constructor will
     * be deleted with delete[] if size is > 0.
     */
    QueueItem(T* pItem, const int size)
        : m_pItem{pItem}, m_size{size}
    {  }
    /*! \brief Copy constructor. */
    QueueItem(const QueueItem& qi)
        : m_pItem{qi.m_pItem}, m_size{qi.m_size}
    {
    }
    /*! \brief Move constructor. */
    QueueItem(QueueItem&& qi)
    {
        *this = std::move(qi);
    }
    /*! \brief Destructor. */
    ~QueueItem()
    {
        if (m_pItem)
        {
            if (m_size > 0)
            {
                delete [] m_pItem;
            }
            else
            {
                delete m_pItem;
            }
        }
    }
    /*! \brief Copy assignment operator. */
    QueueItem& operator=(const QueueItem& qi)
    {
        std::swap(*this, QueueItem(qi));
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
        m_pItem = nullptr;
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
    int Size() const
    {
        return m_size;
    }

private:
    /*! \brief Pointer to item. */
    T* m_pItem{};
    /*! \brief Number of objects of type T pointer to by m_item. */
    int m_size{};
};

} // namespace cq_private

/*!
 * \brief Class defining a concurrent queue.
 *
 * This class implements a fully thread-safe queue, that can be
 * used as single/multiple producer and single/multiple consumer,
 * in any combination.
 */
template<typename T>
class ConcurrentQueue final
{
private:
    /*! \brief Queue item typedef. */
    typedef cq_private::QueueItem<T> queue_item_t;

public:
	/*!
	 * \brief Initialising constructor.
     * \param[in] (Optional) Set the queue's delete option.
	 *
	 * Queue is created to not auto-delete objects.
	 */
	explicit ConcurrentQueue(const eQueueOptions queueOptions
								 = eQueueOptions::autoDelete)
		: m_autoDelete{queueOptions == eQueueOptions::autoDelete}
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
     * \param[in] Pointer to object of type T to push into queue.
	 *
     * An item pushed on with this method will be deleted with
	 * delete.
	 */
	void Push(T* pItem)
	{
		{
			std::lock_guard<std::mutex> lock{m_mutex};
            m_queue.push_back(queue_item_t(pItem));
		}

		m_itemEvent.Signal();
	}
	/*!
	 * \brief Push an array items onto the queue.
     * \param[in] Pointer to object of type T to push into queue.
     * \param[in] Number of items of type T pointed to by pItem.
	 *
     * Items pushed on with this method will be deleted with
	 * delete[].
	 */
	void Push(T* pItem, const int size)
    {
		{
			std::lock_guard<std::mutex> lock{m_mutex};
            m_queue.push_back(queue_item_t(pItem, size));
		}

		m_itemEvent.Signal();
	}
	/*!
	 * \brief Push a null item onto the queue.
	 *
	 * Useful to force a call to Wait or WaitForTime to return.
	 */
	void Push()
	{
		Push(nullptr, 0);
	}
	/*!
	 * \brief Pop an item off the queue if there are any else wait.
     * \param[out] Number of items of type T pointed to by returned pointer. Special value of -1 implies single item.
	 * \return The popped item.
	 *
     * Method will block forever or until an item is placed on the
	 * queue.
	 */
	T* Pop(int* size = nullptr)
	{
		m_itemEvent.Wait();
		int tempSize{};
		T* pItem{PopNow(tempSize)};

		if (size)
		{
			*size = tempSize;
		}

		return pItem;
	}
	/*!
	 * \brief Pop an item off the queue if there are any else return.
     * \param[out] The popped item.
     * \param[out] Number of items of type T pointed to by returned pointer. Special value of -1 implies single item.
	 * \return True if item popped off queue, false otherwise.
	 */
	bool TryPop(T* &pItem, int* size = nullptr)
	{
		int tempSize{};
		bool isEmpty{true};
		pItem = PopNow(tempSize, &isEmpty);

		if (size)
		{
			*size = tempSize;
		}

		return !isEmpty;
	}
	/*!
	 * \brief Pop an item off the queue..
     * \param[out] Number of items of type T pointed to by returned pointer. Special value of -1 implies single item.
	 * \return A pointer to the popped item.
	 *
	 * This will throw xQueuePopQueueEmptyError if there are no items
	 * on the queue when called.
	 */
	T* TryPopThrow(int* size = nullptr)
	{
		int tempSize{};
		bool isEmpty{true};
		T* pItem{PopNow(tempSize, &isEmpty)};

		if (size)
		{
			*size = tempSize;
		}

		if (isEmpty)
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}

		return pItem;
	}
	/*!
	 * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param[in] Amount of time to wait.
     * \param[in] The popped item.
     * \param[out] Number of objects of type T pointed to by returned pointer. Special value of -1 implies single item.
	 * \return True if item popped successfully, false if timed out.
	 */
    bool TimedPop(const unsigned int timeoutMilliseconds, T* &pItem, int* size = nullptr)
	{
		pItem = nullptr;
		int tempSize{};
		bool isEmpty{true};

		if (m_itemEvent.WaitForTime(timeoutMilliseconds))
		{
			pItem = PopNow(tempSize, &isEmpty);
		}

		if (size)
		{
			*size = tempSize;
		}

		return !isEmpty;
	}
	/*!
	 * \brief Pop an item off the queue but only wait for a given amount of time.
     * \param[in] Amount of time to wait.
     * \param[out] Number of objects of type T pointed to by returned pointer. Special value of -1 implies single item.
	 * \return The popped item.
	 *
	 * If no items have been put onto the queue after the specified amount to time
	 * then a xQueuePopTimeoutError exception is throw.
	 */
	T* TimedPopThrow(const unsigned int timeoutMilliseconds, int* size = nullptr)
	{
		if (!m_itemEvent.WaitForTime(timeoutMilliseconds))
		{
			BOOST_THROW_EXCEPTION(xQueuePopTimeoutError());
		}

		int tempSize;
		bool isEmpty{true};
		T* pItem{PopNow(tempSize)};

		if (size)
		{
			*size = tempSize;
		}

		if (isEmpty)
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}

		return pItem;
	}
	/*!
	 * \brief Steal an item from the back of the queue if there are any else return.
     * \param[out] The stolen item.
     * \param[out] Number of items of type T pointed to by returned pointer. Special value of -1 implies single item.
	 * \return True if item stolen off queue, false otherwise.
	 */
	bool TrySteal(T* &pItem, int* size = nullptr)
	{
		int tempSize{};
		bool isEmpty{true};
		pItem = PopNow(tempSize, &isEmpty, back);

		if (size)
		{
			*size = tempSize;
		}

		return !isEmpty;
	}
	/*!
	 * \brief Steal an item from the back of the queue.
     * \param[out] Number of items of type T pointed to by returned pointer. Special value of -1 implies single item.
	 * \return A pointer to the stolen item.
	 *
	 * This will throw xQueuePopQueueEmptyError if there are no items
	 * on the queue when called.
	 */
	T* TryStealThrow(int* size = nullptr)
	{
		int tempSize{};
		bool isEmpty{true};
		T* pItem{PopNow(tempSize, &isEmpty, back)};

		if (size)
		{
			*size = tempSize;
		}

		if (isEmpty)
		{
			BOOST_THROW_EXCEPTION(xQueuePopQueueEmptyError());
		}

		return pItem;
	}
	/*!
	 * \brief Take a peek at an item at a given index on the queue.
     * \param[in] Zero-based index of the queue item to peek at.
     * \param[out] Number of objects of type T pointed to by returned pointer.
	 * \return The peeked item.
	 *
     * This method returns nullptr if the index does not exist in the queue.
     * The use of this method can be dangerous if there are multiple consumers.
     * or if there is a single consumer but the method is called from a different
	 * thread to the consumer.
	 */
    const T* Peek(const size_t index, int* size = nullptr) const
	{
		std::lock_guard<std::mutex> lock{m_mutex};
        const T* pItem{};

		if (m_queue.empty() || (index >= m_queue.size()))
		{
			if (size)
			{
				*size = 0;
			}
		}
		else
		{
            const queue_item_t& queueItem = m_queue[index];
			pItem = queueItem.pItem();

			if (size)
			{
				*size = queueItem.Size();
			}
		}

		return pItem;
	}
	/*!
	 * \brief Clear the queue.
	 *
     * This method will clear the queue of its contents and if
	 * the correct options are set then the items on the queue are
	 * deleted.
	 *
     * The method should only be called when no threads are blocked
     * on any of the pop methods.
	 */
	void Clear()
	{
		std::lock_guard<std::mutex> lock{m_mutex};

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
	/*! \brief Synchronization mutex. */
	mutable std::mutex m_mutex;
	/*! \brief Auto-delete items when Clear() is called. */
	const bool m_autoDelete{false};
	/*! \brief Synchronization event. */
	SyncEvent m_itemEvent{eNotifyType::signalOneThread
						  , eResetCondition::manualReset
						  , eIntialCondition::notSignalled};
	/*! \brief Underlying deque container acting as the queue. */
    std::deque<queue_item_t> m_queue;

    /*! \brief Enumeration controlling end of queue to pop from. */
	enum eQueueEnd
	{
        /*! \brief Pop front of the queue. */
		front,
        /*! \brief Pop back of the queue. */
		back
	};

	/*!
	 * \brief Pop an item off the queue.
     * \param[out] Number of items of type T pointed to by returned pointer.
     * \param[out] (Optional) Flag indicating if queue was empty on entry to method.
	 * \return The popped item or null if there is a problem.
	 */
	T* PopNow(int& size
			  , bool* pIsEmpty = nullptr
			  , const eQueueEnd whichEnd = front)
	{
		T* pItem{};
		size = 0;

		{
			std::lock_guard<std::mutex> lock{m_mutex};
			bool isEmpty = m_queue.empty();

			if (!isEmpty)
			{
                pItem = whichEnd == front
                        ? PopFront(size)
                        : PopBack(size);
			}

			if (m_queue.empty())
			{
				m_itemEvent.Reset();
			}

			if (pIsEmpty)
			{
				*pIsEmpty = isEmpty;
			}
		}

		return pItem;
	}


    /*!
     * \brief Pop an item off the front of the queue.
     * \param[out] Number of items of type T pointed to by returned pointer.
     * \return The popped item.
     */
    T* PopFront(int& size)
    {
        queue_item_t& queueItem = m_queue.front();
        T* pItem{queueItem.pItem()};
        size = queueItem.Size();
        queueItem.Release();
        m_queue.pop_front();
        return pItem;
    }

    /*!
     * \brief Pop an item off the back of the queue.
     * \param[out] Number of items of type T pointed to by returned pointer.
     * \return The popped item.
     */
    T* PopBack(int& size)
    {
        queue_item_t& queueItem = m_queue.back();
        T* pItem{queueItem.pItem()};
        size = queueItem.Size();
        queueItem.Release();
        m_queue.pop_back();
        return pItem;
    }
};

} // namespace threads
} // namespace core_lib

#endif // CONCURRENTQUEUE_HPP
