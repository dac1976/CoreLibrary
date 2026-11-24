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
#include <cstring>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <thread>
#include "Threads/SyncEvent.h"
#include "Threads/PeriodicThread.h"
#include "Threads/ThreadBase.h"
#include "Threads/ThreadGroup.h"
#include "Threads/ConcurrentQueue.h"
#include "Threads/MessageQueueThread.h"
#include "Asio/IoContextThreadGroup.h"
#include "Threads/DeadlineTimer.h"
#include "gtest/gtest.h"
#include "gtest_cout.h"

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
        std::lock_guard<std::mutex>                     lock(m_mutex);
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
    mutable std::mutex              m_mutex;
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
        std::lock_guard<std::mutex>                     lock(m_mutex);
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
    core_lib::threads::SyncEvent&                 m_event;
    mutable std::mutex              m_mutex;
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
    mutable std::mutex m_mutex;
    core_lib::threads::SyncEvent     m_event;
    size_t             m_counter;
    bool&              m_terminateCondition;

    virtual void ThreadFunction() noexcept
    {
        if (!m_event.WaitForTime(100))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max() ? 0 : m_counter + 1;
        }
    }

    virtual void ProcessTerminationConditions() noexcept
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
    QueueMsg(QueueMsg&&)                 = default;
    QueueMsg& operator=(QueueMsg&&) = default;

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
    mutable std::mutex         m_mutex;
    size_t                     m_counter{};

    virtual void ThreadFunction() noexcept
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

    virtual void ProcessTerminationConditions() noexcept
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
    mutable std::mutex                 m_mutex;
    size_t                             m_counter{};

    virtual void ThreadFunction() noexcept
    {
        QueueMsg* message{};

        if (!m_queue.Pop(message) && message)
        {
            return;
        }

        delete message;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max() ? 0 : m_counter + 1;
        }
    }

    virtual void ProcessTerminationConditions() noexcept
    {
        m_queue.BreakPopWait();
    }
};

class QueuedThread3 final : public core_lib::threads::ThreadBase
{
public:
    QueuedThread3(core_lib::threads::SyncEvent& readyEvent, int maxCount)
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
    core_lib::threads::SyncEvent&                    m_readyEvent;
    int                                m_maxCount;
    core_lib::threads::ConcurrentQueue<QueueMsg*> m_queue;
    mutable std::mutex                 m_mutex;
    size_t                             m_counter{};

    virtual void ThreadFunction() noexcept
    {
        QueueMsg* message{};

        if (!m_queue.Pop(message) && message)
        {
            return;
        }

        delete message;
        int count = 0;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<int>::max() ? 0 : m_counter + 1;
            count     = static_cast<int>(m_counter);
        }

        if (count == m_maxCount)
        {
            m_readyEvent.Signal();
        }
    }

    virtual void ProcessTerminationConditions() noexcept
    {
        m_queue.BreakPopWait();
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
        Message(Message&&)                 = default;
        Message& operator=(Message&&) = default;

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

    ~MessageQueueThreadTest() {}

    void PushMessageId(MessageIds id)
    {
        m_mqt.Push(std::make_shared<Message>(id));
    }

    size_t CountMessageId(MessageIds id)
    {
        return m_countMap[id];
    }

private:
    typedef std::shared_ptr<Message>        message_t;
    core_lib::threads::MessageQueueThread<int, message_t> m_mqt;
    std::map<int, size_t>                   m_countMap;

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

class Sum
{
public:
    Sum()           = default;
    Sum(const Sum&) = delete;
    Sum& operator=(const Sum&) = delete;

    void Add(const uint64_t n)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_total += n;
        m_threadIds.insert(std::this_thread::get_id());
    }

    uint64_t Total() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_total;
    }

    size_t NumThreadsUsed() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_threadIds.size();
    }

private:
    mutable std::mutex        m_mutex;
    uint64_t                  m_total{0};
    std::set<std::thread::id> m_threadIds;
};

class DeadlineTimerHelper
{
public:
    DeadlineTimerHelper() = default;

    bool Wait(uint32_t timeout)
    {
        return m_event.WaitForTime(timeout);
    }

    void OnTimeOut()
    {
        m_event.Signal();
    }

private:
    core_lib::threads::SyncEvent m_event;
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
    ThreadsTest() {}
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
    core_lib::threads::ThreadGroup             tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id              tId = t->get_id();
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
    core_lib::threads::ThreadGroup             tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id              tId = t->get_id();
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
    core_lib::threads::ThreadGroup             tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id              tId = t->get_id();
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
    core_lib::threads::ThreadGroup             tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id              tId = t->get_id();
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
    std::thread::id  tId =
        tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper))->get_id();
    EXPECT_TRUE(tg.IsThreadIn(tId));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));
    tg.JoinAll();
    EXPECT_TRUE(m_helper.GetThreadRunState(tId));
}

