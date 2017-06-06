#ifndef DISABLE_THREADS_TESTS

#include <ostream>
#include <memory>
#include <functional>
#include <map>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <future>
#include "Threads/SyncEvent.h"
#include "Threads/ThreadBase.h"
#include "Threads/ThreadRunner.h"
#include "Threads/ThreadGroup.h"
#include "Threads/ConcurrentQueue.h"
#include "Threads/MessageQueueThread.h"
#include "Threads/BoundedBuffer.h"

#include "gtest/gtest.h"

// ****************************************************************************
// Helper classes for tests
// ****************************************************************************
class ThreadTestHelper
{
public:
    void ThreadFunction()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_threadRunMap[std::this_thread::get_id()] = true;
    }

    bool GetThreadRunState(const std::thread::id& tId) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::map<std::thread::id, bool>::const_iterator t_it = m_threadRunMap.find(tId);

        if (t_it == m_threadRunMap.end())
            return false;
        else
            return t_it->second;
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_threadRunMap.clear();
    }

private:
    mutable std::mutex m_mutex;
    std::map<std::thread::id, bool> m_threadRunMap;
};

class ThreadTestHelper2
{
public:
    explicit ThreadTestHelper2(core_lib::threads::SyncEvent& event)
        : m_event(event)
    {
    }

    ~ThreadTestHelper2()
    {
        m_event.Signal();
    }

    void ThreadFunction1()
    {
        m_event.Wait();
        SetSignalled(std::this_thread::get_id(), true);
    }

    void ThreadFunction2()
    {
        SetSignalled(std::this_thread::get_id(), m_event.WaitForTime(1000));
    }

    bool GetEventSignalledState(const std::thread::id& tId) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::map<std::thread::id, bool>::const_iterator t_it = m_eventSignalledMap.find(tId);

        if (t_it == m_eventSignalledMap.end())
            return false;
        else
            return t_it->second;
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_eventSignalledMap.clear();
    }

private:
    core_lib::threads::SyncEvent& m_event;
    mutable std::mutex            m_mutex;
    std::map<std::thread::id, bool> m_eventSignalledMap;

    void SetSignalled(const std::thread::id& tId, bool signalled)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_eventSignalledMap[tId] = signalled;
    }
};

class ThreadDerived final : public core_lib::threads::ThreadBase
{
public:
    explicit ThreadDerived(bool& terminateCondition)
        : ThreadBase()
        , m_counter(0)
        , m_terminateCondition(terminateCondition)
    {
        // Do this last in constructor.
        Start();
    }

    virtual ~ThreadDerived()
    {
        // Do this first in destructor.
        Stop();
    }

    size_t GetCounter() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

private:
    mutable std::mutex           m_mutex;
    core_lib::threads::SyncEvent m_event;
    size_t                       m_counter;
    bool&                        m_terminateCondition;

    virtual void ThreadIteration() NO_EXCEPT_
    {
        if (!m_event.WaitForTime(100))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max() ? 0 : m_counter + 1;
        }
    }

    virtual void ProcessTerminationConditions() NO_EXCEPT_
    {
        m_terminateCondition = true;
        m_event.Signal();
    }
};

class ThreadRunnerWrapper final
{
public:
    ThreadRunnerWrapper(bool& terminateCondition)
        : m_threadRunner(std::bind(&ThreadRunnerWrapper::ThreadIteration, this),
                         std::bind(&ThreadRunnerWrapper::ProcessTerminationConditions, this))
        , m_counter(0)
        , m_terminateCondition(terminateCondition)
    {
        m_threadRunner.Start();
    }

    ~ThreadRunnerWrapper()
    {
        m_threadRunner.Stop();
    }

    size_t GetCounter() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

    bool IsStarted() const
    {
        return m_threadRunner.IsStarted();
    }

    bool Start()
    {
        return m_threadRunner.Start();
    }

    bool Stop()
    {
        return m_threadRunner.Stop();
    }

    std::thread::id ThreadID() const
    {
        return m_threadRunner.ThreadID();
    }

    std::thread::native_handle_type NativeHandle() const
    {
        return m_threadRunner.NativeHandle();
    }

private:
    core_lib::threads::ThreadRunner m_threadRunner;
    mutable std::mutex              m_mutex;
    core_lib::threads::SyncEvent    m_event;
    size_t                          m_counter;
    bool&                           m_terminateCondition;

    void ThreadIteration() NO_EXCEPT_
    {
        if (!m_event.WaitForTime(100))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max() ? 0 : m_counter + 1;
        }
    }

    void ProcessTerminationConditions() NO_EXCEPT_
    {
        m_terminateCondition = true;
        m_event.Signal();
    }
};

