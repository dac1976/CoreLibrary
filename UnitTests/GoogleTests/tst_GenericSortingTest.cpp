#ifndef DISABLE_SORTING_TESTS

#include <vector>
#include "Sorting/GenericSorting.h"

#include "gtest/gtest.h"

namespace
{
// Fixture class for handling unit tests' shared data and setup.
class GenericSortingTest : public ::testing::Test
{
protected:
	std::vector<int> m_v1;
	std::vector<int> m_v2;
	std::vector<int> m_v3;

protected:
    GenericSortingTest()
    {
        // Required to initialise vectors in a way that works with msvc++
        // and gcc/clang (initializer lists in constructors or where member
        // is declared do not work in msvc 2013 and earlier).
        m_v1.emplace_back(9);
        m_v1.emplace_back(12);
        m_v1.emplace_back(5);
        m_v1.emplace_back(4);
        m_v1.emplace_back(19);
        m_v1.emplace_back(13);
        m_v1.emplace_back(17);
        m_v1.emplace_back(15);
        m_v1.emplace_back(6);
        m_v1.emplace_back(1);
        m_v1.emplace_back(3);
        m_v1.emplace_back(20);
        m_v1.emplace_back(14);
        m_v1.emplace_back(18);
        m_v1.emplace_back(16);
        m_v1.emplace_back(7);
        m_v1.emplace_back(8);
        m_v1.emplace_back(2);
        m_v1.emplace_back(11);
        m_v1.emplace_back(10);
        m_v1.emplace_back(30);
        m_v1.emplace_back(27);
        m_v1.emplace_back(21);
        m_v1.emplace_back(28);
        m_v1.emplace_back(26);
        m_v1.emplace_back(22);
        m_v1.emplace_back(23);
        m_v1.emplace_back(29);
        m_v1.emplace_back(24);
        m_v1.emplace_back(25);

        for (unsigned int n = 1U; n <= 30U; ++n)
        {
            m_v2.emplace_back(n);
        }

        m_v3 = m_v2;

		std::reverse(m_v3.begin(), m_v3.end());
	}

	static bool CompareVectors(const std::vector<int>& v1
							   , const std::vector<int>& v2)
	{
		bool equals = true;

		for(auto i1 = v1.begin(), i2 = v2.begin()
			; (i1 != v1.end()) && (i2 != v2.end()) && equals
			; ++i1, ++i2)
		{
			equals = *i1 == *i2;
		}

		return equals;
	}
};

} // End of unnamed namespace.

// Unit test cases.
TEST_F(GenericSortingTest, Case1_BubbleSort_Ascending)
{
	core_lib::sorting::Bubble<int>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v2));
}

TEST_F(GenericSortingTest, Case2_BubbleSort_Decending)
{
	core_lib::sorting::Bubble<int, std::greater<int>>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v3));
}

TEST_F(GenericSortingTest, Case3_SelectionSort_Ascending)
{
	core_lib::sorting::Selection<int>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v2));
}

TEST_F(GenericSortingTest, Case4_SelectionSort_Decending)
{
	core_lib::sorting::Selection<int, std::greater<int>>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v3));
}

TEST_F(GenericSortingTest, Case5_InsertionSort_Ascending)
{
	core_lib::sorting::Insertion<int>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v2));
}

TEST_F(GenericSortingTest, Case6_InsertionSort_Decending)
{
	core_lib::sorting::Insertion<int, std::greater<int>>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v3));
}

TEST_F(GenericSortingTest, Case7_QuickSort_Ascending)
{
	core_lib::sorting::Quick<int>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v2));
}

TEST_F(GenericSortingTest, Case8_QuickSort_Decending)
{
	core_lib::sorting::Quick<int, std::greater<int>>::Sort(m_v1.begin(), m_v1.end());
	EXPECT_TRUE(CompareVectors(m_v1, m_v3));
}

TEST_F(GenericSortingTest, Case9_BucketSort_Ascending)
{
	core_lib::sorting::Bucket<int>::bucket_definitions
			bucketDefs{std::make_pair<int, int>(0, 4), std::make_pair<int, int>(5, 9)
					  , std::make_pair<int, int>(10, 14), std::make_pair<int, int>(15, 19)
					  , std::make_pair<int, int>(20, 24), std::make_pair<int, int>(25, 30)};
	core_lib::sorting::Bucket<int>::Sort(m_v1.begin(), m_v1.end(), bucketDefs);
	EXPECT_TRUE(CompareVectors(m_v1, m_v2));
}

TEST_F(GenericSortingTest, Case10_BucketSort_Decending)
{
	core_lib::sorting::Bucket<int>::bucket_definitions
			bucketDefs{std::make_pair<int, int>(0, 4), std::make_pair<int, int>(5, 9)
					  , std::make_pair<int, int>(10, 14), std::make_pair<int, int>(15, 19)
					  , std::make_pair<int, int>(20, 24), std::make_pair<int, int>(25, 30)};
	core_lib::sorting::Bucket<int, std::greater<int>>::Sort(m_v1.begin(), m_v1.end(), bucketDefs);
	EXPECT_TRUE(CompareVectors(m_v1, m_v3));
}

#endif // DISABLE_SORTING_TESTS