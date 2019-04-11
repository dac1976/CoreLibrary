#ifndef DISABLE_GPROTOBUF_TESTS

#include "Asio/SimpleTcpServer.h"
#include "Asio/SimpleTcpClient.h"
#include "test.pb.h"
#include "gtest/gtest.h"

using namespace core_lib::asio;
using namespace core_lib::asio::defs;
using namespace core_lib::asio::tcp;
using namespace core_lib::serialize;
using namespace core_lib::threads;
using namespace core_lib::asio::messages;
using namespace core_lib_test;

class ProtoBufMessageDispatcher
{
public:
    ProtoBufMessageDispatcher() = default;

    void DispatchMessage(default_received_message_ptr_t message)
    {
        if (message->header.messageId == 666)
        {
            m_header = message->header;

            if (!message->body.empty())
            {
                m_myMessage = ToObject<TestMessage, archives::in_protobuf_t>(message->body);
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

    const TestMessage& Message() const
    {
        return m_myMessage;
    }

private:
    SyncEvent     m_messageEvent;
    MessageHeader m_header;
    TestMessage   m_myMessage;
};

// Unit test cases.
TEST(GoogleProtobuf, testCase_serialize)
{
    TestMessage m;
    m.set_name("I am a test message");
    m.set_counter(666);
    m.mutable_values()->Resize(100, 666.666);

    auto charVector = ToCharVector<TestMessage, archives::out_protobuf_t>(m);
    auto mOut       = ToObject<TestMessage, archives::in_protobuf_t>(charVector);

    EXPECT_EQ(m.name(), mOut.name());
    EXPECT_EQ(m.counter(), mOut.counter());

    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(m.values(i), mOut.values(i));
    }
}

TEST(GoogleProtobuf, testCase_protobufOverTcp)
{
    ProtoBufMessageDispatcher serverDispatcher;
    SimpleTcpServer           server(22222,
                           std::bind(&ProtoBufMessageDispatcher::DispatchMessage,
                                     &serverDispatcher,
                                     std::placeholders::_1));

    connection_t              serverConn = std::make_pair("127.0.0.1", 22222);
    ProtoBufMessageDispatcher clientDispatcher;
    SimpleTcpClient           client(serverConn,
                           std::bind(&ProtoBufMessageDispatcher::DispatchMessage,
                                     &clientDispatcher,
                                     std::placeholders::_1));

    TestMessage m;
    m.set_name("I am a test message");
    m.set_counter(666);
    m.mutable_values()->Resize(100, 666.666);

    client.SendMessageToServerAsync<TestMessage, archives::out_protobuf_t>(m, 666);
    serverDispatcher.WaitForMessage(3000);

    TestMessage receivedMessage = serverDispatcher.Message();

    EXPECT_EQ(m.name(), receivedMessage.name());
    EXPECT_EQ(m.counter(), receivedMessage.counter());

    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(m.values(i), receivedMessage.values(i));
    }
}

#endif // DISABLE_GPROTOBUF_TESTS
