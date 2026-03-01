
#ifndef DISABLE_GPROTOBUF_TESTS

#include <iostream>
#include "Asio/SimpleTcpServer.h"
#include "Asio/SimpleTcpClient.h"
#include "test_generated.h"
#include "gtest/gtest.h"

using namespace core_lib::asio;
using namespace core_lib::asio::defs;
using namespace core_lib::asio::tcp;
using namespace core_lib::asio::udp;
using namespace core_lib::serialize;
using namespace core_lib;
using namespace core_lib::threads;
using namespace core_lib::asio::messages;


class FlatBufferMessageDispatcher
{
public:
    FlatBufferMessageDispatcher() = default;

    void DispatchMessage(default_received_message_ptr_t message)
    {
        if (message->header.messageId == 666)
        {
            m_header = message->header;

            if (!message->body.empty())
            {
                 ToObjectFlatBuf(
                    message->body,
                    m_myMessage,
                    [](auto& verifier) { return core_lib_test_fb::VerifyTestMessageBuffer(verifier); },
                    [](const void* buf) { return core_lib_test_fb::GetTestMessage(buf); });
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

    const core_lib_test_fb::TestMessageT& Message() const
    {
        return m_myMessage;
    }

private:
    SyncEvent                   m_messageEvent;
    MessageHeader               m_header;
    core_lib_test_fb::TestMessageT m_myMessage;
};

// Unit test cases.
TEST(GoogleFlatBuffers, testCase_serialize)
{
    core_lib_test_fb::TestMessageT m;
    m.name = "I am a test message";
    m.counter = 666;
    m.values.resize(100, 666.666);

    auto msgBuf = ToCharVectorFlatBuf(m, [](flatbuffers::FlatBufferBuilder& b,
                                                const core_lib_test_fb::TestMessageT* o,
                                                const flatbuffers::rehasher_function_t*)
                                                {
                                                    return core_lib_test_fb::TestMessage::Pack(b, o, nullptr);
                                                });

    auto mOut       = ToObjectFlatBuf<core_lib_test_fb::TestMessageT>(
                        msgBuf,
                        [](auto& verifier) { return core_lib_test_fb::VerifyTestMessageBuffer(verifier); },
                        [](const void* buf) { return core_lib_test_fb::GetTestMessage(buf); });

    EXPECT_EQ(m.name, mOut.name);
    EXPECT_EQ(m.counter, mOut.counter);

    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(m.values[i], mOut.values[i]);
    }
}

TEST(GoogleFlatBuffers, testCase_flatbufferOverTcp)
{
    // TODO: Need to tweak network clases so that we can set the correct archive type when using flat buffers.

    FlatBufferMessageDispatcher serverDispatcher;
    SimpleTcpServer           server(22222,
                           std::bind(&FlatBufferMessageDispatcher::DispatchMessage,
                                     &serverDispatcher,
                                     std::placeholders::_1));

    connection_t              serverConn = std::make_pair("127.0.0.1", 22222);
    FlatBufferMessageDispatcher clientDispatcher;
    SimpleTcpClient           client(serverConn,
                           std::bind(&FlatBufferMessageDispatcher::DispatchMessage,
                                     &clientDispatcher,
                                     std::placeholders::_1));

    core_lib_test_fb::TestMessageT m;
    m.name = "I am a test message";
    m.counter = 666;
    m.values.resize(100, 666.666);

    auto msgBuf = ToCharVectorFlatBuf(m, [](flatbuffers::FlatBufferBuilder& b,
                                                const core_lib_test_fb::TestMessageT* o,
                                                const flatbuffers::rehasher_function_t*)
                                                {
                                                    return core_lib_test_fb::TestMessage::Pack(b, o, nullptr);
                                                });

    client.SendMessageToServerAsync(666, msgBuf, defs::NULL_CONNECTION, defs::eArchiveType::flatBuffer);
    serverDispatcher.WaitForMessage(3000);

    auto mOut = serverDispatcher.Message();

    EXPECT_EQ(m.name, mOut.name);
    EXPECT_EQ(m.counter, mOut.counter);

    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(m.values[i], mOut.values[i]);
    }
}

#endif // DISABLE_GPROTOBUF_TESTS
