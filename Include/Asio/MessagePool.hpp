
// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
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
 * \file MessagePool.hpp
 * \brief File containing message pool definitions.
 */
#ifndef MESSAGEPOOL_HPP
#define MESSAGEPOOL_HPP

#include <type_traits>
#include <cstddef>
#include <algorithm>
#include "asio/AsioDefines.h"
#include "Concurrency/cpp11/MutexHelpers.hpp"

namespace core_lib
{
namespace asio
{
namespace messages
{

/*!
 * \brief Statistics for reusable message pools.
 *
 * All cumulative counters relate to the period since
 * construction or the last call to ResetStatistics().
 *
 * Retired slots and current occupancy are lifetime state
 * and are not reset by ResetStatistics().
 */
struct MessagePoolStatistics
{
    size_t poolSize{0};
    size_t availableCount{0};
    size_t inUseCount{0};
    size_t peakInUseCount{0};
    size_t retiredSlotCount{0};

    std::uint64_t pooledCheckoutCount{0};
    std::uint64_t dynamicAcquisitionCount{0};
    std::uint64_t oversizedAcquisitionCount{0};
    std::uint64_t exhaustedAcquisitionCount{0};
};

/*!
 * \brief Reusable message pool class.
 *
 * The MessageHandler class now uses this specialised to our MessageHeader.
 * This pool is smart, if it cannot acquire a message from the pool, either
 * because there are no free messages or the required message size is too bg
 * then it falls back to dynamic allocation.
 *
 * The really nice thing is the pool messages release themselves pack to the
 * pool once the last instance of the shared_ptr to the message goes out of
 * scope. Dynamically allocated "fallback" messages self-destruct as normal
 * through the shared_ptr's default destruction logic.
 *
 * Header must be default constructable.
 * Message must be default constructible and contain a member of type Header
 * called header and member of type std::vector<char> called body, e.g.
 * defs::ReceivedMessage<HGL_MSG_HDR> or NxBttDataMessage etc.
 */
template <typename Header, typename Message>
class MessagePool : public std::enable_shared_from_this<MessagePool<Header, Message>>
{
    static_assert(std::is_default_constructible<Header>::value,
                  "MessagePool requires Header to be default constructible");

    static_assert(std::is_default_constructible<Message>::value,
                  "MessagePool requires Message to be default constructible");

public:
    using msg_t        = Message;
    using msg_sh_ptr_t = std::shared_ptr<msg_t>;

#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Default constructor. */
    MessagePool()
    {
    }
#else
    /*! \brief Default constructor. */
    MessagePool() = default;
#endif

    /*!
     * \brief Initialisation constructor.
     * \param[in] poolSize - How many messages to have in the pool.
     * \param[in] reservedBodySize - The maximum size allowed per message.
     */
    MessagePool(size_t poolSize, size_t reservedBodySize)
        : m_pool(poolSize)
        , m_reservedBodySize(reservedBodySize)
    {
        m_available.reserve(poolSize);

        for (size_t i = 0; i < poolSize; ++i)
        {
            m_pool[i] = std::make_unique<msg_t>();
            m_pool[i]->body.reserve(reservedBodySize);
            m_available.push_back(i);
        }
    }

    /*! \brief Default destructor. */
    ~MessagePool() = default;
    /*! \brief Deleted copy constructor. */
    MessagePool(const MessagePool&) = delete;
    /*! \brief Deleted copy assignment operator. */
    MessagePool& operator=(const MessagePool&) = delete;

    /*!
     * \brief Get the pool size
     * \return Return the number of messages in the pool.
     */
    size_t PoolSize() const
    {
        return m_pool.size();
    }

    /*!
     * \brief Get the reserved message size.
     * \return Return the number of bytes reserved per message.
     */
    size_t ReservedBodySize() const
    {
        return m_reservedBodySize;
    }

