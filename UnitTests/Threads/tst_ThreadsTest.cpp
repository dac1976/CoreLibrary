#include <QString>
#include <QtTest>
#include <ostream>
#include <memory>
#include <functional>
#include <map>
#include <vector>
#include <set>
#include <limits>
#include "../../SyncEvent.hpp"
#include "../../ThreadBase.hpp"
#include "../../ThreadGroup.hpp"
#include "../../ConcurrentQueue.hpp"
#include "../../MessageQueueThread.hpp"
#include "../../BoundedBuffer.hpp"
#include "../../Asio/IoServiceThreadGroup.hpp"

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

	void Push(char* item, int size)
	{
		m_queue.Push(item, size);
	}

	void Push(char* item)
	{
		m_queue.Push(item);
	}

private:
	core_lib::threads::ConcurrentQueue<char> m_queue;
	mutable std::mutex m_mutex;
	size_t m_counter;

	virtual void ThreadIteration()
	{
		int size;
		char* message = m_queue.Pop(&size);

		if (message && (size != 0))
		{
			if (size > 0)
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
		m_queue.Push();
	}
};

class BoundedBufThread final : public core_lib::threads::ThreadBase
{
public:
	BoundedBufThread(core_lib::threads::BoundedBuffer<int>& buf)
		: ThreadBase()
		, m_buf(buf)
		, m_blocked(false)
	{
		//Do this last in constructor.
		Start();
	}

	virtual ~BoundedBufThread()
	{
		//Do this first in destructor.
		Stop();
	}

	bool GetBlocked() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_blocked;
	}

private:
	core_lib::threads::BoundedBuffer<int>& m_buf;
	mutable std::mutex m_mutex;
	bool m_blocked;

	virtual void ThreadIteration()
	{
		SetBlocked(true);
		int temp;
		m_buf.PopBack(temp);
		SetBlocked(false);
	}

	virtual void ProcessTerminationConditions()
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
	BoundedBufThread2(core_lib::threads::BoundedBuffer<int>& buf)
		: ThreadBase()
		, m_buf(buf)
		, m_blocked(false)
	{
		//Do this last in constructor.
		Start();
	}

	virtual ~BoundedBufThread2()
	{
		//Do this first in destructor.
		Stop();
	}

	bool GetBlocked() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_blocked;
	}

private:
	core_lib::threads::BoundedBuffer<int>& m_buf;
	mutable std::mutex m_mutex;
	bool m_blocked;

	virtual void ThreadIteration()
	{
		SetBlocked(true);
		m_buf.PushFront(6);
		SetBlocked(false);
	}

	virtual void ProcessTerminationConditions()
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
	struct Message
	{
		int id;

		Message() = default;
		Message(const Message& ) = default;
		Message(Message&& ) = default;
		Message& operator=(const Message& ) = default;
		Message& operator=(Message&&) = default;

		Message(int id_)
			: id(id_)
		{
		}
	};

	enum MessageIds
	{
		M1,
		M2,
		M3
	};

	MessageQueueThreadTest()
		: m_mqt(std::bind(&MessageQueueThreadTest::MessageDecoder, this
						  , std::placeholders::_1, std::placeholders::_2))
	{
		m_mqt.RegisterMessageHandler(M1, std::bind(&MessageQueueThreadTest::MessageHandler, this
												 , std::placeholders::_1, std::placeholders::_2));
		m_mqt.RegisterMessageHandler(M2, std::bind(&MessageQueueThreadTest::MessageHandler, this
												 , std::placeholders::_1, std::placeholders::_2));
		m_mqt.RegisterMessageHandler(M3, std::bind(&MessageQueueThreadTest::MessageHandler, this
												 , std::placeholders::_1, std::placeholders::_2));
		m_countMap[M1] = 0;
		m_countMap[M2] = 0;
		m_countMap[M3] = 0;
	}

	~MessageQueueThreadTest()
	{
	}

	void PushMessageId(MessageIds id)
	{
		Message* message = new Message(id);
		m_mqt.Push(message);
	}

	size_t CountMessageId(MessageIds id)
	{
		return m_countMap[id];
	}