TEST_F(ThreadsTest, testCase_ThreadGroup7)
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup             tg;
    std::vector<std::thread::id> tIds;

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
    std::thread*     t   = tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id  tId = t->get_id();
    EXPECT_TRUE(tg.IsThreadIn(t));
    EXPECT_EQ(tg.Size(), static_cast<size_t>(1));

    bool correctException = false;

    try
    {
        tg.AddThread(t);
    }
    catch (std::exception&)
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
    core_lib::threads::SyncEvent    event;
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup  tg;
    std::thread*      t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
}

TEST_F(ThreadsTest, testCase_SyncEvent2)
{
    core_lib::threads::SyncEvent    event(core_lib::threads::eNotifyType::signalOneThread,
                         core_lib::threads::eResetCondition::autoReset,
                         core_lib::threads::eIntialCondition::signalled);
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup  tg;
    std::thread*      t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    tg.JoinAll();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
}

TEST_F(ThreadsTest, testCase_SyncEvent3)
{
    core_lib::threads::SyncEvent    event;
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup  tg;
    std::thread*      t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction2, &helper));
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
    core_lib::threads::SyncEvent    event;
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup  tg;
    std::thread*      t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction2, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    tg.JoinAll();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
}

TEST_F(ThreadsTest, testCase_SyncEvent5)
{
    core_lib::threads::SyncEvent               event(core_lib::threads::eNotifyType::signalAllThreads,
                         core_lib::threads::eResetCondition::manualReset,
                         core_lib::threads::eIntialCondition::notSignalled);
    ThreadTestHelper2            helper(event);
    core_lib::threads::ThreadGroup             tg;
    std::vector<std::thread::id> tIds;

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
    core_lib::threads::SyncEvent               event(core_lib::threads::eNotifyType::signalAllThreads,
                         core_lib::threads::eResetCondition::manualReset,
                         core_lib::threads::eIntialCondition::signalled);
    ThreadTestHelper2            helper(event);
    core_lib::threads::ThreadGroup             tg;
    std::vector<std::thread::id> tIds;

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
						 
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup  tg;
    std::thread*      t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread::id tId = t->get_id();
    EXPECT_FALSE(helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
    t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tId = t->get_id();
    EXPECT_TRUE(helper.GetEventSignalledState(tId));
    tg.JoinAll();
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

TEST(ThreadsTest_Alt, testCase_SyncEvent8)
{
    core_lib::threads::SyncEvent event;

    auto startTime = std::chrono::steady_clock::now();

    auto wakeupTime = startTime + std::chrono::seconds{3};
    bool result     = false;
    ASSERT_NO_THROW(result = event.WaitUntilTime(wakeupTime));
    EXPECT_FALSE(result);

    auto endTime = std::chrono::steady_clock::now();

    using namespace std::chrono_literals;

    auto elapsed = endTime - startTime;

    bool approximatelyThreeSeconds = std::chrono::abs(elapsed - 3s) <= 10ms;
    EXPECT_TRUE(approximatelyThreeSeconds);
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
    catch (std::exception&)
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
    catch (std::exception&)
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
    catch (std::exception&)
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

    delete[] m;
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

    delete[] m;
}

TEST(QueueStressTest, testCase_ConcurrentQueue11)
{
    int            max_i         = 10000000;
    int            max_i_quarter = 2500000;
    core_lib::threads::SyncEvent readyEvent;
    QueuedThread3  qt(readyEvent, max_i);
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

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M1) == 11);
    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M2) == 11);
    EXPECT_TRUE(mqtt.CountMessageId(MessageQueueThreadTest::M3) == 11);
}

// ****************************************************************************
// DeadlineTimer tests
// ****************************************************************************
TEST(TimerTest, test_DeadlineTimer_1)
{
    DeadlineTimerHelper helper;
    core_lib::threads::DeadlineTimer  timer;
    timer.Start(1000, std::bind(&DeadlineTimerHelper::OnTimeOut, &helper));
    EXPECT_TRUE(helper.Wait(2000));
}

TEST(TimerTest, test_DeadlineTimer_2)
{
    DeadlineTimerHelper helper;
    core_lib::threads::DeadlineTimer  timer;
    timer.Start(1000, std::bind(&DeadlineTimerHelper::OnTimeOut, &helper));
    timer.Cancel();
    EXPECT_FALSE(helper.Wait(1000));
}

TEST(TimerTest, test_DeadlineTimer_3)
{
    DeadlineTimerHelper helper1;
    core_lib::threads::DeadlineTimer  timer;
    timer.Start(1000, std::bind(&DeadlineTimerHelper::OnTimeOut, &helper1));
    EXPECT_TRUE(helper1.Wait(2000));

    DeadlineTimerHelper helper2;
    timer.Start(1000, std::bind(&DeadlineTimerHelper::OnTimeOut, &helper2));
    EXPECT_TRUE(helper2.Wait(2000));
}

