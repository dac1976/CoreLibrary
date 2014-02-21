// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
// Contact <duncan.crutchley+corelibrary@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License and GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// and GNU Lesser General Public License along with this program. If
// not, see <http://www.gnu.org/licenses/>.


/*!
 * \file GenericSorting.hpp
 * \brief File containing declarations generic sorting algorithms.
 */

#ifndef GenericSorting_HPP
#define GenericSorting_HPP

#include <functional>
#include <algorithm>
#include <utility>
#include <vector>
#include <list>
#include "CustomException.hpp"

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief sorting core_lib namespace. */
namespace sorting {

/*!
 * \brief Templated static class to perform a bubble sort.
 *
 * Template args are T, the type of value to be sorted, and
 * Pred, the comparison functor to compare items of type T.
 * The default comparison predicate is std::less<T>.
 */
template <typename T,
          typename Pred = std::less<T>
          >
class Bubble
{
public:
    /*! \brief Public typedef for predicate. */
    typedef Pred item_compare;

    /*!
     * \brief In-place static sort function.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last otem to be sorted).
     *
     * This function sorts a collections (vector, list etc)
     * of items of type T between a begin and end iterator.
     */
    template <typename TIterator>
    static void Sort(TIterator begin, TIterator end)
    {
        item_compare compare;

        bool changesMade;

        do
        {
            changesMade = false;

            for (TIterator item = begin; item != end - 1; ++item)
            {
                if (compare(*(item + 1), *item))
                {
                    std::swap(*item, *(item + 1));
                    changesMade = true;
                }
            }
        }
        while(changesMade);
    }
};

/*!
 * \brief Templated static class to perform a selection sort.
 *
 * Template args are T, the type of value to be sorted, and
 * Pred, the comparison functor to compare items of type T.
 * The default comparison predicate is std::less<T>.
 */
template <typename T,
          typename Pred = std::less<T>
          >
class Selection
{
public:
    /*! \brief Public typedef for predicate. */
    typedef Pred item_compare;

    /*!
     * \brief In-place static sort function.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last otem to be sorted).
     *
     * This function sorts a collections (vector, list etc)
     * of items of type T between a begin and end iterator.
     */
    template <typename TIterator>
    static void Sort(TIterator begin, TIterator end)
    {
        item_compare compare;
        TIterator item = begin;

        while (item != end)
        {
            TIterator minValue(std::min_element(item, end, compare));

            if (minValue != item)
            {
                std::swap(*item, *minValue);
            }

            ++item;
        }
    }
};

/*!
 * \brief Templated static class to perform a insertion sort.
 *
 * Template args are T, the type of value to be sorted, and
 * Pred, the comparison functor to compare items of type T.
 * The default comparison predicate is std::less<T>.
 */
template <typename T,
          typename Pred = std::less<T>
          >
class Insertion
{
public:
    /*! \brief Public typedef for predicate. */
    typedef Pred item_compare;

    /*!
     * \brief In-place static sort function.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last otem to be sorted).
     *
     * This function sorts a collections (vector, list etc)
     * of items of type T between a begin and end iterator.
     */
    template <typename TIterator>
    static void Sort(TIterator begin, TIterator end)
    {
        item_compare compare;

        // we'll iterate over our items and for each item we'll move it to
        // the correct position in the collection inserting it and shuffling
        // the other items in the correct direction.
        for (TIterator item = begin; item != end; ++item)
        {
            T valueToInsert = *item;
            TIterator holePos = item;

            while ((holePos != begin) && compare(valueToInsert, *(holePos - 1)))
            {
                *holePos = *(holePos - 1);
                --holePos;
            }

            *holePos = valueToInsert;
        }
    }
};

/*!
 * \brief Templated static class to perform a quick sort.
 *
 * Template args are T, the type of value to be sorted, and
 * Pred, the comparison functor to compare items of type T.
 * The default comparison predicate is std::less<T>. This is a
 * recursive implementation of quick sort.
 */
template <typename T,
          typename Pred = std::less<T>
          >
class Quick
{
public:
    /*! \brief Public typedef for predicate. */
    typedef Pred item_compare;

