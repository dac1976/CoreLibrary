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
 * \file SerialPort.cpp
 * \brief File containing useful definitions.
 */
 
#include "Asio/SerialPort.h"
#include <stdexcept>
#include <boost/bind.hpp>
#if defined(USE_SOCKET_DEBUG)
#include <boost/exception/all.hpp>
#include "DebugLog/DebugLogging.h"
#endif

namespace core_lib
{
namespace asio
{
namespace serial
{

SerialPort::SerialPort(asio_compat::io_service_t& ioService, std::string const& comPort,
				   defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
				   defs::message_received_handler_t const& messageReceivedHandler,
				   SerialPortSettings const& settings)
    : m_strand(asio_compat::make_strand(ioService))
    , m_comPort(comPort)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_settings(settings)
    , m_receiveBuffer(m_settings.recvBufLengthLength, 0)
    , m_serialPort(ioService)
{
    CreateSerialPort();
}

SerialPort::SerialPort(std::string const&                      comPort,
				   defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
				   defs::message_received_handler_t const& messageReceivedHandler,
				   SerialPortSettings const& settings, uint32_t numIoSvcThreads)
    : m_ioThreadGroup(std::make_unique<IoContextThreadGroup>(numIoSvcThreads))
    , m_strand(asio_compat::make_strand(m_ioThreadGroup->IoService()))
    , m_comPort(comPort)
    , m_checkBytesLeftToRead(checkBytesLeftToRead)
    , m_messageReceivedHandler(messageReceivedHandler)
    , m_settings(settings)
    , m_receiveBuffer(m_settings.recvBufLengthLength, 0)
    , m_serialPort(m_ioThreadGroup->IoService())
{
    CreateSerialPort();
}

SerialPort::~SerialPort()
{
    ClosePort();
}

std::string SerialPort::ComPort() const
{
    return m_comPort;
}

void SerialPort::ClosePort()
{
    if (!m_serialPort.is_open())
    {
        return;
    }

    SetClosing(true);
    asio_compat::post(m_strand, boost::bind(&SerialPort::ProcessClosePort, this));
    m_closedEvent.Wait();
}

bool SerialPort::SendMsg(const defs::char_buffer_t& message)
{
    boost_sys::error_code error;
    auto bytesWritten = m_serialPort.write_some(boost_asio::buffer(message), error);

    if ((bytesWritten != message.size()) || error)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in SendMessage, for serial port: "
                               << m_comPort << ", bytes written = " << bytesWritten
                               << ", message length = " << message.size()
                               << ", error message = " << error.message());
#endif
        return false;
    }

    return true;
}

void SerialPort::CreateSerialPort()
{
    // Reserve memory for message buffer.
    m_messageBuffer.reserve(m_settings.msgBufLength);

    // Open port.
    boost_sys::error_code error;
    m_serialPort.open(m_comPort, error);

    if (error)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in CreateSerialPort, for serial port: "
                               << m_comPort << ", error message = " << error.message());
#endif

        throw std::runtime_error(error.message());
    }

    // Set option settings.
    m_serialPort.set_option(boost_asio::serial_port_base::baud_rate(m_settings.baudRate));
    m_serialPort.set_option(boost_asio::serial_port_base::character_size(m_settings.characterSize));
    m_serialPort.set_option(m_settings.stopBits);
    m_serialPort.set_option(m_settings.parity);
    m_serialPort.set_option(m_settings.flowControl);

    StartAsyncRead();
}

void SerialPort::SetClosing(bool closing)
{
    std::lock_guard<std::mutex> lock(m_closingMutex);
    m_closing = closing;
}

bool SerialPort::IsClosing() const
{
    // Only call IsClosing() from already mutex protected functions
    // that lock the m_closingMutex member.

    return m_closing;
}

void SerialPort::ProcessClosePort()
{
    try
    {
        std::lock_guard<std::mutex> lock(m_closingMutex);

        m_serialPort.cancel();
        m_serialPort.close();
    }
    catch (...)
    {
#if defined(USE_SOCKET_DEBUG)
        DEBUG_MESSAGE_EX_ERROR("Error in ProcessClosePort, error: "
                               << boost::current_exception_diagnostic_information());
#endif
        // Do nothing consume error.
    }

    m_closedEvent.Signal();
}

void SerialPort::StartAsyncRead()
{
    asio_compat::post(m_strand,
                      boost::bind(&SerialPort::AsyncRead, this, m_settings.minAmountToRead));
}

void SerialPort::AsyncRead(size_t amountToRead)
{
    std::lock_guard<std::mutex> lock(m_closingMutex);

    if (IsClosing())
    {
        return;
    }

    m_receiveBuffer.resize(amountToRead);

    m_serialPort.async_read_some(
        boost_asio::buffer(m_receiveBuffer),
        asio_compat::wrap(m_strand,
                          boost::bind(&SerialPort::ReadComplete,
                                      this,
                                      boost_placeholders::error,
                                      boost_placeholders::bytes_transferred)));
}

void SerialPort::ReadComplete(const boost_sys::error_code& error, size_t bytesReceived)
{
    size_t numBytesToRead = m_settings.minAmountToRead;

    // Reduce scope of mutex.
    {
        std::lock_guard<std::mutex> lock(m_closingMutex);

        if (IsClosing() || error)
        {
#if defined(USE_SOCKET_DEBUG)
            DEBUG_MESSAGE_EX_ERROR("ReadComplete called but port is closing or error reported: "
                                   << error.message() << ", on com port: " << m_comPort);
#endif
            // This will be because we are closing our socket.
            return;
        }

        bool clearMsgBuf = false;

        try
        {
            // The buffer copying below has been optimised for performance
            // after evaluating various possible mechanisms and is a few
            // orders of magnitude faster than the original iterator plus
            // back_inserter solution.
            m_messageBuffer.resize(m_messageBuffer.size() + bytesReceived);
            auto dataWritePos = m_messageBuffer.data() + (m_messageBuffer.size() - bytesReceived);
            std::copy(m_receiveBuffer.data(), m_receiveBuffer.data() + bytesReceived, dataWritePos);

            const size_t numBytesLeft = m_checkBytesLeftToRead(m_messageBuffer);

            if (numBytesLeft == 0)
            {
                m_messageReceivedHandler(m_messageBuffer);
                clearMsgBuf = true;
            }
            else if (std::numeric_limits<size_t>::max() == numBytesLeft)
            {
                // We have a problem.
                clearMsgBuf = true;
            }
            else
            {
                numBytesToRead = numBytesLeft;
            }
        }
        catch (...)
        {
#if defined(USE_SOCKET_DEBUG)
            DEBUG_MESSAGE_EX_ERROR("Error in ReadComplete, error: "
                                   << boost::current_exception_diagnostic_information());
#endif
            // Nothing to do here for now.
            clearMsgBuf = true;
        }

        if (clearMsgBuf)
        {
            m_messageBuffer.clear();
        }
    }

    asio_compat::post(m_strand, boost::bind(&SerialPort::AsyncRead, this, numBytesToRead));
}

} // namespace serial
} // namespace asio
} // namespace core_lib

