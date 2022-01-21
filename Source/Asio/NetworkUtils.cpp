#include "NetworkUtils.h"
#include <sstream>
#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <sstream> // For std::ostringstream and std::istringstream.
#include <stdexcept>
#include <cmath>
#if __BORLANDC__
#include <boost/math/special_functions/round.hpp>
#endif

#if !defined(USING_QT) || defined(FORCE_USE_FULL_NET_ADMIN_TOOLS)
#include <thread>
#include <chrono>
#if BOOST_OS_LINUX
#include <iterator>
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
#include <fcntl.h>           // For calls in GetMacAddressForAdapter
#include <net/if_arp.h>      // For calls in GetMacAddressForAdapter
#include <boost/process.hpp> // For boost::process::system.
#include <boost/algorithm/string/case_conv.hpp>
#else
#include <winsock2.h>
#include <iphlpapi.h>
#include <shellapi.h>
#include <ws2tcpip.h>
#endif
#include "StringUtils.h"

#if BOOST_OS_LINUX
namespace bp = boost::process;
#else
// If using MinGW on Windows, say in Qt, then
// you must add the following to your project:
// LIBS += -liphlpapi
// Else the line below (#pragma) should link the
// library.
#if !defined(BOOST_COMP_GNUC)
#pragma comment(lib, "IPHLPAPI.lib")
#endif
#define MY_MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define MY_FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#endif
#endif

namespace hgl
{

ip_octets_t OctetsFromIpAddress(std::string const& ipAddress)
{
    int                octet1, octet2, octet3, octet4;
    char               dot;
    std::istringstream iss(ipAddress); // input stream that now contains the ip address string

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

std::string IpAddressFromOctets(ip_octets_t const& octets)
{
    if (octets.size() != 4)
    {
        return "";
    }

    std::ostringstream oss;
    oss << static_cast<int>(octets[0]) << "." << static_cast<int>(octets[1]) << "."
        << static_cast<int>(octets[2]) << "." << static_cast<int>(octets[3]);
    return oss.str();
}

bool IsValidIpAddress(std::string const& address)
{
    auto octets = OctetsFromIpAddress(address);

    // 1.0.0.0 to 255.255.255.254 but excluding multicast range.
    return !(octets.empty() || (octets[0] < 1) || (octets[3] > 254) ||
             ((octets[0] >= 224) && (octets[0] <= 239)));
}

bool IsValidBroadcastAddress(std::string const& address)
{
    auto octets = OctetsFromIpAddress(address);

    // 1.0.0.0 to 255.255.255.255 but excluding multicast range.
    return !(octets.empty() || (octets[0] < 1) || ((octets[0] >= 224) && (octets[0] <= 239)));
}

bool IsValidMulticastGroupAddress(std::string const& address)
{
    auto octets = OctetsFromIpAddress(address);

    // 224.0.0.0 to 239.255.255.255.
    return !(octets.empty() || (octets[0] < 224) || (octets[0] > 239));
}

std::string BuildBroadcastAddress(std::string const& address, std::string const& subnetMask)
{
    std::string broadcastAddress;
    uint32_t    ipOctets[4];
    uint32_t    subnetOctets[4];

    if ((std::sscanf(address.c_str(),
                     "%u.%u.%u.%u",
                     &ipOctets[0],
                     &ipOctets[1],
                     &ipOctets[2],
                     &ipOctets[3]) == 4) &&
        (std::sscanf(subnetMask.c_str(),
                     "%u.%u.%u.%u",
                     &subnetOctets[0],
                     &subnetOctets[1],
                     &subnetOctets[2],
                     &subnetOctets[3]) == 4))
    {
        if (std::any_of(
                std::begin(ipOctets), std::end(ipOctets), [](uint32_t o) { return o > 255; }))
        {
            throw std::runtime_error("IP address octets must be between 0 and 255.");
        }

        if (std::any_of(std::begin(subnetOctets), std::end(subnetOctets), [](uint32_t o) {
                return o > 255;
            }))
        {
            throw std::runtime_error("Subnet mask octets must be between 0 and 255.");
        }

        // check subnet is contigious...
        if (subnetOctets[0] == 0)
        {
            if ((subnetOctets[1] != 0) || (subnetOctets[2] != 0) || (subnetOctets[3] != 0))
            {
                throw std::runtime_error("Subnet must be contiguous.");
            }
        }
        else
        {
            if (subnetOctets[1] == 0)
            {
                if ((subnetOctets[2] != 0) || (subnetOctets[3] != 0))
                {
                    throw std::runtime_error("Subnet must be contiguous.");
                }
            }
            else
            {
                if (subnetOctets[2] == 0)
                {
                    if (subnetOctets[3] != 0)
                    {
                        throw std::runtime_error("Subnet must be contiguous.");
                    }
                }
            }
        }

        // build net address...
        auto octet = static_cast<uint8_t>(ipOctets[0]);
        auto temp  = static_cast<uint8_t>(subnetOctets[0]);
        octet |= ~temp;
        broadcastAddress = std::to_string(octet);

        broadcastAddress += ".";
        octet = static_cast<uint8_t>(ipOctets[1]);
        temp  = static_cast<uint8_t>(subnetOctets[1]);
        octet |= ~temp;
        broadcastAddress += std::to_string(octet);

        broadcastAddress += ".";
        octet = static_cast<uint8_t>(ipOctets[2]);
        temp  = static_cast<uint8_t>(subnetOctets[2]);
        octet |= ~temp;
        broadcastAddress += std::to_string(octet);

        broadcastAddress += ".";
        octet = static_cast<uint8_t>(ipOctets[3]);
        temp  = static_cast<uint8_t>(subnetOctets[3]);
        octet |= ~temp;
        broadcastAddress += std::to_string(octet);
    }

    return broadcastAddress;
}

bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string const& ipAddress,
                                              std::string const& netmask,
                                              std::string const& adapterAddress,
                                              std::string const& adapterNetmask)
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
            auto broadcastAddress1 = hgl::BuildBroadcastAddress(ipAddress, tempNetMask);
            auto broadcastAddress2 = hgl::BuildBroadcastAddress(adapterAddress, adapterNetmask);

