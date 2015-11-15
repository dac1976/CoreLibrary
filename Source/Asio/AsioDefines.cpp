// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014,2015 Duncan Crutchley
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
 * \file AsioDefines.cpp
 * \brief File containing useful definitions.
 */

#include "Asio/AsioDefines.h"

namespace core_lib {
namespace asio {
namespace defs {

MessageHeader::MessageHeader()
{
	strncpy(responseAddress, "0.0.0.0", RESPONSE_ADDRESS_LEN);
	magicString[RESPONSE_ADDRESS_LEN - 1] = 0;

	strncpy(magicString, DEFAULT_MAGIC_STRING, MAGIC_STRING_LEN);
	magicString[MAGIC_STRING_LEN - 1] = 0;
}

#ifdef __USE_EXPLICIT_MOVE__
    MessageHeader::MessageHeader(MessageHeader&& header)
    {
		strncpy(responseAddress, "0.0.0.0", RESPONSE_ADDRESS_LEN);
		magicString[RESPONSE_ADDRESS_LEN - 1] = 0;

        strncpy(magicString, DEFAULT_MAGIC_STRING, MAGIC_STRING_LEN);
        magicString[MAGIC_STRING_LEN - 1] = 0;

        *this = std::move(header);
    }
    
	MessageHeader& MessageHeader::operator=(MessageHeader&& header)
    {
        std::swap_ranges(magicString, magicString + MAGIC_STRING_LEN, header.magicString);
        std::swap_ranges(responseAddress, responseAddress + RESPONSE_ADDRESS_LEN, header.responseAddress);
        std::swap(responsePort, header.responsePort);
        std::swap(messageId, header.messageId);
        std::swap(archiveType, header.archiveType);
        std::swap(totalLength, header.totalLength);
        return *this;
    }
#endif

} // namespace defs
} // namespace asio
} // namespace core_lib
