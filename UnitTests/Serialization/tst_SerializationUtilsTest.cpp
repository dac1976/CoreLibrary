#include <QString>
#include <QtTest>
#include <string>
#include "boost/serialization/string.hpp"
#include "boost/serialization/vector.hpp"
#include "../../Include/Serialization/SerializeToVector.hpp"

class MyObject
{
public:
	MyObject() = default;
	~MyObject() { }

	float Fred() const { return fred; }
	void Fred(float _fred) { fred = _fred; }
	std::string Harry() const { return harry; }
	void Harry(const std::string& _harry) { harry = _harry; }
	auto George() -> std::vector<unsigned int> const { return george; }
	void George(const std::vector<unsigned int>& _george) { george = _george; }

	bool operator==(const MyObject& obj) const
	{
		return (this == &obj)
			   || ((fred == obj.fred)
				   && (harry == obj.harry)
				   && (george == obj.george));
	}

private:
	float fred{5.0};
	std::string harry{"Wibble!"};
	std::vector<unsigned int> george{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(fred);
		ar & BOOST_SERIALIZATION_NVP(harry);
		ar & BOOST_SERIALIZATION_NVP(george);
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

	void testCase_SerializeObjectPortBinArch();
	void testCase_SerializeObjectBinArch();
	void testCase_SerializeObjectTextArch();
	void testCase_SerializeObjectXmlArch();
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

void SerializationUtilsTest::testCase_SerializeObjectPortBinArch()
{
	using namespace core_lib::serialize;
	MyObject objectIn{};
	MyObject objectOut{};
	objectIn.Fred(10.0);
	objectIn.Harry("jnkjn");
	std::vector<unsigned int> vec{1, 2, 3, 4, 5};
	objectIn.George(vec);
	char_vector_t charVector;
	charVector = ToCharVector(objectIn);
	objectOut = ToObject<MyObject>(charVector);

	QVERIFY(objectOut == objectIn);
}

void SerializationUtilsTest::testCase_SerializeObjectBinArch()
{
	using namespace core_lib::serialize;
	MyObject objectIn{};
	MyObject objectOut{};
	objectIn.Fred(10.0);
	objectIn.Harry("jnkjn");
	std::vector<unsigned int> vec{1, 2, 3, 4, 5};
	objectIn.George(vec);
	char_vector_t charVector = ToCharVector<MyObject, boost_arch::binary_oarchive>(objectIn);
	objectOut = ToObject<MyObject, boost_arch::binary_iarchive>(charVector);

	QVERIFY(objectOut == objectIn);
}

void SerializationUtilsTest::testCase_SerializeObjectTextArch()
{
	using namespace core_lib::serialize;
	MyObject objectIn{};
	MyObject objectOut{};
	objectIn.Fred(10.0);
	objectIn.Harry("jnkjn");
	std::vector<unsigned int> vec{1, 2, 3, 4, 5};
	objectIn.George(vec);
	char_vector_t charVector;
	charVector = ToCharVector<MyObject, boost_arch::text_oarchive>(objectIn);
	objectOut = ToObject<MyObject, boost_arch::text_iarchive>(charVector);

	QVERIFY(objectOut == objectIn);
}

void SerializationUtilsTest::testCase_SerializeObjectXmlArch()
{
	using namespace core_lib::serialize;
	MyObject objectIn{};
	MyObject objectOut{};
	objectIn.Fred(10.0);
	objectIn.Harry("jnkjn");
	std::vector<unsigned int> vec{1, 2, 3, 4, 5};
	objectIn.George(vec);
	char_vector_t charVector;
	charVector = ToCharVector<MyObject, boost_arch::xml_oarchive>(objectIn);
	objectOut = ToObject<MyObject, boost_arch::xml_iarchive>(charVector);

	QVERIFY(objectOut == objectIn);
}


QTEST_APPLESS_MAIN(SerializationUtilsTest)

#include "tst_SerializationUtilsTest.moc"
