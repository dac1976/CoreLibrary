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
 * \file TcpConnection.h
 * \brief File containing TCP connection class declaration.
 */

#ifndef TCPCONNECTION
#define TCPCONNECTION

#include "AsioDefines.h"
#include "Threads/SyncEvent.h"
#include <memory>
#include <mutex>
#include <deque>
#include <utility>

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{

/*! \brief Forward declaration of TCP connections class. */
class TcpConnections;

/*!
 * \brief TCP connection class.
 *
 * This class is the one of the fundamental building blocks for the other TCP networking classes.
 */
class CORE_LIBRARY_DLL_SHARED_API TcpConnection final
    : public std::enable_shared_from_this<TcpConnection>
{
    using msg_ptr_t  = std::shared_ptr<defs::char_buffer_t>;
    using msg_pool_t = std::vector<msg_ptr_t>;

public:
    /*! \brief Default constructor - deleted. */
    TcpConnection() = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - Reference to I/O service.
     * \param[in] connections - Reference to TCP connections object.
     * \param[in] minAmountToRead - Minimum amount to read.
     * \param[in] checkBytesLeftToRead - Check bytes left to read callback.
     * \param[in] messageReceivedHandler - Message received handler callback.
     * \param[in] sendOption - Socket send option.
     * \param[in] maxAllowedUnsentAsyncMessages - Maximum allowed number of unsent async messages.
     * \param[in] sendPoolMsgSize - Default size of message in pool. Set to 0 to not use the pool
     * and instead use dynamic allocation.
     */
    TcpConnection(boost_iocontext_t& ioContext, TcpConnections& connections, size_t minAmountToRead,
                  defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                  defs::message_received_handler_t const& messageReceivedHandler,
                  eSendOption                             sendOption = eSendOption::nagleOn,
                  size_t maxAllowedUnsentAsyncMessages               = MAX_UNSENT_ASYNC_MSG_COUNT,
                  size_t sendPoolMsgSize                             = 0);
    /*! \brief Copy constructor deleted. */
    TcpConnection(const TcpConnection&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    TcpConnection& operator=(const TcpConnection&) = delete;
    /*! \brief Move constructor deleted. */
    TcpConnection(TcpConnection&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    TcpConnection& operator=(TcpConnection&&) = delete;
    /*! \brief Default virtual destructor. */
    virtual ~TcpConnection() = default;
    /*!
     * \brief Get mutable reference to the socket object.
     * \return Socket reference.
     */
    boost_tcp_t::socket& Socket();
    /*!
     * \brief Get a const reference to the socket object.
     * \return Const socket reference.
     */
    const boost_tcp_t::socket& Socket() const;
    /*!
     * \brief Connect to an endpoint.
     * \param[in] endPoint - Connection details of some endpoint.
     */
    void Connect(const defs::connection_t& endPoint);
    /*! \brief Close this connection. */
    void CloseConnection();
    /*! \brief Start aysnchronously reading data when available. */
    void StartAsyncRead();
    /*!
     * \brief Send an asynchronous message.
     * \param[in] message - Message buffer to send.
     * \return Returns true if posted async message, retruns false if failed to post message.
     */
    bool SendMessageAsync(const defs::char_buffer_t& message);
    /*!
     * \brief Send a synchronous message.
     * \param[in] message - Message buffer to send.
     * \return True if sent successfully, false otherwise.
     */
    bool SendMessageSync(const defs::char_buffer_t& message);
    /*!
     * \brief Get number of unsent async messages.
     * \return Number of pending queued async messages
     */
    size_t NumberOfUnsentAsyncMessages() const;

private:
    /*!
     * \brief Set closing flag.
     * \param[in] closing - Closing state flag.
     */
    void SetClosing(bool closing);
    /*!
     * \brief Is the connection closing?
     * \return True if closing, false otherwise.
     */
    bool IsClosing() const;
    /*! \brief Process closing of socket. */
    void ProcessCloseSocket();
    /*! \brief Self destruct this object. */
    void DestroySelf();
    /*!
     * \brief Asynchronously read an amount.
     * \param[in] amountToRead - Number of bytes to read from socket.
     */
    void AsyncReadFromSocket(size_t amountToRead);
    /*!
     * \brief Read completion handler.
     * \param[in] error - Error flag if fault occurred.
     * \param[in] bytesReceived - Number of bytes received.
     * \param[in] bytesExpected - Number of bytes expected.
     */
    void ReadComplete(const boost_sys::error_code& error, size_t bytesReceived,
                      size_t bytesExpected);
    /*!
     * \brief Asynchronously write to the socket.
     * \param[in] message - Message buffer to write.
     * \param[in] poolIndex - Message pool index.
     */
    void AsyncWriteToSocket(defs::char_buffer_t const& message, size_t poolIndex);
    /*!
     *  \brief Decrement unsent async message counter.
     *  \param[in] messagePoolIndex - The message pool index that can now be reused.
     */
    void DecrementUnsentAsyncCounter(size_t messagePoolIndex);
    /*!
     * \brief Initialise message pool.
     * \param[in] memPoolMsgCount - Pool size as number of messages.
     * \param[in] defaultMsgSize - Initial size of a message in the pool.
     */
    void InitialiseMsgPool(size_t memPoolMsgCount, size_t defaultMsgSize);
    /*!
     * \brief Get next message to use from pool
     * \param[in] msgItem - Pointer to message item and its pool index if using a pool.
     * \param[in] sourceMsg - Reference to source message to copy into message item.
     * \return Could a valid message item could be obtained?
     */
    bool GetNewMessgeObject(std::pair<msg_ptr_t, size_t>& msgItem,
                            defs::char_buffer_t const&    sourceMsg);

private:
    /*! \brief Access mutex for thread safety. */
    mutable std::mutex m_mutex;
    /*! \brief Access mutex for thread safety. */
    mutable std::mutex m_asyncPoolMutex;
    /*! \brief Connection close event. */
    threads::SyncEvent m_closedEvent{};
    /*! \brief Closing connection flag. */
    bool m_closing{false};
    /*! \brief I/O context strand. */
    boost_iocontext_t::strand m_strand;
    /*! \brief Reference to TCP connections object. */
    TcpConnections& m_connections;
    /*! \brief Minimum amount to read from socket. */
    size_t m_minAmountToRead{0};
    /*! \brief Check bytes left to read callback. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead{};
    /*! \brief Message received handler callback. */
    defs::message_received_handler_t m_messageReceivedHandler{};
    /*! \brief Socket send option. */
    eSendOption m_sendOption{eSendOption::nagleOn};
    /*! \brief Socket receive buffer. */
    defs::char_buffer_t m_receiveBuffer{};
    /*! \brief Message buffer. */
    defs::char_buffer_t m_messageBuffer{};
    /*! \brief Max allowed unsent async message counter. */
    size_t m_maxAllowedUnsentAsyncMessages{MAX_UNSENT_ASYNC_MSG_COUNT};
    /*! \brief Unsent async message counter. */
    size_t m_numUnsentAsyncMessages{0};
    /*! \brief Positions in message pool. */
    std::deque<size_t> m_availablePoolIndices;
    /*! \brief Async message pool. */
    msg_pool_t m_msgPool;
    /*! \brief TCP socket. */
    boost_tcp_t::socket m_socket;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTION
