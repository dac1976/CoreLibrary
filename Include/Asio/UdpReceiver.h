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
 * \file UdpReceiver.h
 * \brief File containing UDP receiver class declaration.
 */

#ifndef UDPRECEIVER
#define UDPRECEIVER

#include <mutex>
#include "IoServiceThreadGroup.h"
#include "Threads/SyncEvent.h"

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
 * \brief A general purpose UDP receiver.
 *
 * This class is used as the underpinnings of the SimpleUdpReceiver class.
 */
class CORE_LIBRARY_DLL_SHARED_API UdpReceiver final
{
public:
    /*! \brief Default constructor - deleted. */
    UdpReceiver() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and
     * disptaching it accordingly.
     * \param[in] receiveOptions - Socket receive option to control the use of broadcasts/unicast.
     * \param[in] receiveBufferSize - Socket receive option to control receive buffer size.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    UdpReceiver(boost_ioservice_t& ioService, const uint16_t listenPort,
                const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                const defs::message_received_handler_t& messageReceivedHandler,
                const eUdpOption                        receiveOptions    = eUdpOption::broadcast,
                const size_t                            receiveBufferSize = DEFAULT_UDP_BUF_SIZE);
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object cpable of handling a received message and
     * disptaching it accordingly.
     * \param[in] receiveOptions - Socket receive option to control the use of broadcasts/unicast.
     * \param[in] receiveBufferSize - Socket receive option to control receive buffer size.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommened.
     */
    UdpReceiver(const uint16_t                          listenPort,
                const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                const defs::message_received_handler_t& messageReceivedHandler,
                const eUdpOption                        receiveOptions    = eUdpOption::broadcast,
                const size_t                            receiveBufferSize = DEFAULT_UDP_BUF_SIZE);
    /*! \brief Copy constructor - deleted. */
    UdpReceiver(const UdpReceiver&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    UdpReceiver& operator=(const UdpReceiver&) = delete;
    /*! \brief Destructor. */
    ~UdpReceiver();
    /*!
     * \brief Retrieve this receiver's listen port.
     * \return The listen port.
     */
    uint16_t ListenPort() const;

private:
    /*!
     * \brief Create UDP socket.
     * \param[in] receiveOptions - UDP receive options.
     * \param[in] receiveBufferSize - Receive buffer size.
     */
    void CreateUdpSocket(const eUdpOption receiveOptions, const size_t receiveBufferSize);
    /*! \brief Start asynchronous read. */
    void StartAsyncRead();
    /*!
     * \brief Read completion handler.
     * \param[in] error - Error code if one has happened.
     * \param[in] bytesReceived - Number of bytes received.
     */
    void ReadComplete(const boost_sys::error_code& error, const size_t bytesReceived);
    /*!
     * \brief Set closing state.
     * \param[in] closing - Closing socket flag.
     */
    void SetClosing(const bool closing);
    /*!
     * \brief Get closing state.
     * \return True if closing socket, false otherwise.
     */
    bool IsClosing() const;
    /*! \brief Process asynchronous close socket. */
    void ProcessCloseSocket();

private:
    /*! \brief Mutex to protect shutdown of receiver. */
    mutable std::mutex m_closingMutex;
    /*! \brief Event to synchronise shutdown of receiver. */
    threads::SyncEvent m_closedEvent;
    /*! \brief Flag to show were are closing socket. */
    bool m_closing;
    /*! \brief I/O service thread group. */
    std::unique_ptr<IoServiceThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service reference. */
    boost_ioservice_t& m_ioService;
    /*! \brief Receiver listen port. */
    const uint16_t m_listenPort{0};
    /*! \brief UDP socket. */
    boost_udp_t::socket m_socket;
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Callback to handle received message. */
    defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Socket receive buffer. */
    defs::char_buffer_t m_receiveBuffer;
    /*! \brief Message buffer. */
    defs::char_buffer_t m_messageBuffer;
    /*! \brief Sender end-point. */
    boost_udp_t::endpoint m_senderEndpoint;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // UDPRECEIVER