struct QueueMsg
{
    std::vector<int> data;

    QueueMsg()                = default;
    ~QueueMsg()               = default;
    QueueMsg(const QueueMsg&) = default;
    QueueMsg& operator=(const QueueMsg&) = default;
#ifdef USE_EXPLICIT_MOVE_
    QueueMsg(QueueMsg&& qm)
    {
        *this = std::move(qm);
    }
    QueueMsg& operator=(QueueMsg&& qm)
    {
        data.swap(qm.data);
        return *this;
    }
#else
    QueueMsg(QueueMsg&&) = default;
    QueueMsg& operator=(QueueMsg&&) = default;
#endif

    QueueMsg(size_t size, int value)
        : data(size, value)
    {
    }
};

static std::shared_ptr<QueueMsg> CreateQueueMsg(size_t size, int value)
{
    return std::make_shared<QueueMsg>(size, value);
}

static std::shared_ptr<char> CreateQueueArrayMsg(size_t size)
{
    return std::shared_ptr<char>(new char[size], core_lib::threads::ArrayDeleter<char>());
}

static QueueMsg CreateQueueMsgObj(size_t size, int value)
{
    return QueueMsg(size, value);
}

static QueueMsg* CreateQueueMsgPtr(size_t size, int value)
{
    return new QueueMsg(size, value);
}

template <typename T> class QueuedThread1 final : public core_lib::threads::ThreadBase
{
public:
    QueuedThread1()
        : ThreadBase()
        , m_counter(0)
    {
        // Do this last in constructor.
        Start();
    }

    virtual ~QueuedThread1()
    {
        // Do this first in destructor.
        Stop();
    }

    size_t GetCounter() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

    void Push(T&& item)
    {
        m_queue.Push(std::forward<T>(item));
    }

private:
    core_lib::threads::ConcurrentQueue<T> m_queue;
    mutable std::mutex                    m_mutex;
    size_t                                m_counter{};

    virtual void ThreadIteration() NO_EXCEPT_
    {
        T message{};

        if (!m_queue.Pop(message))
        {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max() ? 0 : m_counter + 1;
        }
    }

    virtual void ProcessTerminationConditions() NO_EXCEPT_
    {
        m_queue.BreakPopWait();
    }
};

class QueuedThread2 final : public core_lib::threads::ThreadBase
{
public:
    QueuedThread2()
        : ThreadBase()
        , m_counter(0)
    {
        // Do this last in constructor.
        Start();
    }

    virtual ~QueuedThread2()
    {
        // Do this first in destructor.
        Stop();
    }

    size_t GetCounter() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

    void Push(QueueMsg* item)
    {
        m_queue.Push(std::move(item));
    }

private:
    core_lib::threads::ConcurrentQueue<QueueMsg*> m_queue;
    mutable std::mutex                            m_mutex;
    size_t                                        m_counter{};

    virtual void ThreadIteration() NO_EXCEPT_
    {
        QueueMsg* message{nullptr};

        if (!m_queue.Pop(message))
        {
            return;
        }

        delete message;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max() ? 0 : m_counter + 1;
        }
    }

    virtual void ProcessTerminationConditions() NO_EXCEPT_
    {
        m_queue.BreakPopWait();
    }
};

class QueuedThread3 final : public core_lib::threads::ThreadBase
{
public:
    QueuedThread3(core_lib::threads::SyncEvent& readyEvent, size_t maxCount)
        : ThreadBase()
        , m_readyEvent(readyEvent)
        , m_maxCount(maxCount)
        , m_counter(0)
    {
        // Do this last in constructor.
        Start();
    }

    virtual ~QueuedThread3()
    {
        // Do this first in destructor.
        Stop();
    }

    size_t GetCounter() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

    void Push(QueueMsg* item)
    {
        m_queue.Push(std::move(item));
    }

private:
    core_lib::threads::SyncEvent&                 m_readyEvent;
    size_t                                        m_maxCount;
    core_lib::threads::ConcurrentQueue<QueueMsg*> m_queue;
    mutable std::mutex                            m_mutex;
    size_t                                        m_counter{};

    virtual void ThreadIteration() NO_EXCEPT_
    {
        QueueMsg* message{nullptr};

        if (!m_queue.Pop(message))
        {
            return;
        }

        delete message;
        size_t count = 0;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max() ? 0 : m_counter + 1;
            count     = m_counter;
        }

        if (count == m_maxCount)
        {
            m_readyEvent.Signal();
        }
    }

    virtual void ProcessTerminationConditions() NO_EXCEPT_
    {
        m_queue.BreakPopWait();
    }
};

