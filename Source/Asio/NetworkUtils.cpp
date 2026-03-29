#include "Asio/NetworkUtils.h"
#include <sstream>
#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <limits>
#include <bitset>
#include <unordered_map>
#if BOOST_OS_LINUX
#include <system_error>      // For std::error_code.
#include <cstring>           // For calls in GetIpAddressAndNetmask
#include <unistd.h>          // For calls in GetIpAddressAndNetmask
#include <sys/types.h>       // For calls in GetIpAddressAndNetmask
#include <sys/socket.h>      // For calls in GetIpAddressAndNetmask
#include <sys/ioctl.h>       // For calls in GetIpAddressAndNetmask
#include <netinet/in.h>      // For calls in GetIpAddressAndNetmask
#include <net/if.h>          // For calls in GetIpAddressAndNetmask
#include <arpa/inet.h>       // For calls in GetIpAddressAndNetmask
#include <netdb.h>           // For calls in GetIpAddressAndNetmask
#include <ifaddrs.h>         // For calls in GetIpAddressAndNetmask
#else
#include <winsock2.h>  // For calls in GetIpAddressAndNetmask
#include <iphlpapi.h>  // For calls in GetIpAddressAndNetmask
#include <shellapi.h>  // For calls in GetIpAddressAndNetmask
#include <ws2tcpip.h>  // For calls in GetIpAddressAndNetmask
#endif
#include "StringUtils/StringUtils.h"
#if !BOOST_OS_LINUX
// Windows needs the library: iphlpapi, linking to the build
#define MY_MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define MY_FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#endif

