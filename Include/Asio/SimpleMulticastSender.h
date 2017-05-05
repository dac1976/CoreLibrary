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
 * \file SimpleMulticastSender.h
 * \brief File containing simple multicast sender class declaration.
 */

#ifndef SIMPLEMULTICASTSENDER
#define SIMPLEMULTICASTSENDER

#include "MulticastTypedSender.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The udp namespace. */
namespace udp
{

/*! \brief A simplified Multicast sender, which uses the class MessageHeader as the message header
 * type. */
class CORE_LIBRARY_DLL_SHARED_API SimpleMulticastSender final
{
public:
    /*! \brief Default constructor - deleted. */
    SimpleMulticastSender() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] interfaceAddress - Optional interface IP address for outgoing network messages.
     * \param[in] enableLoopback - Optional allow multicasts to loopback on same adapter.
     * \param[in] ttl - Optional time-to-live for multicast messages.
     * \param[in] sendBufferSize - Socket send option to control send buffer size.
     *
     * Typically use this constructor when managing a pool of threads using an instance of
     * core_lib::asio::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    SimpleMulticastSender(boost_ioservice_t&        ioService,
                          const defs::connection_t& multicastConnection,
                          const std::string&        interfaceAddress = "",
                          const bool                enableLoopback   = true,
                          const eMulticastTTL       ttl              = eMulticastTTL::sameSubnet,
                          const size_t              sendBufferSize   = DEFAULT_UDP_BUF_SIZE);
    /*!
     * \brief Initialisation constructor.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] interfaceAddress - Optional interface IP address for outgoing network messages.
     * \param[in] enableLoopback - Optional allow multicasts to loopback on same adapter.
     * \param[in] ttl - Optional time-to-live for multicast messages.
     * \param[in] sendBufferSize - Socket send option to control send buffer size.
     *
     * Typically use this constructor when managing a pool of threads using an instance of
     * core_lib::asio::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    SimpleMulticastSender(const defs::connection_t& multicastConnection,
                          const std::string&        interfaceAddress = "",
                          const bool                enableLoopback   = true,
                          const eMulticastTTL       ttl              = eMulticastTTL::sameSubnet,
                          const size_t              sendBufferSize   = DEFAULT_UDP_BUF_SIZE);
    /*! \brief Copy constructor - deleted. */
    SimpleMulticastSender(const SimpleMulticastSender&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    SimpleMulticastSender& operator=(const SimpleMulticastSender&) = delete;
    /*! \brief Default destructor. */
    ~SimpleMulticastSender() = default;
    /*!
     * \brief Retrieve multicast connection details.
     * \return - Connection object describing target multicast group address and port.
     */
    defs::connection_t MulticastConnection() const;
    /*!
     * \brief Retrieve interface IP address.
     * \return - Interface IP address.
     */
    std::string InterfaceAddress() const;
    /*!
     * \brief Send a header-only message to the receiver.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the receiver should send
     * the response, the default value will mean the response address will point to this client
     * socket.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessage(const uint32_t            messageId,
                     const defs::connection_t& responseAddress = defs::NULL_CONNECTION);
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
    bool SendMessage(const T& message, const uint32_t messageId,
                     const defs::connection_t& responseAddress = defs::NULL_CONNECTION)
    {
        return m_multicastTypedSender.SendMessage<T, A>(message, messageId, responseAddress);
    }

private:
    /*! \brief Default message builder object of type core_lib::asio::messages::MessageBuilder. */
    messages::MessageBuilder m_messageBuilder;
    /*! \brief Our actual typed Multicast sender object. */
    MulticastTypedSender<messages::MessageBuilder> m_multicastTypedSender;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // SIMPLEMULTICASTSENDER