    /*!
     * \brief Acquire a message from the pool.
     * \param[in] requiredBodySize - The amount of bytes needed to be reserved in the returned
     * message.
     * \return Returns a shared_ptr to a pool message.
     *
     * If the requiredBodySize is larger than ReservedBodySize() or the pool is
     * currently exhausted then it returns a dynamically allocated message instead.
     */
    msg_sh_ptr_t Acquire(size_t requiredBodySize)
    {
        const auto reservedSize = ReservedBodySize();

        if (requiredBodySize <= reservedSize)
        {
            STD_UNIQUE_LOCK(m_mutex);

            if (!m_available.empty())
            {
                // Obtain shared ownership before removing an index.
                // If shared_from_this() throws, the pool remains
                // unchanged.
                auto self = this->shared_from_this();

                const auto index = m_available.back();
                m_available.pop_back();

                // Record checkout while we already hold the mutex.
                RecordPooledCheckoutLocked();

                auto* msg = m_pool[index].get();

                // We now have exclusive access to this message.
                lock.unlock();

                // Construct the shared pointer outside the mutex.
                auto result = msg_sh_ptr_t(
                    msg, [self, index](msg_t* p) noexcept { self->ReleaseToPool(index, p); });

                // Reset the message after establishing RAII ownership.
                result->header = Header{};
                result->body.clear();

                // Restore the configured buffer reservation if a
                // previous caller reduced its capacity.
                if (result->body.capacity() < reservedSize)
                {
                    result->body.reserve(reservedSize);
                }

                return result;
            }
        }

        // Perform dynamic allocation outside the mutex.
        auto result = MakeDynamic(requiredBodySize);

        // Only record successful dynamic allocations.
        {
            STD_LOCK_GUARD(m_mutex);

            if (requiredBodySize > reservedSize)
            {
                ++m_oversizedAcquisitionCount;
            }
            else
            {
                ++m_exhaustedAcquisitionCount;
            }
        }

        return result;
    }

    /*!
     * \brief Get a thread-safe snapshot of the message pool
     * statistics.
     *      * \return Current message pool statistics.
     */
    MessagePoolStatistics GetStatistics() const
    {
        STD_LOCK_GUARD(m_mutex);

        MessagePoolStatistics stats;

        stats.poolSize         = m_pool.size();
        stats.availableCount   = m_available.size();
        stats.inUseCount       = InUseCountLocked();
        stats.peakInUseCount   = m_peakInUseCount;
        stats.retiredSlotCount = m_retiredSlotCount;

        stats.pooledCheckoutCount = m_pooledCheckoutCount;

        stats.oversizedAcquisitionCount = m_oversizedAcquisitionCount;

        stats.exhaustedAcquisitionCount = m_exhaustedAcquisitionCount;

        stats.dynamicAcquisitionCount = m_oversizedAcquisitionCount + m_exhaustedAcquisitionCount;

        return stats;
    }

    /*!
     * \brief Reset cumulative acquisition statistics and the
     * peak pool occupancy.
     *      * Current occupancy and permanently retired slots are
     * unaffected.
     */
    void ResetStatistics()
    {
        STD_LOCK_GUARD(m_mutex);

        m_peakInUseCount = InUseCountLocked();

        m_pooledCheckoutCount       = 0;
        m_oversizedAcquisitionCount = 0;
        m_exhaustedAcquisitionCount = 0;
    }

private:
    /*!
     * \brief Make a message dynamically.
     * \param[in] requiredBodySize - The amount of bytes needed to be reserved in the returned
     * message.
     * \return Returns a shared_ptr to a dynamically allocated message.
     */
    msg_sh_ptr_t MakeDynamic(size_t requiredBodySize)
    {
        auto msg = std::make_shared<msg_t>();
        msg->body.reserve(requiredBodySize);
        return msg;
    }

    /*!
     * \brief Release a message back to the pool for re-use.
     * \param[in] index - The pool index to make available again.
     * \param[in] msg - The message being released to the pool.
     */
    void ReleaseToPool(size_t index, msg_t* msg) noexcept
    {
        if (nullptr == msg)
        {
            return;
        }

        try
        {
            // Reset message contents.
            msg->header = Header{};
            msg->body.clear();
        }
        catch (...)
        {
            // Permanently retire the affected slot.
            m_pool[index].reset();

            {
                STD_LOCK_GUARD(m_mutex);
                ++m_retiredSlotCount;
            }

            return;
        }

        const auto actualReservedBodySize = ReservedBodySize();

        // Reclaim oversized buffers on a best-effort basis.
        if (msg->body.capacity() > actualReservedBodySize)
        {
            try
            {
                defs::char_buffer_t tmp;
                tmp.reserve(actualReservedBodySize);

                msg->body.swap(tmp);
            }
            catch (...)
            {
                // Retain the existing usable buffer if
                // capacity reclamation fails.
            }
        }

        // Only free-list access requires the mutex.
        STD_LOCK_GUARD(m_mutex);

        m_available.push_back(index);
    }