class BoundedBufThread final : public core_lib::threads::ThreadBase
{
public:
    explicit BoundedBufThread(core_lib::threads::BoundedBuffer<int>& buf)
        : ThreadBase()
        , m_buf(buf)
        , m_blocked(false)
    {
        // Do this last in constructor.
        Start();
    }

    virtual ~BoundedBufThread()
    {
        // Do this first in destructor.
        Stop();
    }

    bool GetBlocked() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_blocked;
    }

private:
    core_lib::threads::BoundedBuffer<int>& m_buf;
    mutable std::mutex                     m_mutex;
    bool                                   m_blocked;

    virtual void ThreadIteration() NO_EXCEPT_
    {
        SetBlocked(true);
        int temp;
        m_buf.PopBack(temp);
        SetBlocked(false);
    }

    virtual void ProcessTerminationConditions() NO_EXCEPT_
    {
        if (GetBlocked())
        {
            m_buf.PushFront(0);
        }
    }

    void SetBlocked(bool blocked)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_blocked = blocked;
    }
};

class BoundedBufThread2 final : public core_lib::threads::ThreadBase
{
public:
    explicit BoundedBufThread2(core_lib::threads::BoundedBuffer<int>& buf)
        : ThreadBase()
        , m_buf(buf)
        , m_blocked(false)
    {
        // Do this last in constructor.
        Start();
    }

    virtual ~BoundedBufThread2()
    {
        // Do this first in destructor.
        Stop();
    }

    bool GetBlocked() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_blocked;
    }

private:
    core_lib::threads::BoundedBuffer<int>& m_buf;
    mutable std::mutex                     m_mutex;
    bool                                   m_blocked;

    virtual void ThreadIteration() NO_EXCEPT_
    {
        SetBlocked(true);
        m_buf.PushFront(6);
        SetBlocked(false);
    }

    virtual void ProcessTerminationConditions() NO_EXCEPT_
    {
        if (GetBlocked())
        {
            int temp;
            m_buf.PopBack(temp);
            (void)temp;
        }
    }

    void SetBlocked(bool blocked)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_blocked = blocked;
    }
};

class MessageQueueThreadTest final
{
public:
    enum MessageIds
    {
        UNKNOWN = -1,
        M1      = 0,
        M2,
        M3
    };

    struct Message
    {
        int id;

        Message()               = default;
        Message(const Message&) = default;
        Message& operator=(const Message&) = default;

#ifdef USE_EXPLICIT_MOVE_
        Message(Message&& m)
            : id(UNKNOWN)
        {
            *this = std::move(m);
        }
        Message& operator=(Message&& m)
        {
            std::swap(id, m.id);
            return *this;
        }
#else
        Message(Message&&)          = default;
        Message& operator=(Message&&) = default;
#endif

        explicit Message(int id_)
            : id(id_)
        {
        }
    };

    MessageQueueThreadTest()
        : m_mqt(std::bind(&MessageQueueThreadTest::MessageDecoder, std::placeholders::_1))
    {
        m_mqt.RegisterMessageHandler(
            M1, std::bind(&MessageQueueThreadTest::MessageHandler, this, std::placeholders::_1));
        m_mqt.RegisterMessageHandler(
            M2, std::bind(&MessageQueueThreadTest::MessageHandler, this, std::placeholders::_1));
        m_mqt.RegisterMessageHandler(
            M3, std::bind(&MessageQueueThreadTest::MessageHandler, this, std::placeholders::_1));
        m_countMap[M1] = 0;
        m_countMap[M2] = 0;
        m_countMap[M3] = 0;
    }

    ~MessageQueueThreadTest()
    {
    }

    void PushMessageId(MessageIds id)
    {
        m_mqt.Push(std::make_shared<Message>(id));
    }

    size_t CountMessageId(MessageIds id)
    {
        return m_countMap[id];
    }

private:
    typedef std::shared_ptr<Message> message_t;
    core_lib::threads::MessageQueueThread<int, message_t> m_mqt;
    std::map<int, size_t>                                 m_countMap;

    static int MessageDecoder(const message_t& message)
    {
        if (!message)
        {
            throw std::runtime_error("invalid message");
        }

        return message->id;
    }

    bool MessageHandler(message_t& message)
    {
        if (!message)
        {
            throw std::runtime_error("invalid message");
        }

        m_countMap[message->id] = m_countMap[message->id] + 1;
        return true;
    }
};

// ****************************************************************************
// ThreadGroup Fixture
// ****************************************************************************
namespace
{
// Fixture class for handling unit tests' shared data and setup.
class ThreadsTest : public ::testing::Test
{
protected:
    ThreadTestHelper m_helper;

protected:
    ThreadsTest()
    {
    }
};

} // End of unnamed namespace.

