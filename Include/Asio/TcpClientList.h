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
 * \file TcpClientList.h
 * \brief File containing TCP client list class declaration.
 */
#ifndef TCPCLIENTLIST_H
#define TCPCLIENTLIST_H

#include <map>
#include <mutex>
#include "AsioDefines.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{

/*! \brief Forward declaration of TcpClient. */
class TcpClient;

/*! \brief A class implementing a collection of bi-directional TCP clients. */
class CORE_LIBRARY_DLL_SHARED_API TcpClientList final
{
    using client_ptr_t = std::shared_ptr<TcpClient>;
    using client_map_t = std::map<defs::connection_t, client_ptr_t>;

public:
    /*! \brief Default constructor - deleted. */
    TcpClientList() = delete;
    /*! \brief Copy constructor - deleted. */
    TcpClientList(TcpClientList const&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    TcpClientList& operator=(TcpClientList const&) = delete;
    /*! \brief Move constructor - deleted. */
    TcpClientList(TcpClientList&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    TcpClientList& operator=(TcpClientList&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and
     * disptaching it accordingly.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asio::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     */
    TcpClientList(boost_iocontext_t& ioContext, size_t minAmountToRead,
                  defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                  defs::message_received_handler_t const& messageReceivedHandler,
                  eSendOption                             sendOption = eSendOption::nagleOn);
    /*!
     * \brief Initialisation constructor.
     * \param[in] minAmountToRead - Minimum amount of data to read on each receive, typical size of
     * header block.
     * \param[in] checkBytesLeftToRead - Function object capable of decoding the message and
     * computing how many bytes are left until a complete message.
     * \param[in] messageReceivedHandler - Function object capable of handling a received message
     * and disptaching it accordingly. \param[in] sendOption - Socket send option to control the use
     * of the Nagle algorithm.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommened.
     */
    TcpClientList(size_t                                  minAmountToRead,
                  defs::check_bytes_left_to_read_t const& checkBytesLeftToRead,
                  defs::message_received_handler_t const& messageReceivedHandler,
                  eSendOption                             sendOption = eSendOption::nagleOn);
    /*! \brief Default destructor. */
    ~TcpClientList();
    /*!
     * \brief Retrieve server connection details for a client.
     * \param[in] clientConn - Connection object describing server's address and port.
     * \return - Connection object describing target server's address and port.
     */
    defs::connection_t ServerConnection(defs::connection_t const& clientConn) const;
    /*!
     * \brief Check if the client is connected to the server.
     * \param[in] server - Connection object describing server's address and port.
     * \return True if conneced, false otherwise.
     */
    bool Connected(defs::connection_t const& server) const;
    /*!
     * \brief Retrieve this client's connection details.
     * \param[in] server - Connection object describing server's address and port.
     * \return Connection object describing this client's address and port.
     *
     * Throws xUnknownConnectionError is remoteEnd is not valid.
     */
    defs::connection_t GetClientDetailsForServer(defs::connection_t const& server) const;
    /*!
     * \brief Manually close the client's connection.
     * \param[in] server - Connection object describing server's address and port.
     *
     * Note that this object uses RAII so will close all connections when destroyed.
     */
    void CloseConnection(defs::connection_t const& server) const;
    /*!
     * \brief Manually close the all client connections.
     *
     * Note that this object uses RAII so will close all connections when destroyed.
     */
    void CloseConnections() const;
    /*! \brief Destroy all TCP clients and clear map. */
    void ClearConnections();
    /*!
     * \brief Send a message buffer to the server asynchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unlessa an exception is thrown. This
     * method gives best performance when sending.
     */
    void SendMessageToServerAsync(defs::connection_t const&  server,
                                  defs::char_buffer_t const& message);
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(defs::connection_t const&  server,
                                 defs::char_buffer_t const& message);

private:
    /*!
     * \brief Create the client connection for the server.
     * \param[in] server - Connection object describing server's address and port.
     * \return A std::shared_ptr to a TcpClient or a null pointer of not found.
     */
    client_ptr_t CreateTcpClient(defs::connection_t const& server);
    /*!
     * \brief Find the client connection for the server.
     * \param[in] server - Connection object describing server's address and port.
     * \return A std::shared_ptr to a TcpClient or a null pointer of not found.
     */
    client_ptr_t FindTcpClient(defs::connection_t const& server) const;

private:
    /*! \brief Mutex to make access to map thread safe. */
    mutable std::mutex m_mutex;
    /*! \brief External boost IO context to manage ASIO. */
    boost_iocontext_t* m_ioContextPtr{nullptr};
    /*! \brief Minimum amount of data to read on each receive, typical size of header block. */
    size_t m_minAmountToRead{0};
    /*! \brief Function object capable of decoding the message and computing how many bytes are left
     * until a complete message. */
    defs::check_bytes_left_to_read_t m_checkBytesLeftToRead{};
    /*! \brief Function object cpable of handling a received message and disptaching it accordingly.
     */
    defs::message_received_handler_t m_messageReceivedHandler{};
    /*! \brief Socket send option to control the use of the Nagle algorithm. */
    eSendOption m_sendOption{eSendOption::nagleOn};
    /*! \brief Map of TCP clients. */
    client_map_t m_clientMap{};
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPCLIENTLIST_H
