#ifndef DISABLE_SERIALIZATION_TESTS

#include <string>
#include <boost/predef.h>
#include "Serialization/SerializeToVector.h"
#include <cereal/types/string.hpp>
#include "gtest/gtest.h"
#include "gtest_cout.h"

namespace
{
// Helper class to faciltate the test cases.
class MyObject
{
public:
    MyObject()
    {
        // Required to initialise vectors in a way that works with msvc++
        // and gcc/clang (initializer lists in constructors or where member
        // is declared do not work in msvc 2013 and earlier).
        for (unsigned int n = 1U; n <= 10U; ++n)
        {
            george.emplace_back(n);
        }
    }

    float Fred() const
    {
        return fred;
    }
    void Fred(float _fred)
    {
        fred = _fred;
    }
    std::string Harry() const
    {
        return harry;
    }
    void Harry(const std::string& _harry)
    {
        harry = _harry;
    }
    auto George() -> std::vector<unsigned int> const
    {
        return george;
    }
    void George(const std::vector<unsigned int>& _george)
    {
        george = _george;
    }

    bool operator==(const MyObject& obj) const
    {
        return (this == &obj) ||
               ((fred == obj.fred) && (harry == obj.harry) && (george == obj.george));
    }

private:
    float                     fred{5.0f};
    std::string               harry{"Wibble!"};
    std::vector<unsigned int> george{};

#if defined(USE_BOOST_SERIALIZATION)
    friend class boost::serialization::access;
#else
    friend class cereal::access;
#endif

    template <class Archive> void serialize(Archive& ar, const unsigned int /*version*/)
    {
#if defined(USE_BOOST_SERIALIZATION)
        ar& BOOST_SERIALIZATION_NVP(fred);
        ar& BOOST_SERIALIZATION_NVP(harry);
        ar& BOOST_SERIALIZATION_NVP(george);
#else
        ar(CEREAL_NVP(fred));
        ar(CEREAL_NVP(harry));
        ar(CEREAL_NVP(george));
#endif
    }
};

} // End of unnamed namespace.

CEREAL_CLASS_VERSION(MyObject, 1)

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
    objectOut  = ToObject<MyObject>(charVector);

    EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectPortBinArch_Alt)
{
    using namespace core_lib::serialize;
    MyObject objectIn{};
    MyObject objectOut{};
    objectIn.Fred(10.0);
    objectIn.Harry("jnkjn");
    std::vector<unsigned int> vec{1, 2, 3, 4, 5};
    objectIn.George(vec);
    char_vector_t charVector;
    ToCharVector(objectIn, charVector);
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
    objectOut                = ToObject<MyObject, archives::in_bin_t>(charVector);

    EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectBinArch_Alt)
{
    using namespace core_lib::serialize;
    MyObject objectIn{};
    MyObject objectOut{};
    objectIn.Fred(10.0);
    objectIn.Harry("jnkjn");
    std::vector<unsigned int> vec{1, 2, 3, 4, 5};
    objectIn.George(vec);
    char_vector_t charVector;
    ToCharVector<MyObject, archives::out_bin_t>(objectIn, charVector);
    objectOut = ToObject<MyObject, archives::in_bin_t>(charVector);

    EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectTxtArch)
{
    using namespace core_lib::serialize;
    MyObject objectIn{};
    MyObject objectOut{};
    objectIn.Fred(10.0);
    objectIn.Harry("jnkjn");
    std::vector<unsigned int> vec{1, 2, 3, 4, 5};
    objectIn.George(vec);
    char_vector_t charVector;
    charVector = ToCharVector<MyObject, archives::out_json_t>(objectIn);
    objectOut  = ToObject<MyObject, archives::in_json_t>(charVector);

    EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectTxtArch_Alt)
{
    using namespace core_lib::serialize;
    MyObject objectIn{};
    MyObject objectOut{};
    objectIn.Fred(10.0);
    objectIn.Harry("jnkjn");
    std::vector<unsigned int> vec{1, 2, 3, 4, 5};
    objectIn.George(vec);
    char_vector_t charVector;
    ToCharVector<MyObject, archives::out_json_t>(objectIn, charVector);
    objectOut = ToObject<MyObject, archives::in_json_t>(charVector);

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
    objectOut  = ToObject<MyObject, archives::in_xml_t>(charVector);

    EXPECT_EQ(objectOut, objectIn);
}

TEST(SerializationUtilsTest, testCase_SerializeObjectXmlArch_Alt)
{
    using namespace core_lib::serialize;
    MyObject objectIn{};
    MyObject objectOut{};
    objectIn.Fred(10.0);
    objectIn.Harry("jnkjn");
    std::vector<unsigned int> vec{1, 2, 3, 4, 5};
    objectIn.George(vec);
    char_vector_t charVector;
    ToCharVector<MyObject, archives::out_xml_t>(objectIn, charVector);
    objectOut = ToObject<MyObject, archives::in_xml_t>(charVector);

    EXPECT_EQ(objectOut, objectIn);
}

#endif // DISABLE_SERIALIZATION_TESTS
