/*!
 * \file ThreadBase.cpp
 * \brief File containing definition of ThreadBase class.
 */

#include "../ThreadBase.hpp"
#include <chrono>

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xThreadNotStartedError' definition
// ****************************************************************************
xThreadNotStartedError::xThreadNotStartedError()
    : exceptions::xCustomException("thread not started")
{
}

xThreadNotStartedError::xThreadNotStartedError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xThreadNotStartedError::~xThreadNotStartedError()
{
}

// ****************************************************************************
// 'class ThreadBase' definition
// ****************************************************************************

ThreadBase::~ThreadBase()
{
}

bool ThreadBase::IsStarted() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_started;
}

void ThreadBase::Start()
{
    if (!IsStarted() && !IsTerminating())
    {
        m_thread = std::thread(&ThreadBase::Run, this);
        SetStarted(true);
        SetThreadIdAndNativeHandle(m_thread.get_id()
                                   , m_thread.native_handle());
    }
}

void ThreadBase::Stop()
{
    if (IsStarted() && !IsTerminating())
    {
        SetTerminating(true);
        ProcessTerminationConditions();

        if (m_thread.joinable()) m_thread.join();

        SetTerminating(false);
    }
}

std::thread::id ThreadBase::ThreadID() const
{
    if (!IsStarted() || IsTerminating())
        BOOST_THROW_EXCEPTION(xThreadNotStartedError());

    std::lock_guard<std::mutex> lock(m_mutex);
    return m_threadId;
}

std::thread::native_handle_type ThreadBase::NativeHandle() const
{
    if (!IsStarted() || IsTerminating())
        BOOST_THROW_EXCEPTION(xThreadNotStartedError());

    std::lock_guard<std::mutex> lock(m_mutex);
    return m_nativeHandle;
}

ThreadBase::ThreadBase()
    : m_started(false),
      m_terminating(false)
{
}

bool ThreadBase::IsTerminating() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_terminating;
}

void ThreadBase::SleepForTime(unsigned int milliSecs) const
{
    if (!IsStarted() || IsTerminating())
        BOOST_THROW_EXCEPTION(xThreadNotStartedError());

    std::this_thread::sleep_for(std::chrono::milliseconds(milliSecs));
}

void ThreadBase::ProcessTerminationConditions()
{
    // nothing required here but override in derived class
}

void ThreadBase::SetThreadIdAndNativeHandle(const std::thread::id& threadId
                                            , const std::thread::native_handle_type&
                                                nativeHandle)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_threadId = threadId;
    m_nativeHandle = nativeHandle;
}

void ThreadBase::SetStarted(bool started)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_started = started;
}

void ThreadBase::SetTerminating(bool terminating)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_terminating = terminating;
}

void ThreadBase::Run()
{
    while (!IsTerminating())
        ThreadIteration();

    SetStarted(false);
}

} // namespace threads
} // namespace core_lib
