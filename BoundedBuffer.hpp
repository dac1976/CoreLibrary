/*!
 * \file BoundedBuffer.hpp
 * \brief File containing bounded buffer definition.
 */

#ifndef BOUNDEDBUFFER_HPP
#define BOUNDEDBUFFER_HPP

#include <mutex>
#include <condition_variable>
#include <functional>
#include "boost/circular_buffer.hpp"
#include "boost/call_traits.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The threads namespace. */
namespace threads {

/*!
 * \brief Class defining a bounded buffer.
 *
 * This class implements a fully thread-safe bounded circular
 * buffer. That blocksconsumer thread when empty and blocks
 * prodicer thread when full.
 */
template<typename T>
class BoundedBuffer final
{
public:
    // public typedefs
    typedef boost::circular_buffer<T> container_type;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::value_type value_type;
    typedef typename boost::call_traits<value_type>::param_type param_type;
    /*!
     * \brief Constructor.
     * \param [IN] The capcaity for the underlying circular buffer.
     */
    explicit BoundedBuffer(size_type capacity)
            : m_unreadCount(0), m_container(capacity)
    {
    }
    /*! \brief Copy constructor deleted.*/
    BoundedBuffer(const BoundedBuffer&) = delete;
    /*! \brief Copy assignment operator deleted.*/
    BoundedBuffer& operator=(const BoundedBuffer&) = delete;
    /*! \brief Destructor.*/
    ~BoundedBuffer() { }
    /*!
     * \brief Push new item to the front.
     * \param [IN] The item to push to the front.
     *
     * This function blocks if the buffer is at
     * capacity.
     */
    void PushFront(param_type item)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_notFullEvent.wait(lock,
                    std::bind(&BoundedBuffer<value_type>::IsNotFull,
                            this));
            m_container.push_front(item);
            ++m_unreadCount;
        }

        m_notEmptyEvent.notify_one();
    }
    /*!
     * \brief Pop item from the back.
     * \param [OUT] The item to pop from the back.
     *
     * This function blocks if the buffer is at
     * capacity.
     */
    void PopBack(value_type& item)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_notEmptyEvent.wait(lock,
                    std::bind(&BoundedBuffer<value_type>::IsNotEmpty,
                            this));
           item = m_container[--m_unreadCount];
        }

        m_notFullEvent.notify_one();
    }

private:
    /*! \brief Synchronization mutex. */
    std::mutex m_mutex;
    /*! \brief Condition variable to flag not empty. */
    std::condition_variable m_notEmptyEvent;
    /*! \brief Condition variable to flag not full. */
    std::condition_variable m_notFullEvent;
    /*! \brief Unread count. */
    size_type m_unreadCount;
    /*! \brief Circular buffer. */
    container_type m_container;
    /*!
     * \brief Test if buffer not empty.
     * \return True if buffer not empty, false otherwise.
     */
    bool IsNotEmpty() const
    {
        return m_unreadCount > 0;
    }
    /*!
     * \brief Test if buffer not full.
     * \return True if buffer not full, false otherwise.
     */
    bool IsNotFull() const
    {
        return m_unreadCount < m_container.capacity();
    }
};

} // namespace threads
} // namespace core_lib
#endif // BOUNDEDBUFFER_HPP