namespace core_lib
{
namespace net_utils
{

#if defined(IS_CPP17)
ip_octets_t OctetsFromIpAddress(std::string_view ipAddress)
#else
ip_octets_t OctetsFromIpAddress(std::string const& ipAddress)
#endif
{
    int                octet1, octet2, octet3, octet4;
    char               dot;
    // input stream that now contains the ip address string
    std::stringstream iss;
    iss << ipAddress;

    iss >> octet1 >> dot >> octet2 >> dot >> octet3 >> dot >> octet4;

    if ((octet1 < 0) || (octet1 > 255) || (octet2 < 0) || (octet2 > 255) || (octet3 < 0) ||
        (octet3 > 255) || (octet4 < 0) || (octet4 > 255))
    {
        return {};
    }

    return {static_cast<unsigned char>(octet1),
            static_cast<unsigned char>(octet2),
            static_cast<unsigned char>(octet3),
            static_cast<unsigned char>(octet4)};
}

#if defined(IS_CPP17)
uint32_t Uint32FromIpAddress(std::string_view ipAddress)
#else
uint32_t Uint32FromIpAddress(std::string const& ipAddress)
#endif
{
    auto octets = OctetsFromIpAddress(ipAddress);

    if (octets.size() != 4)
    {
        return 0;
    }

    uint32_t ipAsUint32 =
        (static_cast<uint32_t>(octets[0]) << 24) + (static_cast<uint32_t>(octets[1]) << 16) +
        (static_cast<uint32_t>(octets[2]) << 8) + static_cast<uint32_t>(octets[3]);

    return ipAsUint32;
}

std::string IpAddressFromOctets(ip_octets_t const& octets)
{
    if (octets.size() != 4)
    {
        return "";
    }

    std::ostringstream oss;
    oss << static_cast<int32_t>(octets[0]) << "." << static_cast<int32_t>(octets[1]) << "."
        << static_cast<int32_t>(octets[2]) << "." << static_cast<int32_t>(octets[3]);
    return oss.str();
}

std::string IpAddressFromUint32(uint32_t ipValue)
{
    ip_octets_t octets{static_cast<uint8_t>((ipValue & 0xFF000000) >> 24),
                       static_cast<uint8_t>((ipValue & 0x00FF0000) >> 16),
                       static_cast<uint8_t>((ipValue & 0x0000FF00) >> 8),
                       static_cast<uint8_t>(ipValue & 0x000000FF)};

    return IpAddressFromOctets(octets);
}

#if defined(IS_CPP17)
bool IsValidIpAddress(std::string_view address)
#else
bool IsValidIpAddress(std::string const& address)
#endif
{
    auto ip = Uint32FromIpAddress(address);

    // This exploits the fact that all valid multicast addresses start
    // with 0xE-------, e.g. 224.0.0.0 to 239.255.255.255.
    return (0xE0000000 != (0xF0000000 & ip)) && (0 != ip) &&
           (std::numeric_limits<uint32_t>::max() != ip);
}

#if defined(IS_CPP17)
bool IsValidSubnetMask(std::string_view subnetMask)
#else
bool IsValidSubnetMask(std::string const& subnetMask)
#endif
{
    auto mask = Uint32FromIpAddress(subnetMask);

    // If we are 0.0.0.0 or 255.255.255.255
    // then these are valid subnet masks.
    if ((0 == mask) || (std::numeric_limits<uint32_t>::max() == mask))
    {
        return true;
    }

    auto maskBits    = std::bitset<32>(mask);
    auto numOnes     = static_cast<int32_t>(maskBits.count());
    auto compNetmask = CidrPrefixToSubnetMask(numOnes);

    if (compNetmask.empty())
    {
        return false;
    }

    return (compNetmask == subnetMask) && (0x80000000 == (0x80000000 & mask));
}

#if defined(IS_CPP17)
bool IsValidBroadcastAddress(std::string_view address)
#else
bool IsValidBroadcastAddress(std::string const& address)
#endif
{
    auto ip = Uint32FromIpAddress(address);

    // If we are anything in the range 1.0.0.0 to 255.255.255.255
    // and not in the multicast range 224.0.0.0 to 239.255.255.255
    // then we are a valid broadcast address.
    return (0xE0000000 != (0xF0000000 & ip)) && (0 != ip);
}

#if defined(IS_CPP17)
bool IsValidMulticastGroupAddress(std::string_view address)
#else
bool IsValidMulticastGroupAddress(std::string const& address)
#endif
{
    auto ip = Uint32FromIpAddress(address);

    // This exploits the fact that all valid multicast addresses start
    // with 0xE-------, e.g. 224.0.0.0 to 239.255.255.255.
    return 0xE0000000 == (0xF0000000 & ip);
}

std::string CidrPrefixToSubnetMask(int32_t prefix)
{
    if ((prefix < 0) || (prefix > 32))
    {
        return "";
    }

    uint32_t subnetBits = 0;

    if (prefix > 0)
    {
        for (int32_t n = 0; n < prefix; ++n)
        {
            subnetBits += (1 << n);
        }

        subnetBits = (subnetBits << (32 - prefix));
    }

    return IpAddressFromUint32(subnetBits);
}

#if defined(IS_CPP17)
int32_t SubnetMaskToCidrPrefix(std::string_view subnetMask)
#else
int32_t SubnetMaskToCidrPrefix(std::string const& subnetMask)
#endif
{
    if (!IsValidSubnetMask(subnetMask))
    {
        return -1;
    }

    auto mask     = Uint32FromIpAddress(subnetMask);
    auto maskBits = std::bitset<32>(mask);

    return static_cast<int32_t>(maskBits.count());
}

uint32_t PrefixToMaskNetworkOrder(uint8_t prefixLength)
{
    if (prefixLength == 0)
    {
        return 0;
    }

    if (prefixLength >= 32)
    {
        return 0xFFFFFFFFu;
    }

    return htonl(0xFFFFFFFFu << (32 - prefixLength));
}


#if defined(IS_CPP17)
std::string MakeCidrAddress(std::string_view address, std::string_view subnetMask)
#else
std::string MakeCidrAddress(std::string const& address, std::string const& subnetMask)
#endif
{
    if (!IsValidIpAddress(address))
    {
        throw std::invalid_argument("invalid ip address");
    }

    if (!IsValidSubnetMask(subnetMask))
    {
        throw std::invalid_argument("invalid subnetMask");
    }

    auto prefix = SubnetMaskToCidrPrefix(subnetMask);

    std::ostringstream ossCidrAddress;
    ossCidrAddress << address << "/" << prefix;

    return ossCidrAddress.str();
}

#if defined(IS_CPP17)
std::string BuildBroadcastAddress(std::string_view address, std::string_view subnetMask)
#else
std::string BuildBroadcastAddress(std::string const& address, std::string const& subnetMask)
#endif
{
    if (!IsValidIpAddress(address))
    {
        throw std::invalid_argument("invalid ip address");
    }

    if (!IsValidSubnetMask(subnetMask))
    {
        throw std::invalid_argument("invalid subnetMask");
    }

    auto ip         = Uint32FromIpAddress(address);
    auto mask       = Uint32FromIpAddress(subnetMask);
    auto bcastAsU32 = (ip | ~mask);

    return IpAddressFromUint32(bcastAsU32);
}

#if defined(IS_CPP17)
bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string_view ipAddress, std::string_view netmask,
                                              std::string_view adapterAddress,
                                              std::string_view adapterNetmask)
#else
bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string const& ipAddress,
                                              std::string const& netmask,
                                              std::string const& adapterAddress,
                                              std::string const& adapterNetmask)