    /*!
     * \brief Calculate current pooled message occupancy.
     *      * The caller must already hold m_mutex.
     */
    size_t InUseCountLocked() const noexcept
    {
        return m_pool.size() - m_available.size() - m_retiredSlotCount;
    }

    /*!
     * \brief Record a pooled message checkout and update
     * the peak pool occupancy.
     *      * The caller must already hold m_mutex.
     */
    void RecordPooledCheckoutLocked() noexcept
    {
        ++m_pooledCheckoutCount;

        const auto inUseCount = InUseCountLocked();

        if (inUseCount > m_peakInUseCount)
        {
            m_peakInUseCount = inUseCount;
        }
    }

private:
    /*! \brief Mutex protecting the available pool indices. */
    mutable std::mutex m_mutex;
    /*! \brief Message pointer definition. */
    using msg_ptr_t = std::unique_ptr<msg_t>;
    /*! \brief The actual message pool */
    std::vector<msg_ptr_t> m_pool;
    /*! \brief Preallocated LIFO free-list of available pool indices. */
    std::vector<size_t> m_available;
    /*! \brief The reserved size in bytes for pool messages. */
    size_t m_reservedBodySize{0};
    /*! \brief Peak number of simultaneously occupied pool slots. */
    size_t m_peakInUseCount{0};
    /*! \brief Number of permanently retired pool slots. */
    size_t m_retiredSlotCount{0};
    /*! \brief Number of pooled message checkouts. */
    std::uint64_t m_pooledCheckoutCount{0};
    /*! \brief Number of successful oversized dynamic acquisitions. */
    std::uint64_t m_oversizedAcquisitionCount{0};
    /*! \brief Number of successful exhausted pool acquisitions. */
    std::uint64_t m_exhaustedAcquisitionCount{0};
};

/*!
 * \brief Reusable generic message pool class.
 *
 * Generic version of the MessagePool class where you justy want to hold a pool
 * of message structures withut any specialised header.
 *
 * The message structure type must be default constructible.
 */
template <typename Message>
class GenericMessagePool : public std::enable_shared_from_this<GenericMessagePool<Message>>
{
    static_assert(std::is_default_constructible<Message>::value,
                  "GenericMessagePool requires Message to be default constructible");

public:
    using msg_t        = Message;
    using msg_sh_ptr_t = std::shared_ptr<msg_t>;

#ifdef USE_DEFAULT_CONSTRUCTOR_
    /*! \brief Default constructor. */
    GenericMessagePool()
    {
    }
#else
    /*! \brief Default constructor. */
    GenericMessagePool() = default;
#endif

    /*!
     * \brief Initialisation constructor.
     * \param[in] poolSize - How many messages to have in the pool.
     */
    explicit GenericMessagePool(size_t poolSize)
        : m_pool(poolSize)
    {
        m_available.reserve(poolSize);

        for (size_t i = 0; i < poolSize; ++i)
        {
            m_pool[i] = std::make_unique<msg_t>();
            m_available.push_back(i);
        }
    }

    /*! \brief Default destructor. */
    ~GenericMessagePool() = default;
    /*! \brief Deleted copy constructor. */
    GenericMessagePool(const GenericMessagePool&) = delete;
    /*! \brief Deleted copy assignment operator. */
    GenericMessagePool& operator=(const GenericMessagePool&) = delete;

    /*!
     * \brief Get the pool size
     * \return Return the number of messages in the pool.
     */
    size_t PoolSize() const
    {
        return m_pool.size();
    }

    /*!
     * \brief Acquire a message from the pool.
     *
     * If the pool is currently exhausted then it returns a dynamically
     * allocated message instead.
     */
    msg_sh_ptr_t Acquire()
    {
        {
            STD_UNIQUE_LOCK(m_mutex);

            if (!m_available.empty())
            {
                // Obtain shared ownership before removing an index.
                auto self = this->shared_from_this();

                const auto index = m_available.back();
                m_available.pop_back();

                // Record checkout while we already hold the mutex.
                RecordPooledCheckoutLocked();

                auto* msg = m_pool[index].get();

                // The acquired message is exclusively owned.
                lock.unlock();

                return msg_sh_ptr_t(
                    msg, [self, index](msg_t* p) noexcept { self->ReleaseToPool(index, p); });
            }
        }

        // Dynamic allocation takes place outside the mutex.
        auto result = MakeDynamic();

        // Record successful dynamic allocation.
        {
            STD_LOCK_GUARD(m_mutex);
            ++m_exhaustedAcquisitionCount;
        }

        return result;
    }

