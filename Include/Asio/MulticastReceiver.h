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
 * \file MulticastReceiver.h
 * \brief File containing multicast receiver class declaration.
 */

#ifndef MULTICASTRECEIVER
#define MULTICASTRECEIVER

#include <mutex>
#include "AsioDefines.h"
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
 * \brief A general purpose multicast receiver.
 *
 * This class is used as the underpinnings of the SimpleMulticastReceiver class.
 */
class CORE_LIBRARY_DLL_SHARED_API MulticastReceiver final
{
public:
    /*! \brief Default constructor - deleted. */
    MulticastReceiver() = delete;
    /*! \brief Deleted copy constructor. */
    MulticastReceiver(const MulticastReceiver&) = delete;
    /*! \brief Deleted copy assignment operator. */
    MulticastReceiver& operator=(const MulticastReceiver&) = delete;
    /*! \brief Deleted move constructor. */
    MulticastReceiver(MulticastReceiver&&) = delete;
    /*! \brief Deleted move assignment operator. */
    MulticastReceiver& operator=(MulticastReceiver&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] interfaceAddress - Optional interface IP address for incoming network messages.
     * \param[in] receiveBufferSize - Optional socket receive option to control receive buffer size.
     * \param[in] messageReceivedHandlerEx - Special callback for when socket is used for special
     * use cases where the message handler needs the endpoint details passed to it. If this is
     * defined then you ideally would set messageReceivedHandler = {}.
     * \param[in] checkBytesLeftToReadEx - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message. Extended to take endpoint
     * details.
     *
     * Typically use this constructor when managing a pool of threads using an instance of
     * hgl::IoServiceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     */
    MulticastReceiver(asio_compat::io_service_t& ioService,
				   defs::connection_t const& multicastConnection,
				   defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
				   defs::message_received_handler_t const& messageReceivedHandler,
				   std::string const& interfaceAddress = "",
				   size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE,
				   defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
				   defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] interfaceAddress - Optional interface IP address for incoming network messages.
     * \param[in] receiveBufferSize - Optional socket receive option to control receive buffer size.
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
    MulticastReceiver(defs::connection_t const& multicastConnection,
				   defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
				   defs::message_received_handler_t const& messageReceivedHandler,
				   std::string const& interfaceAddress = "",
				   size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE,
				   defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
				   defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx = {});
    /*! \brief Destructor. */
    ~MulticastReceiver();
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

    void CloseSocket();

private:
    /*!
     * \brief Create multicast socket.
     * \param[in] receiveBufferSize - Receive buffer size.
     */
    void CreateMulticastSocket(size_t receiveBufferSize);
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
    threads::SyncEvent m_closeEvent;
    /*! \brief I/O service thread group. */
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service strand. */
    asio_compat::strand_t m_strand;
    /*! \brief Multicast connection details. */
    defs::connection_t m_multicastConnection;
    /*! \brief Interface IP address of outgoing network adaptor. */
    std::string m_interfaceAddress;
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
    /*! \brief The multicast socket. */
    boost_udp_t::socket m_socket;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // MULTICASTRECEIVER
