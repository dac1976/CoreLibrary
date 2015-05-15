#include <QString>
#include <QtTest>
#include <ostream>
#include "../../Include/DebugLog/DebugLogging.hpp"


// ****************************************************************************
// Unit test wrapper
// ****************************************************************************
class DebugLogTest : public QObject
{
    Q_OBJECT

public:
    DebugLogTest();

private Q_SLOTS:
    void testCase_DebugLog1();
    void testCase_DebugLog2();
    void testCase_DebugLog3();
    void testCase_DebugLog4();
    void testCase_DebugLog5();
    void testCase_DebugLog6();
    void testCase_DebugLog7();    
    void testCase_DebugLog8();
    void testCase_DebugLog9();
};

DebugLogTest::DebugLogTest()
{
    boost::filesystem::remove("test_log.txt");
    boost::filesystem::remove("test_log_old.txt");
}

// ****************************************************************************
// DebugLog tests
// ****************************************************************************
void DebugLogTest::testCase_DebugLog1()
{
    core_lib::log::DefaultLogFormat dlf;
    std::stringstream ss;
    time_t messageTime;
    time(&messageTime);
    int lineNo = __LINE__;
    dlf(ss, messageTime, "I am a test message", "Info", __FILE__, lineNo, std::this_thread::get_id());

    std::string time = ctime(&messageTime);
    std::replace_if(time.begin(), time.end(),
                    [](char c) { return (c == '\n') || (c == '\r'); }, 0);

    std::stringstream test;
    test << time.c_str()
         << " | I am a test message"
         << " | Info"
         << " | File = " << __FILE__
         << " | Line = " << lineNo
         << " | Thread ID = " << std::this_thread::get_id()
         << std::endl;

    QVERIFY(ss.str() == test.str());
}

void DebugLogTest::testCase_DebugLog2()
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
    }

    std::ifstream ifs("test_log.txt");
    QVERIFY(ifs.is_open());
    std::string line;
    size_t lineCount = 0;

    while(!ifs.eof())
    {
        std::getline(ifs, line);

        switch(++lineCount)
        {
        case 1:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STARTED");
            break;
        case 2:
            QVERIFY(line.substr(25, line.size() - 25) == "| Software Version 1.0.0.0");
            break;
        case 3:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STOPPED");
            break;
        case 4:
            QVERIFY(line == "");
            break;
        default:
            QFAIL("Too many lines");
        }
    }

    ifs.close();

    if (lineCount < 4)
    {
        QFAIL("Too few lines");
    }

    boost::filesystem::remove("test_log.txt");
}

void DebugLogTest::testCase_DebugLog3()
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        dl.AddLogMessage("Message 1", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 2", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 3", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
    }

    std::ifstream ifs("test_log.txt");
    QVERIFY(ifs.is_open());
    std::string line;
    size_t lineCount = 0;

    while(!ifs.eof())
    {
        std::getline(ifs, line);

        switch(++lineCount)
        {
        case 1:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STARTED");
            break;
        case 2:
            QVERIFY(line.substr(25, line.size() - 25) == "| Software Version 1.0.0.0");
            break;
        case 3:
            QVERIFY(line.substr(25, 13) == "| Message 1 |");
            break;
        case 4:
            QVERIFY(line.substr(25, 13) == "| Message 2 |");
            break;
        case 5:
            QVERIFY(line.substr(25, 13) == "| Message 3 |");
            break;
        case 6:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            QVERIFY(line == "");
            break;
        default:
            QFAIL("Too many lines");
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        QFAIL("Too few lines");
    }

    boost::filesystem::remove("test_log.txt");
}

void DebugLogTest::testCase_DebugLog4()
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log", 1024);
        dl.AddLogMessage("Message 1", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 2", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 3", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 4", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 5", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 6", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 7", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 8", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 9", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 10", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
    }

    bool filesExist = boost::filesystem::exists("test_log.txt")
                      && boost::filesystem::exists("test_log_old.txt");

    boost::filesystem::remove("test_log.txt");
    boost::filesystem::remove("test_log_old.txt");

    QVERIFY(filesExist);
}

void DebugLogTest::testCase_DebugLog5()
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        dl.AddLogMsgLevelFilter(core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 1", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::warning);
        dl.AddLogMessage("Message 2", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::info);
        dl.AddLogMessage("Message 3", __FILE__, __LINE__, core_lib::log::eLogMessageLevel::warning);
    }

    std::ifstream ifs("test_log.txt");
    QVERIFY(ifs.is_open());
    std::string line;
    size_t lineCount = 0;

    while(!ifs.eof())
    {
        std::getline(ifs, line);

        switch(++lineCount)
        {
        case 1:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STARTED");
            break;
        case 2:
            QVERIFY(line.substr(25, line.size() - 25) == "| Software Version 1.0.0.0");
            break;
        case 3:
            QVERIFY(line.substr(25, 13) == "| Message 2 |");
            break;
        case 4:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STOPPED");
            break;
        case 5:
            QVERIFY(line == "");
            break;
        default:
            QFAIL("Too many lines");
        }
    }

    ifs.close();

    if (lineCount < 5)
    {
        QFAIL("Too few lines");
    }

    boost::filesystem::remove("test_log.txt");
}