    /*!
     * \brief In-place static sort function.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last otem to be sorted).
     *
     * This function sorts a collections (vector, list etc)
     * of items of type T between a begin and end iterator.
     */
    template <typename TIterator>
    static void Sort(TIterator begin, TIterator end)
    {
        if (begin >= end) return;

        // choose pivot
        TIterator pivot = begin + ((end - begin) / 2);

        // partition
        pivot = Partition(begin, end, pivot);

        // recurse on left half [begin, pivot)
        Sort(begin, pivot);

        // recurse on right half (pivot, end)
        Sort(pivot + 1, end);
    }

private:
    /*!
     * \brief Static partitioning function.
     * \param [IN] The begining of the sub-collection to sort.
     * \param [IN] The end of the sub-collection to sort (in STL fashion this is the item just after the last otem to be sorted).
     * \param [IN] Pivot point within the collection about which to partition.
     * \return The new pivot iterator.
     *
     * This function partitions the sub-collection as part of
     * the quick sort algorithm.
     */
    template <typename TIterator>
    static TIterator Partition(TIterator begin
                              , TIterator end
                              , TIterator pivot)
    {
        item_compare compare;

        // swap pivot value with right-most item
        std::swap(*pivot, *(end - 1));

        // update current pivot again
        pivot = end - 1;

        // initialise new pivot
        TIterator newPivot = begin;

        // perform partitioning moving items < pivot
        // into left half
        TIterator item = begin;

        while(item != end - 1)
        {
            if (compare(*item, *pivot))
            {
                std::swap(*item, *newPivot++);
            }

            ++item;
        }

        // move pivot value to final place
        std::swap(*newPivot, *(end - 1));

        // return pivot
        return newPivot;
    }
};

/*!
 * \brief Bucket value out of range.
 *
 * This exception class is intended to be thrown by the bucket sort
 * class if a value is found in the collection to be sorted that does
 * not belong to any defined bucket.
 */
class xBucketValueOutOfRangeError : public exceptions::xCustomException
{
public:
    /*! \brief Default constructor. */
    xBucketValueOutOfRangeError();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xBucketValueOutOfRangeError(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xBucketValueOutOfRangeError();
};

/*!
 * \brief Templated static class to perform a bucket sort.
 *
 * Template args are T, the type of value to be sorted, and
 * Pred, the comparison functor to compare items of type T.
 * The default comparison predicate is std::less<T>.
 */
template <typename T,
          typename Pred = std::less<T>
          >
class Bucket
{
public:
    /*! \brief Public typedef for predicate. */
    typedef Pred item_compare;
    /*! \brief Public typedef for bucket definitions. */
    typedef std::vector< std::pair<T, T> > bucket_definitions;
    /*! \brief Public typedef for bucket values. */
    typedef std::vector< std::list<T> > bucket_values;

    /*!
     * \brief In-place static sort function.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last item to be sorted).
     * \param [IN] The definition of the ranges of the different buckets, e.g. the min and max values per bucket.
     * \param [OUT] The collection of buckets containing the values for each bucket range.
     *
     * This function sorts a collection (vector, list etc)
     * of items of type T between a begin and end iterator.
     * This version can be used when the user is interested in
     * gaining access to the intermediate collection of buckets
     * containing the partitioned values.
     */
    template <typename TIterator>
    static void Sort(TIterator begin, TIterator end
                     , const bucket_definitions& bucketDefinitions
                     , bucket_values& bucketValues)
    {
        if (begin >= end) return;

        PutIntoBuckets(begin, end, bucketDefinitions, bucketValues);
        WriteBackBuckets(begin, bucketValues);
        SortResult(begin, end);
    }

