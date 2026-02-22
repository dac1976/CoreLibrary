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
#include "IoContextThreadGroup.h"
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
    /*! \brief Deleted copy constructor. */
    UdpReceiver(const UdpReceiver&) = delete;
    /*! \brief Deleted copy assignment operator. */
    UdpReceiver& operator=(const UdpReceiver&) = delete;
    /*! \brief Deleted move constructor. */
    UdpReceiver(UdpReceiver&&) = delete;
    /*! \brief Deleted move assignment operator. */
    UdpReceiver& operator=(UdpReceiver&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] receiveOptions - Socket receive option to control
     * the use of broadcasts/unicast.
     * \param[in] receiveBufferSize - Socket receive option to
     * control receive buffer size.
     * \param[in] listenAddress - Specific NIC IP address to bind the socket to,
     * empty string will bind to all NICs
     * \param[in] messageReceivedHandlerEx - Special callback for when socket is used for special
     * use cases where the message handler needs the endpoint details passed to it. If this is
     * defined then you ideally would set messageReceivedHandler = {}.
     * \param[in] checkBytesLeftToReadEx - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message. Extended to take endpoint
     * details.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * hgl::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    UdpReceiver(asio_compat::io_service_t& ioService, 
	         uint16_t listenPort,
			 defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
			 defs::message_received_handler_t const& messageReceivedHandler,
			 eUdpOption receiveOptions = eUdpOption::broadcast,
			 size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE,
			 std::string const& listenAddress     = "",
			 defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
			 defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] listenPort - Our listen port for all detected networks.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] receiveOptions - Socket receive option to control
     * the use of broadcasts/unicast.
     * \param[in] receiveBufferSize - Socket receive option to
     * control receive buffer size.
     * \param[in] listenAddress - Specific NIC IP address to bind the socket to,
     * empty string will bind to all NICs
     * \param[in] messageReceivedHandlerEx - Special callback for when socket is used for special
     * use cases where the message handler needs the endpoint details passed to it. If this is
     * defined then you ideally would set messageReceivedHandler = {}.
     * \param[in] checkBytesLeftToReadEx - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message. Extended to take endpoint
     * details.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommended.
     */
    UdpReceiver(uint16_t listenPort, 
	         defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
			 defs::message_received_handler_t const& messageReceivedHandler,
			 eUdpOption receiveOptions = eUdpOption::broadcast,
			 size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE,
			 std::string const& listenAddress = "",
			 defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
			 defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx = {});
    /*! \brief Destructor. */
    ~UdpReceiver();
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

    /*! \brief Close the socket. */
    void CloseSocket();

private:
    /*!
     * \brief Create UDP socket.
     * \param[in] receiveOptions - UDP receive options.
     * \param[in] receiveBufferSize - Receive buffer size.
     */
    void CreateUdpSocket(eUdpOption receiveOptions, size_t receiveBufferSize);
    /*! \brief Start asynchronous read. */
    void StartAsyncRead();
    /*!
     * \brief Read completion handler.
     * \param[in] error - Error code if one has happened.
     * \param[in] bytesReceived - Number of bytes received.
     */
    void ReadComplete(const boost_sys::error_code& error, size_t bytesReceived);
    /*! \brief Are we closing. */
    bool Closing() const NO_EXCEPT_;
    /*!
     * \brief Set closing.
     * \param[in] close - True or false to set if closing.
     */
    void SetClosing(bool close) NO_EXCEPT_;

private:
    /*! \brief Mutex to protect shutdown of receiver. */
    mutable std::mutex m_closingMutex;
    /*! \brief Flag to show were are closing socket. */
    bool m_closing{false};
    /*! \brief Close event. */
    SyncEvent m_closeEvent;
    /*! \brief I/O service thread group. */
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service strand. */
    asio_compat::strand_t m_strand;
    /*! \brief Receiver listen port. */
    uint16_t m_listenPort{0};
    /*! \brief Receiver listen address. */
    std::string m_listenAddress{};
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_ex_t m_checkBytesLeftToReadEx;
    /*! \brief Callback to handle received message. */
    defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Callback to pass endpoint details to message handler. */
    defs::message_received_handler_ex_t m_messageReceivedHandlerEx;
    /*! \brief Socket receive buffer. */
    defs::char_buffer_t m_receiveBuffer;
    /*! \brief Sender end-point. */
    boost_udp_t::endpoint m_senderEndpoint;
    /*! \brief UDP socket. */
    boost_udp_t::socket m_socket;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // UDPRECEIVER
