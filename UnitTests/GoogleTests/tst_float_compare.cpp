#ifndef DISABLE_FLOAT_COMPARE_TESTS

#include <vector>
#include "Comparison/FloatCompare.hpp"

#include "gtest/gtest.h"
#include "gtest/gtest_cout.h"

TEST(FloatCompare, testCase_CompareZero_Success_Float)
{
    float a = 0.0f;

    ASSERT_TRUE(core_lib::comp::float_compare(a, 0.0f));
}

TEST(FloatCompare, testCase_CompareZero_Fail_Float)
{
    float a = 0.1f;

    ASSERT_FALSE(core_lib::comp::float_compare(a, 0.0f));
}

TEST(FloatCompare, testCase_CompareNegative_Success_Float)
{
    float a = -0.5f;

    ASSERT_TRUE(core_lib::comp::float_compare(a, -0.5f));
}

TEST(FloatCompare, testCase_CompareNegative_Fail_Float)
{
    float a = -0.5f;

    ASSERT_FALSE(core_lib::comp::float_compare(a, -0.7f));
}

TEST(FloatCompare, testCase_ComparePositive_Success_Float)
{
    float a = 0.5f;

    ASSERT_TRUE(core_lib::comp::float_compare(a, 0.5f));
}

TEST(FloatCompare, testCase_ComparePositive_Fail_Float)
{
    float a = 0.5f;

    ASSERT_FALSE(core_lib::comp::float_compare(a, 0.7f));
}

TEST(FloatCompare, testCase_CompareLarge_Success_Float)
{
    float a = 234854.0f / 3.0f;
    float b = a;

    ASSERT_TRUE(core_lib::comp::float_compare(a, b));
}

TEST(FloatCompare, testCase_CompareLarge_Fail_Float)
{
    float a = 234854.0f / 3.0f;
    float b = a + (9.0f / 100.0f);

    ASSERT_FALSE(core_lib::comp::float_compare(a, b));
}

TEST(FloatCompare, testCase_CompareZero_Success_Double)
{
    double a = 0.0;

    ASSERT_TRUE(core_lib::comp::float_compare(a, 0.0));
}

TEST(FloatCompare, testCase_CompareZero_Fail_Double)
{
    double a = 0.1;

    ASSERT_FALSE(core_lib::comp::float_compare(a, 0.0));
}

TEST(FloatCompare, testCase_CompareNegative_Success_Double)
{
    double a = -0.5;

    ASSERT_TRUE(core_lib::comp::float_compare(a, -0.5));
}

TEST(FloatCompare, testCase_CompareNegative_Fail_Double)
{
    double a = -0.5;

    ASSERT_FALSE(core_lib::comp::float_compare(a, -0.7));
}

TEST(FloatCompare, testCase_ComparePositive_Success_Double)
{
    double a = 0.5;

    ASSERT_TRUE(core_lib::comp::float_compare(a, 0.5));
}

TEST(FloatCompare, testCase_ComparePositive_Fail_Double)
{
    double a = 0.5;

    ASSERT_FALSE(core_lib::comp::float_compare(a, 0.7));
}

TEST(FloatCompare, testCase_CompareLarge_Success_Double)
{
    double a = 40989336.0 / 3.0;
    double b = a;

    ASSERT_TRUE(core_lib::comp::float_compare(a, b));
}

TEST(FloatCompare, testCase_CompareLarge_Fail_Double)
{
    double a = 40989336.0 / 3.0;
    double b = a + 0.000001;

    ASSERT_FALSE(core_lib::comp::float_compare(a, b));
}

TEST(FloatCompare, testCase_CompareZero_Success_LongDouble)
{
    long double a = 0.0l;

    ASSERT_TRUE(core_lib::comp::float_compare(a, 0.0l));
}

TEST(FloatCompare, testCase_CompareZero_Fail_LongDouble)
{
    long double a = 0.1l;

    ASSERT_FALSE(core_lib::comp::float_compare(a, 0.0l));
}

TEST(FloatCompare, testCase_CompareNegative_Success_LongDouble)
{
    long double a = -0.5l;

    ASSERT_TRUE(core_lib::comp::float_compare(a, -0.5l));
}

TEST(FloatCompare, testCase_CompareNegative_Fail_LongDouble)
{
    long double a = -0.5l;

    ASSERT_FALSE(core_lib::comp::float_compare(a, -0.7l));
}

TEST(FloatCompare, testCase_ComparePositive_Success_LongDouble)
{
    long double a = 0.5l;

    ASSERT_TRUE(core_lib::comp::float_compare(a, 0.5l));
}

TEST(FloatCompare, testCase_ComparePositive_Fail_LongDouble)
{
    long double a = 0.5l;

    ASSERT_FALSE(core_lib::comp::float_compare(a, 0.7l));
}

TEST(FloatCompare, testCase_CompareLarge_Success_LongDouble)
{
    long double a = 7809870253.0l / 3.0l;
    long double b = a;

    ASSERT_TRUE(core_lib::comp::float_compare(a, b));
}

TEST(FloatCompare, testCase_CompareLarge_Fail_LongDouble)
{
    long double a = 7809870253.0l / 3.0l;
    long double b = a + 0.000009l;

    ASSERT_FALSE(core_lib::comp::float_compare(a, b));
}

TEST(FloatCompare, testCase_CompareRange_1_Double)
{
    std::vector<double> values = {
        0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0, 500.0, 1000.0, 2000.0};

    double comparand  = 0.1;
    size_t count      = 0;
    double multiplier = 1.0;

    for (auto value : values)
    {
        EXPECT_TRUE(core_lib::comp::float_compare(value, comparand));

        ++count;

        switch (count)
        {
        case 1:
            comparand = 0.2 * multiplier;
            break;
        case 2:
            comparand = 0.5 * multiplier;
            multiplier *= 10.0;
            break;
        case 3:
            comparand = 0.1 * multiplier;
            count     = 0;
            break;
        }
    }
}

TEST(FloatCompare, testCase_CompareRange_2_Double)
{
    std::vector<double> values = {1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.,  10., 11., 12., 13.,
                                  14., 15., 16., 17., 18., 19., 20., 21., 22., 23., 24., 25.};

    double comparand = 1.0;
    double offset    = 1.0;

    for (auto value : values)
    {
        EXPECT_TRUE(core_lib::comp::float_compare(value, comparand));
        comparand += offset;
    }
}

#endif // DISABLE_FLOAT_COMPARE_TESTS
