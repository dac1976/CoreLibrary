/*!
 * \file ThreadGroup.cpp
 * \brief File containing thread group definition.
 */

#include "../ThreadGroup.hpp"
#include <algorithm>

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xThreadNotStartedError' definition
// ****************************************************************************
xThreadGroupError::xThreadGroupError()
    : exceptions::xCustomException("thread group error")
{
}

xThreadGroupError::xThreadGroupError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xThreadGroupError::~xThreadGroupError()
{
}

// ****************************************************************************
// 'class ThreadGroup' definition
// ****************************************************************************

ThreadGroup::ThreadGroup()
{
}

ThreadGroup::~ThreadGroup()
{
    std::for_each(m_threadGroup.begin()
                  , m_threadGroup.end()
                  , DeleteThread);
}

bool ThreadGroup::IsThisThreadIn() const
{
    return IsThreadIn(std::this_thread::get_id());
}

bool ThreadGroup::IsThreadIn(std::thread* threadPtr) const
{
    if (!threadPtr) return false;
    else return IsThreadIn(threadPtr->get_id());
}

bool ThreadGroup::IsThreadIn(const std::thread::id& id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (const auto threadPtr : m_threadGroup)
        if (threadPtr->get_id() == id) return true;

    return false;
}

void ThreadGroup::AddThread(std::thread* threadPtr)
{
    if (!threadPtr) return;

    if (IsThreadIn(threadPtr))
        BOOST_THROW_EXCEPTION(xThreadGroupError("thread already in group"));

    std::lock_guard<std::mutex> lock(m_mutex);
    m_threadGroup.push_back(threadPtr);
}

void ThreadGroup::RemoveThread(std::thread* threadPtr)
{
    if (!threadPtr) return;

    if (!IsThreadIn(threadPtr)) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    thread_list_iter threadIt = std::find(m_threadGroup.begin()
                                          , m_threadGroup.end()
                                          , threadPtr);
    if (threadIt != m_threadGroup.end())
        m_threadGroup.erase(threadIt);
}

std::thread* ThreadGroup::RemoveThread(const std::thread::id& id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::thread* t = nullptr;

    for (thread_list_iter tIt = m_threadGroup.begin()
         ; tIt != m_threadGroup.end()
         ; ++tIt)
    {
        if ((*tIt)->get_id() == id)
        {
            t = *tIt;
            m_threadGroup.erase(tIt);
            break;
        }
    }

    return t;
}

void ThreadGroup::JoinAll()
{
    if (IsThisThreadIn())
        BOOST_THROW_EXCEPTION(xThreadGroupError("thread cannot join itself"));

    std::lock_guard<std::mutex> lock(m_mutex);
    std::for_each(m_threadGroup.begin()
                  , m_threadGroup.end()
                  , JoinThread);
}

size_t ThreadGroup::Size() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_threadGroup.size();
}

bool ThreadGroup::Empty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_threadGroup.empty();
}

void ThreadGroup::DeleteThread(std::thread* threadPtr)
{
    delete threadPtr;
}

void ThreadGroup::JoinThread(std::thread* threadPtr)
{
    if (threadPtr->joinable()) threadPtr->join();
}

} // namespace threads
} // namespace core_lib
