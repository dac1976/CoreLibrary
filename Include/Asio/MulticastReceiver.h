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
 * \file MulticastReceiver.h
 * \brief File containing multicast receiver class declaration.
 */

#ifndef MULTICASTRECEIVER
#define MULTICASTRECEIVER

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
 * \brief A general purpose multicast receiver.
 *
 * This class is used as the underpinnings of the SimpleMulticastReceiver class.
 */
class CORE_LIBRARY_DLL_SHARED_API MulticastReceiver final
{
public:
    /*! \brief Default constructor - deleted. */
    MulticastReceiver() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] multicastConnection - Connection object describing target multicast group address
     * and port.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] interfaceAddress - Optional interface IP address for incoming network messages.
     * \param[in] receiveBufferSize - Optional socket receive option to control receive buffer size.
     *
     * Typically use this constructor when managing a pool of threads using an instance of
     * core_lib::asio::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     */
    MulticastReceiver(boost_iocontext_t& ioContext, const defs::connection_t& multicastConnection,
                      const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                      const defs::message_received_handler_t& messageReceivedHandler,
                      const std::string&                      interfaceAddress = "",
                      size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE);
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
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommended.
     */
    MulticastReceiver(const defs::connection_t&               multicastConnection,
                      const defs::check_bytes_left_to_read_t& checkBytesLeftToRead,
                      const defs::message_received_handler_t& messageReceivedHandler,
                      const std::string&                      interfaceAddress = "",
                      size_t receiveBufferSize = DEFAULT_UDP_BUF_SIZE);
    /*! \brief Copy constructor - deleted. */
    MulticastReceiver(const MulticastReceiver&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    MulticastReceiver& operator=(const MulticastReceiver&) = delete;
    /*! \brief Move constructor - deleted. */
    MulticastReceiver(MulticastReceiver&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    MulticastReceiver& operator=(MulticastReceiver&&) = delete;
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
    /*! \brief Close socket. */
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
    /*!
     * \brief Set closing state.
     * \param[in] closing - Closing socket flag.
     */
    void SetClosing(bool closing);
    /*!
     * \brief Get closing state.
     * \return True if closing socket, false otherwise.
     */
    bool IsClosing() const;
    /*! \brief Process asynchronous close socket. */
    void ProcessCloseSocket();

private:
    /*! \brief Mutex to protect shutdown of receiver. */
    mutable std::mutex m_closingMutex{};
    /*! \brief Event to synchronise shutdown of receiver. */
    threads::SyncEvent m_closedEvent{};
    /*! \brief Flag to show were are closing socket. */
    bool m_closing{false};
    /*! \brief I/O context thread group. */
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O context reference. */
    boost_iocontext_t& m_ioContext;
    /*! \brief I/O context strand. */
    boost_iocontext_t::strand m_strand;
    /*! \brief Multicast connection details. */
    defs::connection_t m_multicastConnection{};
    /*! \brief Interface IP address of outgoing network adaptor. */
    std::string m_interfaceAddress{};
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead{};
    /*! \brief Callback to handle received message. */
    defs::message_received_handler_t m_messageReceivedHandler{};
    /*! \brief Socket receive buffer. */
    defs::char_buffer_t m_receiveBuffer{};
    /*! \brief Message buffer. */
    defs::char_buffer_t m_messageBuffer{};
    /*! \brief Sender end-point. */
    boost_udp_t::endpoint m_senderEndpoint{};
    /*! \brief The multicast socket. */
    boost_udp_t::socket m_socket;
};

} // namespace udp
} // namespace asio
} // namespace core_lib

#endif // MULTICASTRECEIVER
