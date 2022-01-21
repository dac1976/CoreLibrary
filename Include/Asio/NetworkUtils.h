#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include <boost/predef.h>

namespace core_lib
{

using ip_octets_t = std::vector<unsigned char>;

// Give an IP address, e.g. 192.168.1.1, get a vector of the octets,
// e.g. {192, 168, 1, 1}.
ip_octets_t OctetsFromIpAddress(std::string const& ipAddress);

// Give a vector of octets, e.g. {192, 168, 1, 1}, get an IP address,
// e.g. 192.168.1.1.
std::string IpAddressFromOctets(ip_octets_t const& octets);

// Check if an IP address is valid. Disallows multicast addresses.
bool IsValidIpAddress(std::string const& address);

// Check if a broadcast address is valid. Disallows multicast addresses.
// Can also be used to check if a subnet mask is valid.
bool IsValidBroadcastAddress(std::string const& address);

// Check if address is valid muticast group address.
// 224.0.0.0 to 239.255.255.255.
bool IsValidMulticastGroupAddress(std::string const& address);

// Given an IP address and subnet mask create the appropriat ebroadcast address, e.g. 160.50.100.76
// and 255.255.0.0 becomes 160.50.255.255.
std::string BuildBroadcastAddress(std::string const& address, std::string const& subnetMask);

bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string const& ipAddress,
                                              std::string const& netmask,
                                              std::string const& adapterAddress,
                                              std::string const& adapterNetmask);
											  
std::string ConvertToCIDRAddress(std::string const& ipAddress, std::string const& netmask);											  
											  
// This functions require root access in Linux and Administartor access in Windows.
#if BOOST_OS_LINUX
// On Linux we allow for possibility of multi-homing a NIC.
// To set primary address to DHCP specify it as "dhcp" or "DHCP" , it's case insensitive
// and leave the netmask blank. The secondary address and netmask can only be static.
bool SetupNetwork(std::string const& ipAddress, std::string const& netmask,
                  std::string const& adapterName, std::string const& ipAddress2 = "",
                  std::string const& netmask2 = "");

std::vector<std::pair<std::string, std::string>>
GetAllIpAddressesAndNetmasks(std::string const& adapterName);

bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string const& ipAddress,
                                              std::string const& netmask,
                                              std::string const& adapterName);

std::string GetMacAddressForAdapter(std::string const& adapterName);
#else
bool SetupNetwork(std::string const& ipAddress, std::string const& netmask,
                  std::string const& adapterName);
#endif

// This functions require root access in Linux and Administartor access in Windows.
std::pair<std::string, std::string> GetIpAddressAndNetmask(std::string const& adapterName);

} // namespace core_lib

#endif // HGLQTNETWORKUTILS_H
