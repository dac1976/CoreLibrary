
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
 * \file TcpTypedClient.hpp
 * \brief File containing TCP typed client class declaration.
 */

#ifndef TCPTYPEDCLIENT_HPP
#define TCPTYPEDCLIENT_HPP

#include "TcpClient.hpp"
#include <iterator>
#include <algorithm>
#include <cstring>

/*! \brief The core_lib namespace. */
namespace core_lib {
/*! \brief The asio namespace. */
namespace asio {
/*! \brief The tcp namespace. */
namespace tcp {

class TcpTypedClient final
{
public:

    TcpTypedClient(boost_ioservice& ioService
			  , const defs::connection& server
			  , const defs::message_dispatcher& messageDispatcher
			  , const eSendOption sendOption = eSendOption::nagleOn);

	TcpTypedClient(const defs::connection& server
			  , const defs::message_dispatcher& messageHandler
			  , const eSendOption sendOption = eSendOption::nagleOn);

	~TcpTypedClient() = default;
	TcpTypedClient(const TcpTypedClient& ) = delete;
	TcpTypedClient& operator=(const TcpTypedClient& ) = delete;

    void CloseConnection();template<typename T>
	void SendMessageToServerAsync(T&& message, const uint32_t messageId
                                  , const defs::connection& responseAddress
                                  , const eArchiveType archive = eArchiveType::portableBinary)
    {
        defs::char_buffer message{BuildMessage(std::forward(message), messageId 
                                               , responseAddress, archive)};
        m_tcpClient.SendMessageToServerAsync(message);
    }

    template<typename T>
	bool SendMessageToServerSync(T&& message, const uint32_t messageId
                                 , const defs::connection& responseAddress
                                 , const eArchiveType archive = eArchiveType::portableBinary)
    {
        defs::char_buffer message{BuildMessage(std::forward(message), messageId 
                                               , responseAddress, archive)};
        return m_tcpClient.SendMessageToServerSync(message);
    }

    template<typename T>
	void SendMessageToServerAsync(T&& message, const uint32_t messageId
                                   , const eArchiveType archive 
                                        = eArchiveType::portableBinary)
    {
        auto responseAddress = m_tcpClient.GetClientDetailsForServer();
        SendMessageToServerAsync(std::forward(message), messageId, responseAddress, archive);
    }

    template<typename T>
	bool SendMessageToServerSync(T&& message, const uint32_t messageId 
                                 , const eArchiveType archive
                                   = eArchiveType::portableBinary)
    {
        auto responseAddress = m_tcpClient.GetClientDetailsForServer();
        return SendMessageToServerSync(std::forward(message), messageId, responseAddress, archive);
    }

private:
    defs::message_dispatcher m_messageDispatcher;
    TcpClient m_tcpClient;

    static void CheckMessage(const char_buffer& message);
    
    static size_t CheckBytesLeftToRead(const defs::char_buffer& message);

    static void MessageReceivedHandler(const defs::char_buffer& message);
    
    template<typename T>
	defs::char_buffer BuildMessage(T&& message, const uint32_t messageId 
                                  , const defs::connection& responseAddress
                                  , const eArchiveType archive)
    {
        messages::MessageHeader header;
        strncpy(header.responseAddress, responseAddress.first.c_str(), responseAddress.first.length());
        header.responseAddress[RESPONSE_ADDRESS_LEN - 1] = 0;
        header.responsePort = responseAddress.second;
        header.messageId = messageId;
        header.archiveType = archive;
        
        defs::char_buffer body;
        
        switch(archive)
        {
            case eArchiveType::text:
                body = ToCharVector<T, eos::text_oarchive>(message);
                break;
            case eArchiveType::binary:
                body = ToCharVector<T, eos::binary_oarchive>(message);
                break;
            case eArchiveType::xml:
                body = ToCharVector<T, eos::xml_oarchive>(message);
                break;
            case eArchiveType::portableBinary:
            default:
                body = ToCharVector<T, eos::portable_oarchive>(message);
                break;
        }
        
        header.totalLength += body.size();
        
        defs::char_buffer message;
        message.reserve(header.totalLength);
        
        const char* headerCharBuf = reinterpret_cast<const char*>(&header);
        std::copy(headerCharBuf, headerCharBuf + sizeof(header)
                  , std::back_inserter(message));
                  
        std::copy(body.begin(), body.end()
                  , std::back_inserter(message));
                  
        return message;
    }
};

} // namespace tcp
} // namespace asio
} // namespace core_lib

#endif // TCPTYPEDCLIENT_HPP
