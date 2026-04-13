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
 * \file ManagedSingleton.hpp
 * \brief File containing definition of ManagedSingleton class.
 *
 * Provides a thread-safe, lazily initialised singleton wrapper with
 * explicit lifetime management via Destroy().
 *
 * Two implementations are provided:
 * - ManagedSingleton_Modern: lock-free fast path using atomics (C++14+)
 * - ManagedSingleton_Legacy: mutex-based implementation (pre-C++14 fallback)
 *
 * The public alias ManagedSingleton<T> resolves to the appropriate
 * implementation depending on IS_CPP14.
 */

#ifndef MANAGED_SINGLETON_HPP
#define MANAGED_SINGLETON_HPP

#include <atomic>
#include <memory>
#include <mutex>
#include <utility>
#include <type_traits>
#include "Platform/PlatformDefines.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{

#if defined(IS_CPP14)
/*!
 * \brief Modern thread-safe managed singleton implementation.
 *
 * \tparam T Type of the singleton instance.
 *
 * This implementation uses a double-checked locking pattern with
 * std::atomic for a fast lock-free read path after initialisation.
 *
 * Behaviour:
 * - Instance() lazily constructs the singleton on first call.
 * - TryInstance() returns nullptr if not yet constructed.
 * - Destroy() destroys the current managed instance.
 * - A subsequent call to Instance() creates a new managed instance.
 *
 * Requirements:
 * - T must be constructible from the arguments supplied to the first
 *   Instance() call after each creation cycle.
 *
 * Thread Safety:
 * - Instance() is thread-safe.
 * - Destroy() must only be called when no other threads are accessing
 *   Instance() or TryInstance().
 */
template <typename T>
class ManagedSingleton_Modern
{
public:
    /*!
     * \brief Access the singleton instance.
     *
     * \tparam Args Constructor argument types.
     * \param args Arguments forwarded to T's constructor on first
     *             initialisation after a create/destroy cycle.
     * \return Reference to the singleton instance.
     *
     * Notes:
     * - Constructor arguments are only used when creating a new instance.
     * - Subsequent calls while the instance exists ignore provided arguments.
     */
    template <typename... Args>
    static T& Instance(Args&&... args)
    {
        T* ptr = m_instance.load(std::memory_order_acquire);

        if (nullptr == ptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            ptr = m_instance.load(std::memory_order_relaxed);

            if (nullptr == ptr)
            {
                std::unique_ptr<T> newInstance =
                    std::make_unique<T>(std::forward<Args>(args)...);

                ptr = newInstance.get();
                m_owned = std::move(newInstance);
                m_instance.store(ptr, std::memory_order_release);
            }
        }

        return *ptr;
    }

    /*!
     * \brief Attempt to get the current singleton instance.
     *
     * \return Pointer to the instance if it exists, otherwise nullptr.
     *
     * This function does not create the instance.
     */
    static T* TryInstance()
    {
        return m_instance.load(std::memory_order_acquire);
    }

    /*!
     * \brief Destroy the current managed singleton instance.
     *
     * After destruction:
     * - TryInstance() returns nullptr.
     * - A later call to Instance() creates a fresh singleton instance.
     *
     * \warning Must only be called when no other threads are accessing
     *          Instance() or TryInstance().
     */
    static void Destroy()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_instance.store(nullptr, std::memory_order_release);
        m_owned.reset();
    }

private:
    static std::atomic<T*>    m_instance; /*!< Raw pointer to managed instance */
    static std::unique_ptr<T> m_owned;    /*!< Owning pointer for lifetime management */
    static std::mutex         m_mutex;    /*!< Mutex for initialisation and destruction */
};

template <typename T>
std::atomic<T*> ManagedSingleton_Modern<T>::m_instance{nullptr};

template <typename T>
std::unique_ptr<T> ManagedSingleton_Modern<T>::m_owned;

template <typename T>
std::mutex ManagedSingleton_Modern<T>::m_mutex;

/*!
 * \brief Alias selecting modern singleton implementation.
 */
template <typename T>
using ManagedSingleton = ManagedSingleton_Modern<T>;

#else
/*!
 * \brief Legacy thread-safe managed singleton implementation.
 *
 * \tparam T Type of the singleton instance.
 *
 * This implementation uses a mutex for all access and is intended
 * for environments where the modern atomic-based implementation
 * is not available.
 *
 * Behaviour:
 * - Instance() lazily constructs the singleton on first call.
 * - TryInstance() returns nullptr if not yet constructed.
 * - Destroy() destroys the current managed instance.
 * - A subsequent call to Instance() creates a new managed instance.
 */
template <typename T>
class ManagedSingleton_Legacy
{
public:
    /*!
     * \brief Access the singleton instance.
     *
     * \tparam Args Constructor argument types.
     * \param args Arguments forwarded to T's constructor on first
     *             initialisation after a create/destroy cycle.
     * \return Reference to the singleton instance.
     */
    template <typename... Args>
    static T& Instance(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (nullptr == m_instance)
        {
            m_instance.reset(new T(std::forward<Args>(args)...));
        }

        return *m_instance;
    }

    /*!
     * \brief Attempt to get the current singleton instance.
     *
     * \return Pointer to the instance if it exists, otherwise nullptr.
     */
    static T* TryInstance()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_instance.get();
    }

    /*!
     * \brief Destroy the current managed singleton instance.
     *
     * After destruction, a later call to Instance() creates a fresh
     * singleton instance.
     *
     * \warning Must only be called when no other threads are accessing
     *          Instance() or TryInstance().
     */
    static void Destroy()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_instance.reset();
    }

private:
    static std::unique_ptr<T> m_instance; /*!< Owning pointer to instance */
    static std::mutex         m_mutex;    /*!< Mutex protecting all access */
};

template <typename T>
std::unique_ptr<T> ManagedSingleton_Legacy<T>::m_instance{nullptr};

template <typename T>
std::mutex ManagedSingleton_Legacy<T>::m_mutex;

/*!
 * \brief Alias selecting legacy singleton implementation.
 */
template <typename T>
using ManagedSingleton = ManagedSingleton_Legacy<T>;
#endif

} // namespace core_lib

#endif // MANAGED_SINGLETON_HPP