private:
	core_lib::threads::MessageQueueThread<int, Message> m_mqt;
	std::map<int, size_t> m_countMap;

	int MessageDecoder(const Message* message, const int length)
	{
		if (!message || (length == 0))
		{
			throw std::runtime_error("invalid message");
		}

		return message->id;
	}

	bool MessageHandler(Message* message, const int length)
	{
		if (!message || (length == 0))
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
	Sum() = default;
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
	mutable std::mutex m_mutex;
	uint64_t m_total{0};
	std::set<std::thread::id> m_threadIds;

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
	void testCase_BoundedBuffer1();
	void testCase_BoundedBuffer2();
	void testCase_BoundedBuffer3();
	void testCase_MessageQueuetThread1();
	void testCase_IoThreadGroup();
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
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	std::thread::id tId = t->get_id();
	QVERIFY(!helper.GetEventSignalledState(tId));
	event.Signal();
	tg.JoinAll();
	tg.RemoveThread(t);
	delete t;
	QVERIFY(helper.GetEventSignalledState(tId));
	t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	tId = t->get_id();
	QVERIFY(helper.GetEventSignalledState(tId));
	tg.JoinAll();
	tg.RemoveThread(t);
	delete t;
	event.Reset();
	helper.Clear();
	t = tg.CreateThread(std::bind(&ThreadTestHelper2::ThreadFunction1, &helper));
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
	char* temp = new char[10];
	std::fill(temp, temp + 10, 'A');
	qt.Push(temp, 10);
	temp = new char[5];
	std::fill(temp, temp + 5, 'B');
	qt.Push(temp, 5);
	temp = new char[1];
	std::fill(temp, temp + 1, 'C');
	qt.Push(temp, 1);
	temp = new char;
	*temp = 'D';
	qt.Push(temp);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	QVERIFY(qt.GetCounter() == 4);
}

void ThreadsTest::testCase_ConcurrentQueue2()
{
	core_lib::threads::ConcurrentQueue<char> m_queue;
	QVERIFY(m_queue.Empty());
	char* temp = new char[2];
	std::fill(temp, temp + 2, 'A');
	m_queue.Push(temp, 2);
	temp = new char[3];
	std::fill(temp, temp + 3, 'B');
	m_queue.Push(temp, 3);
	temp = new char[4];
	std::fill(temp, temp + 4, 'C');
	m_queue.Push(temp, 4);
	m_queue.Push();
	QVERIFY(m_queue.Size() == 4);
	int size;
	const char* item = nullptr;
	/*const char* item = m_queue.Peek(0, &size);
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
	QVERIFY(!m_queue.Empty());*/
	QVERIFY(m_queue.TimedPop(100, temp, &size));
	QVERIFY(temp != nullptr);
	QVERIFY(size == 2);
	delete [] temp;
	QVERIFY(m_queue.TimedPop(100, temp, &size));
	QVERIFY(temp != nullptr);
	QVERIFY(size == 3);
	delete [] temp;
	QVERIFY(m_queue.TimedPop(100, temp, &size));
	QVERIFY(temp != nullptr);
	QVERIFY(size == 4);
	delete [] temp;
	QVERIFY(m_queue.TimedPop(100, temp, &size));
	QVERIFY(temp == nullptr);
	QVERIFY(size == 0);
	QVERIFY(m_queue.Empty());
	QVERIFY(m_queue.Size() == 0);
	QVERIFY(!m_queue.TimedPop(100, temp, &size));

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

	m_queue.Push(new char[2], 2);
	QVERIFY(m_queue.TryPop(temp, &size));
	QVERIFY(temp != nullptr);
	QVERIFY(size == 2);
	delete [] temp;

	QVERIFY(!m_queue.TryPop(temp, &size));
	QVERIFY(temp == nullptr);
	QVERIFY(size == 0);
	delete [] temp;

	m_queue.Push(new char[2], 2);

	try
	{
		item = m_queue.TryPopThrow(&size);
		correctException = false;
		QVERIFY(item != nullptr);
		QVERIFY(size == 2);
		delete [] item;
	}
	catch(core_lib::threads::xQueuePopTimeoutError&)
	{
		correctException = true;
	}
	catch(...)
	{
		correctException = false;
	}

	QVERIFY(!correctException);

	try
	{
		item = m_queue.TryPopThrow(&size);
		delete [] item;
		correctException = false;
	}
	catch(core_lib::threads::xQueuePopQueueEmptyError&)
	{
		correctException = true;
	}
	catch(...)
	{
		correctException = false;
	}

	QVERIFY(correctException);

	m_queue.Push(new char[2], 2);
	m_queue.Push(new char[4], 4);
	QVERIFY(m_queue.TrySteal(temp, &size));
	QVERIFY(temp != nullptr);
	QVERIFY(size == 4);
	delete [] temp;
	m_queue.Clear();

	QVERIFY(!m_queue.TrySteal(temp, &size));
	QVERIFY(temp == nullptr);
	QVERIFY(size == 0);
	delete [] temp;

	m_queue.Push(new char[2], 2);
	m_queue.Push(new char[4], 4);

	try
	{
		item = m_queue.TryStealThrow(&size);
		correctException = false;
		QVERIFY(item != nullptr);
		QVERIFY(size == 4);
		delete [] item;
	}
	catch(core_lib::threads::xQueuePopTimeoutError&)
	{
		correctException = true;
	}
	catch(...)
	{
		correctException = false;
	}

	QVERIFY(!correctException);
	m_queue.Clear();

	try
	{
		item = m_queue.TryStealThrow(&size);
		delete [] item;
		correctException = false;
	}
	catch(core_lib::threads::xQueuePopQueueEmptyError&)
	{
		correctException = true;
	}
	catch(...)
	{
		correctException = false;
	}

	QVERIFY(correctException);
}

