#include <QString>
#include <QtTest>
#include "../../IniFile.hpp"
#include <fstream>
#include "boost/predef.h"

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

    std::list<int> myList{1, 2, 3, 4, 5, 6,7, 8, 9, 10};
    std::list<int>::iterator i1{myList.begin()};
    std::list<int>::iterator i2{myList.begin()};
    std::list<int>::iterator i3{myList.begin()};
    std::list<int>::iterator i4{myList.begin()};
    std::list<int>::iterator i5{myList.begin()};
    std::advance(i1, 3);
    std::advance(i2, 4);
    std::advance(i3, 5);
    std::advance(i4, 2);
    std::advance(i5, 6);

    for (auto i : myList)
    {
        std::cout << i << ", ";
    }

    std::cout << std::endl;

    std::cout << "i1 = " << *i1 << std::endl;
    std::cout << "i2 = " << *i2 << std::endl;
    std::cout << "i3 = " << *i3 << std::endl;
    std::cout << "i4 = " << *i4  <<std::endl;
    std::cout << "i5 = " << *i5 << std::endl;

    myList.erase(i2);
    for (auto i : myList)
    {
        std::cout << i << ", ";
    }

    std::cout << std::endl;
    std::list<int>::iterator iNext{i1};
    std::list<int>::iterator iPrev{i1};
    std::cout << "i1 = " << *i1 << ", --i1 = " << *(--iPrev) << ", ++i1 = " << *(++iNext) <<std::endl;
    iNext = i3;
    iPrev = i3;
    std::cout << "i3 = " << *i3 << ", --i3 = " << *(--iPrev) << ", ++i3 = " << *(++iNext) <<std::endl;
    iNext = i4;
    iPrev = i4;
    std::cout << "i4 = " << *i4 << ", --i4 = " << *(--iPrev) << ", ++i4 = " << *(++iNext) <<std::endl;
    iNext = i5;
    iPrev = i5;
    std::cout << "i5 = " << *i5 << ", --i5 = " << *(--iPrev) << ", ++i5 = " << *(++iNext) <<std::endl;
}

QTEST_APPLESS_MAIN(IniFileTest)

#include "tst_IniFileTest.moc"