// ****************************************************************************
// ThreadGroup tests
// ****************************************************************************
TEST_F(ThreadsTest, testCase_ThreadGroup1)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    EXPECT_FALSE(tg.IsThisThreadIn());
}

TEST_F(ThreadsTest, testCase_ThreadGroup2)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread>   t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id                tId = t->get_id();
    EXPECT_FALSE(tg.IsThreadIn(tId));
    EXPECT_TRUE(tg.Empty());
    tg.AddThread(t.release());
    EXPECT_TRUE(tg.IsThreadIn(tId));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));
    tg.JoinAll();
    EXPECT_TRUE(m_helper.GetThreadRunState(tId));
}

TEST_F(ThreadsTest, testCase_ThreadGroup3)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread>   t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id                tId = t->get_id();
    EXPECT_FALSE(tg.IsThreadIn(t.get()));
    EXPECT_TRUE(tg.Empty());
    std::thread* temp = t.release();
    tg.AddThread(temp);
    EXPECT_TRUE(tg.IsThreadIn(temp));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));
    tg.JoinAll();
    EXPECT_TRUE(m_helper.GetThreadRunState(tId));
}

TEST_F(ThreadsTest, testCase_ThreadGroup4)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread>   t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id                tId = t->get_id();
    EXPECT_FALSE(tg.IsThreadIn(tId));
    EXPECT_TRUE(tg.Empty());
    tg.AddThread(t.release());
    EXPECT_TRUE(tg.IsThreadIn(tId));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));
    t.reset(tg.RemoveThread(tId));
    EXPECT_TRUE(tg.Empty());
    EXPECT_TRUE(t.get() != nullptr);
    t->join();
    EXPECT_TRUE(m_helper.GetThreadRunState(tId));
}

TEST_F(ThreadsTest, testCase_ThreadGroup5)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread>   t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id                tId = t->get_id();
    EXPECT_FALSE(tg.IsThreadIn(t.get()));
    EXPECT_TRUE(tg.Empty());
    tg.AddThread(t.get());
    EXPECT_TRUE(tg.IsThreadIn(t.get()));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));
    tg.RemoveThread(t.get());
    EXPECT_TRUE(tg.Empty());
    t->join();
    EXPECT_TRUE(m_helper.GetThreadRunState(tId));
}

TEST_F(ThreadsTest, testCase_ThreadGroup6)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::thread::id                tId =
        tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper))->get_id();
    EXPECT_TRUE(tg.IsThreadIn(tId));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));
    tg.JoinAll();
    EXPECT_TRUE(m_helper.GetThreadRunState(tId));
}

TEST_F(ThreadsTest, testCase_ThreadGroup7)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::vector<std::thread::id>   tIds;

    for (size_t i = 0; i < 10; ++i)
    {
        tIds.push_back(
            tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper))->get_id());
        EXPECT_TRUE(tg.IsThreadIn(tIds.back()));
    }

    EXPECT_EQ(tg.Size(), static_cast<size_t>(10));
    tg.JoinAll();
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_TRUE(m_helper.GetThreadRunState(tIds[i]));
    }
}

TEST_F(ThreadsTest, testCase_ThreadGroup8)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::thread*    t   = tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id tId = t->get_id();
    EXPECT_TRUE(tg.IsThreadIn(t));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));

    bool correctException = false;

    try
    {
        tg.AddThread(t);
    }
    catch (core_lib::threads::xThreadGroupError&)
    {
        correctException = true;
    }

    EXPECT_TRUE(correctException);

    tg.JoinAll();
    EXPECT_TRUE(m_helper.GetThreadRunState(tId));
}

// ****************************************************************************
// SyncEvent tests
// ****************************************************************************

TEST_F(ThreadsTest, testCase_SyncEvent1)
{
    core_lib::threads::SyncEvent   event;
    ThreadTestHelper2              helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
}

TEST_F(ThreadsTest, testCase_SyncEvent2)
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalOneThread,
                                       core_lib::threads::eResetCondition::autoReset,
                                       core_lib::threads::eIntialCondition::signalled);
    ThreadTestHelper2              helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    tg.JoinAll();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
}

TEST_F(ThreadsTest, testCase_SyncEvent3)
{
    core_lib::threads::SyncEvent   event;
    ThreadTestHelper2              helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction2, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    event.Signal();
    tg.JoinAll();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
}

TEST_F(ThreadsTest, testCase_SyncEvent4)
{
    core_lib::threads::SyncEvent   event;
    ThreadTestHelper2              helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction2, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    tg.JoinAll();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
}

