#ifndef DISABLE_STRINGUTILS_TESTS

#include "StringUtils/StringUtils.h"
#include <boost/predef.h>

#include "gtest/gtest.h"

TEST(StringUtilsTest, PackStdString)
{
    const std::string comparator = "I am a test string.";
    std::string       testString = "I am a test string. I am a test string.";
    testString[19]               = '\0';
    EXPECT_NE(comparator.size(), testString.size());

    core_lib::string_utils::PackStdString(testString);
    EXPECT_EQ(comparator.size(), testString.size());
    EXPECT_EQ(comparator, testString);
}

TEST(StringUtilsTest, SplitString_1)
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "value1,value2,value3";
        std::string subStr1, subStr2;
        core_lib::string_utils::SplitString(
            subStr1, subStr2, toSplit, "=", core_lib::string_utils::eSplitStringResult::trimmed);
        exceptionThrown = false;
    }
    catch (std::invalid_argument& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch (...)
    {
        exceptionThrown = false;
    }

    EXPECT_TRUE(exceptionThrown);
}

TEST(StringUtilsTest, SplitString_2)
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "value1,value2,value3";
        std::string subStr1, subStr2;
        core_lib::string_utils::SplitString(
            subStr1, subStr2, toSplit, ",", core_lib::string_utils::eSplitStringResult::trimmed);
        exceptionThrown = false;
    }
    catch (std::runtime_error& e)
    {
        (void)e;
        exceptionThrown = true;
    }
    catch (...)
    {
        exceptionThrown = false;
    }

    EXPECT_TRUE(exceptionThrown);
}

TEST(StringUtilsTest, SplitString_3)
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "key = value";
        std::string subStr1, subStr2;
        core_lib::string_utils::SplitString(
            subStr1, subStr2, toSplit, "=", core_lib::string_utils::eSplitStringResult::notTrimmed);
        exceptionThrown = false;
        EXPECT_STREQ(subStr1.c_str(), "key ");
        EXPECT_STREQ(subStr2.c_str(), " value");
    }
    catch (...)
    {
        exceptionThrown = true;
    }

    EXPECT_TRUE(!exceptionThrown);
}

TEST(StringUtilsTest, SplitString_4)
{
    bool exceptionThrown;

    try
    {
        std::string toSplit = "key = value";
        std::string subStr1, subStr2;
        core_lib::string_utils::SplitString(
            subStr1, subStr2, toSplit, "=", core_lib::string_utils::eSplitStringResult::trimmed);
        exceptionThrown = false;
        EXPECT_STREQ(subStr1.c_str(), "key");
        EXPECT_STREQ(subStr2.c_str(), "value");
    }
    catch (...)
    {
        exceptionThrown = true;
    }

    EXPECT_TRUE(!exceptionThrown);
}

TEST(StringUtilsTest, FormatFloatString_1)
{
    float       value  = 1234.123f;
    std::string result = core_lib::string_utils::FormatFloatString(
        value, 15, core_lib::string_utils::eFloatStringFormat::normal);
    EXPECT_TRUE((result.size() <= 20U) && (result.size() >= 8U));
    EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, FormatFloatString_2)
{
    float       value  = 1234.123f;
    std::string result = core_lib::string_utils::FormatFloatString(
        value, 15, core_lib::string_utils::eFloatStringFormat::fixed);
    EXPECT_TRUE(result.size() == 20U);
    EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, FormatFloatString_3)
{
    float       value  = 1234.123f;
    std::string result = core_lib::string_utils::FormatFloatString(
        value, 15, core_lib::string_utils::eFloatStringFormat::scientific);
#if BOOST_OS_WINDOWS
#if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
    EXPECT_TRUE(result.size() == 21U);
#else
    EXPECT_TRUE(result.size() == 22U);
#endif
#else
    EXPECT_TRUE(result.size() == 21U);
#endif
    EXPECT_STREQ(result.substr(0, 8).c_str(), "1.234123");
}

