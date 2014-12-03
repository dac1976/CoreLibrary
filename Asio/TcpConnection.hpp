// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 Duncan Crutchley
// Contact <duncan.crutchley+corelibrary@gmail.com>
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
 * \file TcpConnection.hpp
 * \brief File containing TCP connection class declaration.
 */ 
 
#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "../SyncEvent.hpp"
#include "AsioDefines.hpp"
#include <mutex>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The tcp_conn namespace. */
namespace tcp_conn{

class TcpConnections;

class TcpConnection final
    : public std::enable_shared_from_this<TcpConnection>
{
public:
	enum eSendOption
	{
        nagleOff, // Implies send immediately.
		nagleOn
	};
	
	TcpConnection(boost_ioservice& ioService
				  , TcpConnections& connections
				  , const size_t minAmountToRead
				  , const asio_defs::check_bytes_left_to_read& checkBytesLeftToRead
				  , const asio_defs::message_received_handler& messageReceivedHandler
                  , const eSendOption sendOption = nagleOn);
				  
	TcpConnection(const TcpConnection& ) = delete;
	
	const TcpConnection& operator=(const TcpConnection& ) = delete;
	
	virtual ~TcpConnection() = default;
	
	const boost_tcp::socket& Socket() const;
	
	void Connect(const boost_tcp::endpoint& tcpEndPoint);
	
	void CloseConnection();
	
	void StartAsyncRead();    
	
	void SendMessageAsync(const asio_defs::char_vector& message);
					   
	bool SendMessageSync(asio_defs::char_vector message);

private:
	mutable std::mutex m_mutex;	
	threads::SyncEvent m_closedEvent;	
	threads::SyncEvent m_sendEvent;	
	bool m_closing;	
	boost_ioservice& m_ioService;	
	boost_ioservice::strand m_strand;
	boost_tcp::socket m_socket;	
	TcpConnections& m_connections;	
	const size_t m_minAmountToRead;
	asio_defs::check_bytes_left_to_read m_checkBytesLeftToRead;	
	asio_defs::message_received_handler m_messageReceivedHandler;	
    const eSendOption m_sendOption;
	asio_defs::char_vector m_receiveBuffer;
	asio_defs::char_vector m_messageBuffer;
	bool m_sendSuccess;
	
	void SetClosing(const bool closing);
	
	bool IsClosing() const;
	
	void ProcessCloseSocket();	
	
	void DestroySelf();
	
	void AsyncReadFromSocket(const size_t amountToRead);
	
	void ReadSomeData(const boost_sys::error_code& error
					  , const size_t bytesReceived
					  , const size_t bytesExpected);
					  
	void AsyncWriteToSocket(asio_defs::char_vector message);
	
	void SyncWriteToSocket(const asio_defs::char_vector& message);
	
	void WriteComplete(const boost_sys::error_code& error
	                   , const bool synchronous);
};


} // namespace tcp_conn
} // namespace core_lib

#endif // TCPCONNECTION_H
