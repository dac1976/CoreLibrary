/**
 * @file ExceptionsBase.hpp
 * @brief File containing definitions relating to custom exception handling.
 */

#include "../CustomException.hpp"

namespace core_lib {
namespace exceptions {

// ****************************************************************************
// 'class xSplitStringBadDelimError' definition
// ****************************************************************************
xCustomException::xCustomException()
    : boost::exception(), std::exception(),
      m_message("custom exception")
{
}

xCustomException::xCustomException(const std::string& message)
    : boost::exception(), std::exception(),
      m_message(message)
{
}

xCustomException::~xCustomException()
{
}

const char* xCustomException::what() const noexcept
{
    return m_message.c_str();
}

} // namespace exceptions
} // namespace core_lib
