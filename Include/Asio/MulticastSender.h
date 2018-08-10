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
 * \file MulticastSender.h
 * \brief File containing multicast sender class declaration.
 */

#ifndef MULTICASTSENDER
#define MULTICASTSENDER

#include "IoServiceThreadGroup.h"

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
 * \brief A general purpose multicast sender.
 *
 * This class forms the underpinnings of the MulticastTypedSender class.
 */
class CORE_LIBRARY_DLL_SHARED_API MulticastSender final
{
public:
    /*! \brief Default constructor - deleted. */
    MulticastSender() = delete;
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
    MulticastSender(boost_ioservice_t& ioService, const defs::connection_t& multicastConnection,
                    const std::string& interfaceAddress = "", bool enableLoopback = true,
                    eMulticastTTL ttl            = eMulticastTTL::sameSubnet,
                    size_t        sendBufferSize = DEFAULT_UDP_BUF_SIZE);
    /*!
     * \brief Initialisation constructor.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] interfaceAddress - Optional interface IP address for outgoing network messages.
     * \param[in] enableLoopback - Optional allow multicasts to loopback on same adapter.
     * \param[in] ttl - Optional time-to-live for multicast messages.
     * \param[in] sendBufferSize - Socket send option to control send buffer size.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommended.
     */
    MulticastSender(const defs::connection_t& multicastConnection,
                    const std::string& interfaceAddress = "", bool enableLoopback = true,
                    eMulticastTTL ttl            = eMulticastTTL::sameSubnet,
                    size_t        sendBufferSize = DEFAULT_UDP_BUF_SIZE);

    /*! \brief Copy constructor - deleted. */
    MulticastSender(const MulticastSender&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    MulticastSender& operator=(const MulticastSender&) = delete;
    /*! \brief Move constructor - deleted. */
    MulticastSender(MulticastSender&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    MulticastSender& operator=(MulticastSender&&) = delete;
    /*! \brief Default destructor. */
    ~MulticastSender() = default;
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
     * \brief Send a message buffer to the receiver.
     * \param[in] message - The message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessage(const defs::char_buffer_t& message);

private:
    /*!
     * \brief Create multicast socket.
     * \param[in] enableLoopback - Allow multicasts to loopback on same adapter.
     * \param[in] ttl - Time-to-live for multicast messages.
     * \param[in] sendBufferSize - Send buffer size.
     */
    void CreateMulticastSocket(bool enableLoopback, eMulticastTTL ttl, size_t sendBufferSize);
    /*!
     * \brief Synchronised send to method.
     * \param[in] message - Message buffer to send.
     * \return True if successfully sent, false otherwise.
     */
    bool SyncSendTo(const defs::char_buffer_t& message);

private:
    /*! \brief I/O service thread group. */
    std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service reference. */
    boost_ioservice_t& m_ioService;
    /*! \brief Multicast connection details. */
    defs::connection_t m_multicastConnection{};
    /*! \brief Interface IP address of outgoing network adaptor. */
    std::string m_interfaceAddress{};
    /*! \brief Multicast receiver end-point. */
    boost_udp_t::endpoint m_multicastEndpoint{};
    /*! \brief Multicast socket. */
    boost_udp_t::socket m_socket;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // MULTICASTSENDER
