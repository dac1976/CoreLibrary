#ifndef DISABLE_STRINGUTILS_TESTS

#include "StringUtils/StringUtils.h"
#include "boost/predef.h"

#include "gtest/gtest.h"

TEST(StringUtilsTest, Case1_PackStdString)
{
	const std::string comparator = "I am a test string.";
	std::string testString = "I am a test string. I am a test string.";
	testString[19] = '\0';
	EXPECT_NE(comparator.size(), testString.size());

	core_lib::string_utils::PackStdString(testString);
	EXPECT_EQ(comparator.size(), testString.size());
	EXPECT_EQ(comparator, testString);
}

TEST(StringUtilsTest, Case2_xSplitStringBadDelim_1)
{
	try
	{
		BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringBadDelim());
	}
	catch(core_lib::string_utils::xSplitStringBadDelim& e)
	{
		EXPECT_STREQ(e.what(), "split string bad delimiter");
		std::string info = boost::diagnostic_information(e);
		EXPECT_TRUE(info != "");
	}
}

TEST(StringUtilsTest, Case3_xSplitStringBadDelim_2)
{
	try
	{
		BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringBadDelim("user defined message"));
	}
	catch(core_lib::string_utils::xSplitStringBadDelim& e)
	{
		EXPECT_STREQ(e.what(), "user defined message");
		std::string info = boost::diagnostic_information(e);
		EXPECT_TRUE(info != "");
	}
}

TEST(StringUtilsTest, Case4_xSplitStringTooManySubstrings_1)
{
	try
	{
		BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringTooManySubstrings());
	}
	catch(core_lib::string_utils::xSplitStringTooManySubstrings& e)
	{
		EXPECT_STREQ(e.what(), "too many substrings");
		std::string info = boost::diagnostic_information(e);
		EXPECT_TRUE(info != "");
	}
}

TEST(StringUtilsTest, Case5_xSplitStringTooManySubstrings_2)
{
	try
	{
		BOOST_THROW_EXCEPTION(core_lib::string_utils::xSplitStringTooManySubstrings("user defined message"));
	}
	catch(core_lib::string_utils::xSplitStringTooManySubstrings& e)
	{
		EXPECT_STREQ(e.what(), "user defined message");
		std::string info = boost::diagnostic_information(e);
		EXPECT_TRUE(info != "");
	}
}

TEST(StringUtilsTest, Case6_SplitString_1)
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

	EXPECT_TRUE(exceptionThrown);
}

TEST(StringUtilsTest, Case7_SplitString_2)
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

	EXPECT_TRUE(exceptionThrown);
}

TEST(StringUtilsTest, Case8_SplitString_3)
{
	bool exceptionThrown;

	try
	{
		std::string toSplit = "key = value";
		std::string subStr1,  subStr2;
		core_lib::string_utils::SplitString(subStr1, subStr2, toSplit, "=",
											core_lib::string_utils::eSplitStringResult::notTrimmed);
		exceptionThrown = false;
		EXPECT_STREQ(subStr1.c_str(), "key ");
		EXPECT_STREQ(subStr2.c_str(), " value");
	}
	catch(...)
	{
		exceptionThrown = true;
	}

	EXPECT_TRUE(!exceptionThrown);
}

TEST(StringUtilsTest, Case9_SplitString_4)
{
	bool exceptionThrown;

	try
	{
		std::string toSplit = "key = value";
		std::string subStr1,  subStr2;
		core_lib::string_utils::SplitString(subStr1, subStr2, toSplit, "=",
											core_lib::string_utils::eSplitStringResult::trimmed);
		exceptionThrown = false;
		EXPECT_STREQ(subStr1.c_str(), "key");
		EXPECT_STREQ(subStr2.c_str(), "value");
	}
	catch(...)
	{
		exceptionThrown = true;
	}

	EXPECT_TRUE(!exceptionThrown);
}

TEST(StringUtilsTest, Case10_FormatFloatString_1)
{
	float value = 1234.123f;
	std::string result = core_lib::string_utils::FormatFloatString(value, 15, core_lib::string_utils::eFloatStringFormat::normal);
	EXPECT_TRUE((result.size() <= 20U) && (result.size() >= 8U));
	EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, Case11_FormatFloatString_2)
{
	float value = 1234.123f;
	std::string result = core_lib::string_utils::FormatFloatString(value, 15, core_lib::string_utils::eFloatStringFormat::fixed);
	EXPECT_TRUE(result.size() == 20U);
	EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, Case12_FormatFloatString_3)
{
	float value = 1234.123f;
	std::string result = core_lib::string_utils::FormatFloatString(value, 15, core_lib::string_utils::eFloatStringFormat::scientific);
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

TEST(StringUtilsTest, Case13_FormatFloatString_4)
{
	double value = 1234.123;
	std::string result = core_lib::string_utils::FormatFloatString(value, 15, core_lib::string_utils::eFloatStringFormat::normal);
	EXPECT_TRUE((result.size() <= 20U) && (result.size() >= 8U));
	EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, Case14_FormatFloatString_5)
{
	double value = 1234.123;
	std::string result = core_lib::string_utils::FormatFloatString(value, 15, core_lib::string_utils::eFloatStringFormat::fixed);
	EXPECT_TRUE(result.size() == 20U);
	EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

TEST(StringUtilsTest, Case15_FormatFloatString_6)
{
	double value = 1234.123;
	std::string result = core_lib::string_utils::FormatFloatString(value, 15, core_lib::string_utils::eFloatStringFormat::scientific);
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

TEST(StringUtilsTest, Case16_FormatFloatString_rvalue)
{
	std::string result = core_lib::string_utils::FormatFloatString(1234.123, 15, core_lib::string_utils::eFloatStringFormat::normal);
	EXPECT_TRUE((result.size() <= 20U) && (result.size() >= 8U));
	EXPECT_STREQ(result.substr(0, 8).c_str(), "1234.123");
}

#endif // DISABLE_STRINGUTILS_TESTS
