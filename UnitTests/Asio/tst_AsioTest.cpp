#include <QString>
#include <QtTest>
#include "../../Asio/IoServiceThreadGroup.hpp"
#include "../../Asio/TcpServer.hpp"
#include "../../Asio/TcpClient.hpp"
#include "../../Serialization/SerializeToVector.hpp"
#include <cstring>
#include <algorithm>
#include <iterator>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace core_lib::asio;
using namespace core_lib::asio::defs;
using namespace core_lib::asio::tcp;
using namespace core_lib::serialize;
using namespace core_lib::threads;

// ****************************************************************************
// Helper classes/functions
// ****************************************************************************
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

#pragma pack(push, 1)
struct MyHeader
{
	char         magicString[16]{"MyHeader"};
	int          command{1};
	unsigned int totalLength{sizeof(*this)};
};
#pragma pack(pop)

struct MyMessage
{
	std::string			name;
	std::vector<double> data;

	bool operator==(const MyMessage& m) const
	{
		return (name == m.name) && (data == m.data);
	}

	void FillMessage()
	{
		name = "MyMessage";
		data = std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0};
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(name);
		ar & BOOST_SERIALIZATION_NVP(data);
	}
};

char_buffer BuildMessage()
{
	MyHeader header;
	MyMessage myMessage;
	myMessage.FillMessage();
	char_buffer body = ToCharVector(myMessage);
	header.totalLength += body.size();
	const char* headCharBuf = reinterpret_cast<const char*>(&header);
	char_buffer message;
	std::copy(headCharBuf, headCharBuf + sizeof(header)
			  , std::back_inserter(message));
	std::copy(body.begin(), body.end()
			  , std::back_inserter(message));
	return message;
}

class MessageReceiver
{
public:
	static size_t CheckBytesLeftToRead(const char_buffer& message)
	{
		CheckMessage(message);

		const MyHeader* pHeader
				= reinterpret_cast<const MyHeader*>(&message.front());

		if (std::string(pHeader->magicString) != "MyHeader")
		{
			throw std::runtime_error("cannot find magic string");
		}

		if (pHeader->totalLength < message.size())
		{
			throw std::length_error("invalid total length in header");
		}

		return pHeader->totalLength - message.size();
	}

	void MessageReceivedHandler(const char_buffer& message)
	{
		CheckMessage(message);

		{
			char_buffer body(message.begin() + sizeof(MyHeader), message.end());
			m_myMessage = ToObject<MyMessage>(body);
		}

		m_messageEvent.Signal();
	}

	bool WaitForMessage(const size_t milliseconds)
	{
		return m_messageEvent.WaitForTime(milliseconds);
	}

	const MyMessage& Message() const
	{
		return m_myMessage;
	}

private:
	SyncEvent m_messageEvent;
	MyMessage m_myMessage;

	static void CheckMessage(const char_buffer& message)
	{
		if (message.size() < sizeof(MyHeader))
		{
			throw std::length_error("message buffer contains too few bytes");
		}
	}
};

// ****************************************************************************
// Unit test wrapper
// ****************************************************************************
class AsioTest : public QObject
{
	Q_OBJECT

public:
	AsioTest();

private Q_SLOTS:
	// Asio tests
	void testCase_IoThreadGroup1();
	void testCase_IoThreadGroup2();
	void testCase_TestAsync();
	void testCase_TestSync();
	void testCase_TestAsync_ExternalIoService();
	void testCase_TestSync_ExternalIoService();
};

AsioTest::AsioTest()
{
}

// ****************************************************************************
// Asio tests
// ****************************************************************************
void AsioTest::testCase_IoThreadGroup1()
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

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	QVERIFY(sum1.Total() == static_cast<uint64_t>(500000500000));
	QVERIFY(sum2.Total() == static_cast<uint64_t>(500000500000));
	QVERIFY(sum1.NumThreadsUsed() == std::thread::hardware_concurrency());
	QVERIFY(sum2.NumThreadsUsed() == std::thread::hardware_concurrency());
}

