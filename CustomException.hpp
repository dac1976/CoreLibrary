/*!
 * \file ExceptionsBase.hpp
 * \brief File containing declarations relating to custom exception handling.
 */

#include <string>
#include "boost/exception/all.hpp"

#ifndef CUSTOMEXCEPTION_HPP
#define CUSTOMEXCEPTION_HPP

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The exceptions namespace. */
namespace exceptions {

/*!
 * \brief Exception base class from which to define custom exceptions.
 *
 * This class inherits virtually from std::exception and most importantly
 * boost::exception. Inheriting from the latter ensures that we can get
 * extended information when exceptions are thrown using the boost macro
 * BOOST_THROW_EXCEPTION. The extended information contains line, function
 * and file information of where the exception was thrown, as well as
 * providing access to the what() message.
 *
 * To access extended info do the following:
 *
 * try
 * {
 *     // something throws a n exception derived from
 *     // core_lib::exceptions::xCustomException
 * }
 * catch(core_lib::exceptions::xCustomException& e)
 * {
 *     std::cerr << boost::diagnostic_information(e);
 * }
 * catch(...)
 * {
 *     std::cerr << "Unhandled exception!" << std::endl
 *               << boost::current_exception_diagnostic_information();
 * }
 */
class xCustomException : public virtual boost::exception
        , public virtual std::exception
{
public:
    /*! \brief Default constructor. */
    xCustomException();
    /*!
     * \brief Initializing constructor.
     * \param [IN] A user specifed message string.
     */
    explicit xCustomException(const std::string& message);
    /*! \brief Virtual destructor. */
    virtual ~xCustomException();
    /*!
     * \brief Function to get the exception message.
     * \return The exception message.
     */
    virtual const char* what() const noexcept final;

protected:
    /*! \brief The exception message. */
    std::string m_message;
};


} // namespace exceptions
} // namespace core_lib

#endif // CUSTOMEXCEPTION_HPP