// ****************************************************************************
// Asio tests
// ****************************************************************************
TEST(AsioTest, testCase_IoThreadGroup1)
{
    Sum sum1{};
    Sum sum2{};

    {
        core_lib::asio::IoContextThreadGroup ioThreadGroup(8);

        for (uint64_t i = 1; i <= 1000000; ++i)
        {
            core_lib::asio_compat::post(ioThreadGroup.IoService(), std::bind(&Sum::Add, &sum1, i));
            core_lib::asio_compat::post(ioThreadGroup.IoService(), std::bind(&Sum::Add, &sum2, i));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    EXPECT_TRUE(sum1.Total() == static_cast<uint64_t>(500000500000));
    EXPECT_TRUE(sum2.Total() == static_cast<uint64_t>(500000500000));
    EXPECT_TRUE(sum1.NumThreadsUsed() == 8);
    EXPECT_TRUE(sum2.NumThreadsUsed() == 8);
}

TEST(AsioTest, testCase_IoThreadGroup2)
{
    Sum sum1{};
    Sum sum2{};

    {
        core_lib::asio::IoContextThreadGroup ioThreadGroup(8);

        for (uint64_t i = 1; i <= 1000000; ++i)
        {
            core_lib::asio_compat::post(ioThreadGroup.IoService(), std::bind(&Sum::Add, &sum1, i));
            core_lib::asio_compat::post(ioThreadGroup.IoService(), std::bind(&Sum::Add, &sum2, i));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    EXPECT_TRUE(sum1.Total() == static_cast<uint64_t>(500000500000));
    EXPECT_TRUE(sum2.Total() == static_cast<uint64_t>(500000500000));
    EXPECT_TRUE(sum1.NumThreadsUsed() == 8);
    EXPECT_TRUE(sum2.NumThreadsUsed() == 8);
}

// ****************************************************************************
// PeriodicThread tests
// ****************************************************************************
namespace
{

using PeriodicTestClock = std::chrono::steady_clock;

class PeriodicTickRecorder
{
public:
    void Record(core_lib::threads::PeriodicTickInfo const& info)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_ticks.push_back(info);
        }

        m_condition.notify_all();
    }

    bool WaitForTicks(std::size_t               count,
                      std::chrono::milliseconds timeout = std::chrono::milliseconds(5000))
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        return m_condition.wait_for(
            lock, timeout, [this, count]() { return m_ticks.size() >= count; });
    }

    std::vector<core_lib::threads::PeriodicTickInfo> Ticks() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_ticks;
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_ticks.clear();
    }

private:
    mutable std::mutex                 m_mutex;
    std::condition_variable            m_condition;
    std::vector<core_lib::threads::PeriodicTickInfo> m_ticks;
};

core_lib::threads::PeriodicThreadSettings
MakePeriodicTestSettings(int64_t            period   = 20,
                         core_lib::threads::eWaitTimeUnit timeUnit = core_lib::threads::eWaitTimeUnit::milliseconds)
{
    core_lib::threads::PeriodicThreadSettings settings;

    settings.period        = period;
    settings.timeUnit      = timeUnit;
    settings.delayedStart  = true;
    settings.overrunPolicy = core_lib::threads::eOverrunPolicy::skipMissedTicks;

    return settings;
}

TEST(PeriodicThreadTests, ConstructWithSimpleCallback)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallback = []() {};

    EXPECT_NO_THROW({
        core_lib::threads::PeriodicThread thread(settings);
        EXPECT_FALSE(thread.IsStarted());
    });
}

TEST(PeriodicThreadTests, ConstructWithExtendedCallback)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallbackEx = [](core_lib::threads::PeriodicTickInfo const&) {};

    EXPECT_NO_THROW({
        core_lib::threads::PeriodicThread thread(settings);
        EXPECT_FALSE(thread.IsStarted());
    });
}

TEST(PeriodicThreadTests, RejectMissingCallback)
{
    auto settings = MakePeriodicTestSettings();

    EXPECT_THROW(core_lib::threads::PeriodicThread thread(settings), std::invalid_argument);
}

TEST(PeriodicThreadTests, RejectMultipleCallbacks)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallback = []() {};

    settings.eventCallbackEx = [](core_lib::threads::PeriodicTickInfo const&) {};

    EXPECT_THROW(core_lib::threads::PeriodicThread thread(settings), std::invalid_argument);
}

TEST(PeriodicThreadTests, RejectZeroPeriod)
{
    auto settings = MakePeriodicTestSettings(0);

    settings.eventCallback = []() {};

    EXPECT_THROW(core_lib::threads::PeriodicThread thread(settings), std::invalid_argument);
}

TEST(PeriodicThreadTests, RejectNegativePeriod)
{
    auto settings = MakePeriodicTestSettings(-100);

    settings.eventCallback = []() {};

    EXPECT_THROW(core_lib::threads::PeriodicThread thread(settings), std::invalid_argument);
}

TEST(PeriodicThreadTests, RejectInvalidTimeUnit)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallback = []() {};
    settings.timeUnit      = static_cast<core_lib::threads::eWaitTimeUnit>(999);

    EXPECT_THROW(core_lib::threads::PeriodicThread thread(settings), std::invalid_argument);
}

