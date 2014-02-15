#ifndef BOUNDED_BUFFER_HPP
#define BOUNDED_BUFFER_HPP
//-----------------------------------------------------------------------------
#include <mutex>
#include <condition_variable>
#include <functional>
#include "boost/circular_buffer.hpp"
#include "boost/call_traits.hpp"
//-----------------------------------------------------------------------------
namespace core_lib {
//-----------------------------------------------------------------------------
namespace threads {
//-----------------------------------------------------------------------------
template<typename T>
class BoundedBuffer
{
public:
    typedef boost::circular_buffer<T> container_type;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::value_type value_type;
    // param_type represents the "best" way to pass a 
    // parameter of type value_type to a method...
    typedef typename boost::call_traits<value_type>::param_type param_type;

    explicit BoundedBuffer(size_type capacity)
            : m_unreadCount(0), m_container(capacity)
    {
    }

    // Disable copying
    BoundedBuffer(const BoundedBuffer&) = delete;
    BoundedBuffer& operator=(const BoundedBuffer&) = delete;

    // push item onto front of circular buffer - blocks if buffer
    // full until there is a free slot...
    void PushFront(param_type item)
    {
        {
            // lock access - reduced scope...
            std::unique_lock<std::mutex> lock(m_mutex);
            //wait if full...
            m_notFullEvent.wait(lock,
                    std::bind(&BoundedBuffer<value_type>::IsNotFull,
                            this));
            // add item to container...
            m_container.push_front(item);
            // increment unread count...
            ++m_unreadCount;
        }

        // notify PopBack condition variable...
        m_notEmptyEvent.notify_one();
    }

    // pop item off back of circular buffer - blocks if buffer
    // empty until there is a filled slot...
    void PopBack(value_type* pItem)
    {
        {
            // lock access - reduced scope...
            std::unique_lock<std::mutex> lock(m_mutex);
            // wait if empty...
            m_notEmptyEvent.wait(lock,
                    std::bind(&BoundedBuffer<value_type>::IsNotEmpty,
                            this));
            // retrieve item...
            *pItem = m_container[--m_unreadCount];
        }

        // notify pushFront condition variable...
        m_notFullEvent.notify_one();
    }

private:
    size_type m_unreadCount;
    container_type m_container;
    std::mutex m_mutex;
    std::condition_variable m_notEmptyEvent;
    std::condition_variable m_notFullEvent;

    bool IsNotEmpty() const
    {
        return m_unreadCount > 0;
    }

    bool IsNotFull() const
    {
        return m_unreadCount < m_container.capacity();
    }
};
//-----------------------------------------------------------------------------
} // namespace threads
//-----------------------------------------------------------------------------
} // namespace core_lib
//-----------------------------------------------------------------------------
#endif
