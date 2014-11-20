#include <QString>
#include <QtTest>
#include <string>
#include <boost/serialization/string.hpp>
#include "../../Serialization/SerializeToVector.hpp"

class MyObject
{
public:
    MyObject() = default;
    ~MyObject() { }

    float Fred() const { return fred; }
    void Fred(float _fred) { fred = _fred; }
    std::string Harry() const { return harry; }
    void Harry(const std::string& _harry) { harry = _harry; }

    bool operator==(const MyObject& obj) const
    {
        return (this == &obj)
               || ((fred == obj.fred)
                   && (harry == obj.harry));
    }

private:
    float fred{5.0};
    std::string harry{"Wibble!"};

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int /*version*/)
    {
        ar & fred;
        ar & harry;
    }
};

class SerializationUtilsTest : public QObject
{
    Q_OBJECT

public:
    SerializationUtilsTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_SerializeObject();
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

void SerializationUtilsTest::testCase_SerializeObject()
{
    using namespace core_lib::serialize;
    MyObject objectIn{};
    MyObject objectOut{};
    objectIn.Fred(10.0);
    objectIn.Harry("jnkjn");
    char_vector charVector;
    charVector = ToCharVector(objectIn);
    objectOut = ToObject<MyObject>(charVector);

    QVERIFY(objectOut == objectIn);
}

QTEST_APPLESS_MAIN(SerializationUtilsTest)

#include "tst_SerializationUtilsTest.moc"