TEST_F(ThreadsTest, testCase_SyncEvent5)
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalAllThreads,
                                       core_lib::threads::eResetCondition::manualReset,
                                       core_lib::threads::eIntialCondition::notSignalled);
    ThreadTestHelper2              helper(event);
    core_lib::threads::ThreadGroup tg;
    std::vector<std::thread::id>   tIds;

    for (size_t i = 0; i < 10; ++i)
    {
        tIds.push_back(
            tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper))->get_id());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        EXPECT_FALSE(helper.GetEventSignalledState(tIds[i]));
    }

    event.Signal();
    tg.JoinAll();

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_TRUE(helper.GetEventSignalledState(tIds[i]));
    }
}

TEST_F(ThreadsTest, testCase_SyncEvent6)
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalAllThreads,
                                       core_lib::threads::eResetCondition::manualReset,
                                       core_lib::threads::eIntialCondition::signalled);
    ThreadTestHelper2              helper(event);
    core_lib::threads::ThreadGroup tg;
    std::vector<std::thread::id>   tIds;

    for (size_t i = 0; i < 10; ++i)
    {
        tIds.push_back(
            tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper))->get_id());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    tg.JoinAll();

    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_TRUE(helper.GetEventSignalledState(tIds[i]));
    }
}

TEST_F(ThreadsTest, testCase_SyncEvent7)
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalOneThread,
                                       core_lib::threads::eResetCondition::manualReset,
                                       core_lib::threads::eIntialCondition::notSignalled);
    ThreadTestHelper2              helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    tg.RemoveThread(t);
    delete t;
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
    t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tId = t->get_id();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
    tg.JoinAll();
    tg.RemoveThread(t);
    delete t;
    event.Reset();
    helper.Clear();
    t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
}

// ****************************************************************************
// ThreadBase tests
// ****************************************************************************

TEST_F(ThreadsTest, testCase_ThreadBase)
{
    bool terminateCondition = false;

    {
        bool bigException = false;
        try
        {
            ThreadDerived td(terminateCondition);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            EXPECT_TRUE(td.IsStarted());
            bool exception = false;
            try
            {
                td.ThreadID();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_FALSE(exception);
            exception = false;
            try
            {
                td.NativeHandle();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_FALSE(exception);
            EXPECT_TRUE(td.Stop());
            exception = false;
            try
            {
                td.ThreadID();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_TRUE(exception);
            exception = false;
            try
            {
                td.NativeHandle();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_TRUE(exception);
            size_t c1 = td.GetCounter();
            EXPECT_TRUE(c1 > 0);
            EXPECT_TRUE(terminateCondition);
            terminateCondition = false;
            EXPECT_TRUE(td.Start());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            size_t c2 = td.GetCounter();
            EXPECT_TRUE(c1 != c2);
        }
        catch (...)
        {
            bigException = true;
        }
        EXPECT_FALSE(bigException);
    }

    EXPECT_TRUE(terminateCondition);
}

// ****************************************************************************
// ThreadRunner tests
// ****************************************************************************

TEST_F(ThreadsTest, testCase_ThreadRunner)
{
    bool terminateCondition = false;

    {
        bool bigException = false;
        try
        {
            ThreadRunnerWrapper td(terminateCondition);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            EXPECT_TRUE(td.IsStarted());
            bool exception = false;
            try
            {
                td.ThreadID();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_FALSE(exception);
            exception = false;
            try
            {
                td.NativeHandle();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_FALSE(exception);
            EXPECT_TRUE(td.Stop());
            exception = false;
            try
            {
                td.ThreadID();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_TRUE(exception);
            exception = false;
            try
            {
                td.NativeHandle();
            }
            catch (...)
            {
                exception = true;
            }
            EXPECT_TRUE(exception);
            size_t c1 = td.GetCounter();
            EXPECT_TRUE(c1 > 0);
            EXPECT_TRUE(terminateCondition);
            terminateCondition = false;
            EXPECT_TRUE(td.Start());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            size_t c2 = td.GetCounter();
            EXPECT_TRUE(c1 != c2);
        }
        catch (...)
        {
            bigException = true;
        }
        EXPECT_FALSE(bigException);
    }

    EXPECT_TRUE(terminateCondition);
}

// ****************************************************************************
// ConcurrentQueue tests
// ****************************************************************************

TEST_F(ThreadsTest, testCase_ConcurrentQueue1)
{
    QueuedThread1<std::shared_ptr<QueueMsg>> qt;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 0);
    qt.Push(CreateQueueMsg(10, 666));
    qt.Push(CreateQueueMsg(2, 666));
    qt.Push(CreateQueueMsg(5, 666));
    qt.Push(CreateQueueMsg(7, 666));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 4);
}

TEST_F(ThreadsTest, testCase_ConcurrentQueue2)
{
    QueuedThread1<std::shared_ptr<char>> qt;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 0);
    qt.Push(CreateQueueArrayMsg(10));
    qt.Push(CreateQueueArrayMsg(2));
    qt.Push(CreateQueueArrayMsg(5));
    qt.Push(CreateQueueArrayMsg(7));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 4);
}

TEST_F(ThreadsTest, testCase_ConcurrentQueue3)
{
    QueuedThread1<QueueMsg> qt;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 0);
    qt.Push(CreateQueueMsgObj(10, 666));
    qt.Push(CreateQueueMsgObj(2, 666));
    qt.Push(CreateQueueMsgObj(5, 666));
    qt.Push(CreateQueueMsgObj(7, 666));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 4);
}

TEST_F(ThreadsTest, testCase_ConcurrentQueue4)
{
    QueuedThread2 qt;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 0);
    qt.Push(CreateQueueMsgPtr(10, 666));
    qt.Push(CreateQueueMsgPtr(2, 666));
    qt.Push(CreateQueueMsgPtr(5, 666));
    qt.Push(CreateQueueMsgPtr(7, 666));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 4);
}

