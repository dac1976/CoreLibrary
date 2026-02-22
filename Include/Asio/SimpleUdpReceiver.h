// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <15799155+dac1976@users.noreply.github.com>
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
 * \file SimpleUdpReceiver.h
 * \brief File containing simple UDP receiver class declaration.
 */

#ifndef SIMPLEUDPRECEIVER
#define SIMPLEUDPRECEIVER

#include "UdpReceiver.h"
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

/*! \brief A simplified UDP receiver, which uses the class MessageHeader as the message header type.
 */
class CORE_LIBRARY_DLL_SHARED_API SimpleUdpReceiver final
{
public:
    /*! \brief Default constructor - deleted. */
    SimpleUdpReceiver() = delete;
    /*! \brief Deleted copy constructor. */
    SimpleUdpReceiver(const SimpleUdpReceiver&) = delete;
    /*! \brief Deleted copy assignment operator. */
    SimpleUdpReceiver& operator=(const SimpleUdpReceiver&) = delete;
    /*! \brief Deleted move constructor. */
    SimpleUdpReceiver(SimpleUdpReceiver&&) = delete;
    /*! \brief Deleted move assignment operator. */
    SimpleUdpReceiver& operator=(SimpleUdpReceiver&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] receiveOptions - Socket receive option to control the use of broadcasts/unicast.
     * \param[in] receiveBufferSize - Socket receive option to control receive buffer size.
     * \param[in] memPoolMsgCount - Number of messages in pool for received message handling,
     *                              defaults to 0, which implies no pool used.
     * \param[in] recvPoolMsgSize - Default size of message in received message pool. Only used
     *                              when memPoolMsgCount > 0.
     * \param[in] listenAddress - Specific NIC IP address to bind the socket to,
     * empty string will bind to all NICs
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to use process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleUdpReceiver(asio_compat::io_service_t& ioService, 
	              uint16_t listenPort,
				  const defs::default_message_dispatcher_t& messageDispatcher,
				  eUdpOption receiveOptions = eUdpOption::broadcast,
				  size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE, 
				  size_t memPoolMsgCount = 0,
				  size_t recvPoolMsgSize = defs::RECV_POOL_DEFAULT_MSG_SIZE,
				  std::string const& listenAddress   = "");
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] messageDispatcher - Callback to use to dispatch received messages.
     * \param[in] receiveOptions - Socket receive option to control the use of broadcasts/unicast.
     * \param[in] receiveBufferSize - Socket receive option to control receive buffer size.
     * \param[in] memPoolMsgCount - Number of messages in pool for received message handling,
     *                              defaults to 0, which implies no pool used.
     * \param[in] recvPoolMsgSize - Default size of message in received message pool. Only used
     *                              when memPoolMsgCount > 0.
     * \param[in] listenAddress - Specific NIC IP address to bind the socket to,
     * empty string will bind to all NICs
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommended.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to use process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleUdpReceiver(uint16_t listenPort,
				  const defs::default_message_dispatcher_t& messageDispatcher,
				  eUdpOption receiveOptions = eUdpOption::broadcast,
				  size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE, 
				  size_t memPoolMsgCount = 0,
				  size_t recvPoolMsgSize = defs::RECV_POOL_DEFAULT_MSG_SIZE,
				  std::string const& listenAddress = "");
    /*! \brief Default destructor. */
    ~SimpleUdpReceiver() = default;
    /*!
     * \brief Retrieve this receiver's listen port.
     * \return The listen port.
     */
    uint16_t ListenPort() const;
    /*!
     * \brief Retrieve this receiver's listen address.
     * \return The listen address.
     */
    std::string ListenAddress() const;

private:
    /*! \brief Default message handler object of type asio::messages::MessageHandler. */
    messages::MessageHandler m_messageHandler;
    /*! \brief Our actual typed UDP receiver object. */
    UdpReceiver m_udpReceiver;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // SIMPLEUDPRECEIVER
