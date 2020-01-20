m_receiver // This file is part of CoreLibrary containing useful reusable utility
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
 * \file UdpSender.h
 * \brief File containing UDP sender class declaration.
 */

#ifndef UDPSENDER
#define UDPSENDER

#include "IoContextThreadGroup.h"

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
     * \brief A general purpose UDP sender.
     *
     * This class forms the underpinnings of the UdpTypedSender class.
     */
    class CORE_LIBRARY_DLL_SHARED_API UdpSender final
    {
    public:
        /*! \brief Default constructor - deleted. */
        UdpSender() = delete;
        /*!
         * \brief Initialisation constructor.
         * \param[in] ioContext - External boost IO context to manage ASIO.
         * \param[in] receiver - Connection object describing target receiver's address and port.
         * \param[in] sendOption - Socket send option to control the use of broadcasts/unicast.
         * \param[in] sendBufferSize - Socket send option to control send buffer size.
         *
         * Typically use this constructor when managing a bool of threads using an instance of
         * core_lib::asio::IoContextThreadGroup in your application to manage a pool of
         * std::threads. This means you can use a single thread pool and all ASIO operations will be
         * exectued using this thread pool managed by a single IO context. This is the recommended
         * constructor.
         */
        UdpSender(boost_iocontext_t& ioContext, const defs::connection_t& receiver,
                  eUdpOption sendOption     = eUdpOption::broadcast,
                  size_t     sendBufferSize = DEFAULT_UDP_BUF_SIZE);
        /*!
         * \brief Initialisation constructor.
         * \param[in] receiver - Connection object describing target receiver's address and port.
         * \param[in] sendOption - Socket send option to control the use of broadcasts/unicast.
         * \param[in] sendBufferSize - Socket send option to control send buffer size.
         *
         * This constructor does not require an external IO context to run instead it creates
         * its own IO context object along with its own thread. For very simple cases this
         * version will be fine but in more performance and resource critical situations the
         * external IO context constructor is recommened.
         */
        UdpSender(const defs::connection_t& receiver, eUdpOption sendOption = eUdpOption::broadcast,
                  size_t sendBufferSize = DEFAULT_UDP_BUF_SIZE);

        /*! \brief Copy constructor - deleted. */
        UdpSender(const UdpSender&) = delete;
        /*! \brief Copy assignment operator - deleted. */
        UdpSender& operator=(const UdpSender&) = delete;
        /*! \brief Move constructor - deleted. */
        UdpSender(UdpSender&&) = delete;
        /*! \brief Move assignment operator - deleted. */
        UdpSender& operator=(UdpSender&&) = delete;
        /*! \brief Default destructor. */
        ~UdpSender() = default;
        /*!
         * \brief Retrieve receiver connection details.
         * \return - Connection object describing target receiver's address and port.
         */
        defs::connection_t ReceiverConnection() const;
        /*!
         * \brief Send a message buffer to the receiver.
         * \param[in] message - The message buffer.
         * \return Returns the success state of the send as a boolean.
         */
        bool SendMessage(const defs::char_buffer_t& message);

    private:
        /*!
         * \brief Create UDP socket.
         * \param[in] sendOption - UDP send option.
         * \param[in] sendBufferSize - Send buffer size.
         */
        void CreateUdpSocket(eUdpOption sendOption, size_t sendBufferSize);
        /*!
         * \brief Synchronised send to method.
         * \param[in] message - Message buffer to send.
         * \return True if successfully sent, false otherwise.
         */
        bool SyncSendTo(const defs::char_buffer_t& message);

    private:
        /*! \brief I/O context thread group. */
        std::unique_ptr<IoContextThreadGroup> m_ioThreadGroup{};
        /*! \brief Receiver connection details. */
        defs::connection_t m_receiver{};
        /*! \brief Recevier end-point. */
        boost_udp_t::endpoint m_receiverEndpoint{};
        /*! \brief End-point resolver. */
        boost_udp_t::resolver m_receiverResolver;
        /*! \brief UDP socket. */
        boost_udp_t::socket m_socket;
    };

    } // namespace udp
    } // namespace asio
} // namespace core_lib

#endif // UDPSENDER