TEST(PeriodicThreadTests, RejectDurationOverflow)
{
    auto settings = MakePeriodicTestSettings((std::numeric_limits<int64_t>::max)(),
                                             core_lib::threads::eWaitTimeUnit::seconds);

    settings.eventCallback = []() {};

    EXPECT_THROW(core_lib::threads::PeriodicThread thread(settings), std::overflow_error);
}

TEST(PeriodicThreadTests, AcceptLargeValidPeriod)
{
    auto settings = MakePeriodicTestSettings(86400, core_lib::threads::eWaitTimeUnit::seconds);

    settings.eventCallback = []() {};

    EXPECT_NO_THROW({ core_lib::threads::PeriodicThread thread(settings); });
}

TEST(PeriodicThreadTests, GetConfiguredPeriod)
{
    auto settings = MakePeriodicTestSettings(4992, core_lib::threads::eWaitTimeUnit::microseconds);

    settings.eventCallback = []() {};

    core_lib::threads::PeriodicThread thread(settings);

    core_lib::threads::eWaitTimeUnit unit = core_lib::threads::eWaitTimeUnit::seconds;

    EXPECT_EQ(4992, thread.Period(&unit));
    EXPECT_EQ(core_lib::threads::eWaitTimeUnit::microseconds, unit);

    EXPECT_EQ(4992, thread.Period());
}

TEST(PeriodicThreadTests, SetPeriodAndTimeUnit)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallback = []() {};

    core_lib::threads::PeriodicThread thread(settings);

    thread.SetPeriod(2500, core_lib::threads::eWaitTimeUnit::microseconds);

    core_lib::threads::eWaitTimeUnit unit;

    EXPECT_EQ(2500, thread.Period(&unit));
    EXPECT_EQ(core_lib::threads::eWaitTimeUnit::microseconds, unit);

    thread.SetPeriod(2, core_lib::threads::eWaitTimeUnit::seconds);

    EXPECT_EQ(2, thread.Period(&unit));
    EXPECT_EQ(core_lib::threads::eWaitTimeUnit::seconds, unit);
}

TEST(PeriodicThreadTests, InvalidPeriodChangePreservesSettings)
{
    auto settings = MakePeriodicTestSettings(100, core_lib::threads::eWaitTimeUnit::milliseconds);

    settings.eventCallback = []() {};

    core_lib::threads::PeriodicThread thread(settings);

    EXPECT_THROW(thread.SetPeriod(0), std::invalid_argument);

    EXPECT_THROW(thread.SetPeriod(-1), std::invalid_argument);

    EXPECT_THROW(
        thread.SetPeriod((std::numeric_limits<int64_t>::max)(), core_lib::threads::eWaitTimeUnit::seconds),
        std::overflow_error);

    core_lib::threads::eWaitTimeUnit unit;

    EXPECT_EQ(100, thread.Period(&unit));
    EXPECT_EQ(core_lib::threads::eWaitTimeUnit::milliseconds, unit);
}

TEST(PeriodicThreadTests, SetOverrunPolicy)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallback = []() {};

    core_lib::threads::PeriodicThread thread(settings);

    EXPECT_EQ(core_lib::threads::eOverrunPolicy::skipMissedTicks, thread.OverrunPolicy());

    thread.SetOverrunPolicy(core_lib::threads::eOverrunPolicy::catchUp);

    EXPECT_EQ(core_lib::threads::eOverrunPolicy::catchUp, thread.OverrunPolicy());

    thread.SetOverrunPolicy(core_lib::threads::eOverrunPolicy::resynchronise);

    EXPECT_EQ(core_lib::threads::eOverrunPolicy::resynchronise, thread.OverrunPolicy());

    thread.SetOverrunPolicy(core_lib::threads::eOverrunPolicy::skipMissedTicks);

    EXPECT_EQ(core_lib::threads::eOverrunPolicy::skipMissedTicks, thread.OverrunPolicy());
}

TEST(PeriodicThreadTests, SetMaximumCatchUpTicks)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallback   = []() {};
    settings.maxCatchUpTicks = 4;

    core_lib::threads::PeriodicThread thread(settings);

    EXPECT_EQ(4U, thread.MaxCatchUpTicks());

    thread.SetMaxCatchUpTicks(10);

    EXPECT_EQ(10U, thread.MaxCatchUpTicks());

    thread.SetMaxCatchUpTicks(0);

    EXPECT_EQ(0U, thread.MaxCatchUpTicks());
}

TEST(PeriodicThreadTests, DelayedStart)
{
    std::atomic<uint32_t> count{0};

    auto settings = MakePeriodicTestSettings(10);

    settings.eventCallback = [&count]() { ++count; };

    core_lib::threads::PeriodicThread thread(settings);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_FALSE(thread.IsStarted());
    EXPECT_EQ(0U, count.load());
}

TEST(PeriodicThreadTests, AutomaticStart)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.delayedStart = false;

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    EXPECT_TRUE(thread.IsStarted());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());
    EXPECT_TRUE(received);
    EXPECT_FALSE(thread.IsStarted());
}