#endif
{
    if (adapterNetmask.empty())
    {
        return false;
    }
    else
    {
        try
        {
            auto tempNetMask       = netmask.empty() ? adapterNetmask : netmask;
            auto broadcastAddress1 = BuildBroadcastAddress(ipAddress, tempNetMask);
            auto broadcastAddress2 = BuildBroadcastAddress(adapterAddress, adapterNetmask);

            return broadcastAddress1 == broadcastAddress2;
        }
        catch (...)
        {
            return false;
        }
    }
}

#if defined(IS_CPP17)
std::pair<std::string, std::string> GetIpAddressAndNetmask(std::string_view adapterName)
#else
std::pair<std::string, std::string> GetIpAddressAndNetmask(std::string const& adapterName)
#endif
#if BOOST_OS_LINUX
{
    // Create a temporary socket so we can grab our adapter details.
    auto fd = socket(AF_INET, SOCK_DGRAM, 0);

    // We want to get an IPv4 IP address.
    ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;

    // Set the adapter we want to find info for.
    memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    string_utils::SafeCopyCharArray(ifr.ifr_name, IFNAMSIZ, adapterName.data(), adapterName.size());

    // Use icotl to retrieve IP address.
    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
    {
        // Close the file descriptor.
        close(fd);
        throw std::runtime_error("ioctl error for SIOCGIFADDR");
    }

    std::pair<std::string, std::string> netInfo;
    netInfo.first = inet_ntoa((reinterpret_cast<sockaddr_in*>(&ifr.ifr_addr))->sin_addr);

    // Use ioctl to retrieve subnet mask.
    if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0)
    {
        // Close the file descriptor.
        close(fd);
        throw std::runtime_error("ioctl error for SIOCGIFNETMASK");
    }

    // Close the file descriptor.
    close(fd);

    netInfo.second = inet_ntoa((reinterpret_cast<sockaddr_in*>(&ifr.ifr_netmask))->sin_addr);

    return netInfo;
}
#else
{
    ULONG ulOutBufLen = static_cast<ULONG>(sizeof(IP_ADAPTER_ADDRESSES));
    auto  pAddresses  = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(MY_MALLOC(ulOutBufLen));

    if (nullptr == pAddresses)
    {
        throw std::runtime_error("failed to allocate adapter addresses");
    }

    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    if (ERROR_BUFFER_OVERFLOW ==
        GetAdaptersAddresses(AF_INET, flags, NULL, pAddresses, &ulOutBufLen))
    {
        MY_FREE(pAddresses);

        pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(MY_MALLOC(ulOutBufLen));

        if (nullptr == pAddresses)
        {
            throw std::runtime_error("failed to allocate adapter addresses to adjusted size");
        }
    }

    if (NO_ERROR != GetAdaptersAddresses(AF_INET, flags, NULL, pAddresses, &ulOutBufLen))
    {
        throw std::runtime_error("failed to get adapter addresses");
    }

    std::pair<std::string, std::string> netInfo;

    auto pCurrAddresses = pAddresses;

    while (nullptr != pCurrAddresses)
    {
        if (adapterName == string_utils::WStringToString(pCurrAddresses->FriendlyName))
        {
            char  temp[64];
            DWORD len = 64;

            if (0 ==
                WSAAddressToStringA(pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr,
								pCurrAddresses->FirstUnicastAddress->Address.iSockaddrLength,
								nullptr,
								temp,
								&len))
            {
                netInfo.first = temp;
            }

            ULONG mask;

            if (NO_ERROR == ConvertLengthToIpv4Mask(
                                pCurrAddresses->FirstUnicastAddress->OnLinkPrefixLength, &mask))
            {
                auto result = inet_ntop(AF_INET, &mask, temp, sizeof(temp));

                if (nullptr != result)
                {
                    netInfo.second = temp;
                }
            }

            break;
        }

        pCurrAddresses = pCurrAddresses->Next;
    }

    if (nullptr != pAddresses)
    {
        MY_FREE(pAddresses);
    }

    if (netInfo.first.empty() || netInfo.second.empty())
    {
        throw std::runtime_error("failed to find network adapter");
    }

    return netInfo;
}
#endif

