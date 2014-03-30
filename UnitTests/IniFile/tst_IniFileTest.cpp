#include <QString>
#include <QtTest>
#include "../../IniFile.hpp"

class IniFileTest : public QObject
{
    Q_OBJECT

public:
    IniFileTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void Case1();
};

IniFileTest::IniFileTest()
{
}

void IniFileTest::initTestCase()
{
}

void IniFileTest::cleanupTestCase()
{
}

void IniFileTest::Case1()
{
    
}

QTEST_APPLESS_MAIN(IniFileTest)

#include "tst_IniFileTest.moc"
