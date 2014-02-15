/*!
 * \file MessageQueueThread.cpp
 * \brief File containing definition of MessageQueueThread class.
 */

#include "../MessageQueueThread.hpp"

namespace core_lib {
namespace threads {

// ****************************************************************************
// 'class xMsgHandlerError' definition
// ******************************B**********************************************
xMsgHandlerError::xMsgHandlerError()
    : exceptions::xCustomException("message handler error")
{
}

xMsgHandlerError::xMsgHandlerError(const std::string& message)
    : exceptions::xCustomException(message)
{
}

xMsgHandlerError::~xMsgHandlerError()
{
}

} // namespace threads
} // namespace core_lib