TEST(PeriodicThreadTests, SimpleCallbackExecution)
{
    std::mutex              mutex;
    std::condition_variable condition;
    uint32_t                count = 0;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallback = [&]()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            ++count;
        }

        condition.notify_all();
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool received;

    {
        std::unique_lock<std::mutex> lock(mutex);

        received =
            condition.wait_for(lock, std::chrono::seconds(5), [&count]() { return count >= 5; });
    }

    EXPECT_TRUE(thread.Stop());
    EXPECT_TRUE(received);
    EXPECT_GE(count, 5U);
}

TEST(PeriodicThreadTests, ExtendedCallbackExecution)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());

    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 5U);

    EXPECT_EQ(0U, ticks.front().tickIndex);
    EXPECT_EQ(0U, ticks.front().skippedTicks);
    EXPECT_FALSE(ticks.front().catchUpTick);
    EXPECT_FALSE(ticks.front().resynchronised);

    EXPECT_EQ(core_lib::threads::PeriodicTickInfo::duration_t::zero(), ticks.front().previousCallbackDuration);

    for (auto const& tick : ticks)
    {
        EXPECT_EQ(std::chrono::milliseconds(20), tick.period);

        EXPECT_GE(tick.actualTime, tick.scheduledTime);

        EXPECT_EQ(tick.actualTime - tick.scheduledTime, tick.wakeupLatency);
    }
}

TEST(PeriodicThreadTests, NoCallbacksAfterStop)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(10);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    ASSERT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const count = recorder.Ticks().size();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(count, recorder.Ticks().size());
    EXPECT_FALSE(thread.IsStarted());
}

TEST(PeriodicThreadTests, RepeatedStartAndStop)
{
    auto settings = MakePeriodicTestSettings();

    settings.eventCallback = []() {};

    core_lib::threads::PeriodicThread thread(settings);

    EXPECT_TRUE(thread.Start());
    EXPECT_TRUE(thread.Start());

    EXPECT_TRUE(thread.IsStarted());

    EXPECT_TRUE(thread.Stop());
    EXPECT_TRUE(thread.Stop());

    EXPECT_FALSE(thread.IsStarted());
}

TEST(PeriodicThreadTests, AbsoluteDeadlineProgression)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(20);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 20U);

    auto const referenceTime = ticks.front().scheduledTime;

    for (auto const& tick : ticks)
    {
        auto const expectedTime =
            referenceTime + std::chrono::milliseconds(20 * static_cast<int64_t>(tick.tickIndex));

        EXPECT_EQ(expectedTime, tick.scheduledTime);
    }
}

TEST(PeriodicThreadTests, CallbackDoesNotAccumulateDrift)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(30);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 30U);

    auto const& first = ticks.front();
    auto const& last  = ticks[29];

    auto const expectedElapsed =
        std::chrono::milliseconds(20 * static_cast<int64_t>(last.tickIndex - first.tickIndex));

    auto const scheduledElapsed = last.scheduledTime - first.scheduledTime;

    auto const actualElapsed = last.actualTime - first.actualTime;

    EXPECT_EQ(expectedElapsed, scheduledElapsed);

    auto const error = actualElapsed - expectedElapsed;

    // Allow for operating system scheduling latency.
    EXPECT_GE(error, -std::chrono::milliseconds(50));
    EXPECT_LE(error, std::chrono::milliseconds(50));
}

TEST(PeriodicThreadTests, PreviousCallbackDuration)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(100);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 5U);

    EXPECT_EQ(core_lib::threads::PeriodicTickInfo::duration_t::zero(), ticks.front().previousCallbackDuration);

    for (std::size_t i = 1; i < ticks.size(); ++i)
    {
        EXPECT_GE(ticks[i].previousCallbackDuration, std::chrono::milliseconds(20));
    }
}

TEST(PeriodicThreadTests, UnlimitedCatchUp)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.overrunPolicy   = core_lib::threads::eOverrunPolicy::catchUp;
    settings.maxCatchUpTicks = 0;

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        if (info.tickIndex == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(10);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 10U);

    EXPECT_FALSE(ticks[0].catchUpTick);
    EXPECT_TRUE(ticks[1].catchUpTick);
    EXPECT_TRUE(ticks[2].catchUpTick);
    EXPECT_TRUE(ticks[3].catchUpTick);

    for (std::size_t i = 0; i < ticks.size(); ++i)
    {
        EXPECT_EQ(static_cast<uint64_t>(i), ticks[i].tickIndex);
        EXPECT_EQ(0U, ticks[i].skippedTicks);
        EXPECT_FALSE(ticks[i].resynchronised);

        EXPECT_EQ(ticks.front().scheduledTime +
                      std::chrono::milliseconds(20 * static_cast<int64_t>(i)),
                  ticks[i].scheduledTime);
    }
}

