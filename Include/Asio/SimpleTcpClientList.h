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
 * \file SimpleTcpClientList.h
 * \brief File containing simple TCP client list class declaration.
 */
#ifndef SIMPLETCPCLIENTLIST_H
#define SIMPLETCPCLIENTLIST_H

#include <map>
#include <mutex>
#include "Asio/SimpleTcpClient.h"

/*! \brief The core_lib namespace. */
namespace core_lib
{
/*! \brief The asio namespace. */
namespace asio
{
/*! \brief The tcp namespace. */
namespace tcp
{

/*! \brief A class implementing a collection of bi-directional simple TCP clients. */
class CORE_LIBRARY_DLL_SHARED_API SimpleTcpClientList final
{
    using client_ptr_t = std::shared_ptr<SimpleTcpClient>;
    using client_map_t = std::map<defs::connection_t, client_ptr_t>;

public:
    /*! \brief Default constructor - deleted. */
    SimpleTcpClientList() = delete;
    /*! \brief Copy constructor - deleted. */
    SimpleTcpClientList(SimpleTcpClientList const&) = delete;
    /*! \brief Copy assignment operator - deleted. */
    SimpleTcpClientList& operator=(SimpleTcpClientList const&) = delete;
    /*! \brief Move constructor - deleted. */
    SimpleTcpClientList(SimpleTcpClientList&&) = delete;
    /*! \brief Move assignment operator - deleted. */
    SimpleTcpClientList& operator=(SimpleTcpClientList&&) = delete;
    /*!
     * \brief Initialisation constructor.
     * \param[in] ioContext - External boost IO context to manage ASIO.
     * \param[in] messageDispatcher - Function object capable of handling a received message and
     * disptaching it accordingly.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::asioIoServoceThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be exectued
     * using this thread pool managed by a single IO context. This is the recommended constructor.
     */
    SimpleTcpClientList(boost_iocontext_t&                        ioContext,
                        defs::default_message_dispatcher_t const& messageDispatcher,
                        eSendOption sendOption = eSendOption::nagleOn);
    /*!
     * \brief Initialisation constructor.
     * \param[in] messageDispatcher - Function object capable of handling a received message and
     * disptaching it accordingly.
     * \param[in] sendOption - Socket send option to control the use of the Nagle algorithm.
     *
     * This constructor does not require an external IO context to run instead it creates
     * its own IO context object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO context constructor is recommened.
     */
    SimpleTcpClientList(defs::default_message_dispatcher_t const& messageDispatcher,
                        eSendOption sendOption = eSendOption::nagleOn);
    /*! \brief Default destructor. */
    ~SimpleTcpClientList();
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
    void CloseConnection(defs::connection_t const& server);
    /*!
     * \brief Manually close the all client connections.
     *
     * Note that this object uses RAII so will close all connections when destroyed.
     */
    void CloseConnections();
    /*! \brief Destroy all simple TCP clients and clear map. */
    void ClearConnections();
    /*!
     * \brief Send a header-only message to the server asynchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send
     * the
     * response, the default value will mean the response address will point to this client socket.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the server.
     */
    void
    SendMessageToServerAsync(defs::connection_t const& server, int32_t messageId,
                             defs::connection_t const& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header-only message to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method only sends a simple core_lib::asio::defs::MessageHeader
     * object to the server.
     */
    bool SendMessageToServerSync(defs::connection_t const& server, int32_t messageId,
                                 defs::connection_t const& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header plus message buffer to the server asynchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send
     * the
     * response, the default value will mean the response address will point to this client socket.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::defs::MessageHeader object to
     * the server.
     */
    void
    SendMessageToServerAsync(defs::connection_t const& server, const defs::char_buffer_t& message,
                             int32_t                   messageId,
                             defs::connection_t const& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header plus message buffer to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method only sends a simple core_lib::asio::defs::MessageHeader
     * object to the server.
     */
    bool SendMessageToServerSync(defs::connection_t const&  server,
                                 const defs::char_buffer_t& message, int32_t messageId,
                                 defs::connection_t const& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a full message to the server asynchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     *
     * This function is asynchronous so will return immediately, with no
     * success or failure reported, unless an exception is thrown. This
     * method gives best performance when sending. Furthermore this method
     * uses the a core_lib::asio::defs::MessageHeader object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    void SendMessageToServerAsync(defs::connection_t const& server, T const& message,
                                  int32_t                   messageId,
                                  defs::connection_t const& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto clientPtr = FindTcpClient(server);

        if (!clientPtr)
        {
            clientPtr = CreateTcpClient(server);
        }

        if (clientPtr)
        {
            clientPtr->SendMessageToServerAsync<T, A>(message, messageId, responseAddress);
        }
    }
    /*!
     * \brief Send a full message to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - The message of type T to send behind the header serialized to an
     * boost::serialization-compatible archive of type A.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method uses the a core_lib::asio::defs::MessageHeader object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerSync(defs::connection_t const& server, T const& message,
                                 int32_t                   messageId,
                                 defs::connection_t const& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        bool success   = false;
        auto clientPtr = FindTcpClient(server);

        if (!clientPtr)
        {
            clientPtr = CreateTcpClient(server);
        }

        if (clientPtr)
        {
            success = clientPtr->SendMessageToServerSync<T, A>(message, messageId, responseAddress);
        }

        return success;
    }
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
                                  const defs::char_buffer_t& message);
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(defs::connection_t const&  server,
                                 const defs::char_buffer_t& message);

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
    /*! \brief Function object cpable of handling a received message and disptaching it accordingly.
     */
    defs::default_message_dispatcher_t m_messageDispatcher{};
    /*! \brief Socket send option to control the use of the Nagle algorithm. */
    eSendOption m_sendOption{eSendOption::nagleOn};
    /*! \brief Map of simple TCP clients. */
    client_map_t m_clientMap{};
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // SIMPLETCPCLIENTLIST_H
