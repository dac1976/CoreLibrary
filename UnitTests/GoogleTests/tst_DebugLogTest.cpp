#ifndef DISABLE_DEBUGLOG_TESTS

#include <ostream>
#include "DebugLog/DebugLogging.h"
#include "boost/filesystem.hpp"

#include "gtest/gtest.h"

namespace bfs = boost::filesystem;

// ****************************************************************************
// DebugLogTest Fixture
// ****************************************************************************
namespace
{
// Fixture class for handling unit tests' shared data and setup.
class DebugLogTest : public ::testing::Test
{
protected:
    DebugLogTest()
    {
        bfs::remove("test_log.txt");
        bfs::remove("test_log_old.txt");
    }
};

} // End of unnamed namespace.

// ****************************************************************************
// DebugLog tests
// ****************************************************************************

TEST_F(DebugLogTest, testCase_DebugLog1)
{
    core_lib::log::DefaultLogFormat dlf;
    std::stringstream               ss;
    using std::chrono::system_clock;
    time_t messageTime = system_clock::to_time_t(system_clock::now());
    int    lineNo      = __LINE__;
    dlf(ss,
        messageTime,
        "I am a test message",
        "Info",
        __FILE__,
        BOOST_CURRENT_FUNCTION,
        lineNo,
        std::this_thread::get_id());

    std::stringstream time;
    time << std::put_time(std::localtime(&messageTime), "%F %T");

    std::stringstream test;
    test << time.str() << " | Info"
         << " | I am a test message"
         << " | " << __FILE__ << " | " << BOOST_CURRENT_FUNCTION << " | Line = " << lineNo
         << " | Thread ID = " << std::this_thread::get_id() << std::endl;

    EXPECT_TRUE(ss.str() == test.str());
}

TEST_F(DebugLogTest, testCase_DebugLog2)
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
    }

    std::ifstream ifs("test_log.txt");
    EXPECT_TRUE(ifs.is_open());
    std::string line;
    size_t      lineCount = 0;

    while (!ifs.eof())
    {
        std::getline(ifs, line);

        switch (++lineCount)
        {
        case 1:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STARTED");
            break;
        case 2:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| Software Version 1.0.0.0");
            break;
        case 3:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STOPPED");
            break;
        case 4:
            EXPECT_TRUE(line == "");
            break;
        default:
            FAIL() << "Too many lines";
        }
    }

    ifs.close();

    if (lineCount < 4)
    {
        FAIL() << "Too many lines";
    }

    bfs::remove("test_log.txt");
}

TEST_F(DebugLogTest, testCase_DebugLog3)
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        dl.AddLogMessage("Message 1",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 2",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 3",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
    }

    std::ifstream ifs("test_log.txt");
    EXPECT_TRUE(ifs.is_open());
    std::string line;
    size_t      lineCount = 0;

    while (!ifs.eof())
    {
        std::getline(ifs, line);

        switch (++lineCount)
        {
        case 1:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STARTED");
            break;
        case 2:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| Software Version 1.0.0.0");
            break;
        case 3:
            EXPECT_TRUE(line.substr(27, 13) == "| Message 1 |");
            break;
        case 4:
            EXPECT_TRUE(line.substr(27, 13) == "| Message 2 |");
            break;
        case 5:
            EXPECT_TRUE(line.substr(27, 13) == "| Message 3 |");
            break;
        case 6:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            EXPECT_TRUE(line == "");
            break;
        default:
            FAIL() << "Too many lines";
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        FAIL() << "Too many lines";
    }

    bfs::remove("test_log.txt");
}

TEST_F(DebugLogTest, testCase_DebugLog4)
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl(
            "1.0.0.0", "", "test_log", 1024);
        dl.AddLogMessage("Message 1",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 2",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 3",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 4",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 5",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 6",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 7",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 8",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 9",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 10",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
    }

    bool filesExist = bfs::exists("test_log.txt") && bfs::exists("test_log_old.txt");

    bfs::remove("test_log.txt");
    bfs::remove("test_log_old.txt");

    EXPECT_TRUE(filesExist);
}

TEST_F(DebugLogTest, testCase_DebugLog5)
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        dl.AddLogMsgLevelFilter(core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 1",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 2",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 3",
                         __FILE__,
                         BOOST_CURRENT_FUNCTION,
                         __LINE__,
                         core_lib::log::eLogMessageLevel::warning);
    }

    std::ifstream ifs("test_log.txt");
    EXPECT_TRUE(ifs.is_open());
    std::string line;
    size_t      lineCount = 0;

    while (!ifs.eof())
    {
        std::getline(ifs, line);

        switch (++lineCount)
        {
        case 1:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STARTED");
            break;
        case 2:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| Software Version 1.0.0.0");
            break;
        case 3:
            EXPECT_TRUE(line.substr(27, 13) == "| Message 2 |");
            break;
        case 4:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STOPPED");
            break;
        case 5:
            EXPECT_TRUE(line == "");
            break;
        default:
            FAIL() << "Too many lines";
        }
    }

    ifs.close();

    if (lineCount < 5)
    {
        FAIL() << "Too many lines";
    }

    bfs::remove("test_log.txt");
}