TEST(StringUtilsTest, FormatFloatString_4)
{
    double      value  = 1234.123;
    std::string result = core_lib::string_utils::FormatFloatString(
        value, 15, core_lib::string_utils::eFloatStringFormat::normal);
    EXPECT_TRUE((result.size() <= 20U) && (result.size() >= 8U));
    EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, FormatFloatString_5)
{
    double      value  = 1234.123;
    std::string result = core_lib::string_utils::FormatFloatString(
        value, 15, core_lib::string_utils::eFloatStringFormat::fixed);
    EXPECT_TRUE(result.size() == 20U);
    EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, FormatFloatString_6)
{
    double      value  = 1234.123;
    std::string result = core_lib::string_utils::FormatFloatString(
        value, 15, core_lib::string_utils::eFloatStringFormat::scientific);
#if BOOST_OS_WINDOWS
#if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
    EXPECT_TRUE(result.size() == 21U);
#else
    EXPECT_TRUE(result.size() == 22U);
#endif
#else
    EXPECT_TRUE(result.size() == 21U);
#endif
    EXPECT_STREQ(result.substr(0, 8).c_str(), "1.234123");
}

TEST(StringUtilsTest, FormatFloatString_rvalue)
{
    std::string result = core_lib::string_utils::FormatFloatString(
        1234.123, 15, core_lib::string_utils::eFloatStringFormat::normal);
    EXPECT_TRUE((result.size() <= 20U) && (result.size() >= 8U));
    EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, AutoFormatFloatString_1)
{
    std::string result = core_lib::string_utils::AutoFormatFloatString(1234.1234, 1);
    EXPECT_EQ(result, "1234.1");
}

TEST(StringUtilsTest, AutoFormatFloatString_2)
{
    std::string result = core_lib::string_utils::AutoFormatFloatString(0.12341234, 4);
    EXPECT_EQ(result, "0.1234");
}

TEST(StringUtilsTest, AutoFormatFloatString_3)
{
    std::string result = core_lib::string_utils::AutoFormatFloatString(12341234.1234, 3);
    EXPECT_EQ(result, "1.234e+07");
}

TEST(StringUtilsTest, RemoveIllegalChars_Wide)
{
    std::wstring source = L"+I\\contain| bad* chars\" that<need>: to be#removed% and{replaced} "
                          L"with a safe&char/ for file? names~";
    std::wstring check = L"_I_contain_ bad_ chars_ that_need__ to be_removed_ and_replaced_ with a "
                         L"safe_char_ for file_ names_";
    auto result = core_lib::string_utils::RemoveIllegalChars(source);
    EXPECT_EQ(result, check);
}

TEST(StringUtilsTest, RemoveIllegalChars_Narrow)
{
    std::string source = "+I\\contain| bad* chars\" that<need>: to be#removed% and{replaced} "
                         "with a safe&char/ for file? names~";
    std::string check = "_I_contain_ bad_ chars_ that_need__ to be_removed_ and_replaced_ with a "
                        "safe_char_ for file_ names_";
    auto result = core_lib::string_utils::RemoveIllegalChars(source);
    EXPECT_EQ(result, check);
}

TEST(StringUtilsTest, StringConversion)
{
    std::string  narrow     = "I am a test string";
    std::wstring wide       = L"I am a test string";
    auto         wideResult = core_lib::string_utils::StringToWString(narrow);
    EXPECT_EQ(wide, wideResult);

    auto narrowResult = core_lib::string_utils::WStringToString(wide);
    EXPECT_EQ(narrow, narrowResult);
}

TEST(StringUtilsTest, Tokenise1)
{
    auto                 substrings = core_lib::string_utils::TokeniseString("1-2-3-4", "-", true);
    decltype(substrings) comp       = {"1", "2", "3", "4"};
    EXPECT_EQ(substrings, comp);

    substrings = core_lib::string_utils::TokeniseString("1-2-3-4", "-", false);
    EXPECT_EQ(substrings, comp);
}

TEST(StringUtilsTest, Tokenise2)
{
    auto substrings = core_lib::string_utils::TokeniseString("1 - 2 - 3 - 4", " - ", true);
    decltype(substrings) comp = {"1", "", "", "2", "", "", "3", "", "", "4"};
    EXPECT_EQ(substrings, comp);

    comp       = {"1", "2", "3", "4"};
    substrings = core_lib::string_utils::TokeniseString("1 - 2 - 3 - 4", " - ", false);
    EXPECT_EQ(substrings, comp);
}

TEST(StringUtilsTest, ReplaceTokens)
{
    auto copyText = core_lib::string_utils::ReplaceTokens(
        "%1 %2 %3 %4", {{"\\%1", "I"}, {"\\%2", "AM"}, {"\\%3", "THE"}, {"\\%4", "BOMB"}});

    EXPECT_EQ(copyText, "I AM THE BOMB");
}

#endif // DISABLE_STRINGUTILS_TESTS
