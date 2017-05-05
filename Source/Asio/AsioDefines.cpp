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
 * \file AsioDefines.cpp
 * \brief File containing useful definitions.
 */

#include "Asio/AsioDefines.h"
#include <cstdio>

namespace core_lib {
namespace asio {
namespace defs {

MessageHeader::MessageHeader()
{
    std::snprintf(responseAddress, sizeof(responseAddress), "%s", "0.0.0.0");
    std::snprintf(magicString, sizeof(magicString), "%s", DEFAULT_MAGIC_STRING);
}

#ifdef USE_EXPLICIT_MOVE_
    MessageHeader::MessageHeader(MessageHeader&& header)
    {
        std::snprintf(responseAddress, sizeof(responseAddress), "%s", "0.0.0.0");
        std::snprintf(magicString, sizeof(magicString), "%s", DEFAULT_MAGIC_STRING);
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