bool IsBadIPv4HostOrder(uint32_t hostIp)
{
    if (hostIp == 0)
        return true;

    if ((hostIp & 0xFF000000u) == 0x7F000000u)
    {
        return true;
    }

    if ((hostIp & 0xFFFF0000u) == 0xA9FE0000u)
    {
        return true;
    }

    return false;
}

std::string IPv4ToString(const in_addr& address)
{
    char buffer[INET_ADDRSTRLEN]{};

    if (!inet_ntop(AF_INET, &address, buffer, sizeof(buffer)))
    {
        return {};
    }

    return buffer;
}

#if BOOST_OS_LINUX
bool IsInterfaceUp(const ifaddrs* iface)
{
    return iface && (iface->ifa_flags & IFF_UP);
}

bool SupportsBroadcast(const ifaddrs* iface)
{
    return iface && (iface->ifa_flags & IFF_BROADCAST);
}

bool SupportsMulticast(const ifaddrs* iface)
{
    return iface && (iface->ifa_flags & IFF_MULTICAST);
}

std::vector<IPv4Adapter> GetIPv4Adapters(eInterfaceFilter filter)
{
    std::vector<IPv4Adapter> adapters;
    std::unordered_map<std::string, size_t> adapterIndex;

    ifaddrs* interfaceList = nullptr;

    if (getifaddrs(&interfaceList) != 0 || !interfaceList)
    {
        return adapters;
    }

    for (auto* iface = interfaceList; iface; iface = iface->ifa_next)
    {
        if (!iface->ifa_name)
        {
            continue;
        }

        if (!iface->ifa_addr || !iface->ifa_netmask)
        {
            continue;
        }

        if (iface->ifa_addr->sa_family != AF_INET)
        {
            continue;
        }

        if (filter == eInterfaceFilter::RealOnly && !IsInterfaceUp(iface))
        {
            continue;
        }

        auto* address = reinterpret_cast<sockaddr_in*>(iface->ifa_addr);
        auto* mask = reinterpret_cast<sockaddr_in*>(iface->ifa_netmask);

        uint32_t hostIp = ntohl(address->sin_addr.s_addr);

        if (filter == eInterfaceFilter::RealOnly && IsBadIPv4HostOrder(hostIp))
        {
            continue;
        }

        std::string ipString = IPv4ToString(address->sin_addr);
        std::string maskString = IPv4ToString(mask->sin_addr);

        if (ipString.empty() || maskString.empty())
        {
            continue;
        }

        std::string adapterName = iface->ifa_name;
        size_t index;

        auto it = adapterIndex.find(adapterName);

        if (it == adapterIndex.end())
        {
            index = adapters.size();
            adapterIndex.emplace(adapterName, index);

            IPv4Adapter record;
            record.name = adapterName;
            record.supportsBroadcast = SupportsBroadcast(iface);
            record.supportsMulticast = SupportsMulticast(iface);

            adapters.push_back(std::move(record));
        }
        else
        {
            index = it->second;

            adapters[index].supportsBroadcast =
                adapters[index].supportsBroadcast || SupportsBroadcast(iface);

            adapters[index].supportsMulticast =
                adapters[index].supportsMulticast || SupportsMulticast(iface);
        }

        adapters[index].addresses.push_back({ipString, maskString});
    }

    freeifaddrs(interfaceList);

    adapters.erase(
        std::remove_if(
            adapters.begin(),
            adapters.end(),
            [](const IPv4Adapter& adapter)
            {
                return adapter.addresses.empty();
            }),
        adapters.end());

    return adapters;
}
#else
bool IsAdapterUp(const IP_ADAPTER_ADDRESSES* adapter)
{
    return adapter && adapter->OperStatus == IfOperStatusUp;
}

