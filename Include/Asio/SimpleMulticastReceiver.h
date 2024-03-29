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
 * \file SimpleMulticastReceiver.h
 * \brief File containing simple multicast receiver class declaration.
 */

#ifndef SIMPLEMULTICASTRECEIVER
#define SIMPLEMULTICASTRECEIVER

#include "MulticastReceiver.h"
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
 * \brief A simplified multicast receiver, which uses the class MessageHeader as the message header
 * type.
 */
class CORE_LIBRARY_DLL_SHARED_API SimpleMulticastReceiver final
{
public:
    /*! \brief Default constructor - deleted. */
    SimpleMulticastReceiver() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] interfaceAddress - Optional interface IP address for incoming network messages.
     * \param[in] receiveBufferSize - Optional socket receive option to control receive buffer size.
     * \param[in] memPoolMsgCount - Number of messages in pool for received message handling,
     *                              defaults to 0, which implies no pool used.
     * \param[in] recvPoolMsgSize - Default size of message in received message pool. Only used
     *                              when memPoolMsgCount > 0.
     *
     * Typically use this constructor when managing a pool of threads using an instance of
     * core_lib::asio::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleMulticastReceiver(boost_iocontext_t&                        ioContext,
                            const defs::connection_t&                 multicastConnection,
                            const defs::default_message_dispatcher_t& messageDispatcher,
                            const std::string&                        interfaceAddress = "",
                            size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE,
                            size_t memPoolMsgCount   = 0,
                            size_t recvPoolMsgSize   = defs::RECV_POOL_DEFAULT_MSG_SIZE);
    /*!
     * \brief Initialisation constructor.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] interfaceAddress - Optional interface IP address for incoming network messages.
     * \param[in] receiveBufferSize - Optional socket receive option to control receive buffer size.
     * \param[in] memPoolMsgCount - Number of messages in pool for received message handling,
     *                              defaults to 0, which implies no pool used.
     * \param[in] recvPoolMsgSize - Default size of message in received message pool. Only used
     *                              when memPoolMsgCount > 0.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommended.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleMulticastReceiver(const defs::connection_t&                 multicastConnection,
                            const defs::default_message_dispatcher_t& messageDispatcher,
                            const std::string&                        interfaceAddress = "",
                            size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE,
                            size_t memPoolMsgCount   = 0,
                            size_t recvPoolMsgSize   = defs::RECV_POOL_DEFAULT_MSG_SIZE);
    /*! \brief Copy constructor - deleted. */
    SimpleMulticastReceiver(const SimpleMulticastReceiver&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    SimpleMulticastReceiver& operator=(SimpleMulticastReceiver&&) = delete;
    /*! \brief Move constructor - deleted. */
    SimpleMulticastReceiver(SimpleMulticastReceiver&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    SimpleMulticastReceiver& operator=(const SimpleMulticastReceiver&) = delete;
    /*! \brief Default destructor. */
    ~SimpleMulticastReceiver() = default;
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

private:
    /*! \brief Default message handler object of type core_lib::asio::messages::MessageHandler. */
    messages::MessageHandler m_messageHandler{};
    /*! \brief Our actual typed multicast receiver object. */
    MulticastReceiver m_mcastReceiver;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // SIMPLEMULTICASTRECEIVER
