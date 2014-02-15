/*!
 * \file SyncEvent.cpp
 * \brief File containing definition of SyncEvent class.
 */

#include "../SyncEvent.hpp"
#include <chrono>

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class SyncEvent' definition
// ****************************************************************************
SyncEvent::SyncEvent()
    : m_signalAllThreads(false)
    , m_autoReset(true)
    , m_signalFlag(false)
{
}

SyncEvent::SyncEvent(eNotifyType notifyCondition
                     , eResetCondition resetCondition
                     , eIntialCondition initialCondition)
        : m_signalAllThreads(notifyCondition == eNotifyType::signalAllThreads)
        , m_autoReset(m_signalAllThreads
                      ? false
                      : resetCondition == eResetCondition::autoReset)
        , m_signalFlag(initialCondition == eIntialCondition::signalled)
{
}

SyncEvent::~SyncEvent()
{
}

void SyncEvent::Wait()
{
    std::unique_lock<std::mutex> lock(m_signalMutex);
    m_signalCondVar.wait(lock, [this]{ return m_signalFlag; });

    if (m_autoReset && m_signalFlag)
        m_signalFlag = false;
}

bool SyncEvent::WaitForTime(size_t milliseconds)
{
    std::unique_lock<std::mutex> lock(m_signalMutex);
    bool result
            = m_signalCondVar.wait_for(lock, std::chrono::milliseconds(milliseconds)
                                       , [this]{ return m_signalFlag; });

    //if  (!result)
    //    m_signalFlag = true;

    if (m_autoReset && m_signalFlag)
        m_signalFlag = false;

    return result;
}

void SyncEvent::Signal()
{
    {
        std::lock_guard<std::mutex> lock(m_signalMutex);
        m_signalFlag = true;
    }

    if (m_signalAllThreads)
        m_signalCondVar.notify_all();
    else
        m_signalCondVar.notify_one();
}

void SyncEvent::Reset()
{
    std::lock_guard<std::mutex> lock(m_signalMutex);

    if (!m_autoReset)
        m_signalFlag = false;
}

} // namespace threads
} // namespace core_lib
