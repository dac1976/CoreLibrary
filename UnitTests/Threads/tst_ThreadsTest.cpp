#include <QString>
#include <QtTest>
#include <ostream>
#include <memory>
#include <functional>
#include <map>
#include <vector>
#include <limits>
#include "../../SyncEvent.hpp"
#include "../../ThreadBase.hpp"
#include "../../ThreadGroup.hpp"
#include "../../ConcurrentQueue.hpp"
#include "../../MessageQueueThread.hpp"

// ****************************************************************************
// Helper classes for tests
// ****************************************************************************
class ThreadTestHelper
{
public:
    void ThreadFunction()
    {
        std::lock_guard< std::mutex > lock(m_mutex);
        m_threadRunMap[std::this_thread::get_id()] = true;
    }

    bool GetThreadRunState(const std::thread::id& tId) const
    {
        std::lock_guard< std::mutex > lock(m_mutex);
        std::map<std::thread::id, bool>::const_iterator t_it
                    = m_threadRunMap.find(tId);

        if (t_it == m_threadRunMap.end())
            return false;
        else
            return t_it->second;
    }

    void Clear()
    {
        std::lock_guard< std::mutex > lock(m_mutex);
        m_threadRunMap.clear();
    }

private:
    mutable std::mutex m_mutex;
    std::map<std::thread::id, bool> m_threadRunMap;
};

class ThreadTestHelper2
{
public:
    ThreadTestHelper2(core_lib::threads::SyncEvent& event)
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
        std::lock_guard< std::mutex > lock(m_mutex);
        std::map<std::thread::id, bool>::const_iterator t_it
                    = m_eventSignalledMap.find(tId);

        if (t_it == m_eventSignalledMap.end())
            return false;
        else
            return t_it->second;
    }

    void Clear()
    {
        std::lock_guard< std::mutex > lock(m_mutex);
        m_eventSignalledMap.clear();
    }

private:
    core_lib::threads::SyncEvent& m_event;
    mutable std::mutex m_mutex;
    std::map<std::thread::id, bool> m_eventSignalledMap;

    void SetSignalled(const std::thread::id& tId, bool signalled)
    {
        std::lock_guard< std::mutex > lock(m_mutex);
        m_eventSignalledMap[tId] = signalled;
    }
};

class ThreadDerived final : public core_lib::threads::ThreadBase
{
public:
    ThreadDerived(bool* pTerminateCondition)
        : ThreadBase()
        , m_counter(0)
        , m_pTerminateCondition(pTerminateCondition)
    {
        //Do this last in constructor.
        Start();
    }

    virtual ~ThreadDerived()
    {
        //Do this first in destructor.
        Stop();
    }

    size_t GetCounter() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

private:
    mutable std::mutex m_mutex;
    size_t m_counter;
    bool* m_pTerminateCondition;

    virtual void ThreadIteration()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max()
                        ? 0
                        : m_counter + 1;
        }

        SleepForTime(100);
    }

    virtual void ProcessTerminationConditions()
    {
        if (m_pTerminateCondition)
        {
            *m_pTerminateCondition = true;
        }
    }
};

class QueuedThread1 final : public core_lib::threads::ThreadBase
{
public:
    QueuedThread1()
        : ThreadBase()
        , m_counter(0)
    {
        //Do this last in constructor.
        Start();
    }

    virtual ~QueuedThread1()
    {
        //Do this first in destructor.
        Stop();
    }

    size_t GetCounter() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

    void Push(char* item, size_t size)
    {
        m_queue.Push(item, size);
    }

private:
    core_lib::threads::ConcurrentQueue<char> m_queue;
    mutable std::mutex m_mutex;
    size_t m_counter;

    virtual void ThreadIteration()
    {
        size_t size;
        char* message = m_queue.Pop(&size);

        if (message && (size > 0))
        {
            if (size > 1)
                delete [] message;
            else
                delete message;

            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter = m_counter == std::numeric_limits<size_t>::max()
                        ? 0
                        : m_counter + 1;
        }
    }