TEST(PeriodicThreadTests, BoundedCatchUp)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.overrunPolicy   = core_lib::threads::eOverrunPolicy::catchUp;
    settings.maxCatchUpTicks = 2;

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        if (info.tickIndex == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(8);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 8U);

    EXPECT_EQ(0U, ticks[0].tickIndex);
    EXPECT_FALSE(ticks[0].catchUpTick);

    EXPECT_EQ(1U, ticks[1].tickIndex);
    EXPECT_TRUE(ticks[1].catchUpTick);

    EXPECT_EQ(2U, ticks[2].tickIndex);
    EXPECT_TRUE(ticks[2].catchUpTick);

    // Catch-up limit reached: remaining expired ticks skipped.
    EXPECT_FALSE(ticks[3].catchUpTick);
    EXPECT_GT(ticks[3].skippedTicks, 0U);

    EXPECT_EQ(ticks[2].tickIndex + 1 + ticks[3].skippedTicks, ticks[3].tickIndex);

    uint32_t consecutiveCatchUpTicks = 0;

    for (auto const& tick : ticks)
    {
        if (tick.catchUpTick)
        {
            ++consecutiveCatchUpTicks;

            EXPECT_LE(consecutiveCatchUpTicks, 2U);
        }
        else
        {
            consecutiveCatchUpTicks = 0;
        }

        EXPECT_FALSE(tick.resynchronised);

        EXPECT_EQ(ticks.front().scheduledTime +
                      std::chrono::milliseconds(20 * static_cast<int64_t>(tick.tickIndex)),
                  tick.scheduledTime);
    }

    // The temporary recovery policy must not modify settings.
    EXPECT_EQ(core_lib::threads::eOverrunPolicy::catchUp, thread.OverrunPolicy());
}

TEST(PeriodicThreadTests, SkipMissedTicks)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.overrunPolicy = core_lib::threads::eOverrunPolicy::skipMissedTicks;

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        if (info.tickIndex == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 5U);

    EXPECT_EQ(0U, ticks[0].tickIndex);
    EXPECT_GE(ticks[1].skippedTicks, 7U);

    for (std::size_t i = 1; i < ticks.size(); ++i)
    {
        auto const& previous = ticks[i - 1];
        auto const& current  = ticks[i];

        EXPECT_EQ(previous.tickIndex + 1 + current.skippedTicks, current.tickIndex);

        EXPECT_EQ(ticks.front().scheduledTime +
                      std::chrono::milliseconds(20 * static_cast<int64_t>(current.tickIndex)),
                  current.scheduledTime);

        EXPECT_FALSE(current.catchUpTick);
        EXPECT_FALSE(current.resynchronised);
    }
}

TEST(PeriodicThreadTests, ResynchroniseAfterOverrun)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.overrunPolicy = core_lib::threads::eOverrunPolicy::resynchronise;

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        if (info.tickIndex == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 5U);

    EXPECT_FALSE(ticks[0].resynchronised);
    EXPECT_TRUE(ticks[1].resynchronised);

    EXPECT_FALSE(ticks[1].catchUpTick);
    EXPECT_GE(ticks[1].skippedTicks, 7U);

    auto const expectedDeadline =
        ticks[0].actualTime + ticks[1].previousCallbackDuration + std::chrono::milliseconds(20);

    EXPECT_EQ(expectedDeadline, ticks[1].scheduledTime);

    for (std::size_t i = 1; i < ticks.size(); ++i)
    {
        auto const& previous = ticks[i - 1];
        auto const& current  = ticks[i];

        EXPECT_EQ(previous.tickIndex + 1 + current.skippedTicks, current.tickIndex);

        EXPECT_FALSE(current.catchUpTick);

        if (!current.resynchronised)
        {
            EXPECT_EQ(previous.scheduledTime + std::chrono::milliseconds(20),
                      current.scheduledTime);
        }
    }
}

TEST(PeriodicThreadTests, SustainedOverload)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(10);

    settings.overrunPolicy   = core_lib::threads::eOverrunPolicy::catchUp;
    settings.maxCatchUpTicks = 2;

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(15);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 15U);

    uint32_t consecutiveCatchUpTicks = 0;
    uint64_t totalSkippedTicks       = 0;
    uint64_t totalCatchUpTicks       = 0;

    for (auto const& tick : ticks)
    {
        totalSkippedTicks += tick.skippedTicks;

        if (tick.catchUpTick)
        {
            ++totalCatchUpTicks;
            ++consecutiveCatchUpTicks;

            EXPECT_LE(consecutiveCatchUpTicks, 2U);
        }
        else
        {
            consecutiveCatchUpTicks = 0;
        }
    }

    EXPECT_GT(totalSkippedTicks, 0U);
    EXPECT_GT(totalCatchUpTicks, 0U);
}