TEST_F(DebugLogTest, testCase_DebugLog6)
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        DEBUG_LOG_EX(dl, "Message 1", core_lib::log::eLogMessageLevel::info);
        DEBUG_LOG_EX(dl, "Message 2", core_lib::log::eLogMessageLevel::info);
        DEBUG_LOG_EX(dl, "Message 3", core_lib::log::eLogMessageLevel::info);
    }

    std::ifstream ifs("test_log.txt");
    EXPECT_TRUE(ifs.is_open());
    std::string line;
    size_t      lineCount = 0;

    while (!ifs.eof())
    {
        std::getline(ifs, line);

        switch (++lineCount)
        {
        case 1:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STARTED");
            break;
        case 2:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| Software Version 1.0.0.0");
            break;
        case 3:
            EXPECT_TRUE(line.substr(27, 13) == "| Message 1 |");
            break;
        case 4:
            EXPECT_TRUE(line.substr(27, 13) == "| Message 2 |");
            break;
        case 5:
            EXPECT_TRUE(line.substr(27, 13) == "| Message 3 |");
            break;
        case 6:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            EXPECT_TRUE(line == "");
            break;
        default:
            FAIL() << "Too many lines";
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        FAIL() << "Too many lines";
    }

    bfs::remove("test_log.txt");
}

TEST_F(DebugLogTest, testCase_DebugLog7)
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        DEBUG_LOG(dl, "Message 1");
        DEBUG_LOG(dl, "Message 2");
        DEBUG_LOG(dl, "Message 3");
    }

    std::ifstream ifs("test_log.txt");
    EXPECT_TRUE(ifs.is_open());
    std::string line;
    size_t      lineCount = 0;

    while (!ifs.eof())
    {
        std::getline(ifs, line);

        switch (++lineCount)
        {
        case 1:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STARTED");
            break;
        case 2:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| Software Version 1.0.0.0");
            break;
        case 3:
            EXPECT_TRUE(line.substr(20, 11) == "| Message 1");
            break;
        case 4:
            EXPECT_TRUE(line.substr(20, 11) == "| Message 2");
            break;
        case 5:
            EXPECT_TRUE(line.substr(20, 11) == "| Message 3");
            break;
        case 6:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            EXPECT_TRUE(line == "");
            break;
        default:
            FAIL() << "Too many lines";
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        FAIL() << "Too many lines";
    }

    bfs::remove("test_log.txt");
}

TEST_F(DebugLogTest, testCase_DebugLog8)
{
    DEBUG_MESSAGE_INSTANTIATE("1.0.0.0", "", "test_log");
    Loki::DeletableSingleton<core_lib::log::default_log_t>::GracefulDelete();

    std::ifstream ifs("test_log.txt");
    EXPECT_TRUE(ifs.is_open());
    std::string line;
    size_t      lineCount = 0;

    while (!ifs.eof())
    {
        std::getline(ifs, line);

        switch (++lineCount)
        {
        case 1:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STARTED");
            break;
        case 2:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| Software Version 1.0.0.0");
            break;
        case 3:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STOPPED");
            break;
        case 4:
            EXPECT_TRUE(line == "");
            break;
        default:
            FAIL() << "Too many lines";
        }
    }

    ifs.close();

    if (lineCount < 4)
    {
        FAIL() << "Too many lines";
    }

    bfs::remove("test_log.txt");
}

TEST_F(DebugLogTest, testCase_DebugLog9)
{
    DEBUG_MESSAGE_INSTANTIATE("1.0.0.0", "", "test_log");
    DEBUG_MESSAGE_EX("Message 1", core_lib::log::eLogMessageLevel::info);
    DEBUG_MESSAGE_EX("Message 2", core_lib::log::eLogMessageLevel::info);
    DEBUG_MESSAGE_EX("Message 3", core_lib::log::eLogMessageLevel::info);
    DEBUG_MESSAGE_DELETE_SINGLETON();

    std::ifstream ifs("test_log.txt");
    EXPECT_TRUE(ifs.is_open());
    std::string line;
    size_t      lineCount = 0;

    while (!ifs.eof())
    {
        std::getline(ifs, line);

        switch (++lineCount)
        {
        case 1:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STARTED");
            break;
        case 2:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| Software Version 1.0.0.0");
            break;
        case 3:
            EXPECT_TRUE(line.substr(27, 11) == "| Message 1");
            break;
        case 4:
            EXPECT_TRUE(line.substr(27, 11) == "| Message 2");
            break;
        case 5:
            EXPECT_TRUE(line.substr(27, 11) == "| Message 3");
            break;
        case 6:
            EXPECT_TRUE(line.substr(20, line.size() - 20) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            EXPECT_TRUE(line == "");
            break;
        default:
            FAIL() << "Too many lines";
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        FAIL() << "Too many lines";
    }

    bfs::remove("test_log.txt");
}

#endif // DISABLE_DEBUGLOG_TESTS
