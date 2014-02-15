/*!
 * \file GenericSorting.cpp
 * \brief File containing definitions generic sorting algorithms.
 */

#include "../GenericSorting.hpp"

namespace core_lib {
namespace sorting {

// ****************************************************************************
// 'class xBucketValueOutOfRangeError' definition
// ****************************************************************************
xBucketValueOutOfRangeError::xBucketValueOutOfRangeError()
    : exceptions::xCustomException("bucket value out of range")
{
}

xBucketValueOutOfRangeError::xBucketValueOutOfRangeError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xBucketValueOutOfRangeError::~xBucketValueOutOfRangeError()
{
}

}// namespace sorting
}// namespace core_lib
