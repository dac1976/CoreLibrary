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
    /*! \brief Deleted copy constructor. */
    SimpleTcpClientList(const SimpleTcpClientList&) = delete;
    /*! \brief Deleted copy assignment operator. */
    SimpleTcpClientList& operator=(const SimpleTcpClientList&) = delete;
    /*! \brief Deleted move constructor. */
    SimpleTcpClientList(SimpleTcpClientList&&) = delete;
    /*! \brief Deleted move assignment operator. */
    SimpleTcpClientList& operator=(SimpleTcpClientList&&) = delete;

    /*!
     * \brief Initialisation constructor.
     * \param[in] ioService - External boost IO service to manage ASIO.
     * \param[in] messageDispatcher - Function object capable of handling a received message and
     * dispatching it accordingly.
     * \param[in] settings - structure containing connection options and behavioural settings.
     *
     * Typically use this constructor when managing a bool of threads using an instance of
     * core_lib::IoContextThreadGroup in your application to manage a pool of std::threads.
     * This means you can use a single thread pool and all ASIO operations will be executed
     * using this thread pool managed by a single IO service. This is the recommended constructor.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to use process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    SimpleTcpClientList(asio_compat::io_service_t& ioService,
                    defs::default_message_dispatcher_t const& messageDispatcher,
                    SimpleTcpSettings const& settings = {});
    /*!
     * \brief Initialisation constructor.
     * \param[in] messageDispatcher - Function object capable of handling a received message and
     * dispatching it accordingly.
     * \param[in] settings - structure containing connection options and behavioural settings.
     *
     * This constructor does not require an external IO service to run instead it creates
     * its own IO service object along with its own thread. For very simple cases this
     * version will be fine but in more performance and resource critical situations the
     * external IO service constructor is recommended.
     *
     * NOTE: When the message pool feature is used then all messages passed to the
     * the registered dispatcher are managed by the internal pool. Care must be taken
     * in the dispatcher to use process the messages as quickly as possibly so the pool
     * doesn't fill and start overwriting older messages. If the messages need to be kept
     * then it is the dispatchers job to make a suitable copy of the received message.
     */
    explicit SimpleTcpClientList(defs::default_message_dispatcher_t const& messageDispatcher,
                            SimpleTcpSettings const& settings = {});
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
    /*!
     * \brief Pre-emptively create connection.
     * \param[in] server - Connection object describing server's address and port.
     * \return Returns the success state of the send as a boolean.
     *
     * NOTE: The various send functions automatically create connecstions as necessary
     * but this function is useful if you want to establish a connection before
     * sending for the first time.
     */
    bool CreateConnectionToServer(defs::connection_t const& server);
    /*!
     * \brief Send a header-only message to the server asynchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send
     * the
     * response, the default value will mean the response address will point to this client socket.
     *
     * This function is asynchronous so will return immediately
     * return
     *	false indicates failure
     * 	true  means the message has been passed in to the io service to be sent out later,
     * 	      but does not indicate that the message has been sent out successfully.
     * This method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::HGL_MSG_HDR object to
     * the server.
     */
    bool
    SendMessageToServerAsync(defs::connection_t const& server,
	                     int32_t messageId,
                         defs::connection_t const& responseAddress = defs::NULL_CONNECTION);
    /*!
     * \brief Send a header-only message to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] messageId - Unique message ID to insert into message header.
     * \param[in] responseAddress - (Optional) The address and port where the server should send the
     * response, the default value will mean the response address will point to this client socket.
     * \return Returns the success state of the send as a boolean.
     *
     * This method only sends a simple core_lib::asio::HGL_MSG_HDR
     * object to the server.
     */
    bool SendMessageToServerSync(defs::connection_t const& server,
	                        int32_t messageId,
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
     * This function is asynchronous so will return immediately
     * return
     *	false indicates failure
     * 	true  means the message has been passed in to the io service to be sent out later,
     * 	      but does not indicate that the message has been sent out successfully.
     * This method gives best performance when sending. Furthermore this method
     * only sends a simple core_lib::asio::HGL_MSG_HDR object to
     * the server.
     */
    bool
    SendMessageToServerAsync(defs::connection_t const& server,
	                    defs::char_buf_cspan_t message,
                        int32_t messageId,
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
     * This method only sends a simple core_lib::asio::HGL_MSG_HDR
     * object to the server.
     */
    bool SendMessageToServerSync(defs::connection_t const& server,
                            defs::char_buf_cspan_t message,
							int32_t messageId,
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
     * This function is asynchronous so will return immediately
     * return
     *	false indicates failure
     * 	true  means the message has been passed in to the io service to be sent out later,
     * 	      but does not indicate that the message has been sent out successfully.
     * This method gives best performance when sending. Furthermore this method
     * uses the a core_lib::asio::HGL_MSG_HDR object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerAsync(defs::connection_t const& server,
	                         T const& message,
                             int32_t messageId,
                             defs::connection_t const& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_mapMutex);

        auto clientPtr = FindTcpClient(server);

        if (!clientPtr)
        {
            clientPtr = CreateTcpClient(server);
        }

        if (clientPtr)
        {
            return clientPtr->SendMessageToServerAsync<T, A>(message, messageId, responseAddress);
        }
        return false;
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
     * This method uses the a core_lib::asio::HGL_MSG_HDR object as the header.
     */
    template <typename T, typename A = serialize::archives::out_port_bin_t>
    bool SendMessageToServerSync(defs::connection_t const& server,
	                        T const& message,
                            int32_t messageId,
                            defs::connection_t const& responseAddress = defs::NULL_CONNECTION)
    {
        std::lock_guard<std::mutex> lock(m_mapMutex);

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
     * \return Returns true if posted async message, retruns false if failed to post message.
     *
     * This function is asynchronous so will return immediately. with no
     * indication of failure or success.
     * This method gives best performance when sending.
     */
    bool SendMessageToServerAsync(defs::connection_t const&  server,
                             defs::char_buf_cspan_t message);
    /*!
     * \brief Send a message buffer to the server synchronously.
     * \param[in] server - Connection object describing server's address and port.
     * \param[in] message - Message buffer.
     * \return Returns the success state of the send as a boolean.
     */
    bool SendMessageToServerSync(defs::connection_t const&  server,
                            defs::char_buf_cspan_t message);

    /*! \brief Clear all TCP clients from list. */
    void ClearList();
    /*!
     * \brief Get list of connections.
     * \param[out] serverDetailsList - list of server connection details.
     */
    void GetServerList(std::list<defs::connection_t>& serverDetailsList) const;
    /*!
     * \brief Get list of connections.
     * \return - list of server connection details.
     */
    std::vector<defs::connection_t> GetServerList() const;
    /*!
     * \brief Get number of unsent async messages.
     * \param[in] server - Target connection details.
     * \return Number of unsent messages
     */
    size_t NumberOfUnsentAsyncMessages(const defs::connection_t& server) const;

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
    /*! \brief Mutex to protect client map. */
    mutable std::mutex m_mapMutex;
    /*! \brief External boost IO service to manage ASIO. */
    asio_compat::io_service_t* m_ioServicePtr{nullptr};
    /*! \brief Function object capable of handling a received message and dispatching it
     * accordingly.
     */
    defs::default_message_dispatcher_t m_messageDispatcher;
    /*! \brief Structure holding socket connection options and behavioural settings. */
    SimpleTcpSettings m_settings;
    /*! \brief Map of simple TCP clients. */
    client_map_t m_clientMap{};
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // SIMPLETCPCLIENTLIST_H