static bool CheckQueueMsg(const QueueMsg& msg, const int compValue = 666)
{

    return std::all_of(msg.data.begin(), msg.data.end(), [compValue](const int value) {
        return value == compValue;
    });
}

TEST_F(ThreadsTest, testCase_ConcurrentQueue5)
{
    core_lib::threads::ConcurrentQueue<std::shared_ptr<QueueMsg>> m_queue;
    EXPECT_TRUE(m_queue.Empty());
    m_queue.Push(CreateQueueMsg(2, 666));
    m_queue.Push(CreateQueueMsg(3, 666));
    m_queue.Push(CreateQueueMsg(4, 666));
    EXPECT_TRUE(m_queue.Size() == 3);

    const std::shared_ptr<QueueMsg>* pTempPtr = m_queue.Peek(0);

    EXPECT_TRUE(pTempPtr != nullptr);
    EXPECT_TRUE((*pTempPtr)->data.size() == 2);
    EXPECT_TRUE(CheckQueueMsg(*(*pTempPtr), 666));
    pTempPtr = m_queue.Peek(1);
    EXPECT_TRUE(pTempPtr != nullptr);
    EXPECT_TRUE((*pTempPtr)->data.size() == 3);
    EXPECT_TRUE(CheckQueueMsg(*(*pTempPtr), 666));
    pTempPtr = m_queue.Peek(2);
    EXPECT_TRUE(pTempPtr != nullptr);
    EXPECT_TRUE((*pTempPtr)->data.size() == 4);
    EXPECT_TRUE(CheckQueueMsg(*(*pTempPtr), 666));

    std::shared_ptr<QueueMsg> tempPtr;

    EXPECT_TRUE(m_queue.Pop(tempPtr));
    EXPECT_TRUE(tempPtr->data.size() == 2);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));

    EXPECT_TRUE(m_queue.Pop(tempPtr));
    EXPECT_TRUE(tempPtr->data.size() == 3);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));

    bool correctException;

    try
    {
        m_queue.PopThrow(tempPtr);
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
    EXPECT_TRUE(tempPtr->data.size() == 4);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));

    try
    {
        m_queue.BreakPopWait();
        m_queue.PopThrow(tempPtr);
        correctException = false;
    }
    catch (...)
    {
        correctException = true;
    }

    EXPECT_TRUE(correctException);

    EXPECT_TRUE(m_queue.Empty());
    m_queue.Push(CreateQueueMsg(2, 666));
    m_queue.Push(CreateQueueMsg(3, 666));
    m_queue.Push(CreateQueueMsg(4, 666));
    EXPECT_TRUE(m_queue.Size() == 3);

    EXPECT_TRUE(m_queue.TimedPop(100, tempPtr));
    EXPECT_TRUE(tempPtr->data.size() == 2);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));
    EXPECT_TRUE(m_queue.TimedPop(100, tempPtr));
    EXPECT_TRUE(tempPtr->data.size() == 3);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));
    EXPECT_TRUE(m_queue.TimedPop(100, tempPtr));
    EXPECT_TRUE(tempPtr->data.size() == 4);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));

    try
    {
        m_queue.TimedPopThrow(100, tempPtr);
        correctException = false;
    }
    catch (core_lib::threads::xQueuePopTimeoutError&)
    {
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);

    EXPECT_TRUE(m_queue.Empty());
    m_queue.Push(CreateQueueMsg(2, 666));

    EXPECT_TRUE(m_queue.TryPop(tempPtr));
    EXPECT_TRUE(tempPtr->data.size() == 2);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));
    EXPECT_TRUE(!m_queue.TryPop(tempPtr));

    EXPECT_TRUE(m_queue.Empty());
    m_queue.Push(CreateQueueMsg(2, 666));

    try
    {
        m_queue.TryPopThrow(tempPtr);
        correctException = true;
        EXPECT_TRUE(tempPtr->data.size() == 2);
        EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);

    try
    {
        m_queue.TryPopThrow(tempPtr);
        correctException = false;
    }
    catch (core_lib::threads::xQueuePopQueueEmptyError&)
    {
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);

    EXPECT_TRUE(m_queue.Empty());
    m_queue.Push(CreateQueueMsg(2, 666));
    m_queue.Push(CreateQueueMsg(4, 666));

    EXPECT_TRUE(m_queue.TrySteal(tempPtr));
    EXPECT_TRUE(tempPtr->data.size() == 4);
    EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));
    m_queue.Clear();

    EXPECT_TRUE(!m_queue.TrySteal(tempPtr));

    EXPECT_TRUE(m_queue.Empty());
    m_queue.Push(CreateQueueMsg(2, 666));
    m_queue.Push(CreateQueueMsg(4, 666));

    try
    {
        EXPECT_TRUE(m_queue.TrySteal(tempPtr));
        EXPECT_TRUE(tempPtr->data.size() == 4);
        EXPECT_TRUE(CheckQueueMsg(*tempPtr, 666));
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);
    m_queue.Clear();

    try
    {
        m_queue.TryStealThrow(tempPtr);
        correctException = false;
    }
    catch (core_lib::threads::xQueuePopQueueEmptyError&)
    {
        correctException = true;
    }
    catch (...)
    {
        correctException = false;
    }

    EXPECT_TRUE(correctException);

    EXPECT_TRUE(m_queue.Empty());
    m_queue.Push(CreateQueueMsg(2, 666));
    m_queue.Push(CreateQueueMsg(3, 666));
    m_queue.Push(CreateQueueMsg(4, 666));
    EXPECT_TRUE(m_queue.Size() == 3);

    auto q = m_queue.TakeAll();
    EXPECT_TRUE(m_queue.Empty());
    ASSERT_TRUE(q.size() == 3);

    EXPECT_TRUE(CheckQueueMsg(*q[0], 666));
    EXPECT_TRUE(CheckQueueMsg(*q[1], 666));
    EXPECT_TRUE(CheckQueueMsg(*q[2], 666));
}

