#include <QString>
#include <QtTest>
#include "../../StringUtils.hpp"

class StringUtilsTest : public QObject
{
    Q_OBJECT

public:
    StringUtilsTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void Case1_TidyStringAfterGetLine();
    void Case2_xSplitStringBadDelim_1();
    void Case3_xSplitStringBadDelim_2();
    void Case4_xSplitStringTooManySubstrings_1();
    void Case5_xSplitStringTooManySubstrings_2();
    void Case6_SplitString_1();
    void Case7_SplitString_2();
    void Case8_SplitString_3();
    void Case9_SplitString_4();
    void Case10_FormatFloatString_1();
    void Case11_FormatFloatString_2();
    void Case12_FormatFloatString_3();
    void Case13_FormatFloatString_4();
    void Case14_FormatFloatString_5();
    void Case15_FormatFloatString_6();
    void Case16_FormatFloatString_rvalue();
};

StringUtilsTest::StringUtilsTest()
{
}

void StringUtilsTest::initTestCase()
{
}

void StringUtilsTest::cleanupTestCase()
{
}

void StringUtilsTest::Case1_TidyStringAfterGetLine()
{
    const std::string comparator = "I am a test string.";
    std::string testString = "I am a test string. I am a test string.";
    testString[19] = '\0';
    QVERIFY(comparator.size() != testString.size());

    core_lib::string_utils::TidyStringAfterGetLine(testString);
    QVERIFY(comparator.size() == testString.size());
    QCOMPARE(comparator, testString);
}

void StringUtilsTest::Case2_xSplitStringBadDelim_1()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringBadDelim());
    }
    catch(core_lib::string_utils::xSplitStringBadDelim& e)
    {
        QCOMPARE(e.what(), "split string bad delimiter");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void StringUtilsTest::Case3_xSplitStringBadDelim_2()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringBadDelim("user defined message"));
    }
    catch(core_lib::string_utils::xSplitStringBadDelim& e)
    {
        QCOMPARE(e.what(), "user defined message");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void StringUtilsTest::Case4_xSplitStringTooManySubstrings_1()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringTooManySubstrings());
    }
    catch(core_lib::string_utils::xSplitStringTooManySubstrings& e)
    {
        QCOMPARE(e.what(), "too many substrings");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void StringUtilsTest::Case5_xSplitStringTooManySubstrings_2()
{
    try
    {
        BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringTooManySubstrings("user defined message"));
    }
    catch(core_lib::string_utils::xSplitStringTooManySubstrings& e)
    {
        QCOMPARE(e.what(), "user defined message");
        std::string info = boost::diagnostic_information(e);
        QVERIFY(info != "");
    }
}

void StringUtilsTest::Case6_SplitString_1()
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "value1,value2,value3";
        std::string subStr1,  subStr2;
        core_lib::string_utils::SplitString(subStr1, subStr2, toSplit, "=",
                                            core_lib::string_utils::eSplitStringResult::trimmed);
        exceptionThrown = false;
    }
    catch(core_lib::string_utils::xSplitStringBadDelim& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch(...)
    {
        exceptionThrown = false;
    }

    QVERIFY(exceptionThrown);
}

void StringUtilsTest::Case7_SplitString_2()
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "value1,value2,value3";
        std::string subStr1,  subStr2;
        core_lib::string_utils::SplitString(subStr1, subStr2, toSplit, ",",
                                            core_lib::string_utils::eSplitStringResult::trimmed);
        exceptionThrown = false;
    }
    catch(core_lib::string_utils::xSplitStringTooManySubstrings& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch(...)
    {
        exceptionThrown = false;
    }

    QVERIFY(exceptionThrown);
}

void StringUtilsTest::Case8_SplitString_3()
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "key = value";
        std::string subStr1,  subStr2;
        core_lib::string_utils::SplitString(subStr1, subStr2, toSplit, "=",
                                            core_lib::string_utils::eSplitStringResult::notTrimmed);
        exceptionThrown = false;
        QCOMPARE(subStr1.c_str(), "key ");
        QCOMPARE(subStr2.c_str(), " value");
    }
    catch(...)
    {
        exceptionThrown = true;
    }

    QVERIFY(!exceptionThrown);
}

void StringUtilsTest::Case9_SplitString_4()
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "key = value";
        std::string subStr1,  subStr2;
        core_lib::string_utils::SplitString(subStr1, subStr2, toSplit, "=",
                                            core_lib::string_utils::eSplitStringResult::trimmed);
        exceptionThrown = false;
        QCOMPARE(subStr1.c_str(), "key");
        QCOMPARE(subStr2.c_str(), "value");
    }
    catch(...)
    {
        exceptionThrown = true;
    }

    QVERIFY(!exceptionThrown);
}

void StringUtilsTest::Case10_FormatFloatString_1()
{
    float value = 1234.123;
    std::string result;
    core_lib::string_utils::FormatFloatString(result, value, 15, core_lib::string_utils::eFloatStringFormat::normal);
    QVERIFY((result.size() <= 20U) && (result.size() >= 8U));
    QCOMPARE(result.substr(0, 8).c_str(), "1234.123");
}

void StringUtilsTest::Case11_FormatFloatString_2()
{
    float value = 1234.123;
    std::string result;
    core_lib::string_utils::FormatFloatString(result, value, 15, core_lib::string_utils::eFloatStringFormat::fixed);
    QVERIFY(result.size() == 20U);
    QCOMPARE(result.substr(0, 8).c_str(), "1234.123");
}

void StringUtilsTest::Case12_FormatFloatString_3()
{
    float value = 1234.123;
    std::string result;
    core_lib::string_utils::FormatFloatString(result, value, 15, core_lib::string_utils::eFloatStringFormat::scientific);
    QVERIFY(result.size() == 21U);
    QCOMPARE(result.substr(0, 8).c_str(), "1.234123");
}

void StringUtilsTest::Case13_FormatFloatString_4()
{
    double value = 1234.123;
    std::string result;
    core_lib::string_utils::FormatFloatString(result, value, 15, core_lib::string_utils::eFloatStringFormat::normal);
    QVERIFY((result.size() <= 20U) && (result.size() >= 8U));
    QCOMPARE(result.substr(0, 8).c_str(), "1234.123");
}

void StringUtilsTest::Case14_FormatFloatString_5()
{
    double value = 1234.123;
    std::string result;
    core_lib::string_utils::FormatFloatString(result, value, 15, core_lib::string_utils::eFloatStringFormat::fixed);
    QVERIFY(result.size() == 20U);
    QCOMPARE(result.substr(0, 8).c_str(), "1234.123");
}

void StringUtilsTest::Case15_FormatFloatString_6()
{
    double value = 1234.123;
    std::string result;
    core_lib::string_utils::FormatFloatString(result, value, 15, core_lib::string_utils::eFloatStringFormat::scientific);
    QVERIFY(result.size() == 21U);
    QCOMPARE(result.substr(0, 8).c_str(), "1.234123");
}

void StringUtilsTest::Case16_FormatFloatString_rvalue()
{
    std::string result;
    core_lib::string_utils::FormatFloatString(result, 1234.123, 15, core_lib::string_utils::eFloatStringFormat::normal);
    QVERIFY((result.size() <= 20U) && (result.size() >= 8U));
    QCOMPARE(result.substr(0, 8).c_str(), "1234.123");
}


QTEST_APPLESS_MAIN(StringUtilsTest)

#include "tst_StringUtilsTest.moc"
