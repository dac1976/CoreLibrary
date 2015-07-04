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
		: m_v1{9,12,5,4,19,13,17,15,6,1,3,20,14,18,16,7,8,2,11,10,30,27,21,28,26,22,23,29,24,25}
		, m_v2{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30}
		, m_v3(m_v2)
	{
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