    virtual void ProcessTerminationConditions()
    {
        m_queue.PushNull();
    }
};

// ****************************************************************************
// Unit test wrapper
// ****************************************************************************
class ThreadsTest : public QObject
{
    Q_OBJECT

public:
    ThreadsTest();

private:
    ThreadTestHelper m_helper;

private Q_SLOTS:
    // Thread group tests
    void testCase_ThreadGroup1();
    void testCase_ThreadGroup2();
    void testCase_ThreadGroup3();
    void testCase_ThreadGroup4();
    void testCase_ThreadGroup5();
    void testCase_ThreadGroup6();
    void testCase_ThreadGroup7();
    void testCase_ThreadGroup8();
    void testCase_SyncEvent1();
    void testCase_SyncEvent2();
    void testCase_SyncEvent3();
    void testCase_SyncEvent4();
    void testCase_SyncEvent5();
    void testCase_SyncEvent6();
    void testCase_SyncEvent7();
    void testCase_ThreadBase();
    void testCase_ConcurrentQueue1();
    void testCase_ConcurrentQueue2();
    void testCase_ConcurrentQueue3();
};

ThreadsTest::ThreadsTest()
{
}

// ****************************************************************************
// ThreadGroup tests
// ****************************************************************************
void ThreadsTest::testCase_ThreadGroup1()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    QVERIFY(!tg.IsThisThreadIn());
}

void ThreadsTest::testCase_ThreadGroup2()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!tg.IsThreadIn(tId));
    QVERIFY(tg.Empty());
    tg.AddThread(t.release());
    QVERIFY(tg.IsThreadIn(tId));
    QCOMPARE(tg.Size(), static_cast<size_t>(1));
    tg.JoinAll();
    QVERIFY(m_helper.GetThreadRunState(tId));
}

void ThreadsTest::testCase_ThreadGroup3()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!tg.IsThreadIn(t.get()));
    QVERIFY(tg.Empty());
    std::thread* temp = t.release();
    tg.AddThread(temp);
    QVERIFY(tg.IsThreadIn(temp));
    QCOMPARE(tg.Size(), static_cast<size_t>(1));
    tg.JoinAll();
    QVERIFY(m_helper.GetThreadRunState(tId));
}

void ThreadsTest::testCase_ThreadGroup4()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!tg.IsThreadIn(tId));
    QVERIFY(tg.Empty());
    tg.AddThread(t.release());
    QVERIFY(tg.IsThreadIn(tId));
    QCOMPARE(tg.Size(), static_cast<size_t>(1));
    t.reset(tg.RemoveThread(tId));
    QVERIFY(tg.Empty());
    QVERIFY(t.get() != nullptr);
    t->join();
    QVERIFY(m_helper.GetThreadRunState(tId));
}

void ThreadsTest::testCase_ThreadGroup5()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::unique_ptr<std::thread> t(new std::thread(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!tg.IsThreadIn(t.get()));
    QVERIFY(tg.Empty());
    tg.AddThread(t.get());
    QVERIFY(tg.IsThreadIn(t.get()));
    QCOMPARE(tg.Size(), static_cast<size_t>(1));
    tg.RemoveThread(t.get());
    QVERIFY(tg.Empty());
    t->join();
    QVERIFY(m_helper.GetThreadRunState(tId));
}

void ThreadsTest::testCase_ThreadGroup6()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::thread::id tId = tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper))->get_id();
    QVERIFY(tg.IsThreadIn(tId));
    QCOMPARE(tg.Size(), static_cast<size_t>(1));
    tg.JoinAll();
    QVERIFY(m_helper.GetThreadRunState(tId));
}

void ThreadsTest::testCase_ThreadGroup7()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::vector<std::thread::id> tIds;

    for(size_t i = 0; i < 10; ++i)
    {
        tIds.push_back(tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper))->get_id());
        QVERIFY(tg.IsThreadIn(tIds.back()));
    }

    QCOMPARE(tg.Size(), static_cast<size_t>(10));
    tg.JoinAll();
    for(size_t i = 0; i < 10; ++i)
    {
        QVERIFY(m_helper.GetThreadRunState(tIds[i]));
    }
}