    /*!
     * \brief Get a thread-safe snapshot of the message pool
     * statistics.
     *      * \return Current message pool statistics.
     */
    MessagePoolStatistics GetStatistics() const
    {
        STD_LOCK_GUARD(m_mutex);

        MessagePoolStatistics stats;

        stats.poolSize         = m_pool.size();
        stats.availableCount   = m_available.size();
        stats.inUseCount       = InUseCountLocked();
        stats.peakInUseCount   = m_peakInUseCount;
        stats.retiredSlotCount = m_retiredSlotCount;

        stats.pooledCheckoutCount = m_pooledCheckoutCount;

        stats.oversizedAcquisitionCount = m_oversizedAcquisitionCount;

        stats.exhaustedAcquisitionCount = m_exhaustedAcquisitionCount;

        stats.dynamicAcquisitionCount = m_oversizedAcquisitionCount + m_exhaustedAcquisitionCount;

        return stats;
    }

    /*!
     * \brief Reset cumulative acquisition statistics and the
     * peak pool occupancy.
     *      * Current occupancy and permanently retired slots are
     * unaffected.
     */
    void ResetStatistics()
    {
        STD_LOCK_GUARD(m_mutex);

        m_peakInUseCount = InUseCountLocked();

        m_pooledCheckoutCount       = 0;
        m_oversizedAcquisitionCount = 0;
        m_exhaustedAcquisitionCount = 0;
    }

private:
    /*!
     * \brief Make a message dynamically.
     * \return Returns a shared_ptr to a dynamically allocated message.
     */
    msg_sh_ptr_t MakeDynamic()
    {
        return std::make_shared<msg_t>();
    }

    /*!
     * \brief Release a message back to the pool for re-use.
     * \param[in] index - The pool index to make available again.
     * \param[in] msg - The message being released to the pool.
     */
    void ReleaseToPool(size_t index, msg_t* msg) noexcept
    {
        if (nullptr == msg)
        {
            return;
        }

        try
        {
            // Reset contents to the default-constructed state.
            *msg = msg_t{};
        }
        catch (...)
        {
            // Permanently retire the affected slot.
            m_pool[index].reset();

            {
                STD_LOCK_GUARD(m_mutex);
                ++m_retiredSlotCount;
            }

            return;
        }

        // Only free-list access requires the mutex.
        STD_LOCK_GUARD(m_mutex);

        m_available.push_back(index);
    }

    /*!
     * \brief Calculate current pooled message occupancy.
     *      * The caller must already hold m_mutex.
     */
    size_t InUseCountLocked() const noexcept
    {
        return m_pool.size() - m_available.size() - m_retiredSlotCount;
    }

    /*!
     * \brief Record a pooled message checkout and update
     * the peak pool occupancy.
     *      * The caller must already hold m_mutex.
     */
    void RecordPooledCheckoutLocked() noexcept
    {
        ++m_pooledCheckoutCount;

        const auto inUseCount = InUseCountLocked();

        if (inUseCount > m_peakInUseCount)
        {
            m_peakInUseCount = inUseCount;
        }
    }

private:
    /*! \brief Mutex protecting the available pool indices. */
    mutable std::mutex m_mutex;
    /*! \brief Message pointer definition. */
    using msg_ptr_t = std::unique_ptr<msg_t>;
    /*! \brief The actual message pool */
    std::vector<msg_ptr_t> m_pool;
    /*! \brief Preallocated LIFO free-list of available pool indices. */
    std::vector<size_t> m_available;
    /*! \brief Peak number of simultaneously occupied pool slots. */
    size_t m_peakInUseCount{0};
    /*! \brief Number of permanently retired pool slots. */
    size_t m_retiredSlotCount{0};
    /*! \brief Number of pooled message checkouts. */
    std::uint64_t m_pooledCheckoutCount{0};
    /*! \brief Number of successful oversized dynamic acquisitions. */
    std::uint64_t m_oversizedAcquisitionCount{0};
    /*! \brief Number of successful exhausted pool acquisitions. */
    std::uint64_t m_exhaustedAcquisitionCount{0};
};

} // namespace messages
} // namespace asio
} // namespace core_lib

#endif // MESSAGEPOOL_HPP