            return broadcastAddress1 == broadcastAddress2;
        }
        catch (...)
        {
            return false;
        }
    }
}

std::string ConvertToCIDRAddress(std::string const& ipAddress, std::string const& netmask)
{
    if (!IsValidIpAddress(ipAddress))
    {
        throw std::invalid_argument("invalid IP address");
    }

    auto octets = OctetsFromIpAddress(netmask);

    if (octets.empty())
    {
        throw std::invalid_argument("invalid netmask");
    }

    auto netmaskValue = (static_cast<uint32_t>(octets[0]) << 24) +
                        (static_cast<uint32_t>(octets[1]) << 16) +
                        (static_cast<uint32_t>(octets[2]) << 8) + static_cast<uint32_t>(octets[3]);

    netmaskValue ^= 0xFFFFFFFF;
#if __BORLANDC__
    auto expo =
        static_cast<int>(boost::math::round(std::log2(static_cast<double>(netmaskValue))) + 0.5);
#else
    auto expo = static_cast<int>(std::round(std::log2(static_cast<double>(netmaskValue))) + 0.5);
#endif

    std::ostringstream ssCidrAddr;
    ssCidrAddr << ipAddress << "/" << 32 - expo;

    return ssCidrAddr.str();
}

#if BOOST_OS_LINUX
bool TakeDownAdapter(std::string const& adapterName)
{
    std::ostringstream cmdOss;
    cmdOss << "ip link set " << adapterName << " down";

    std::error_code ec;
    auto            returnCode = bp::system(cmdOss.str(), ec);

    if (ec)
    {
        return false;
    }

    return true;
}

bool BringUpAdapter(std::string const& adapterName)
{

    std::ostringstream cmdOss;
    cmdOss << "ip link set " << adapterName << " up";

    std::error_code ec;
    auto            returnCode = bp::system(cmdOss.str(), ec);

    if (ec)
    {
        oss.str("");
        return false;
    }

    return true;
}

