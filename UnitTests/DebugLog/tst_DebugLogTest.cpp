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

ThreadsTest::ThreadsTest()
{
}

// ****************************************************************************
// DebugLog tests
// ****************************************************************************
void DebugLogTest::testCase_DebugLog1()
{
    
}

QTEST_APPLESS_MAIN(DebugLogTest)

#include "tst_DebugLogTest.moc"