TEST(PeriodicThreadTests, ChangePeriodWhileRunning)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    ASSERT_TRUE(recorder.WaitForTicks(5));

    thread.SetPeriod(40, core_lib::threads::eWaitTimeUnit::milliseconds);

    auto const targetCount = recorder.Ticks().size() + 10;

    bool const received = recorder.WaitForTicks(targetCount);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    EXPECT_EQ(std::chrono::milliseconds(20), ticks.front().period);

    EXPECT_EQ(std::chrono::milliseconds(40), ticks.back().period);

    bool newPeriodObserved = false;

    for (std::size_t i = 0; i < ticks.size(); ++i)
    {
        auto const& tick = ticks[i];

        if (tick.period == std::chrono::milliseconds(40))
        {
            newPeriodObserved = true;
        }

        if (newPeriodObserved)
        {
            EXPECT_EQ(std::chrono::milliseconds(40), tick.period);
        }

        if (i > 0)
        {
            auto const& previous = ticks[i - 1];

            auto const expectedInterval =
                previous.period * static_cast<int64_t>(tick.tickIndex - previous.tickIndex);

            EXPECT_EQ(expectedInterval, tick.scheduledTime - previous.scheduledTime);
        }
    }

    EXPECT_TRUE(newPeriodObserved);
}

TEST(PeriodicThreadTests, ChangeSettingsFromCallback)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    core_lib::threads::PeriodicThread* threadPtr = nullptr;

    settings.eventCallbackEx = [&recorder, &threadPtr](core_lib::threads::PeriodicTickInfo const& info)
    {
        if (info.tickIndex == 0)
        {
            threadPtr->SetPeriod(40, core_lib::threads::eWaitTimeUnit::milliseconds);

            threadPtr->SetOverrunPolicy(core_lib::threads::eOverrunPolicy::catchUp);

            threadPtr->SetMaxCatchUpTicks(3);
        }

        recorder.Record(info);
    };

    core_lib::threads::PeriodicThread thread(settings);

    threadPtr = &thread;

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 5U);

    EXPECT_EQ(std::chrono::milliseconds(20), ticks[0].period);

    EXPECT_EQ(std::chrono::milliseconds(40), ticks[1].period);

    EXPECT_EQ(core_lib::threads::eOverrunPolicy::catchUp, thread.OverrunPolicy());

    EXPECT_EQ(3U, thread.MaxCatchUpTicks());
}

TEST(PeriodicThreadTests, ChangeOverrunPolicyWhileRunning)
{
    PeriodicTickRecorder recorder;

    std::atomic<bool> induceOverrun{false};

    auto settings = MakePeriodicTestSettings(20);

    settings.overrunPolicy = core_lib::threads::eOverrunPolicy::catchUp;

    settings.eventCallbackEx = [&recorder, &induceOverrun](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        if (induceOverrun.exchange(false))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());
    ASSERT_TRUE(recorder.WaitForTicks(5));

    thread.SetOverrunPolicy(core_lib::threads::eOverrunPolicy::resynchronise);

    // Allow the worker to adopt the new policy before
    // deliberately inducing an overrun.
    auto targetCount = recorder.Ticks().size() + 3;

    ASSERT_TRUE(recorder.WaitForTicks(targetCount));

    induceOverrun.store(true);

    targetCount = recorder.Ticks().size() + 5;

    bool const received = recorder.WaitForTicks(targetCount);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    bool resynchronisationObserved = false;

    for (auto const& tick : ticks)
    {
        if (tick.resynchronised)
        {
            resynchronisationObserved = true;
            EXPECT_FALSE(tick.catchUpTick);
            EXPECT_GT(tick.skippedTicks, 0U);
        }
    }

    EXPECT_TRUE(resynchronisationObserved);
}

TEST(PeriodicThreadTests, CallbackExceptionDoesNotStopThread)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        if (info.tickIndex == 0)
        {
            throw std::runtime_error("Intentional callback exception");
        }
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 5U);

    EXPECT_EQ(0U, ticks.front().tickIndex);
    EXPECT_GT(ticks.back().tickIndex, 0U);
}

TEST(PeriodicThreadTests, StopInterruptsLongWait)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(10, core_lib::threads::eWaitTimeUnit::seconds);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());
    ASSERT_TRUE(recorder.WaitForTicks(1));

    // Give the worker time to enter its timed wait.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto const start = PeriodicTestClock::now();

    EXPECT_TRUE(thread.Stop());

    auto const elapsed = PeriodicTestClock::now() - start;

    EXPECT_LT(elapsed, std::chrono::seconds(1));
    EXPECT_FALSE(thread.IsStarted());
    EXPECT_EQ(1U, recorder.Ticks().size());
}

TEST(PeriodicThreadTests, StopDuringCallback)
{
    PeriodicTickRecorder recorder;

    std::atomic<bool> callbackCompleted{false};

    auto settings = MakePeriodicTestSettings(10, core_lib::threads::eWaitTimeUnit::seconds);

    settings.eventCallbackEx = [&recorder, &callbackCompleted](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        callbackCompleted.store(true);
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());
    ASSERT_TRUE(recorder.WaitForTicks(1));

    auto const start = PeriodicTestClock::now();

    EXPECT_TRUE(thread.Stop());

    auto const elapsed = PeriodicTestClock::now() - start;

    EXPECT_TRUE(callbackCompleted.load());
    EXPECT_LT(elapsed, std::chrono::seconds(2));

    EXPECT_EQ(1U, recorder.Ticks().size());
    EXPECT_FALSE(thread.IsStarted());
}