bool FlushAddresses(std::string const& adapterName)
{
    std::ostringstream cmdOss;
    cmdOss << "ip address flush dev " << adapterName;

    std::error_code ec;
    auto            returnCode = bp::system(cmdOss.str(), ec);

    if (ec)
    {
        return false;
    }

    return true;
}
bool SetupNetwork(std::string const& ipAddress, std::string const& netmask,
                  std::string const& adapterName, std::string const& ipAddress2,
                  std::string const& netmask2)
{
    std::ostringstream oss;

    if (adapterName.empty())
    {
        return false;
    }

    bool retFlag        = true;
    bool bringUpAdapter = true;

    try
    {
        // First take down the adapter.
        if (!TakeDownAdapter(adapterName))
        {
            throw std::runtime_error("early return due to error");
        }

        // We want to change the adapter settings.
        if ("DHCP" == boost::to_upper_copy(ipAddress))
        {
            // FLush the current addresses.
            if (!FlushAddresses(adapterName))
            {
                throw std::runtime_error("early return due to error");
            }

            // Bring back up so we get a new DHCP address.
            if (!BringUpAdapter(adapterName))
            {
                throw std::runtime_error("early return due to error");
            }

            bringUpAdapter = false;

            // Give time for network adapter settings to change in OS.
           std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
        else
        {
            if (!IsValidIpAddress(ipAddress))
            {
                throw std::runtime_error("early return due to error");
            }

            if (netmask.empty())
            {
                throw std::runtime_error("early return due to error");
            }

            if (!FlushAddresses(adapterName))
            {
                throw std::runtime_error("early return due to error");
            }

            std::ostringstream cmdOss;

            try
            {
                cmdOss << "ip address add " << ConvertToCIDRAddress(ipAddress, netmask)
                       << " brd + dev " << adapterName;
            }
            catch (...)
            {
                throw std::runtime_error("early return due to error");
            }

            std::error_code ec;
            auto            returnCode = bp::system(cmdOss.str(), ec);

            if (ec)
            {
                throw std::runtime_error("early return due to error");
            }
        }

        // Bind secondary address to adapter.
        if (!ipAddress2.empty() && !netmask2.empty())
        {
            if (!IsValidIpAddress(ipAddress2))
            {
                throw std::runtime_error("early return due to error");
            }

            if (netmask2.empty())
            {
                throw std::runtime_error("early return due to error");
            }

            std::ostringstream cmdOss;

            try
            {
                cmdOss << "ip address add " << ConvertToCIDRAddress(ipAddress2, netmask2)
                       << " brd + dev " << adapterName;
            }
            catch (...)
            {
                throw std::runtime_error("early return due to error");
            }

            std::error_code ec;
            auto            returnCode = bp::system(cmdOss.str(), ec);

            if (ec)
            {
                throw std::runtime_error("early return due to error");
            }
        }
    }
    catch (...)
    {
        // We'll catch exceptions thrown within the
        // above code here to set return flag. Doing it this
        // way means we'll try to bring the adapter back up
        // again in most cases.
        retFlag = false;
    }

    if (bringUpAdapter)
    {
        if (!BringUpAdapter(adapterName))
        {
            return false;
        }

        // Short wait for adapter to come back up.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return retFlag;
}
#else
bool SetupNetwork(std::string const& ipAddress, std::string const& netmask,
                  std::string const& adapterName)
{
    std::ostringstream oss;

    if (adapterName.empty())
    {
        return false;
    }

    std::ostringstream cmdOss;
    std::string        args = "/C ";

    // We want to change the adapter settings using ifconfig and dhclient.
    if (("dhcp" == ipAddress) || ("DHCP" == ipAddress))
    {
        cmdOss << "netsh interface ip set address \"" << adapterName << "\" dhcp";
        args += cmdOss.str();
        HINSTANCE hi = ShellExecuteA(nullptr, "open", "cmd.exe", args.c_str(), "", SW_SHOWDEFAULT);
        auto      retCode = reinterpret_cast<INT_PTR>(hi);

        if (retCode <= 32)
        {
            return false;
        }
    }
    else
    {
        cmdOss << "netsh interface ip set address \"" << adapterName << "\" static " << ipAddress
               << " " << netmask;
        args += cmdOss.str();

        HINSTANCE hi = ShellExecuteA(nullptr, "open", "cmd.exe", args.c_str(), "", SW_SHOWDEFAULT);
        auto      retCode = reinterpret_cast<INT_PTR>(hi);

        if (retCode <= 32)
        {
            return false;
        }

        cmdOss.str("");
        cmdOss.clear();
        args = "/C ";
        cmdOss << "netsh interface ipv4 add dnsservers \"" << adapterName
               << "\" address=" << ipAddress << " index=1";
        args += cmdOss.str();

        hi      = ShellExecuteA(nullptr, "open", "cmd.exe", args.c_str(), "", SW_SHOWDEFAULT);
        retCode = reinterpret_cast<INT_PTR>(hi);

        if (retCode <= 32)
        {
            return false;
        }

        cmdOss.str("");
        cmdOss.clear();
        args = "/C ";
        cmdOss << "netsh interface ipv4 delete dnsservers \"" << adapterName << "\" address=all";
        args += cmdOss.str();

        hi      = ShellExecuteA(nullptr, "open", "cmd.exe", args.c_str(), "", SW_SHOWDEFAULT);
        retCode = reinterpret_cast<INT_PTR>(hi);

        if (retCode <= 32)
        {
            return false;
        }
    }

    // Give time for network adapter settings to change in OS.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return true;
}
#endif

std::pair<std::string, std::string> GetIpAddressAndNetmask(std::string const& adapterName)
#if BOOST_OS_LINUX
{
    // Create a temporary socket so we can grab our adapter details.
    auto fd = socket(AF_INET, SOCK_DGRAM, 0);

    // We want to get an IPv4 IP address.
    ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;

    // Set the adapter we want to find info for.
    memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    adapterName.copy(ifr.ifr_name, IFNAMSIZ - 1);

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
        if (adapterName == hgl::WStringToString(pCurrAddresses->FriendlyName))
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

#if BOOST_OS_LINUX
std::vector<std::pair<std::string, std::string>>
GetAllIpAddressesAndNetmasks(std::string const& adapterName)
{
    struct ifaddrs* ifaddr = nullptr;

    if (-1 == getifaddrs(&ifaddr))
    {
        throw std::runtime_error("getifaddrs failed");
        return {};
    }

    std::vector<std::pair<std::string, std::string>> addresses;
    char                                             ip[NI_MAXHOST];
    char                                             mask[NI_MAXHOST];

    for (struct ifaddrs* ifa = ifaddr; nullptr != ifa; ifa = ifa->ifa_next)
    {
        if (nullptr == ifa->ifa_addr)
        {
            continue;
        }

        if ((0 == strcmp(ifa->ifa_name, adapterName.c_str())) &&
            (AF_INET == ifa->ifa_addr->sa_family))
        {
            if (0 != getnameinfo(ifa->ifa_addr,
                                 sizeof(struct sockaddr_in),
                                 ip,
                                 NI_MAXHOST,
                                 nullptr,
                                 0,
                                 NI_NUMERICHOST))
            {
                continue;
            }

            if (0 != getnameinfo(ifa->ifa_netmask,
                                 sizeof(struct sockaddr_in),
                                 mask,
                                 NI_MAXHOST,
                                 nullptr,
                                 0,
                                 NI_NUMERICHOST))
            {
                continue;
            }

            std::pair<std::string, std::string> address;
            address.first  = ip;
            address.second = mask;
            addresses.emplace_back(address);
        }
    }

    freeifaddrs(ifaddr);

    return addresses;
}

bool IsAddressAndNetmaskOnSameSubnetAsAdapter(std::string const& ipAddress,
                                              std::string const& netmask,
                                              std::string const& adapterName)
{
    bool found            = false;
    auto adapterAddresses = GetAllIpAddressesAndNetmasks(adapterName);

    if (netmask.empty())
    {
        auto octets      = OctetsFromIpAddress(ipAddress);
        int  prevMatches = 0;

        for (auto const& adapterAddr : adapterAddresses)
        {
            auto ourOctets   = OctetsFromIpAddress(adapterAddr.first);
            int  currMatches = 0;

            for (size_t i = 0; i < 4; ++i)
            {
                if (octets[i] != ourOctets[i])
                {
                    break;
                }

                ++currMatches;
            }

            if (currMatches > prevMatches)
            {
                prevMatches = currMatches;
                found       = true;

                // Note: Do not break out of main loop as we may
                //       still find a closer match.
            }
        }
    }
    else
    {
        auto broadcastAddress = hgl::BuildBroadcastAddress(ipAddress, netmask);

        for (auto const& adapterAddr : adapterAddresses)
        {
            auto adBroadcastAddress =
                hgl::BuildBroadcastAddress(adapterAddr.first, adapterAddr.second);

            if (broadcastAddress == adBroadcastAddress)
            {
                found = true;
                break;
            }
        }
    }

    return found;
}

std::string GetMacAddressForAdapter(std::string const& adapterName)
{
    struct ifreq ifr;

    if (adapterName.size() >= sizeof(ifr.ifr_name))
    {
        throw std::invalid_argument("adapter name too long");
    }

    hgl::SafeCopyCharArray(ifr.ifr_name, adapterName, true);

    auto fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (-1 == fd)
    {
        throw std::runtime_error(strerror(errno));
    }

    auto ioctlRes  = ioctl(fd, SIOCGIFHWADDR, &ifr);
    auto tempErrNo = errno;
    close(fd);

    if (-1 == ioctlRes)
    {
        throw std::runtime_error(strerror(tempErrNo));
    }

    if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
    {
        throw std::runtime_error("invalid ethernet adapter");
    }

    auto const* macFirst = reinterpret_cast<uint8_t const*>(ifr.ifr_hwaddr.sa_data);
    auto const* macLast  = std::next(macFirst, 6);

    auto macAddr = hgl::MakeHexString(macFirst, macLast, true, true, ':');
    return macAddr;
}
#endif

} // namespace hgl