    /*!
     * \brief Static sort function.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last item to be sorted).
     * \param [IN] The definition of the ranges of the different buckets, e.g. the min and max values per bucket.
     *
     * This function sorts a collections (vector, list etc)
     * of items of type T between a begin and end iterator.
     * This version can be used when the user is only interested
     * in the final result.
     */
    template <typename TIterator>
    static void Sort(TIterator begin, TIterator end
                     , const bucket_definitions& bucketDefinitions)
    {
        bucket_values bucketValues;
        Sort(begin, end, bucketDefinitions, bucketValues);
    }

private:

    /*!
     * \brief Static function to check bucket values collection size.
     * \param [IN] The definition of the ranges of the different buckets, e.g. the min and max values per bucket.
     * \param [IN] The bucket values collection.
     *
     * This function compares the size of the bucket values collection to
     * that of the bucket definitions collection and resizes the bucket value
     * collection if required.
     */
    static void CheckBucketsSize(const bucket_definitions& bucketDefinitions
                          , bucket_values& bucketValues)
    {
        if (bucketValues.size() != bucketDefinitions.size())
        {
            bucketValues.resize(bucketDefinitions.size());
        }
    }

    /*!
     * \brief Static compute the correct bucket index for a given item.
     * \param [IN] The item to put in to a bucket.
     * \param [IN] The definition of the ranges of the different buckets, e.g. the min and max values per bucket.
     *
     * This function iterates over the buckets until it finds the
     * correct one to hold the given item.
     */
    static size_t GetBucketIndex(const T& value, const bucket_definitions& bucketDefinitions)
    {
        size_t index = 0;

        for (const auto& bucket : bucketDefinitions)
        {
            if ((value >= bucket.first) && (value <= bucket.second))
            {
                break;
            }

            ++index;
        }

        if (index == bucketDefinitions.size())
        {
            BOOST_THROW_EXCEPTION(xBucketValueOutOfRangeError());
        }

        return index;
    }

    /*!
     * \brief Static to put the items in to the correct buckets.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last otem to be sorted).
     * \param [IN] The definition of the ranges of the different buckets, e.g. the min and max values per bucket.
     * \param [OUT] The collection of buckets containing the values for each bucket range.
     *
     * This function builds up the buckets with the correct items.
     */
    template <typename TIterator>
    static void PutIntoBuckets(TIterator begin, TIterator end
                         , const bucket_definitions& bucketDefinitions
                         , bucket_values& bucketValues)
    {
        CheckBucketsSize(bucketDefinitions, bucketValues);

        for (TIterator item = begin; item != end; ++item)
        {
            size_t bucketIndex = GetBucketIndex(*item, bucketDefinitions);
            bucketValues[bucketIndex].push_back(*item);
        }
    }

    /*!
     * \brief Static to move the items from the buckets back into the original collection.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The collection of buckets containing the values for each bucket range.
     *
     * This function moves each bucket item, in bucket order, back into the
     * original collection, overwriting the original collection. After calling
     * this method the collection still needs a final sort performed.
     */
    template <typename TIterator>
    static void WriteBackBuckets(TIterator begin, const bucket_values& bucketValues)
    {
        TIterator pos = begin;

        for (auto& bucket : bucketValues)
        {
            std::move(bucket.begin(), bucket.end(), pos);
            pos += bucket.size();
        }
    }

    /*!
     * \brief Static to perform a final sort on the collection.
     * \param [IN] The begining of the collection to sort.
     * \param [IN] The end of the collection to sort (in STL fashion this is the item just after the last otem to be sorted).
     *
     * This function performs an insertion sort on the modified
     * bucket ordered collection to obtain the sorted result.
     */
    template <typename TIterator>
    static void SortResult(TIterator begin, TIterator end)
    {
        Insertion<T, item_compare>::Sort(begin, end);
    }
};

}// namespace sorting
}// namespace core_lib

#endif // GenericSorting_HPP