TEST(PeriodicThreadTests, StopDuringUnlimitedCatchUp)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(5);

    settings.overrunPolicy   = core_lib::threads::eOverrunPolicy::catchUp;
    settings.maxCatchUpTicks = 0;

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    {
        recorder.Record(info);

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());
    ASSERT_TRUE(recorder.WaitForTicks(5));

    auto const start = PeriodicTestClock::now();

    EXPECT_TRUE(thread.Stop());

    auto const elapsed = PeriodicTestClock::now() - start;

    EXPECT_LT(elapsed, std::chrono::seconds(1));
    EXPECT_FALSE(thread.IsStarted());

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 5U);

    EXPECT_TRUE(ticks[1].catchUpTick);
}

TEST(PeriodicThreadTests, RestartResetsTimingInformation)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    ASSERT_TRUE(thread.Start());
    ASSERT_TRUE(recorder.WaitForTicks(5));
    ASSERT_TRUE(thread.Stop());

    auto const firstRun = recorder.Ticks();

    ASSERT_GE(firstRun.size(), 5U);

    recorder.Clear();

    // Deliberately leave the thread stopped for several periods.
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto const restartTime = PeriodicTestClock::now();

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(5);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const secondRun = recorder.Ticks();

    ASSERT_GE(secondRun.size(), 5U);

    auto const& firstTick = secondRun.front();

    EXPECT_EQ(0U, firstTick.tickIndex);
    EXPECT_EQ(0U, firstTick.skippedTicks);

    EXPECT_FALSE(firstTick.catchUpTick);
    EXPECT_FALSE(firstTick.resynchronised);

    EXPECT_EQ(core_lib::threads::PeriodicTickInfo::duration_t::zero(), firstTick.previousCallbackDuration);

    EXPECT_GE(firstTick.scheduledTime, restartTime);

    EXPECT_GT(firstTick.scheduledTime, firstRun.back().scheduledTime);
}

TEST(PeriodicThreadTests, RepeatedRestartsMaintainTiming)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(20);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    for (uint32_t iteration = 0; iteration < 20; ++iteration)
    {
        SCOPED_TRACE(iteration);

        recorder.Clear();

        ASSERT_TRUE(thread.Start());

        bool const received = recorder.WaitForTicks(2);

        ASSERT_TRUE(thread.Stop());
        ASSERT_TRUE(received);

        auto const ticks = recorder.Ticks();

        ASSERT_GE(ticks.size(), 2U);

        EXPECT_EQ(0U, ticks.front().tickIndex);
        EXPECT_EQ(0U, ticks.front().skippedTicks);

        for (auto const& tick : ticks)
        {
            EXPECT_GE(tick.actualTime, tick.scheduledTime);

            EXPECT_GE(tick.wakeupLatency, core_lib::threads::PeriodicTickInfo::duration_t::zero());
        }
    }
}

TEST(PeriodicThreadTests, RapidStartStopCycles)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(100);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    core_lib::threads::PeriodicThread thread(settings);

    for (uint32_t iteration = 0; iteration < 100; ++iteration)
    {
        SCOPED_TRACE(iteration);

        ASSERT_TRUE(thread.Start());
        ASSERT_TRUE(thread.Stop());

        EXPECT_FALSE(thread.IsStarted());
    }

    recorder.Clear();

    ASSERT_TRUE(thread.Start());

    bool const received = recorder.WaitForTicks(3);

    EXPECT_TRUE(thread.Stop());
    ASSERT_TRUE(received);

    auto const ticks = recorder.Ticks();

    ASSERT_GE(ticks.size(), 3U);

    EXPECT_EQ(0U, ticks.front().tickIndex);

    for (auto const& tick : ticks)
    {
        EXPECT_GE(tick.actualTime, tick.scheduledTime);
    }
}

TEST(PeriodicThreadTests, DestructorStopsRunningThread)
{
    PeriodicTickRecorder recorder;

    auto settings = MakePeriodicTestSettings(10, core_lib::threads::eWaitTimeUnit::seconds);

    settings.eventCallbackEx = [&recorder](core_lib::threads::PeriodicTickInfo const& info)
    { recorder.Record(info); };

    PeriodicTestClock::time_point destructionStart;

    {
        core_lib::threads::PeriodicThread thread(settings);

        ASSERT_TRUE(thread.Start());
        ASSERT_TRUE(recorder.WaitForTicks(1));

        destructionStart = PeriodicTestClock::now();
    }

    auto const elapsed = PeriodicTestClock::now() - destructionStart;

    EXPECT_LT(elapsed, std::chrono::seconds(1));

    auto const count = recorder.Ticks().size();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(count, recorder.Ticks().size());
}

#endif // DISABLE_THREADS_TESTS
