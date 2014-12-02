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
 * \brief File containing TCP connections class definition.
 */ 
 
#include "TcpConnections.hpp" 
#include "TcpConnection.hpp" 

namespace core_lib {
namespace tcp_conn{

// ****************************************************************************
// 'class TcpConnections' definition
// ****************************************************************************

TcpConnections::TcpConnections()
	: m_SyncSendResult(false)
{
}

void TcpConnections::Add(tcp_connection_ptr Connection)
{	
	boost::lock_guard<boost::mutex> lock(m_Mutex);
	m_Connections.insert(Connection); 
}

void TcpConnections::Remove(tcp_connection_ptr Connection) 
{ 
	boost::lock_guard<boost::mutex> lock(m_Mutex);
	m_Connections.erase(Connection); 
}

void TcpConnections::CloseConnections()
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);
	tcp_connection_set::iterator iter = m_Connections.begin();

	while((iter != m_Connections.end()))
	{
		(*iter++)->CloseConnection();
	}

	m_Connections.clear();
}

size_t TcpConnections::NumberOfConnections() const
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);
	return m_Connections.size();
}

void TcpConnections::SetSendResult(bool SendResult)
{
	m_SyncSendResult = SendResult;
}

bool TcpConnections::SendMessageTo(const string& TargetIP,
								   unsigned short TargetPort,
								   const shared_data_byte_vector& MessageToSend,
								   size_t NumBytesToSend,
								   bool QueuedSend)
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);
	tcp_connection_set::iterator iter = m_Connections.begin();
	bool FoundConnection = false;

	while(!FoundConnection &&
		  (iter != m_Connections.end()))
	{
		if (((*iter)->Socket().remote_endpoint().address().to_string() == TargetIP) &&
			((*iter)->Socket().remote_endpoint().port() == TargetPort))
		{
			if (QueuedSend)
			{
				m_SyncSendResult = true;
				(*iter)->SendMessageToQueued(MessageToSend,
											 NumBytesToSend);
			}
			else
			{
				m_SyncSendResult = false;
				(*iter)->SendMessageTo(MessageToSend,
									   NumBytesToSend);
			}
			FoundConnection = true;
		}
		++iter;
	}

	return (FoundConnection && m_SyncSendResult);
}

void TcpConnections::SendMessageToAll(const messages::shared_data_byte_vector& MessageToSend,
                                      const size_t NumBytesToSend)
{
    boost::lock_guard<boost::mutex> lock(m_Mutex);
    tcp_connection_set::iterator iter = m_Connections.begin();
    while (iter != m_Connections.end())
    {
        (*iter)->SendMessageTo(MessageToSend,NumBytesToSend);
        iter++;
    }
}

string TcpConnections::GetLocalIPForRemoteConnection(const string& TargetIP, 
													 unsigned short TargetPort) const
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);
	string LocalIP;
	bool FoundConnection = false;
	tcp_connection_set::iterator iter = m_Connections.begin();	

	while(!FoundConnection &&
		  (iter != m_Connections.end()))
	{
		if (((*iter)->Socket().remote_endpoint().address().to_string() == TargetIP) &&
			((*iter)->Socket().remote_endpoint().port() == TargetPort))
		{
			LocalIP = (*iter)->Socket().local_endpoint().address().to_string();
			FoundConnection = true;
		}
		++iter;
	}

	return LocalIP;
}

unsigned short TcpConnections::GetLocalPortForRemoteConnection(const string& TargetIP, 
															   unsigned short TargetPort) const
{
	boost::lock_guard<boost::mutex> lock(m_Mutex);
	unsigned short LocalPort;
	bool FoundConnection = false;
	tcp_connection_set::iterator iter = m_Connections.begin();	

	while(!FoundConnection &&
		  (iter != m_Connections.end()))
	{
		if (((*iter)->Socket().remote_endpoint().address().to_string() == TargetIP) &&
			((*iter)->Socket().remote_endpoint().port() == TargetPort))
		{
			LocalPort = (*iter)->Socket().local_endpoint().port();
			FoundConnection = true;
		}
		++iter;
	}

	return LocalPort;
}

} // namespace tcp_conn
} // namespace core_lib
 
 