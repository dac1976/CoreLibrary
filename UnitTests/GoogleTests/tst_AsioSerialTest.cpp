#ifndef DISABLE_ASIO_SERIAL_TESTS

#include <memory>
#include <algorithm>
#include <iterator>
#include "Asio/SerialPort.h"
#include "gtest/gtest.h"
#include "gtest_cout.h"

// NOTE: Change these to match a COM port name in your OS that is connected to a Dragonfly
#if BOOST_OS_WINDOWS
const std::string COM_PORT_NAME = "COM1";
#else
const std::string COM_PORT_NAME = "/dev/ttyUSB1";
#endif

TEST(AsioSerialTest, testCase_SimpleSerialPortTest)
{
    auto CheckBytesLeftToRead = [](core_lib::asio::defs::char_buffer_t const& /*message*/) { return 0; };
	
	bool received = false;

    auto HandleMessage = [&](core_lib::asio::defs::char_buffer_t const& message)
    { 
	    GOUT(std::string(message.begin(), message.end())); 
	    received = true;
    };

    std::unique_ptr<core_lib::asio::serial::SerialPort> serialPort;

    ASSERT_NO_THROW(serialPort = std::make_unique<core_lib::asio::serial::SerialPort>(
                 COM_PORT_NAME, CheckBytesLeftToRead, HandleMessage));

    std::string                    request("VER\r\n");
    core_lib::asio::defs::char_buffer_t msgBuf(request.begin(), request.end());
    ASSERT_TRUE(serialPort->SendMsg(msgBuf));

    core_lib::SyncEvent delay;
    delay.WaitForTime(1000);
	
	EXPECT_TRUE(received);
}

#endif // DISABLE_ASIO_SERIAL_TESTS
