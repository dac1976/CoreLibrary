#include <string>
#include "boost/serialization/string.hpp"
#include "boost/serialization/vector.hpp"
#include "Serialization/SerializeToVector.h"

#include "gtest/gtest.h"

namespace 
{
// Helper class to faciltate the test cases.
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

} // End of unnamed namespace.

// Unit test cases.
TEST(SerializationUtilsTest, testCase_SerializeObjectPortBinArch)
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

	EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectBinArch)
{
	using namespace core_lib::serialize;
	MyObject objectIn{};
	MyObject objectOut{};
	objectIn.Fred(10.0);
	objectIn.Harry("jnkjn");
	std::vector<unsigned int> vec{1, 2, 3, 4, 5};
	objectIn.George(vec);
	char_vector_t charVector = ToCharVector<MyObject, archives::out_bin_t>(objectIn);
	objectOut = ToObject<MyObject, archives::in_bin_t>(charVector);

	EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectTextArch)
{
	using namespace core_lib::serialize;
	MyObject objectIn{};
	MyObject objectOut{};
	objectIn.Fred(10.0);
	objectIn.Harry("jnkjn");
	std::vector<unsigned int> vec{1, 2, 3, 4, 5};
	objectIn.George(vec);
	char_vector_t charVector;
	charVector = ToCharVector<MyObject, archives::out_txt_t>(objectIn);
	objectOut = ToObject<MyObject, archives::in_txt_t>(charVector);

	EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectXmlArch)
{
	using namespace core_lib::serialize;
	MyObject objectIn{};
	MyObject objectOut{};
	objectIn.Fred(10.0);
	objectIn.Harry("jnkjn");
	std::vector<unsigned int> vec{1, 2, 3, 4, 5};
	objectIn.George(vec);
	char_vector_t charVector;
	charVector = ToCharVector<MyObject, archives::out_xml_t>(objectIn);
	objectOut = ToObject<MyObject, archives::in_xml_t>(charVector);

	EXPECT_EQ(objectOut, objectIn);
}