// ****************************************************************************
// BoundedBuffer tests
// ****************************************************************************

void ThreadsTest::testCase_BoundedBuffer1()
{
	core_lib::threads::BoundedBuffer<int> bb(10);
	bb.PushFront(6);
	int item;
	bb.PopBack(item);
	QVERIFY(item == 6);
}

void ThreadsTest::testCase_BoundedBuffer2()
{
	core_lib::threads::BoundedBuffer<int> bb(10);
	BoundedBufThread bt(bb);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	QVERIFY(bt.GetBlocked());
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

void ThreadsTest::testCase_BoundedBuffer3()
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
	QVERIFY(bt.GetBlocked());
	int item;
	bb.PopBack(item);
	QVERIFY(item == 1);
}

// ****************************************************************************
// MessageQueuetThread tests
// ****************************************************************************

void ThreadsTest::testCase_MessageQueuetThread1()
{
	MessageQueueThreadTest mqtt;
	mqtt.PushMessageId(MessageQueueThreadTest::M1);
	mqtt.PushMessageId(MessageQueueThreadTest::M2);
	mqtt.PushMessageId(MessageQueueThreadTest::M3);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	QVERIFY(mqtt.CountMessageId(MessageQueueThreadTest::M1) == 1);
	QVERIFY(mqtt.CountMessageId(MessageQueueThreadTest::M2) == 1);
	QVERIFY(mqtt.CountMessageId(MessageQueueThreadTest::M3) == 1);

	for (size_t i = 0; i < 10; ++i)
	{
		mqtt.PushMessageId(MessageQueueThreadTest::M1);
		mqtt.PushMessageId(MessageQueueThreadTest::M2);
		mqtt.PushMessageId(MessageQueueThreadTest::M3);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(250));

	QVERIFY(mqtt.CountMessageId(MessageQueueThreadTest::M1) == 11);
	QVERIFY(mqtt.CountMessageId(MessageQueueThreadTest::M2) == 11);
	QVERIFY(mqtt.CountMessageId(MessageQueueThreadTest::M3) == 11);
}

// ****************************************************************************
// MessageQueuetThread tests
// ****************************************************************************

void ThreadsTest::testCase_IoThreadGroup()
{
	Sum sum1{};
	Sum sum2{};

	{
		core_lib::asio::IoServiceThreadGroup ioThreadGroup{};

		for (uint64_t i = 1; i <= 1000000; ++i)
		{
			ioThreadGroup.IoService().post(std::bind(&Sum::Add, &sum1, i));
			ioThreadGroup.IoService().post(std::bind(&Sum::Add, &sum2, i));
		}
	}

	QVERIFY(sum1.Total() == static_cast<uint64_t>(500000500000));
	QVERIFY(sum2.Total() == static_cast<uint64_t>(500000500000));
	QVERIFY(sum1.NumThreadsUsed() == std::thread::hardware_concurrency());
	QVERIFY(sum2.NumThreadsUsed() == std::thread::hardware_concurrency());
}

QTEST_APPLESS_MAIN(ThreadsTest)

#include "tst_ThreadsTest.moc"
