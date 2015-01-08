#include <QString>
#include <QtTest>
#include "../../Asio/IoServiceThreadGroup.hpp"
#include "../../Asio/TcpServer.hpp"
#include "../../Asio/TcpClient.hpp"
#include "../../TcpTypedServer.hpp"
#include "../../TcpTypedClient.hpp"
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
using namespace core_lib::asio::messages;

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
	char         magicString[16];
	int          command{1};
	unsigned int totalLength{sizeof(*this)};

	MyHeader()
		: magicString{"MyHeader"}
	{

	}
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

char_buffer_t BuildMessage()
{
	MyHeader header;
	MyMessage myMessage;
	myMessage.FillMessage();
    char_buffer_t body = ToCharVector(myMessage);
	header.totalLength += body.size();
	const char* headCharBuf = reinterpret_cast<const char*>(&header);
    char_buffer_t message;
	std::copy(headCharBuf, headCharBuf + sizeof(header)
			  , std::back_inserter(message));
	std::copy(body.begin(), body.end()
			  , std::back_inserter(message));
	return message;
}

class MessageReceiver
{
public:
    static size_t CheckBytesLeftToRead(const char_buffer_t& message)
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

    void MessageReceivedHandler(const char_buffer_t& message)
	{
		CheckMessage(message);

		{
            char_buffer_t body(message.begin() + sizeof(MyHeader), message.end());
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

    static void CheckMessage(const char_buffer_t& message)
	{
		if (message.size() < sizeof(MyHeader))
		{
			throw std::length_error("message buffer contains too few bytes");
		}
	}
};

class MessageDispatcher
{
public:
    void DispatchMessage(default_received_message_ptr_t message)
	{
		if (message->header.messageId == 666)
		{
			m_header = message->header;

            if (!message->body.empty())
            {
                m_myMessage = ToObject<MyMessage>(message->body);
            }
		}

		m_messageEvent.Signal();
	}

	bool WaitForMessage(const size_t milliseconds)
	{
		return m_messageEvent.WaitForTime(milliseconds);
	}

	const MessageHeader& Header() const
	{
		return m_header;
	}

	const MyMessage& Message() const
	{
		return m_myMessage;
	}

private:
	SyncEvent m_messageEvent;
	MessageHeader m_header;
	MyMessage m_myMessage;
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
	void testCase_TestTypedAsync();
    void testCase_TestTypedSync();    
    void testCase_TestTyped_SendToAll_1();
    void testCase_TestTyped_SendToAll_2();
    void testCase_TestTypedAsync_Hdr();
    void testCase_TestTypedSync_Hdr();
    void testCase_TestTyped_SendToAll_1_Hdr();
    void testCase_TestTyped_SendToAll_2_Hdr();
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
    char_buffer_t message = BuildMessage();
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
    char_buffer_t message = BuildMessage();
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

    char_buffer_t message = BuildMessage();
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

    char_buffer_t message = BuildMessage();
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

void AsioTest::testCase_TestTypedAsync()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
	MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);
	MessageDispatcher clientDispatcher;
    MessageHandler cltMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler, std::placeholders::_1)
                          , messageBuilder);

	MyMessage messageToSend;
	messageToSend.FillMessage();

	client.SendMessageToServerAsync(messageToSend, 666);
	serverDispatcher.WaitForMessage(3000);

	MyMessage receivedMessage = serverDispatcher.Message();
	QVERIFY(receivedMessage == messageToSend);

	MessageHeader header = serverDispatcher.Header();
    connection_t respAddress = std::make_pair(header.responseAddress, header.responsePort);
	server.SendMessageToClientAsync(messageToSend, respAddress, 666);
	clientDispatcher.WaitForMessage(3000);

	receivedMessage = clientDispatcher.Message();
	QVERIFY(receivedMessage == messageToSend);

	header = clientDispatcher.Header();
	respAddress = std::make_pair(header.responseAddress, header.responsePort);

	QVERIFY(respAddress == serverConn);
}

