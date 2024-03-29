// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
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
 * \file UdpTypedSender.h
 * \brief File containing UDP typed sender class declaration.
 */

#ifndef UDPTYPEDSENDER
#define UDPTYPEDSENDER

#include <mutex>
#include "UdpSender.h"
#include "MessageUtils.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{
/*!
 * \brief A generic UDP sender.
 *
 * The template argument defines a message builder object that
 * must have an interface compatible with that of the class
 * core_lib::asio::messages::MessageBuilder.
 *
 * This class forms the underpinnings of the SimpleUdpSender class.
 *
 * This is also the class to be used when the user wants to specify their own message builder type
 * and message header type.
 */
template <typename MsgBldr> class UdpTypedSender final
{
public:
    /*! \brief Default constructor - deleted. */
    UdpTypedSender() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] receiver - Connection object describing target receiver's address and port.
     * \param[in] messageBuilder - Message builder object reference.
     * \param[in] sendOption - Socket send option to control the use of broadcasts/unicast.
     * \param[in] sendBufferSize - Socket send option to control send buffer size.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     */
    UdpTypedSender(boost_iocontext_t& ioContext, const defs::connection_t& receiver,
                   const MsgBldr& messageBuilder, eUdpOption sendOption = eUdpOption::broadcast,
                   size_t sendBufferSize = DEFAULT_UDP_BUF_SIZE)
        : m_messageBuilder{messageBuilder}
        , m_udpSender{ioContext, receiver, sendOption, sendBufferSize}
    {
    }
    /*!
     * \brief Initialisation constructor.
     * \param[in] receiver - Connection object describing target receiver's address and port.
     * \param[in] messageBuilder - Message builder object reference.
     * \param[in] sendOption - Socket send option to control the use of broadcasts/unicast.
     * \param[in] sendBufferSize - Socket send option to control send buffer size.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommened.
     */
    UdpTypedSender(const defs::connection_t& receiver, const MsgBldr& messageBuilder,
                   eUdpOption sendOption     = eUdpOption::broadcast,
                   size_t     sendBufferSize = DEFAULT_UDP_BUF_SIZE)

        : m_messageBuilder{messageBuilder}
        , m_udpSender{receiver, sendOption, sendBufferSize}
    {
    }
    /*! \brief Copy constructor - deleted. */
    UdpTypedSender(const UdpTypedSender&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    UdpTypedSender& operator=(const UdpTypedSender&) = delete;
    /*! \brief Move constructor - deleted. */
    UdpTypedSender(UdpTypedSender&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    UdpTypedSender& operator=(UdpTypedSender&&) = delete;
    /*! \brief Default destructor. */
    ~UdpTypedSender() = default;
    /*!
     * \brief Retrieve receiver connection details.
     * \return - Connection object describing target receiver's address and port.
     */
    defs::connection_t ReceiverConnection() const
    {
        return m_udpSender.ReceiverConnection();
    }
    /*!
     * \brief Send a header-only message to the receiver.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the receiver should send
     * the response, the default value will mean the response address will point to this client
     * socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessage(int32_t                   messageId,
                     const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);
        return m_udpSender.SendMessage(m_messageBuilder.Build(messageId, responseAddress));
    }
    /*!
     * \brief Send a header plus message buffer to the receiver.
     * \param[in] message - The message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the receiver should send
     * the response, the default value will mean the response address will point to this client
     * socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessage(const defs::char_buffer_t& message, int32_t messageId,
                     const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);
        return m_udpSender.SendMessage(m_messageBuilder.Build(message, messageId, responseAddress));
    }
    /*!
     * \brief Send a full message to the server.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the receiver should send
     * the response, the default value will mean the response address will point to this client
     * socket.
     * \return Returns the success state of the send as a boolean.
     */
    template <typename T, class A = serialize::archives::out_port_bin_t>
    bool SendMessage(const T& message, int32_t messageId,
                     const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_sendMutex);
        return m_udpSender.SendMessage(
            m_messageBuilder.template Build<T, A>(message, messageId, responseAddress));
    }
    /*!
     * \brief Send a raw message buffer to the receiver.
     * \param[in] message - The message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessage(const defs::char_buffer_t& message)
    {
        return m_udpSender.SendMessage(message);
    }
    /*!
     * \brief Send a raw message buffer to the receiver.
     * \param[in] message - The message buffer pointer.
     * \param[in] message - The message buffer size in bytes.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessage(const char* message, size_t length)
    {
        return m_udpSender.SendMessage(message, length);
    }

private:
    /*! \brief Send message mutex. */
    mutable std::mutex m_sendMutex;
    /*! \brief Const reference to message builder object. */
    const MsgBldr& m_messageBuilder;
    /*! \brief Underlying UDP sender object. */
    UdpSender m_udpSender;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // UDPTYPEDSENDER