TEST(QueueTest, testCase_ConcurrentQueue6)
{
    core_lib::threads::ConcurrentQueue<QueueMsg*> q;
    q.Push(CreateQueueMsgPtr(2, 666));
    q.Push(CreateQueueMsgPtr(3, 666));
    q.Push(CreateQueueMsgPtr(4, 666));
    EXPECT_TRUE(q.Size() == 3);

    q.Clear(core_lib::threads::SingleItemDeleter<QueueMsg>());
    EXPECT_TRUE(q.Empty());
}

TEST(QueueTest, testCase_ConcurrentQueue7)
{
	core_lib::threads::ConcurrentQueue<QueueMsg*> q;

	QueueMsg* m = CreateQueueMsgPtr(2, 666);

	q.Push(std::move(m));
	EXPECT_TRUE(q.Size() == 1);

	m = nullptr;
	EXPECT_TRUE(q.Pop(m));
	EXPECT_TRUE(m != nullptr);
	EXPECT_TRUE(q.Empty());
	EXPECT_TRUE(CheckQueueMsg(*m, 666));

	delete m;
}

TEST(QueueTest, testCase_ConcurrentQueue8)
{
	core_lib::threads::ConcurrentQueue<char*> q;

	char* m = new char[12];
	strcpy(m, "I AM A TEST");

	q.Push(std::move(m));
	EXPECT_TRUE(q.Size() == 1);

	m = nullptr;
	EXPECT_TRUE(q.Pop(m));
	EXPECT_TRUE(m != nullptr);
	EXPECT_TRUE(q.Empty());
	EXPECT_STREQ(m, "I AM A TEST");

	delete [] m;
}

