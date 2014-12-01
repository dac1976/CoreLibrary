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
 * \brief File containing TCP connection classes' declarations.
 */ 
 
#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "../SyncEvent.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <string>
#include <vector>
#include <mutex>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The tcp_conn namespace. */
namespace tcp_conn{

class TcpConnections;

class TcpConnection final
    : public boost::enable_shared_from_this < TcpConnection >
{
public:
	typedef boost::asio::ip::tcp boost_tcp;
	
	typedef std::vector<char> char_vector;
	
	typedef std::function< size_t (const char*, const size_t ) > check_bytes_left_to_read;
	
	typedef std::function< void (const char_vector& ) > message_received_handler;

	enum eSendOption
	{
		nagleOff,
		nagleOn
	};
	
	TcpConnection(boost::asio::io_service& ioService
				  , TcpConnections& connections
				  , const size_t minAmountToRead
				  , const check_bytes_left_to_read& checkBytesLeftToRead
				  , const message_received_handler& messageReceivedHandler
				  , const eSendOption sendImmediately = nagleOn);
				  
	TcpConnection(const TcpConnection& ) = delete;
	const TcpConnection& operator=(const TcpConnection& ) = delete;
	
	virtual ~TcpConnection() = default;
	
	void CloseConnection();	

	void Connect(const boost_tcp::endpoint& tcpEndpoint);
	
	void StartAsyncRead();    
	
	void SendMessageAsync(const char_vector& message);
					   
	bool SendMessageSync(const char_vector& message);
	
	boost_tcp::socket& Socket();

private:
	mutable std::mutex m_closingMutex;	
	core_lib::threads::SyncEvent m_closedEvent;	
	bool m_closing;	
	boost::asio::io_service& m_ioService;	
	boost_tcp::socket m_socket;	
	TcpConnections& m_connections;	
	const size_t m_minAmountToRead;
	check_bytes_left_to_read m_checkBytesLeftToRead;	
	message_received_handler m_messageReceivedHandler;	
	const eSendOption m_sendImmediately;	
	char_vector m_receiveBuffer;
	char_vector m_messageBuffer;
	
	void SetClosing(const bool closing);
	
	bool IsClosing() const;
	
	void ProcessCloseSocket();	
	
	void StartAsyncReadFromSocket(const size_t amountToRead);
	
	void ResizeReceiveBuffer(const size_t newSize);
	
	void ReadSomeData(const boost::system::error_code& error
					  , const size_t bytesReceived
					  , const size_t bytesExpected);
					  
	void ResizeMessageBuffer(const size_t newSize);
};


} // namespace tcp_conn
} // namespace core_lib

#endif // TCPCONNECTION_H