inline bool SupportsMulticast(const IP_ADAPTER_ADDRESSES* adapter)
{
    return adapter && ((adapter->Flags & IP_ADAPTER_NO_MULTICAST) == 0);
}

std::vector<IPv4Adapter> GetIPv4Adapters(eInterfaceFilter filter)
{
    std::vector<IPv4Adapter> adapters;

    ULONG bufferSize = 0;

    GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &bufferSize);

    if (bufferSize == 0)
    {
        return adapters;
    }

    std::vector<unsigned char> buffer(bufferSize);

    auto* adapterList = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());

    if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapterList, &bufferSize) != NO_ERROR)
    {
        return adapters;
    }

    for (auto* adapter = adapterList; adapter; adapter = adapter->Next)
    {
        if ((filter == eInterfaceFilter::RealOnly) && !IsAdapterUp(adapter))
        {
            continue;
        }

        IPv4Adapter record;

        record.name = string_utils::WStringToString(adapter->FriendlyName);

        if (record.name.empty())
        {
            continue;
        }

        record.supportsMulticast = SupportsMulticast(adapter);

        for (auto* address = adapter->FirstUnicastAddress; address; address = address->Next)
        {
            auto* socketAddress = address->Address.lpSockaddr;

            if (!socketAddress || (socketAddress->sa_family != AF_INET))
            {
                continue;
            }

            auto* ipv4 = reinterpret_cast<sockaddr_in*>(socketAddress);

            uint32_t hostIp = ntohl(ipv4->sin_addr.s_addr);

            if ((filter == eInterfaceFilter::RealOnly) && IsBadIPv4HostOrder(hostIp))
            {
                continue;
            }

            std::string ipString = IPv4ToString(ipv4->sin_addr);

            if (ipString.empty())
            {
                continue;
            }

            uint32_t maskNetwork = PrefixToMaskNetworkOrder(address->OnLinkPrefixLength);

            in_addr maskAddr{};
            maskAddr.s_addr = maskNetwork;

            std::string maskString = IPv4ToString(maskAddr);

            if (maskString.empty())
            {
                continue;
            }

            record.addresses.push_back({ipString, maskString});
        }

        if (record.addresses.empty())
        {
            continue;
        }

        // Broadcast policy — matches your KM loopback usage
        record.supportsBroadcast = true;

        adapters.emplace_back(std::move(record));
    }

    return adapters;
}
#endif // #if BOOST_OS_LINUX

} // namespace net_utils
} // namespace core_lib