TEST(QueueTest, testCase_ConcurrentQueue9)
{
	core_lib::threads::ConcurrentQueue<QueueMsg*> q;

	QueueMsg* m = CreateQueueMsgPtr(2, 666);

	q.Push(m);
	EXPECT_TRUE(q.Size() == 1);
	EXPECT_TRUE(CheckQueueMsg(*m, 666));

	m = nullptr;
	EXPECT_TRUE(q.Pop(m));
	EXPECT_TRUE(m != nullptr);
	EXPECT_TRUE(q.Empty());
	EXPECT_TRUE(CheckQueueMsg(*m, 666));

	delete m;
}

TEST(QueueTest, testCase_ConcurrentQueue10)
{
	core_lib::threads::ConcurrentQueue<char*> q;

	char* m = new char[12];
	strcpy(m, "I AM A TEST");

	q.Push(m);
	EXPECT_TRUE(q.Size() == 1);
	EXPECT_STREQ(m, "I AM A TEST");

	m = nullptr;
	EXPECT_TRUE(q.Pop(m));
	EXPECT_TRUE(m != nullptr);
	EXPECT_TRUE(q.Empty());
	EXPECT_STREQ(m, "I AM A TEST");

	delete [] m;
}

TEST(QueueStressTest, testCase_ConcurrentQueue11)
{
    int                          max_i         = 10000000;
    int                          max_i_quarter = 2500000;
    core_lib::threads::SyncEvent readyEvent;
    QueuedThread3                qt(readyEvent, max_i);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(qt.GetCounter() == 0);

    std::vector<std::future<void>> futures;

    auto func = [max_i_quarter, &qt](int i) {
        for (int k = 0; k < max_i_quarter; ++k)
        {
            qt.Push(CreateQueueMsgPtr(10, k + (i * max_i_quarter) + 1));
        }
    };

    // Spawn 4 async tasks (threads) and have each task generate
    // a quarter of our messages and push them onto the QueueThread
    // concurrently with the QueueThread trying to read messages as they arrive.
    for (int i = 0; i < 4; ++i)
    {
        futures.push_back(std::async(std::launch::async, func, i));
    }

    EXPECT_TRUE(readyEvent.WaitForTime(120000));
    size_t count = qt.GetCounter();
    EXPECT_TRUE(count == static_cast<size_t>(max_i));

    // Make sure our futures have all returned from the async calls.
    for (auto& f : futures)
    {
        f.get();
    }
}

// ****************************************************************************
// BoundedBuffer tests
// ****************************************************************************

TEST_F(ThreadsTest, testCase_BoundedBuffer1)
{
    core_lib::threads::BoundedBuffer<int> bb(10);
    bb.PushFront(6);
    int item;
    bb.PopBack(item);
    EXPECT_TRUE(item == 6);
}

TEST_F(ThreadsTest, testCase_BoundedBuffer2)
{
    core_lib::threads::BoundedBuffer<int> bb(10);
    BoundedBufThread                      bt(bb);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(bt.GetBlocked());
    bb.PushFront(1);
    bb.PushFront(2);
    bb.PushFront(3);
    bb.PushFront(4);
    bb.PushFront(5);
    bb.PushFront(6);
    bb.PushFront(7);
    bb.PushFront(8);
    bb.PushFront(9);
    bb.PushFront(10);
}

TEST_F(ThreadsTest, testCase_BoundedBuffer3)
{
    core_lib::threads::BoundedBuffer<int> bb(10);
    bb.PushFront(1);
    bb.PushFront(2);
    bb.PushFront(3);
    bb.PushFront(4);
    bb.PushFront(5);
    bb.PushFront(6);
    bb.PushFront(7);
    bb.PushFront(8);
    bb.PushFront(9);
    bb.PushFront(10);
    BoundedBufThread2 bt(bb);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(bt.GetBlocked());
    int item;
    bb.PopBack(item);
    EXPECT_TRUE(item == 1);
}

// ****************************************************************************
// MessageQueuetThread tests
// ****************************************************************************

TEST_F(ThreadsTest, testCase_MessageQueueThread1)
{
    MessageQueueThreadTest mqtt;
    mqtt.PushMessageId(MessageQueueThreadTest::M1);
    mqtt.PushMessageId(MessageQueueThreadTest::M2);
    mqtt.PushMessageId(MessageQueueThreadTest::M3);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M1) == 1);
    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M2) == 1);
    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M3) == 1);

    for (size_t i = 0; i < 10; ++i)
    {
        mqtt.PushMessageId(MessageQueueThreadTest::M1);
        mqtt.PushMessageId(MessageQueueThreadTest::M2);
        mqtt.PushMessageId(MessageQueueThreadTest::M3);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M1) == 11);
    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M2) == 11);
    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M3) == 11);
}

#endif // DISABLE_THREADS_TESTS