void AsioTest::testCase_TestTypedSync()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
	MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);
	MessageDispatcher clientDispatcher;
    MessageHandler cltMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler, std::placeholders::_1)
                          , messageBuilder);

	MyMessage messageToSend;
	messageToSend.FillMessage();

	QVERIFY(client.SendMessageToServerSync(messageToSend, 666) == true);
	serverDispatcher.WaitForMessage(3000);

	MyMessage receivedMessage = serverDispatcher.Message();
	QVERIFY(receivedMessage == messageToSend);

	MessageHeader header = serverDispatcher.Header();
    connection_t respAddress = std::make_pair(header.responseAddress, header.responsePort);
	QVERIFY(server.SendMessageToClientSync(messageToSend, respAddress, 666) == true);
	clientDispatcher.WaitForMessage(3000);

	receivedMessage = clientDispatcher.Message();
	QVERIFY(receivedMessage == messageToSend);

	header = clientDispatcher.Header();
	respAddress = std::make_pair(header.responseAddress, header.responsePort);

	QVERIFY(respAddress == serverConn);
}

void AsioTest::testCase_TestTyped_SendToAll_1()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
    MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);

    MessageDispatcher clientDispatcher1;
    MessageHandler cltMessageHandler1(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher1, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client1(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler1, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler1, std::placeholders::_1)
                          , messageBuilder);

    MessageDispatcher clientDispatcher2;
    MessageHandler cltMessageHandler2(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher2, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client2(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler2, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler2, std::placeholders::_1)
                          , messageBuilder );

    MyMessage messageToSend;
    messageToSend.FillMessage();

    client1.SendMessageToServerAsync(messageToSend, 666);
    serverDispatcher.WaitForMessage(3000);

    MyMessage receivedMessage = serverDispatcher.Message();
    QVERIFY(receivedMessage == messageToSend);

    client2.SendMessageToServerAsync(messageToSend, 666);
    serverDispatcher.WaitForMessage(3000);

    receivedMessage = serverDispatcher.Message();
    QVERIFY(receivedMessage == messageToSend);

    server.SendMessageToAllClients(messageToSend, 666);
    clientDispatcher1.WaitForMessage(3000);
    clientDispatcher2.WaitForMessage(3000);

    receivedMessage = clientDispatcher1.Message();
    QVERIFY(receivedMessage == messageToSend);

    MessageHeader header = clientDispatcher1.Header();
    QVERIFY(std::string(header.responseAddress) == "0.0.0.0");
    QVERIFY(header.responsePort == serverConn.second);

    receivedMessage = clientDispatcher2.Message();
    QVERIFY(receivedMessage == messageToSend);

    header = clientDispatcher2.Header();
    QVERIFY(std::string(header.responseAddress) == "0.0.0.0");
    QVERIFY(header.responsePort == serverConn.second);
}

void AsioTest::testCase_TestTyped_SendToAll_2()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
    MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);

    MessageDispatcher clientDispatcher1;
    MessageHandler cltMessageHandler1(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher1, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client1(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler1, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler1, std::placeholders::_1)
                          , messageBuilder);

    MessageDispatcher clientDispatcher2;
    MessageHandler cltMessageHandler2(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher2, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client2(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler2, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler2, std::placeholders::_1)
                          , messageBuilder );

    MyMessage messageToSend;
    messageToSend.FillMessage();

    client1.SendMessageToServerAsync(messageToSend, 666);
    serverDispatcher.WaitForMessage(3000);

    MyMessage receivedMessage = serverDispatcher.Message();
    QVERIFY(receivedMessage == messageToSend);

    client2.SendMessageToServerAsync(messageToSend, 666);
    serverDispatcher.WaitForMessage(3000);

    receivedMessage = serverDispatcher.Message();
    QVERIFY(receivedMessage == messageToSend);

    server.SendMessageToAllClients(messageToSend, 666, serverConn);
    clientDispatcher1.WaitForMessage(3000);
    clientDispatcher2.WaitForMessage(3000);

    receivedMessage = clientDispatcher1.Message();
    QVERIFY(receivedMessage == messageToSend);

    MessageHeader header = clientDispatcher1.Header();
    QVERIFY(std::string(header.responseAddress) == serverConn.first);
    QVERIFY(header.responsePort == serverConn.second);

    receivedMessage = clientDispatcher2.Message();
    QVERIFY(receivedMessage == messageToSend);

    header = clientDispatcher2.Header();
    QVERIFY(std::string(header.responseAddress) == serverConn.first);
    QVERIFY(header.responsePort == serverConn.second);
}

