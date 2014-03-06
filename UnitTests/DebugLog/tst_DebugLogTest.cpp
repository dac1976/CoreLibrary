#include <QString>
#include <QtTest>
#include <ostream>
#include "../../DebugLog.hpp"


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
};

DebugLogTest::DebugLogTest()
{
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
    int line = __LINE__;
    dlf(ss, "Info", messageTime, __FILE__, line, std::this_thread::get_id(), "I am a test message");

    std::cout << ss.str();
    std::string time = ctime(&messageTime);
    std::replace_if(time.begin(), time.end(),
                    [](char c) { return (c == '\n') || (c == '\r'); }, 0);

    std::stringstream test;
    test << "< Level: Info >"
         << "< Time: " << time.c_str() << " >"
         << "< File: " << __FILE__  << " >"
         << "< Line: " << line << " >"
         << "< Thread: " << std::this_thread::get_id() << " >"
         << "< Message: I am a test message >"
         << std::endl;

    std::cout << test.str();

    QVERIFY(ss.str() == test.str());
}

QTEST_APPLESS_MAIN(DebugLogTest)

#include "tst_DebugLogTest.moc"