void ThreadsTest::testCase_ThreadGroup8()
{
    m_helper.Clear();
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper::ThreadFunction, &m_helper));
    std::thread::id tId = t->get_id();
    QVERIFY(tg.IsThreadIn(t));
    QCOMPARE(tg.Size(), static_cast<size_t>(1));

    bool correctException = false;

    try
    {
        tg.AddThread(t);
    }
    catch(core_lib::threads::xThreadGroupError&)
    {
        correctException = true;
    }

    QVERIFY(correctException);

    tg.JoinAll();
    QVERIFY(m_helper.GetThreadRunState(tId));
}

void ThreadsTest::testCase_SyncEvent1()
{
    core_lib::threads::SyncEvent event;
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    QVERIFY(helper.GetEventSignalledState(tId));
}

void ThreadsTest::testCase_SyncEvent2()
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalOneThread
                                       , core_lib::threads::eResetCondition::autoReset
                                       , core_lib::threads::eIntialCondition::signalled);
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::thread::id tId = t->get_id();
    tg.JoinAll();
    QVERIFY(helper.GetEventSignalledState(tId));
}

void ThreadsTest::testCase_SyncEvent3()
{
    core_lib::threads::SyncEvent event;
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction2, &helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!helper.GetEventSignalledState(tId));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    event.Signal();
    tg.JoinAll();
    QVERIFY(helper.GetEventSignalledState(tId));
}

void ThreadsTest::testCase_SyncEvent4()
{
    core_lib::threads::SyncEvent event;
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction2, &helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!helper.GetEventSignalledState(tId));
    tg.JoinAll();
    QVERIFY(!helper.GetEventSignalledState(tId));
}

void ThreadsTest::testCase_SyncEvent5()
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalAllThreads
                                       , core_lib::threads::eResetCondition::manualReset
                                       , core_lib::threads::eIntialCondition::notSignalled);
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup tg;
    std::vector<std::thread::id> tIds;

    for(size_t i = 0; i < 10; ++i)
    {
        tIds.push_back(tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper))->get_id());
        QVERIFY(!helper.GetEventSignalledState(tIds[i]));
    }

    event.Signal();
    tg.JoinAll();

    for(size_t i = 0; i < 10; ++i)
    {
        QVERIFY(helper.GetEventSignalledState(tIds[i]));
    }
}

void ThreadsTest::testCase_SyncEvent6()
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalAllThreads
                                       , core_lib::threads::eResetCondition::manualReset
                                       , core_lib::threads::eIntialCondition::signalled);
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup tg;
    std::vector<std::thread::id> tIds;

    for(size_t i = 0; i < 10; ++i)
    {
        tIds.push_back(tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper))->get_id());
    }

    tg.JoinAll();

    for(size_t i = 0; i < 10; ++i)
    {
        QVERIFY(helper.GetEventSignalledState(tIds[i]));
    }
}

