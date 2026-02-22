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
 * \file TcpConnection.h
 * \brief File containing TCP connection class declaration.
 */

#ifndef TCPCONNECTION
#define TCPCONNECTION

#include "AsioDefines.h"
#include "Threads/SyncEvent.h"
#include <mutex>
#include <memory>
#include <deque>
#include <utility>
#include <atomic>

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
    using msg_ptr_t  = std::shared_ptr<defs::char_buffer_t>; // dynamic-only
    using msg_pool_t = std::vector<defs::char_buffer_t>;     // pool blocks (fixed-size)

    struct PendingWrite
    {
        enum class eKind : uint8_t
        {
            pool,
            dynamic
        };

        eKind     kind{eKind::dynamic};
        size_t    len{0};
        size_t    poolIndex{0};
        msg_ptr_t dyn; // valid only when kind==dynamic
    };

    struct EnqueuePreparedSendHandler
    {
        std::shared_ptr<TcpConnection> self;
        TcpConnection::PendingWrite write;

        EnqueuePreparedSendHandler(std::shared_ptr<TcpConnection> s, PendingWrite&& w)
            : self(std::move(s))
            , write(std::move(w))
        {
        }

        void operator()()
        {
            self->EnqueuePreparedSendOnStrand(std::move(write));
        }
    };

