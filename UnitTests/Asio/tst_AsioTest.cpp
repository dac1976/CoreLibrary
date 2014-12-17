#include <QString>
#include <QtTest>
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

		return message.size() - pHeader->totalLength;
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
	// Thread group tests
	void testCase_Test1();
};

AsioTest::AsioTest()
{
}

// ****************************************************************************
// ThreadGroup tests
// ****************************************************************************
void AsioTest::testCase_Test1()
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
}


QTEST_APPLESS_MAIN(AsioTest)

#include "tst_AsioTest.moc"
