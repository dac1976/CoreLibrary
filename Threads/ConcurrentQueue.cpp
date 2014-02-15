/*!
 * \file SyncEvent.cpp
 * \brief File containing definition of SyncEvent class.
 */

#include "../ConcurrentQueue.hpp"

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xQueuePopTimeoutError' definition
// ****************************************************************************
xQueuePopTimeoutError::xQueuePopTimeoutError()
    : exceptions::xCustomException("pop timeout")
{
}

xQueuePopTimeoutError::xQueuePopTimeoutError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xQueuePopTimeoutError::~xQueuePopTimeoutError()
{
}

} // namespace threads
} // namespace core_lib
