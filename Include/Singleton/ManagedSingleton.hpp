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

#ifndef MANAGED_SINGLETON_HPP
#define MANAGED_SINGLETON_HPP

#include <atomic>
#include <memory>
#include <mutex>
#include <utility>
#include <type_traits>
#include "Platform/PlatformDefines.h"

// TODO: This needs testing and maybe tweaking, it is a work-in-progress as a
// hopeful replacement for the Loki singleton.

namespace core_lib
{

#if defined(IS_CPP14)
// NOTE: T must be default-constructible for fallback instance.
template <typename T>
class ManagedSingleton_Modern
{
    static_assert(std::is_default_constructible<T>::value,
              "ManagedSingleton requires T to be default constructible");

public:
    template <typename... Args>
    static T& Instance(Args&&... args)
    {
        if (m_destroyed.load(std::memory_order_relaxed))
        {
            return DestroyedFallback();
        }

        T* ptr = m_instance.load(std::memory_order_acquire);

        if (nullptr == ptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            ptr = m_instance.load(std::memory_order_relaxed);

            if (nullptr == ptr)
            {
                if (m_destroyed.load(std::memory_order_relaxed))
                {
                    return destroyed_fallback();
                }

                std::unique_ptr<T> newInstance =
                    std::make_unique<T>(std::forward<Args>(args)...);

                ptr = newInstance.get();

                m_instance.store(ptr, std::memory_order_release);
                m_owned = std::move(newInstance);
            }
        }

        return *ptr;
    }

    static T* TryInstance()
    {
        return m_instance.load(std::memory_order_acquire);
    }

    // Destroy() must only be called when no other threads
    // are accessing Instance() or TryInstance().
    static void Destroy()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_destroyed.store(true, std::memory_order_relaxed);
        m_owned.reset();
        m_instance.store(nullptr, std::memory_order_release);
    }

private:
    static T& DestroyedFallback()
    {
        static T fallbackInstance{};
        return fallbackInstance;
    }

private:
    static std::atomic<T*>    m_instance;
    static std::unique_ptr<T> m_owned;
    static std::mutex         m_mutex;
    static std::atomic<bool>  m_destroyed;
};

template <typename T>
std::atomic<T*> ManagedSingleton_Modern<T>::m_instance{nullptr};

template <typename T>
std::unique_ptr<T> ManagedSingleton_Modern<T>::m_owned;

template <typename T>
std::mutex ManagedSingleton_Modern<T>::m_mutex;

template <typename T>
std::atomic<bool> ManagedSingleton_Modern<T>::m_destroyed{false};

template <typename T>
using ManagedSingleton = ManagedSingleton_Modern<T>;
#else
// NOTE: T must be default-constructible for fallback instance.
template <typename T>
class ManagedSingleton_Legacy
{
    static_assert(std::is_default_constructible<T>::value,
              "ManagedSingleton requires T to be default constructible");

public:
    template <typename... Args>
    static T& Instance(Args&&... args)
    {
        if (m_destroyed.load(std::memory_order_relaxed))
        {
            return DestroyedFallback();
        }

        std::lock_guard<std::mutex> lock(m_mutex);

        if (nullptr == m_instance)
        {
            if (m_destroyed.load(std::memory_order_relaxed))
            {
                return DestroyedFallback();
            }

            m_instance.reset(new T(std::forward<Args>(args)...));
        }

        return *m_instance;
    }

    static T* TryInstance()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_instance.get();
    }

    // Destroy() must only be called when no other threads are
    // accessing Instance() or TryInstance().
    static void Destroy()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_destroyed.store(true, std::memory_order_relaxed);
        m_instance.reset();
    }

private:
    static T& DestroyedFallback()
    {
        static T fallbackInstance{};
        return fallbackInstance;
    }

private:
    static std::unique_ptr<T> m_instance;
    static std::mutex m_mutex;
    static std::atomic<bool>  m_destroyed;
};

template <typename T>
std::unique_ptr<T> ManagedSingleton_Legacy<T>::m_instance{nullptr};

template <typename T>
std::mutex ManagedSingleton_Legacy<T>::m_mutex;

template <typename T>
std::atomic<bool> ManagedSingleton_Legacy<T>::m_destroyed{false};

template <typename T>
using ManagedSingleton = ManagedSingleton_Legacy<T>;
#endif

} // namespace core_lib

#endif // MANAGED_SINGLETON_HPP