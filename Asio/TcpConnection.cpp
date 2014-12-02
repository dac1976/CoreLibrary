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
 * \file TcpConnection.cpp
 * \brief File containing TCP connection class definition.
 */ 
 
#include "TcpConnection.hpp" 
#include "TcpConnections.hpp" 
#include <iterator>
#include <algorithm>

namespace core_lib {
namespace tcp_conn{

// Reserve 0.5 MiB for each buffer.
static const size_t DEFAULT_RESERVED_SIZE = 512*1024;
 
// ****************************************************************************
// 'class TcpConnection' definition
// ****************************************************************************
	
TcpConnection::TcpConnection(boost::asio::io_service& ioService
							 , TcpConnections& connections
							 , const size_t minAmountToRead
							 , const asio_defs::check_bytes_left_to_read& checkBytesLeftToRead
							 , const asio_defs::message_received_handler& messageReceivedHandler
							 , const eSendOption sendImmediately)
	: m_closing{false}
	, m_ioService{ioService}
	, m_strand{ioService}
	, m_socket(ioService}
	, m_connections{connections}
	, m_minAmountToRead{minAmountToRead}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_sendImmediately{sendImmediately}	
{
	m_receiveBuffer.reserve(RESERVED_SIZE);
	m_messageBuffer.reserve(RESERVED_SIZE);
}

TcpConnection::boost_tcp::socket& Socket()
{
}

void TcpConnection::Connect(const TcpConnection::boost_tcp::endpoint& tcpEndpoint)
{
	m_socket.connect(tcpEndpoint);
		
	boost_tcp::no_delay Option(m_SendImmediately);
	m_socket.set_option(Option);
	
	StartAsyncRead();
}

void TcpConnection::CloseConnection()
{
	if (!m_socket.is_open())
	{
		return;
	}
	
	SetClosing(true);
	
	m_ioService.post(m_strand.wrap(std::bind(&TcpConnection::ProcessCloseSocket
	                                         , shared_from_this())));
	
	m_closedEvent.Wait();

}

void TcpConnection::SetClosing(const bool closing)
{
	std::lock_guard<std::mutex> lock{m_mutex};
	m_closing = Closing;
}

bool TcpConnection::IsClosing() const
{
	std::lock_guard<std::mutex> lock{m_mutex};
	return m_closing;
}

void TcpConnection::ProcessCloseSocket()
{
	m_socket.close();
	m_closedEvent.Signal();
}

void TcpConnection::DestroySelf()
{
	if (!IsClosing())
	{
		m_connections.Remove(shared_from_this());
	}
}

void TcpConnection::StartAsyncRead()
{
	m_connections.Add(shared_from_this());
	
	AsyncReadFromSocket(m_minAmountToRead);
}

void TcpConnection::AsyncReadFromSocket(const size_t amountToRead)
{
	m_receiveBuffer.resize(amountToRead);

	boost::asio::async_read(m_socket, boost::asio::buffer(m_receiveBuffer, amountToRead)
							, m_strand.wrap(std::bind(&TcpConnection::ReadSomeData
												      , shared_from_this()
												      , boost::asio::placeholders::error
												      , boost::asio::placeholders::bytes_transferred
												      , amountToRead)));
}

void TcpConnection::ReadSomeData(const boost::system::error_code& error
								 , const size_t bytesReceived
								 , const size_t bytesExpected)
{
	size_t numBytes = 0;
	bool clearMsgBuf = false;
	
	if (error)
	{
		DestroySelf();
    }
	else if (bytesReceived != bytesExpected)
	{
		numBytes = m_minAmountToRead;
		clearMsgBuf = true;
    }
	else
	{
		try
		{
			std::copy(m_receiveBuffer.begin()
			          , m_receiveBuffer.end()
					  , std::back_inserter(m_messageBuffer));
				
			numBytes = checkBytesLeftToRead(m_messageBuffer);
			
			if (numBytes == 0)
			{
				messageReceivedHandler(m_messageBuffer);
				numBytes = m_minAmountToRead;
				clearMsgBuf = true;
			}
		}
		catch(const std::exception& e)
		{
			numBytes = m_minAmountToRead;
			clearMsgBuf = true;
		}
	}
	
	if (clearMsgBuf)
	{
		m_messageBuffer.clear();
	}
	
	if (numBytes > 0)
	{
		AsyncReadFromSocket(numBytes);
	}	
}

void TcpConnection::SendMessageAsync(const asio_defs::char_vector& message)
{
	// TODO:
}
					   
bool TcpConnection::SendMessageSync(const asio_defs::char_vector& message)
{
	// TODO:
	return false;
}

} // namespace tcp_conn
} // namespace core_lib
 
 