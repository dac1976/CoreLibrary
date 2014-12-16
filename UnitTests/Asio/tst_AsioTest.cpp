#include <QString>
#include <QtTest>

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
	
}


QTEST_APPLESS_MAIN(AsioTest)

#include "tst_AsioTest.moc"
