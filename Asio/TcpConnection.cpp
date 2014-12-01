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
 * \brief File containing TCP connection classes' definitions.
 */ 
 
 #include "TcpConnection.hpp" 
 #include <exception>

namespace core_lib {
namespace tcp_conn{
 
// ****************************************************************************
// 'class TcpConnection' definition
// ****************************************************************************
	
TcpConnection::TcpConnection(boost::asio::io_service& ioService
							 , TcpConnections& connections
							 , const size_t minAmountToRead
							 , const check_bytes_left_to_read& checkBytesLeftToRead
							 , const message_received_handler& messageReceivedHandler
							 , const eSendOption sendImmediately)
	: m_closing{false}
	, m_ioService{ioService}
	, m_connections{connections}
	, m_minAmountToRead{minAmountToRead}
	, m_checkBytesLeftToRead{checkBytesLeftToRead}
	, m_messageReceivedHandler{messageReceivedHandler}
	, m_sendImmediately{sendImmediately}	
{
}

void TcpConnection::CloseConnection()
{
	if (m_socket.is_open())
	{
		return;
	}
	
	SetClosing(true);
	
	m_ioService.post(std::bind(&TcpConnection::ProcessCloseSocket
	                           , shared_from_this()));
	
	m_closedEvent.Wait();

}

void TcpConnection::SetClosing(const bool closing)
{
	std::lock_guard<std::mutex> lock{m_closingMutex};
	m_closing = Closing;
}

bool TcpConnection::IsClosing() const
{
	std::lock_guard<std::mutex> lock{m_closingMutex};
	return m_closing;
}

void TcpConnection::ProcessCloseSocket()
{
	m_socket.close();
	m_closedEvent.Signal();
}

void TcpConnection::Connect(const boost_tcp::endpoint& tcpEndpoint)
{
	m_Socket.connect(TCPEndpoint);
	
	boost_tcp::no_delay Option(m_SendImmediately);
	m_Socket.set_option(Option);
	
	StartAsyncRead();
}

void TcpConnection::StartAsyncRead()
{
	m_connections.Add(shared_from_this());
	
	StartAsyncReadFromSocket(m_minAmountToRead);
}

void TcpConnection::StartAsyncReadFromSocket(const size_t amountToRead)
{
	ResizeReceiveBuffer(m_minAmountToRead);

	boost::asio::async_read(m_socket,
							boost::asio::buffer(*m_receiveBuffer, m_minAmountToRead),
							std::bind(&TcpConnection::ReadSomeData
									  , shared_from_this()
									  , boost::asio::placeholders::error
									  , boost::asio::placeholders::bytes_transferred
									  , m_minAmountToRead));
}

void TcpConnection::ResizeReceiveBuffer(const size_t newSize)
{
	if (m_receiveBuffer.size() < newSize)
	{
		m_receiveBuffer.resize(newSize, 0);
	}
}

void TcpConnection::ReadSomeData(const boost::system::error_code& error
								 , const size_t bytesReceived
								 , const size_t bytesExpected)
{
	bool restartReading = false;
	size_t amountToRead = m_minAmountToRead;
	
	if (error || (bytesReceived != bytesExpected))
	{
		if (error)
		{
			DestroySelf();
		}
		else
		{
			restartReading = true;
		}
    }
	else
	{
		try
		{
			size_t bytesRemaining = checkBytesLeftToRead(&m_receiveBuffer.front()
			                                             , bytesReceived);
			
			if (bytesRemaining > 0)
			{
				ResizeMessageBuffer(bytesReceived + bytesRemaining);
				
				// TODO: Fill message buffer from received buffer then read some more data.
			}
			else
			{
				messageReceivedHandler(m_receiveBuffer);
				restartReading = true;
			}
			
			
			//decode message header and store it...
			m_MessageHeader = *m_ReadVec;
			//make sure message buffer is big enough to hold
			//entire message...
			ResizeMessageBuffer(m_MessageHeader.TotalMessageLength());
			//Accumulate message...
			FillMessageBufferFromReceiveBuffer(0, m_HeaderSize);
			//Do we need to read more data?
			if (m_MessageHeader.TotalMessageLength() > m_HeaderSize)
			{
				//read the rest of the message...
				StartAsyncReadBody(m_MessageHeader.TotalMessageLength());
			}
			else
			{
				//Need to call dispatch message callback as no more to read for this message...
				MessageDispatcher(m_MessageHeader, *m_MessageVec);
				//go back to reading next header...
				StartAsyncReadHeader();
			}
		}
		catch(const std::exception& e)
		{
			restartReading = true;;
		}
	}
	
	if (restartReading)
	{
		StartAsyncReadFromSocket(amountToRead);
	}	
}

void TcpConnection::ResizeMessageBuffer(const size_t newSize)
{
	if (m_messageBuffer.size() < newSize)
	{
		m_messageBuffer.resize(newSize, 0);
	}
}

// ****************************************************************************
// 'class TcpConnections' definition
// ****************************************************************************

} // namespace tcp_conn
} // namespace core_lib
 
 