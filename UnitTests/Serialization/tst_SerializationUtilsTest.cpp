#include <QString>
#include <QtTest>
#include "../../Serialization/SerializeToVector.hpp"

class SerializationUtilsTest : public QObject
{
    Q_OBJECT

public:
    SerializationUtilsTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
};

SerializationUtilsTest::SerializationUtilsTest()
{
}

void SerializationUtilsTest::initTestCase()
{
}

void SerializationUtilsTest::cleanupTestCase()
{
}

QTEST_APPLESS_MAIN(SerializationUtilsTest)

#include "tst_SerializationUtilsTest.moc"