void DebugLogTest::testCase_DebugLog6()
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        DEBUG_LOG_EX(dl, "Message 1", core_lib::log::eLogMessageLevel::info);
        DEBUG_LOG_EX(dl, "Message 2", core_lib::log::eLogMessageLevel::info);
        DEBUG_LOG_EX(dl, "Message 3", core_lib::log::eLogMessageLevel::info);
    }

    std::ifstream ifs("test_log.txt");
    QVERIFY(ifs.is_open());
    std::string line;
    size_t lineCount = 0;

    while(!ifs.eof())
    {
        std::getline(ifs, line);

        switch(++lineCount)
        {
        case 1:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STARTED");
            break;
        case 2:
            QVERIFY(line.substr(25, line.size() - 25) == "| Software Version 1.0.0.0");
            break;
        case 3:
            QVERIFY(line.substr(25, 13) == "| Message 1 |");
            break;
        case 4:
            QVERIFY(line.substr(25, 13) == "| Message 2 |");
            break;
        case 5:
            QVERIFY(line.substr(25, 13) == "| Message 3 |");
            break;
        case 6:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            QVERIFY(line == "");
            break;
        default:
            QFAIL("Too many lines");
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        QFAIL("Too few lines");
    }

    boost::filesystem::remove("test_log.txt");
}

void DebugLogTest::testCase_DebugLog7()
{
    {
        core_lib::log::DebugLog<core_lib::log::DefaultLogFormat> dl("1.0.0.0", "", "test_log");
        DEBUG_LOG(dl, "Message 1");
        DEBUG_LOG(dl, "Message 2");
        DEBUG_LOG(dl, "Message 3");
    }

    std::ifstream ifs("test_log.txt");
    QVERIFY(ifs.is_open());
    std::string line;
    size_t lineCount = 0;

    while(!ifs.eof())
    {
        std::getline(ifs, line);

        switch(++lineCount)
        {
        case 1:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STARTED");
            break;
        case 2:
            QVERIFY(line.substr(25, line.size() - 25) == "| Software Version 1.0.0.0");
            break;
        case 3:
            QVERIFY(line.substr(25, 11) == "| Message 1");
            break;
        case 4:
            QVERIFY(line.substr(25, 11) == "| Message 2");
            break;
        case 5:
            QVERIFY(line.substr(25, 11) == "| Message 3");
            break;
        case 6:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            QVERIFY(line == "");
            break;
        default:
            QFAIL("Too many lines");
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        QFAIL("Too few lines");
    }

    boost::filesystem::remove("test_log.txt");
}

void DebugLogTest::testCase_DebugLog8()
{
    DEBUG_MESSAGE_INSTANTIATE("1.0.0.0", "", "test_log");
    Loki::DeletableSingleton<core_lib::log::default_log_t>::GracefulDelete();

    std::ifstream ifs("test_log.txt");
    QVERIFY(ifs.is_open());
    std::string line;
    size_t lineCount = 0;

    while(!ifs.eof())
    {
        std::getline(ifs, line);

        switch(++lineCount)
        {
        case 1:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STARTED");
            break;
        case 2:
            QVERIFY(line.substr(25, line.size() - 25) == "| Software Version 1.0.0.0");
            break;
        case 3:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STOPPED");
            break;
        case 4:
            QVERIFY(line == "");
            break;
        default:
            QFAIL("Too many lines");
        }
    }

    ifs.close();

    if (lineCount < 4)
    {
        QFAIL("Too few lines");
    }

    boost::filesystem::remove("test_log.txt");
}

void DebugLogTest::testCase_DebugLog9()
{
    DEBUG_MESSAGE_INSTANTIATE("1.0.0.0", "", "test_log");
    DEBUG_MESSAGE_EX("Message 1", core_lib::log::eLogMessageLevel::info);
    DEBUG_MESSAGE_EX("Message 2", core_lib::log::eLogMessageLevel::info);
    DEBUG_MESSAGE_EX("Message 3", core_lib::log::eLogMessageLevel::info);
    DEBUG_MESSAGE_DELETE_SINGLETON();

    std::ifstream ifs("test_log.txt");
    QVERIFY(ifs.is_open());
    std::string line;
    size_t lineCount = 0;

    while(!ifs.eof())
    {
        std::getline(ifs, line);

        switch(++lineCount)
        {
        case 1:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STARTED");
            break;
        case 2:
            QVERIFY(line.substr(25, line.size() - 25) == "| Software Version 1.0.0.0");
            break;
        case 3:
            QVERIFY(line.substr(25, 11) == "| Message 1");
            break;
        case 4:
            QVERIFY(line.substr(25, 11) == "| Message 2");
            break;
        case 5:
            QVERIFY(line.substr(25, 11) == "| Message 3");
            break;
        case 6:
            QVERIFY(line.substr(25, line.size() - 25) == "| DEBUG LOG STOPPED");
            break;
        case 7:
            QVERIFY(line == "");
            break;
        default:
            QFAIL("Too many lines");
        }
    }

    ifs.close();

    if (lineCount < 7)
    {
        QFAIL("Too few lines");
    }

    boost::filesystem::remove("test_log.txt");
}

QTEST_APPLESS_MAIN(DebugLogTest)

#include "tst_DebugLogTest.moc"
