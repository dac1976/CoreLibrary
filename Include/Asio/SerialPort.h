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
 * \file SerialPort.h
 * \brief File containing useful definitions.
 */

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "AsioDefines.h"
#include "IoContextThreadGroup.h"
#include "Threads/SyncEvent.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The serial namespace. */
namespace serial
{

/*!
 * \brief Serial port class.
 *
 * This gives access to serial ports for both Windows and Linux via Boost ASIO.
 */
class CORE_LIBRARY_DLL_SHARED_API SerialPort
{
public:
    /*! \brief Default constructor - deleted. */
    SerialPort() = delete;
    /*! \brief Deleted copy constructor. */
    SerialPort(const SerialPort&) = delete;
    /*! \brief Deleted copy assignment operator. */
    SerialPort& operator=(const SerialPort&) = delete;
    /*! \brief Deleted move constructor. */
    SerialPort(SerialPort&&) = delete;
    /*! \brief Deleted move assignment operator. */
    SerialPort& operator=(SerialPort&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - Reference to I/O service.
     * \param[in] comPort - name of serial port to use in OS, e.g. "COM1" on Windows, "TTYUSB1" on
     * Linux.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] settings - structure containing connection options and behavioural
     * settings.
     */
    SerialPort(asio_compat::io_service_t& ioService,
	         std::string const& comPort,
             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
             defs::message_received_handler_t const& messageReceivedHandler,
             SerialPortSettings const& settings = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] comPort - name of serial port to use in OS, e.g. "COM1" on Windows, "TTYUSB1" on
     * Linux.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and dispatching it accordingly.
     * \param[in] settings - structure containing connection options and behavioural
     * settings.
     * \param[in] numIoSvcThreads - Number of threads to allocate to internal IO service.
     *
     * This constructor creates an internal IO service with N threads associated with it.
     */
    SerialPort(std::string const& comPort,
             defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
             defs::message_received_handler_t const& messageReceivedHandler,
             SerialPortSettings const& settings = {},
			 uint32_t numIoSvcThreads = 1);

    /*! \brief Default destructor. */
    ~SerialPort();

    /*!
     * \brief Retrieve this serial port's com port name.
     * \return The com port name.
     */
    std::string ComPort() const;

    /*! \brief Close the port */
    void ClosePort();

    /*!
     * \brief Send a message buffer to the receiver.
     * \param[in] message - The message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMsg(const defs::char_buffer_t& message);

private:
    /*!
     * \brief Create the serial port by setting its correct options
     */
    void CreateSerialPort();
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
    /*! \brief Process asynchronous close serial port. */
    void ProcessClosePort();
    /*! \brief Start asynchronous read. */
    void StartAsyncRead();
    /*! \brief Asynchronous read. */
    void AsyncRead(size_t amountToRead);
    /*!
     * \brief Read completion handler.
     * \param[in] error - Error code if one has happened.
     * \param[in] bytesReceived - Number of bytes received.
     */
    void ReadComplete(const boost_sys::error_code& error, size_t bytesReceived);

private:
    /*! \brief Mutex to protect shutdown of receiver. */
    mutable std::mutex m_closingMutex;
    /*! \brief Event to synchronise shutdown of receiver. */
    threads::SyncEvent m_closedEvent;
    /*! \brief Flag to show were are closing socket. */
    bool m_closing{false};
    /*! \brief I/O service thread group. */
    std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
    /*! \brief I/O service strand. */
    asio_compat::strand_t m_strand;
    /*! \brief Serial port name. */
    std::string m_comPort;
    /*! \brief Callback to check number of bytes left to read. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Callback to handle received message. */
    defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Serial port settings. */
    SerialPortSettings m_settings;
    /*! \brief Socket receive buffer. */
    defs::char_buffer_t m_receiveBuffer;
    /*! \brief Message buffer. */
    defs::char_buffer_t m_messageBuffer;
    /*! \brief The serial port. */
    boost_asio::serial_port m_serialPort;
};

} // namespace serial
} // namespace asio
} // namespace core_lib

#endif // SERIALPORT_H