void AsioTest::testCase_IoThreadGroup2()
{
	Sum sum1{};
	Sum sum2{};

	{
		core_lib::asio::IoServiceThreadGroup ioThreadGroup{};

		for (uint64_t i = 1; i <= 1000000; ++i)
		{
			ioThreadGroup.Post(std::bind(&Sum::Add, &sum1, i));
			ioThreadGroup.Post(std::bind(&Sum::Add, &sum2, i));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	QVERIFY(sum1.Total() == static_cast<uint64_t>(500000500000));
	QVERIFY(sum2.Total() == static_cast<uint64_t>(500000500000));
	QVERIFY(sum1.NumThreadsUsed() == std::thread::hardware_concurrency());
	QVERIFY(sum2.NumThreadsUsed() == std::thread::hardware_concurrency());
}

void AsioTest::testCase_TestAsync()
{
	char_buffer message = BuildMessage();
	MessageReceiver svrReceiver;
	TcpServer server(22222, sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &svrReceiver, std::placeholders::_1));

	MessageReceiver cltReceiver;
	TcpClient client(std::make_pair("127.0.0.1", 22222), sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &cltReceiver, std::placeholders::_1));

	client.SendMessageToServerAsync(message);

	svrReceiver.WaitForMessage(3000);
	MyMessage expectedMessage;
	expectedMessage.FillMessage();
	MyMessage receivedMessage = svrReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);

	auto clientConn = client.GetClientDetailsForServer();
	server.SendMessageToClientAsync(clientConn, message);

	cltReceiver.WaitForMessage(3000);
	receivedMessage = cltReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);
}

void AsioTest::testCase_TestSync()
{
	char_buffer message = BuildMessage();
	MessageReceiver svrReceiver;
	TcpServer server(22222, sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &svrReceiver, std::placeholders::_1));

	MessageReceiver cltReceiver;
	TcpClient client(std::make_pair("127.0.0.1", 22222), sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &cltReceiver, std::placeholders::_1));

	QVERIFY(client.SendMessageToServerSync(message) == true);

	svrReceiver.WaitForMessage(3000);
	MyMessage expectedMessage;
	expectedMessage.FillMessage();
	MyMessage receivedMessage = svrReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);

	auto clientConn = client.GetClientDetailsForServer();
	QVERIFY(server.SendMessageToClientSync(clientConn, message) == true);

	cltReceiver.WaitForMessage(3000);
	receivedMessage = cltReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);
}

void AsioTest::testCase_TestAsync_ExternalIoService()
{
	IoServiceThreadGroup ioThreadGroup;

	char_buffer message = BuildMessage();
	MessageReceiver svrReceiver;
	TcpServer server(ioThreadGroup.IoService(), 22222, sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &svrReceiver, std::placeholders::_1));

	MessageReceiver cltReceiver;
	TcpClient client(ioThreadGroup.IoService(), std::make_pair("127.0.0.1", 22222), sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &cltReceiver, std::placeholders::_1));

	client.SendMessageToServerAsync(message);

	svrReceiver.WaitForMessage(3000);
	MyMessage expectedMessage;
	expectedMessage.FillMessage();
	MyMessage receivedMessage = svrReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);

	auto clientConn = client.GetClientDetailsForServer();
	server.SendMessageToClientAsync(clientConn, message);

	cltReceiver.WaitForMessage(3000);
	receivedMessage = cltReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);
}

void AsioTest::testCase_TestSync_ExternalIoService()
{
	IoServiceThreadGroup ioThreadGroup;

	char_buffer message = BuildMessage();
	MessageReceiver svrReceiver;
	TcpServer server(ioThreadGroup.IoService(), 22222, sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &svrReceiver, std::placeholders::_1));

	MessageReceiver cltReceiver;
	TcpClient client(ioThreadGroup.IoService(), std::make_pair("127.0.0.1", 22222), sizeof(MyHeader)
					 , std::bind(&MessageReceiver::CheckBytesLeftToRead, std::placeholders::_1)
					 , std::bind(&MessageReceiver::MessageReceivedHandler, &cltReceiver, std::placeholders::_1));

	QVERIFY(client.SendMessageToServerSync(message) == true);

	svrReceiver.WaitForMessage(3000);
	MyMessage expectedMessage;
	expectedMessage.FillMessage();
	MyMessage receivedMessage = svrReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);

	auto clientConn = client.GetClientDetailsForServer();
	QVERIFY(server.SendMessageToClientSync(clientConn, message) == true);

	cltReceiver.WaitForMessage(3000);
	receivedMessage = cltReceiver.Message();
	QVERIFY(receivedMessage == expectedMessage);
}


QTEST_APPLESS_MAIN(AsioTest)

#include "tst_AsioTest.moc"