void ThreadsTest::testCase_SyncEvent7()
{
    core_lib::threads::SyncEvent event(core_lib::threads::eNotifyType::signalOneThread
                                       , core_lib::threads::eResetCondition::manualReset
                                       , core_lib::threads::eIntialCondition::notSignalled);
    ThreadTestHelper2 helper(event);
    core_lib::threads::ThreadGroup tg;
    std::thread* t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    std::thread::id tId = t->get_id();
    QVERIFY(!helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    tg.RemoveThread(t);
    delete t;
    QVERIFY(helper.GetEventSignalledState(tId));
    t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    tId = t->get_id();
    QVERIFY(helper.GetEventSignalledState(tId));
    tg.JoinAll();
    tg.RemoveThread(t);
    delete t;
    event.Reset();
    helper.Clear();
    t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
    tId = t->get_id();
    QVERIFY(!helper.GetEventSignalledState(tId));
    event.Signal();
    tg.JoinAll();
    QVERIFY(helper.GetEventSignalledState(tId));
}

// ****************************************************************************
// ThreadBase tests
// ****************************************************************************

void ThreadsTest::testCase_ThreadBase()
{
    bool terminateCondition = false;

    {
        ThreadDerived td(&terminateCondition);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        QVERIFY(td.IsStarted());
        bool exception = false;
        try
        {
            td.ThreadID();
        }
        catch(...)
        {
            exception = true;
        }
        QVERIFY(!exception);
        exception = false;
        try
        {
            td.NativeHandle();
        }
        catch(...)
        {
            exception = true;
        }
        QVERIFY(!exception);
        td.Stop();
        exception = false;
        try
        {
            td.ThreadID();
        }
        catch(...)
        {
            exception = true;
        }
        QVERIFY(exception);
        exception = false;
        try
        {
            td.NativeHandle();
        }
        catch(...)
        {
            exception = true;
        }
        QVERIFY(exception);
        size_t c1 = td.GetCounter();
        QVERIFY(c1 > 0);
        QVERIFY(terminateCondition);
        terminateCondition = false;
        td.Start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        size_t c2 = td.GetCounter();
        QVERIFY(c1 != c2);
    }

    QVERIFY(terminateCondition);
}

// ****************************************************************************
// ConcurrentQueue tests
// ****************************************************************************

void ThreadsTest::testCase_ConcurrentQueue1()
{
    QueuedThread1 qt;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QVERIFY(qt.GetCounter() == 0);
    qt.Push(new char[10], 10);
    qt.Push(new char[5], 5);
    qt.Push(new char[1], 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QVERIFY(qt.GetCounter() == 3);
}

void ThreadsTest::testCase_ConcurrentQueue2()
{
    core_lib::threads::ConcurrentQueue<char> m_queue;
    QVERIFY(m_queue.Empty());
    m_queue.Push(new char[2], 2);
    m_queue.Push(new char[3], 3);
    m_queue.Push(new char[4], 4);
    m_queue.PushNull();
    QVERIFY(m_queue.Size() == 4);
    size_t size;
    const char* item = m_queue.Peek(0, &size);
    QVERIFY(item != nullptr);
    QVERIFY(size == 2);
    item = m_queue.Peek(1, &size);
    QVERIFY(item != nullptr);
    QVERIFY(size == 3);
    item = m_queue.Peek(2, &size);
    QVERIFY(item != nullptr);
    QVERIFY(size =  4);
    item = m_queue.Peek(3, &size);
    QVERIFY(item == nullptr);
    QVERIFY(size == 0);
    QVERIFY(!m_queue.Empty());
    item = m_queue.TimedPop(100,&size);
    QVERIFY(item != nullptr);
    QVERIFY(size == 2);
    delete [] item;
    item = m_queue.TimedPop(100,&size);
    QVERIFY(item != nullptr);
    QVERIFY(size == 3);
    delete [] item;
    item = m_queue.TimedPop(100,&size);
    QVERIFY(item != nullptr);
    QVERIFY(size == 4);
    delete [] item;
    item = m_queue.TimedPop(100,&size);
    QVERIFY(item == nullptr);
    QVERIFY(size == 0);
    QVERIFY(m_queue.Empty());
    QVERIFY(m_queue.Size() == 0);
    item = m_queue.TimedPop(100,&size);
    QVERIFY(item == nullptr);
    QVERIFY(size == 0);

    bool correctException;

    try
    {
        item = m_queue.TimedPopThrow(100,&size);
        (void)item;
        correctException = false;
    }
    catch(core_lib::threads::xQueuePopTimeoutError&)
    {
        correctException = true;
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void ThreadsTest::testCase_ConcurrentQueue3()
{
    core_lib::threads::ConcurrentQueue<char> m_queue;
}

QTEST_APPLESS_MAIN(ThreadsTest)

#include "tst_ThreadsTest.moc"