public:
    /*! \brief Default constructor - deleted. */
    TcpConnection() = delete;
    /*! \brief Deleted copy constructor. */
    TcpConnection(const TcpConnection&) = delete;
    /*! \brief Deleted copy assignment operator. */
    TcpConnection& operator=(const TcpConnection&) = delete;
    /*! \brief Deleted move constructor. */
    TcpConnection(TcpConnection&&) = delete;
    /*! \brief Deleted move assignment operator. */
    TcpConnection& operator=(TcpConnection&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - Reference to I/O service.
     * \param[in] connections - Reference to TCP connections object.
     * \param[in] checkBytesLeftToRead - Check bytes left to read callback.
     * \param[in] messageReceivedHandler - Message received handler callback.
     * \param[in] settings - structure containing connection options and behavioural settings.
     * \param[in] messageReceivedHandlerEx - Special callback for when socket is used for special
     * use cases where the message handler needs the endpoint details passed to it. If this is
     * defined then you ideally would set messageReceivedHandler = {}.
     * \param[in] checkBytesLeftToReadEx - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message. Extended to take endpoint
     * details.
     */
    TcpConnection(asio_compat::io_service_t&                 ioService,
                std::shared_ptr<TcpConnections> const&     connections,
                defs::check_bytes_left_to_read_t const&    checkBytesLeftToRead,
                defs::message_received_handler_t const&    messageReceivedHandler,
                TcpConnSettings const&                     settings                 = {},
                defs::message_received_handler_ex_t const& messageReceivedHandlerEx = {},
                defs::check_bytes_left_to_read_ex_t const& checkBytesLeftToReadEx   = {});
    /*! \brief Default virtual destructor. */
    ~TcpConnection() = default;
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
     * \return True if connection successful, false otherwise.
     */
    bool Connect(const defs::connection_t& endPoint);
    /*! \brief Close this connection. */
    void CloseConnection();
    /*! \brief Start aysnchronously reading data when available. */
    void StartAsyncRead(defs::connection_t const& endPoint);
    /*!
     * \brief Send an asynchronous message.
     * \param[in] message - Message buffer to send.
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * Note if the unsent count reaches the max level it will block sending new
     * messages and return false.
     */
    bool SendMessageAsync(const defs::char_buffer_t& message);
    /*!
     * \brief Send a synchronous message.
     * \param[in] message - Message buffer to send.
     * \return True if sent successfuilly, false otherwise.
     */
    bool SendMessageSync(const defs::char_buffer_t& message);
    /*!
     * \brief Get number of unsent async messages.
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages() const;

private:
    /*!
     * \brief Is the connection closing?
     * \return True if closing, false otherwise.
     */
    bool IsClosing() const;
    /*! \brief Process closing of socket. */
    void ProcessCloseSocket();
    /*! \brief Self destruct this object. */
    void DestroySelf();
    /*! \brief Self destruct this object via a strand */
    void DestroySelfOnStrand();
    /*!
     * \brief Asyncrhonously read an amount.
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
    /*! \brief Decrement unsent async message counter (strand-only). */
    void DecrementUnsentAsyncCounterOnStrand();
    /*! \brief Initialise message pool. */
    void InitialiseMsgPool();
    /*
     * NOTE:
     * We intentionally prepare pool/dynamic buffers *before* posting to the strand
     * (to avoid per-send allocations when the pool is enabled). That means the pool
     * free-list can be accessed from arbitrary caller threads as well as the strand,
     * so it must be protected with a mutex.
     */
    /*!
     * \brief Enqueue a prepared send on the strand.
     * \param[in] w - Prepared pending write.
     */
    void EnqueuePreparedSendOnStrand(PendingWrite w);

    /*! \brief Acquire a pool index (thread-safe). Returns true on success. */
    bool TryAcquirePoolIndex(size_t& idx);
    /*! \brief Release a pool index back to the free-list (thread-safe). */
    void ReleasePoolIndex(size_t idx);
    /*! \brief Start next write on strand */
    void StartNextWriteOnStrand();  
    /*!
     * \brief Do async write on strand.
     * \param[in] w - The pending write object.
     */
    void DoAsyncWriteOnStrand(PendingWrite const& w);
    /*!
     * \brief Write completion handler.
     * \param[in] error - Error state code.
    * \param[in] bytesTransferred - Number of bytes sent.
     */
    void WriteCompleteOnStrand(const boost_sys::error_code& error, size_t bytesTransferred);
    /*!
     * \brief Release the pending write object
     * \param[in] w - The pending write object.
     */
    void ReleasePendingWriteOnStrand(PendingWrite const& w);
    /*!
     * \brief Handler async connect callback.
     * \param[in] errorIn - Error code received from async_connect.
     * \param[out] errorOut - Error code reported out.
     */
    void ConnectHandler(boost::system::error_code const&                  errorIn,
                        std::shared_ptr<boost::system::error_code> const& errorOut,
                        size_t connectionCounter) NO_EXCEPT_;
    /*!
     * \brief Get next connection ID value.
     * return Next connection ID value.
     */
    size_t NextConnectionId() NO_EXCEPT_;
    /*!
     * \brief Store active connection ID value.
     * \param[in] currentConnectionId - The current connection ID.
     */
    void SetCurrentConnectionId(size_t currentConnectionId) NO_EXCEPT_;
    /*!
     * \brief Get current connection ID value.
     * return Current connection ID value.
     */
    size_t CurrentConnectionId() const NO_EXCEPT_;
    /*!
     * \brief Acquire the next pending write object for async send
     * \param[in] message - Message to send.
     */
    bool AcquirePendingWrite(const defs::char_buffer_t& message, PendingWrite& w);

private:
    /*! \brief Access mutex for thread safety. */
    mutable std::mutex m_mutex;
    /*! \brief Connection close event. */
    threads::SyncEvent m_closedEvent;
    /*! \brief Event to control connection request. */
    threads::SyncEvent m_connectEvent;
    /*! \brief Closing connection flag. */
    bool m_closing{false};
    /*! \brief I/O service strand. */
    asio_compat::strand_t m_strand;
    /*! \brief Reference to TCP connections object. */
    std::weak_ptr<TcpConnections> m_connections;
    /*! \brief Check bytes left to read callback. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead;
    /*! \brief Check bytes left to read callback. */
    defs::check_bytes_left_to_read_ex_t m_checkBytesLeftToReadEx;
    /*! \brief Message received handler callback. */
    defs::message_received_handler_t m_messageReceivedHandler;
    /*! \brief Message received handler extended callback. */
    defs::message_received_handler_ex_t m_messageReceivedHandlerEx;
    /*! \brief Structure holding socket connection options and behavioural settings. */
    TcpConnSettings m_settings;
    /*! \brief Socket receive buffer. */
    defs::char_buffer_t m_receiveBuffer;
    /*! \brief Message buffer. */
    defs::char_buffer_t m_messageBuffer;
    /*! \brief Unsent async message reservation counter (thread-safe). */
    std::atomic_size_t m_numUnsentAsyncMessages{0};
    /*! \brief Positions in message pool (LIFO). Protected by m_poolMutex. */
    std::vector<size_t> m_availablePoolIndices;
    /*! \brief Mutex protecting pool free-list (m_availablePoolIndices). */
    mutable std::mutex m_poolMutex;
    /*! \brief Async message pool blocks (fixed-size, size==sendPoolMsgSize). */
    msg_pool_t m_msgPool;
    /*! \brief Pending async writes (strand-only). */
    std::deque<PendingWrite> m_pendingWrites;
    /*! \brief True if an async_write is currently in flight (strand-only). */
    bool m_writeInProgress{false};
    /*! \brief Next Connection counter. */
    size_t m_nextConnectionId{0};
    /*! \brief Current  Connection counter */
    size_t m_currentConnectionId{0};
    /*! \brief Connection end point details. */
    defs::connection_t m_endPoint;
    /*! \brief Atomic flag to make sure we only remove ourselves once. */
    std::atomic_bool m_removed{false};
    /*! \brief TCP socket. */
    boost_tcp_t::socket m_socket;
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCONNECTION
