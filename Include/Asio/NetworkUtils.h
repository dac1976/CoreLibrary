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
 * \file NetworkUtils.h
 * \brief File containing declaration of network utilities.
 */
#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include "Platform/PlatformDefines.h"
#if defined(IS_CPP17)
#include <string_view>
#endif
#include "CoreLibraryDllGlobal.h"

namespace core_lib
{

using ip_octets_t = std::vector<uint8_t>;

// Give an IP address, e.g. 192.168.1.1, get a vector of the octets,
// e.g. {192, 168, 1, 1}.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API ip_octets_t OctetsFromIpAddress(std::string_view ipAddress);
#else
CORE_LIBRARY_DLL_SHARED_API ip_octets_t OctetsFromIpAddress(std::string const& ipAddress);
#endif

// Give an IP address, e.g. 192.168.1.1, get a uint32_t
// representation of the address, u32 from the addresses octets o[4]
// as follows:
// u32 = (o[0] << 24) + (o[1] << 16) + (o[2] << 9) + o[3]
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API uint32_t Uint32FromIpAddress(std::string_view ipAddress);
#else
CORE_LIBRARY_DLL_SHARED_API uint32_t Uint32FromIpAddress(std::string const& ipAddress);
#endif

// Give a vector of octets, e.g. {192, 168, 1, 1}, get an IP address,
// e.g. 192.168.1.1.
CORE_LIBRARY_DLL_SHARED_API std::string IpAddressFromOctets(ip_octets_t const& octets);

// Given a uint32_t representing an IP address convert it
// to the corresponding octets then convert to a string.
CORE_LIBRARY_DLL_SHARED_API std::string IpAddressFromUint32(uint32_t ipValue);

// Check if an IP address is valid. Disallows multicast addresses.
// 1.0.0.0 to 255.255.255.254 but excluding multicast
// range, 224.0.0.0 to 239.255.255.255, an 0.0.0.0 and
// 255.255.255.255.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API bool IsValidIpAddress(std::string_view address);
#else
CORE_LIBRARY_DLL_SHARED_API bool IsValidIpAddress(std::string const& address);
#endif

// Check to see if the address is a valid subnet mask.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API bool IsValidSubnetMask(std::string_view subnetMask);
#else
bCORE_LIBRARY_DLL_SHARED_API ool IsValidSubnetMask(std::string const& subnetMask);
#endif

// Check if a broadcast address is valid. Disallows multicast addresses.
// Can also be used to check if a subnet mask is valid.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API bool IsValidBroadcastAddress(std::string_view address);
#else
CORE_LIBRARY_DLL_SHARED_API bool IsValidBroadcastAddress(std::string const& address);
#endif

// Check if address is valid multicast group address.
// 224.0.0.0 to 239.255.255.255.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API bool IsValidMulticastGroupAddress(std::string_view address);
#else
CORE_LIBRARY_DLL_SHARED_API bool IsValidMulticastGroupAddress(std::string const& address);
#endif

// Convert a CIDR prefix value e.g. 16, 24 etc.
// to the subnet mask, e.g. 255.255.0.0, 255.255.255.0 etc.
//
// Returns empty string is there is no valid conversion.
CORE_LIBRARY_DLL_SHARED_API std::string CidrPrefixToSubnetMask(int32_t prefix);

// Convert a subnet mask, e.g. 255.255.0.0 to
// a CIDR prefix e.g. 16.
//
// Returns -1 if there is no valid conversion.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API int32_t SubnetMaskToCidrPrefix(std::string_view subnetMask);
#else
CORE_LIBRARY_DLL_SHARED_API int32_t SubnetMaskToCidrPrefix(std::string const& subnetMask);
#endif

// Create a CIDR address from an IP and subnet mask.
// e.g. 192.168.10.1/255.255.0.0 becomes
// 192.168.10.1/16
//
// Throws std::invalid_argument upon error.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API std::string MakeCidrAddress(std::string_view address, std::string_view subnetMask);
#else
CORE_LIBRARY_DLL_SHARED_API std::string MakeCidrAddress(std::string const& address, std::string const& subnetMask);
#endif

// Given an IP address and subnet mask create the appropriate broadcast address, e.g. 160.50.100.76
// and 255.255.0.0 becomes 160.50.255.255.
//
// Throws if an error occurs, std::invalid_argument
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API std::string BuildBroadcastAddress(std::string_view address, std::string_view subnetMask);
#else
CORE_LIBRARY_DLL_SHARED_API std::string BuildBroadcastAddress(std::string const& address, std::string const& subnetMask);
#endif

// Check if 2 addresses (with their subnet masks) are on the same subnet. This is done by checking if the broadcast address
// for the address and subnet mask is the same as the broadcast address for the adapter address and adapter subnet mask.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string_view ipAddress, std::string_view netmask,
                                              std::string_view adapterAddress,
                                              std::string_view adapterNetmask);
#else
CORE_LIBRARY_DLL_SHARED_API bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string const& ipAddress,
                                              std::string const& netmask,
                                              std::string const& adapterAddress,
                                              std::string const& adapterNetmask);
#endif

// Function to look up the IP address and subnet mask for a given adapter name. The adapter name is the friendly name
// of the adapter, e.g. "Ethernet", "Wi-Fi" etc.
#if defined(IS_CPP17)
CORE_LIBRARY_DLL_SHARED_API std::pair<std::string, std::string> GetIpAddressAndNetmask(std::string_view adapterName);
#else
CORE_LIBRARY_DLL_SHARED_API std::pair<std::string, std::string> GetIpAddressAndNetmask(std::string const& adapterName);
#endif


} // namespace core_lib

#endif // HGLQTNETWORKUTILS_H