void AsioTest::testCase_TestTypedAsync_Hdr()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
    MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);
    MessageDispatcher clientDispatcher;
    MessageHandler cltMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    client.SendMessageToServerAsync(666);
    serverDispatcher.WaitForMessage(3000);

    MessageHeader header = serverDispatcher.Header();
    connection_t respAddress = std::make_pair(header.responseAddress, header.responsePort);
    server.SendMessageToClientAsync(respAddress, 666);
    clientDispatcher.WaitForMessage(3000);

    header = clientDispatcher.Header();
    respAddress = std::make_pair(header.responseAddress, header.responsePort);

    QVERIFY(respAddress == serverConn);
}

void AsioTest::testCase_TestTypedSync_Hdr()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
    MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);
    MessageDispatcher clientDispatcher;
    MessageHandler cltMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    QVERIFY(client.SendMessageToServerSync(666) == true);
    serverDispatcher.WaitForMessage(3000);

    MessageHeader header = serverDispatcher.Header();
    connection_t respAddress = std::make_pair(header.responseAddress, header.responsePort);
    QVERIFY(server.SendMessageToClientSync(respAddress, 666) == true);
    clientDispatcher.WaitForMessage(3000);

    header = clientDispatcher.Header();
    respAddress = std::make_pair(header.responseAddress, header.responsePort);

    QVERIFY(respAddress == serverConn);
}

void AsioTest::testCase_TestTyped_SendToAll_1_Hdr()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
    MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);

    MessageDispatcher clientDispatcher1;
    MessageHandler cltMessageHandler1(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher1, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client1(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler1, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler1, std::placeholders::_1)
                           , messageBuilder);

    MessageDispatcher clientDispatcher2;
    MessageHandler cltMessageHandler2(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher2, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client2(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler2, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler2, std::placeholders::_1)
                          , messageBuilder);

    client1.SendMessageToServerAsync(666);
    serverDispatcher.WaitForMessage(3000);

    client2.SendMessageToServerAsync(666);
    serverDispatcher.WaitForMessage(3000);

    server.SendMessageToAllClients(666);
    clientDispatcher1.WaitForMessage(3000);
    clientDispatcher2.WaitForMessage(3000);

    MessageHeader header = clientDispatcher1.Header();
    QVERIFY(std::string(header.responseAddress) == "0.0.0.0");
    QVERIFY(header.responsePort == serverConn.second);

    header = clientDispatcher2.Header();
    QVERIFY(std::string(header.responseAddress) == "0.0.0.0");
    QVERIFY(header.responsePort == serverConn.second);
}

void AsioTest::testCase_TestTyped_SendToAll_2_Hdr()
{
    MessageBuilder messageBuilder(eArchiveType::portableBinary);
    MessageDispatcher serverDispatcher;
    MessageHandler svrMessageHandler(std::bind(&MessageDispatcher::DispatchMessage, &serverDispatcher, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedServer<MessageBuilder> server(22222, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &svrMessageHandler, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &svrMessageHandler, std::placeholders::_1)
                          , messageBuilder);

    connection_t serverConn = std::make_pair("127.0.0.1", 22222);

    MessageDispatcher clientDispatcher1;
    MessageHandler cltMessageHandler1(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher1, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client1(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler1, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler1, std::placeholders::_1)
                          , messageBuilder);

    MessageDispatcher clientDispatcher2;
    MessageHandler cltMessageHandler2(std::bind(&MessageDispatcher::DispatchMessage, &clientDispatcher2, std::placeholders::_1), DEFAULT_MAGIC_STRING);
    TcpTypedClient<MessageBuilder> client2(serverConn, sizeof(MessageHeader)
                          , std::bind(&MessageHandler::CheckBytesLeftToRead, &cltMessageHandler2, std::placeholders::_1)
                          , std::bind(&MessageHandler::MessageReceivedHandler, &cltMessageHandler2, std::placeholders::_1)
                          , messageBuilder);

    client1.SendMessageToServerAsync(666);
    serverDispatcher.WaitForMessage(3000);

    client2.SendMessageToServerAsync(666);
    serverDispatcher.WaitForMessage(3000);

    server.SendMessageToAllClients(666, serverConn);
    clientDispatcher1.WaitForMessage(3000);
    clientDispatcher2.WaitForMessage(3000);

    MessageHeader header = clientDispatcher1.Header();
    QVERIFY(std::string(header.responseAddress) == serverConn.first);
    QVERIFY(header.responsePort == serverConn.second);

    header = clientDispatcher2.Header();
    QVERIFY(std::string(header.responseAddress) == serverConn.first);
    QVERIFY(header.responsePort == serverConn.second);
}

QTEST_APPLESS_MAIN(AsioTest)

#include "tst_AsioTest.moc"
