/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_MAC)

#include <mach/machine.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <malloc/malloc.h>
#include <unordered_map>
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Math/Math.hpp"
#import <IOKit/IOKitLib.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/network/IOEthernetInterface.h>
#import <IOKit/network/IONetworkInterface.h>
#import <IOKit/network/IOEthernetController.h>

NS_FK_BEGIN

namespace detail
{
    std::string getHardwareProperty(const char* name)
    {
        size_t size;
        sysctlbyname(name, NULL, &size, NULL, 0);
        char *machine = new char[size+1];
        memset(machine, 0, size+1);
        sysctlbyname(name, machine, &size, NULL, 0);
        std::string platformName = machine;
        delete[] machine;
        return platformName;
    }
    
    // ioreg -d2 -c IOPlatformExpertDevice
    std::string getPlatformExpertDeviceProperty(const char* name)
    {
        std::string propertyValue;
        io_service_t platformExpert = IOServiceGetMatchingService(
                                                                  kIOMasterPortDefault,
                                                                  IOServiceMatching("IOPlatformExpertDevice"));
        if (platformExpert)
        {
            CFStringRef cfName = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingUTF8);
            CFTypeRef modelString = IORegistryEntryCreateCFProperty(
                                                                    platformExpert,
                                                                    cfName,
                                                                    kCFAllocatorDefault,
                                                                    0);
            CFRelease(cfName);
            if(!modelString)
                return propertyValue;
            
            if (CFGetTypeID(modelString) == CFStringGetTypeID())
            {
                propertyValue = [(__bridge NSString*)(CFStringRef)modelString UTF8String];
            }
            else if(CFGetTypeID(modelString) == CFDataGetTypeID())
            {
                NSData* nsdata = (__bridge NSData*)modelString;
                NSString* nsValue = [[NSString alloc] initWithData:nsdata encoding:NSUTF8StringEncoding];
                propertyValue = [nsValue UTF8String];
                [nsValue autorelease];
                
            }
            CFRelease(modelString);
            IOObjectRelease(platformExpert);
        }
        return propertyValue;
    }
    
    struct NetworkAdaptersInfo
    {
        std::string IPAddressV4;
        std::string IPAddressV6;
        std::string MacAddress;
    };
    
    std::unordered_map<std::string, NetworkAdaptersInfo> GetNetworkAdapters()
    {
        static std::unordered_map<std::string, NetworkAdaptersInfo> NetworkAdaptersInfoList;
        NetworkAdaptersInfoList.clear();
        // retrieve the current interfaces - returns 0 on success
        struct ifaddrs *interfaces;
        if(!getifaddrs(&interfaces))
        {
            // Loop through linked list of interfaces
            struct ifaddrs *interface;
            for(interface=interfaces; interface; interface=interface->ifa_next)
            {
                if(!(interface->ifa_flags & IFF_UP) /* || (interface->ifa_flags & IFF_LOOPBACK) */ )
                {
                    continue; // deeply nested code harder to read
                }
                NetworkAdaptersInfo&  NetworkAdaptersInfoItem = NetworkAdaptersInfoList[interface->ifa_name];
                const struct sockaddr_in *addr = (const struct sockaddr_in*)interface->ifa_addr;
                char addrBuf[ MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN) ];
                switch(addr->sin_family)
                {
                    case AF_INET:
                        {
                            if(inet_ntop(AF_INET, &addr->sin_addr, addrBuf, INET_ADDRSTRLEN))
                            {
                                NetworkAdaptersInfoItem.IPAddressV4 =addrBuf;
                            }
                        }
                        break;
                    case AF_INET6:
                        {
                            const struct sockaddr_in6 *addr6 = (const struct sockaddr_in6*)interface->ifa_addr;
                            if(inet_ntop(AF_INET6, &addr6->sin6_addr, addrBuf, INET6_ADDRSTRLEN))
                            {
                                NetworkAdaptersInfoItem.IPAddressV6 = addrBuf;
                            }
                        }
                        break;
                    case AF_LINK:
                    {
                        const struct sockaddr_dl *addrMac = (struct sockaddr_dl*)interface->ifa_addr;
                        if(addrMac->sdl_alen == 6)
                        {
                            unsigned char macBuf[6];
                            memcpy(macBuf, LLADDR(addrMac), addrMac->sdl_alen);
                            NetworkAdaptersInfoItem.MacAddress = StringUtils::Format("%02X%02X%02X%02X%02X%02X"
                                                                                     ,macBuf[0]
                                                                                     ,macBuf[1]
                                                                                     ,macBuf[2]
                                                                                     ,macBuf[3]
                                                                                     ,macBuf[4]
                                                                                     ,macBuf[5]);
                        }
                    }
                        break;
                    case AF_UNSPEC:
                    default:
                        break;
                }
            }
            // Free memory
            freeifaddrs(interfaces);
        }
        return NetworkAdaptersInfoList;
    }
} // namespace detail

std::string PlatformDevice::GetDeviceId()
{
    return detail::getPlatformExpertDeviceProperty("IOPlatformSerialNumber");
}

std::string PlatformDevice::GetProduct()
{
    return detail::getPlatformExpertDeviceProperty("product-name");
}

std::string PlatformDevice::GetHardware()
{
    return "Apple";
}

std::string PlatformDevice::GetDevice()
{
    return GetProduct();
}

std::string PlatformDevice::GetBrandName()
{
    return "Apple";
}

std::string PlatformDevice::GetModel()
{
    return detail::getPlatformExpertDeviceProperty("model");
}

std::string PlatformDevice::GetManufacturer()
{
    return detail::getPlatformExpertDeviceProperty("manufacturer");
}

std::string PlatformDevice::GetSystemVersion()
{
    NSDictionary* sysinfo = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
    NSString* systemVersion = [sysinfo objectForKey:@"ProductVersion"];
    return [systemVersion UTF8String];
}

std::string PlatformDevice::GetSDKVersion()
{
    return GetSystemVersion();
}

std::string PlatformDevice::GetCPUBrand()
{
    return "";
}

std::string PlatformDevice::GetCPUVendor()
{
    return "";
}

std::string PlatformDevice::GetCPUArch()
{
    cpu_type_t cpuType;
    cpu_subtype_t cpuSubType;
    size_t size = sizeof(cpuType);
    sysctlbyname("hw.cputype", &cpuType, &size, nullptr, 0);
    
    size = sizeof(cpuSubType);
    sysctlbyname("hw.cpusubtype", &cpuSubType, &size, nullptr, 0);
    std::string cpuTypeString;
    switch (cpuType)
    {
        case CPU_TYPE_ARM:
            cpuTypeString.append("arm");
            if(cpuSubType == CPU_SUBTYPE_ARM_V6) cpuTypeString.append("v6");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7) cpuTypeString.append("v7");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7F) cpuTypeString.append("v7f");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7S) cpuTypeString.append("v7s");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V7K) cpuTypeString.append("v7k");
            else if(cpuSubType == CPU_SUBTYPE_ARM_V8) cpuTypeString.append("v8");
            else cpuTypeString.append(StringUtils::Tostring(cpuSubType));
            break;
        case CPU_TYPE_ARM64:
            cpuTypeString.append("arm64");
            break;
        case CPU_TYPE_X86:
            cpuTypeString.append("x86");
            if(cpuSubType == CPU_SUBTYPE_X86_64_H)
            {
                cpuTypeString.append("_64");
            }
            break;
        case CPU_TYPE_X86_64:
            cpuTypeString.append("x86_64");
            break;
        default:
            cpuTypeString.append(StringUtils::Tostring(cpuType) + StringUtils::Tostring(cpuSubType));
            break;
    }
    return cpuTypeString;
}

int PlatformDevice::GetCPUCoreCount()
{
    return static_cast<int>([[NSProcessInfo processInfo] processorCount]);
}

int PlatformDevice::GetCPUFrequency()
{
    int mib[2] = {CTL_HW, HW_CPU_FREQ};
    u_int nameLen = sizeof(mib)/ sizeof(mib[0]);
    size_t cpufreqSize= 0;
    sysctl(mib, nameLen, nullptr, &cpufreqSize, NULL, 0);
    size_t cpufreq = 0;
    int result = sysctl(mib, nameLen, &cpufreq, &cpufreqSize, NULL, 0);
    if(result >= 0)
        return static_cast<int>((cpufreq/1000)); //KHz
    return 0;
}

std::string PlatformDevice::GetGPUBrand()
{
    return "";
}

int PlatformDevice::GetNetworkType()
{
    int result = 0;
    return result;
}

std::string PlatformDevice::GetIpAddressV4()
{
    auto EthernetList = detail::GetNetworkAdapters();
    auto iter = EthernetList.find("en1");
    if (iter == EthernetList.end())
    {
        iter = EthernetList.find("en0");
    }
    if (iter != EthernetList.end())
    {
        return iter->second.IPAddressV4;
    }
    return "";
}

std::string PlatformDevice::GetIpAddressV6()
{
    auto EthernetList = detail::GetNetworkAdapters();
    auto iter = EthernetList.find("en1");
    if (iter == EthernetList.end())
    {
        iter = EthernetList.find("en0");
    }
    if (iter != EthernetList.end())
    {
        return iter->second.IPAddressV6;
    }
    return "";
}

std::string PlatformDevice::GetMacAddress()
{
    auto EthernetList = detail::GetNetworkAdapters();
    auto iter = EthernetList.find("en1");
    if (iter == EthernetList.end())
    {
        iter = EthernetList.find("en0");
    }
    if (iter != EthernetList.end())
    {
        return iter->second.MacAddress;
    }
    return "";
}

PlatformDevice::string_list PlatformDevice::GetDNS()
{
    return PlatformDevice::string_list();
}

long long PlatformDevice::GetTotalMemory()
{
    return [[NSProcessInfo processInfo] physicalMemory];
}

long long PlatformDevice::GetAvailableMemory()
{
    mach_port_t host_port =mach_host_self();
    mach_msg_type_number_t host_size  = HOST_VM_INFO64_COUNT;
    vm_size_t pagesize;
    vm_statistics64_data_t vm_stat;
    
    host_page_size(host_port, &pagesize);
    
    if (host_statistics64(host_port, HOST_VM_INFO64, (host_info64_t)&vm_stat, &host_size) != KERN_SUCCESS)
    {
        FKLog("***** Failed to fetch vm statistics");
        return 0;
    }
    //natural_t mem_used = (vm_stat.active_count+vm_stat.inactive_count+vm_stat.wire_count)*pagesize;
    //natural_t mem_total = mem_used +vm_stat.free_count*pagesize;
    return vm_stat.free_count * pagesize;
}

//std::string PlatformDevice::GetGPURenderer()
//{
//    char* szRenderer = (char*)glGetString(GL_RENDERER);
//    return szRenderer?szRenderer:"";
//}
//
//std::string PlatformDevice::GetGPUVendor()
//{
//    char* szVendor = (char*)glGetString(GL_VENDOR);
//    return szVendor?szVendor:"";
//}


Rect PlatformDevice::GetScreenResolution()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSRect screenRect = [mainScreen visibleFrame];
    Rect bounds(screenRect.origin.x, screenRect.origin.y, screenRect.size.width, screenRect.size.height);
    return bounds;
}

Rect PlatformDevice::GetScreenNativeResolution()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSRect screenRect = [mainScreen frame];
    Rect bounds(screenRect.origin.x, screenRect.origin.y, screenRect.size.width, screenRect.size.height);
    return bounds;
}

float PlatformDevice::GetScreenDPI()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSDictionary* description = [mainScreen deviceDescription];
    NSSize displayPixelSize = [[description objectForKey:NSDeviceResolution] sizeValue];
    return (displayPixelSize.width+displayPixelSize.height) / 2;
    //CGSize displayPhysicalSize = CGDisplayScreenSize([[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);
    //return ((displayPixelSize.width / displayPhysicalSize.width) * 25.4f);
}

float PlatformDevice::GetRefreshRate()
{
    return 60.0f;
}

float PlatformDevice::GetScreenXDPI()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSDictionary* description = [mainScreen deviceDescription];
    NSSize resolution = [[description objectForKey:NSDeviceResolution] sizeValue];
    return resolution.width;
}

float PlatformDevice::GetScreenYDPI()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSDictionary* description = [mainScreen deviceDescription];
    NSSize resolution = [[description objectForKey:NSDeviceResolution] sizeValue];
    return resolution.height;
}

float PlatformDevice::GetNativeScale()
{
    return [[NSScreen mainScreen] backingScaleFactor];
}

PlatformMemoryConstants& PlatformDevice::GetMemoryConstants()
{
    static PlatformMemoryConstants MemoryConstants;
    // Get page size.
    vm_size_t PageSize;
    host_page_size(mach_host_self(), &PageSize);
    
    // Get swap file info
    xsw_usage SwapUsage;
    size_t Size = sizeof(SwapUsage);
    sysctlbyname("vm.swapusage", &SwapUsage, &Size, NULL, 0);
    
    // Get memory.
    int64 TotalPhysical = 0;
    int Mib[] = {CTL_HW, HW_MEMSIZE};
    size_t Length = sizeof(int64);
    sysctl(Mib, 2, &TotalPhysical, &Length, NULL, 0);
    
    // Gather platform memory stats.
    vm_statistics64 Stats;
    mach_msg_type_number_t StatsSize = sizeof(Stats);
    host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&Stats, &StatsSize);

    mach_task_basic_info_data_t TaskInfo;
    mach_msg_type_number_t TaskInfoCount = MACH_TASK_BASIC_INFO_COUNT;
    task_info( mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&TaskInfo, &TaskInfoCount );

    uint64_t FreeMem = Stats.free_count * PageSize;
    
    MemoryConstants.TotalPhysical     = TotalPhysical;
    MemoryConstants.TotalVirtual      = TotalPhysical + SwapUsage.xsu_total;
    MemoryConstants.PageSize          = PageSize;
    MemoryConstants.AvailablePhysical = FreeMem;
    MemoryConstants.AvailableVirtual  = FreeMem + SwapUsage.xsu_avail;
    MemoryConstants.UsedPhysical      = TaskInfo.resident_size;
    MemoryConstants.UsedVirtual       = TaskInfo.virtual_size;
	if(MemoryConstants.UsedPhysical > MemoryConstants.PeakUsedPhysical)
	{
		MemoryConstants.PeakUsedPhysical = MemoryConstants.UsedPhysical;
	}
	if(MemoryConstants.UsedVirtual > MemoryConstants.PeakUsedVirtual)
	{
		MemoryConstants.PeakUsedVirtual = MemoryConstants.UsedVirtual;
	}
    return MemoryConstants;
}

void PlatformDevice::DumpDeviceInfo()
{
    FKLog("============ Device Info===============");
    std::ostringstream ss;
    ss << "GetDeviceId:" << GetDeviceId() << "\n";
    ss << "GetProduct:" << GetProduct() << "\n";
    ss << "GetHardware:" << GetHardware() << "\n";
    ss << "GetDevice:" << GetDevice() << "\n";
    ss << "GetModel:" << GetModel() << "\n";
    ss << "GetManufacturer:" << GetManufacturer() << "\n";
    ss << "GetSystemVersion:" << GetSystemVersion() << "\n";
    ss << "GetSDKVersion:" << GetSDKVersion() << "\n";
    ss << "GetCPUCoreCount:" << GetCPUCoreCount() << "\n";
    ss << "GetCPUFrequency:" << GetCPUFrequency() << "\n";
    ss << "GetNetworkType:" << GetNetworkType() << " 1 WIFI,2 2G,3 3G,4 4G,0 other. \n";
    ss << "GetIpAddressV4:" << GetIpAddressV4() << "\n";
    ss << "GetIpAddressV6:" << GetIpAddressV6() << "\n";
    ss << "GetMacAddress:" << GetMacAddress() << "\n";
    auto dnss = GetDNS();
    for(auto dns : dnss)
    {
        ss << "GetDNS:" << dns << "\n";
    }
    ss << "GetTotalMemory:" << GetTotalMemory() << " bytes\n";
    ss << "GetAvailableMemory:" << GetAvailableMemory() << "bytes\n";
    
    /// Get the screen resolution, not including the virtual button area
    Rect Resolution  = GetScreenResolution();
    ss << "GetScreenResolution:" << Resolution.size.Width<<"*"<< Resolution.size.Height<< "\n";
    /// Get the screen resolution, including the virtual button area
    Resolution = GetScreenNativeResolution();
    ss << "GetScreenNativeResolution:" << Resolution.size.Width<<"*"<< Resolution.size.Height<< "\n";
    ss << "GetScreenDPI:" << GetScreenDPI() << "\n";
    ss << "GetRefreshRate:" << GetRefreshRate() << "\n";
    ss << "GetScreenXDPI:" << GetScreenXDPI() << "\n";
    ss << "GetScreenYDPI:" << GetScreenYDPI() << "\n";
    ss << "GetNativeScale:" << GetNativeScale() << "\n";
    
    PlatformMemoryConstants memConstants =  GetMemoryConstants();
    ss << "PageSize:" << memConstants.PageSize << "\n";
    ss << "TotalPhysical:" << memConstants.TotalPhysical << "\n";
    ss << "TotalVirtual:" << memConstants.TotalVirtual << "\n";
    ss << "AddressLimit:" << memConstants.AddressLimit << "\n";
    ss << "AvailablePhysical:" << memConstants.AvailablePhysical << "\n";
    ss << "AvailableVirtual:" << memConstants.AvailableVirtual << "\n";
    ss << "UsedPhysical:" << memConstants.UsedPhysical << "\n";
    ss << "PeakUsedPhysical:" << memConstants.PeakUsedPhysical << "\n";
    ss << "UsedVirtual:" << memConstants.UsedVirtual << "\n";
    ss << "PeakUsedVirtual:" << memConstants.PeakUsedVirtual << "\n";
    FKLog(ss.str().c_str());
}

bool PlatformDevice::IsDebuggerPresent()
{
    // Based on http://developer.apple.com/library/mac/#qa/qa1361/_index.html
	struct kinfo_proc Info;
	int32 Mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
	size_t Size = sizeof(Info);
	sysctl( Mib, sizeof( Mib ) / sizeof( *Mib ), &Info, &Size, NULL, 0 );
	return ( Info.kp_proc.p_flag & P_TRACED ) != 0;
}

std::string PlatformDevice::ExecuteSystemCommand(const std::string& command)
{
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        char buffer[256] = { 0 };
        //while (!feof(pipe))
        //{
        //    if (fgets(buffer, 256, pipe) != NULL)
        //        result += buffer;
        //}
        for(;;)
        {
            size_t NumRead = fread(buffer, sizeof(char), sizeof(buffer), pipe);
            if (NumRead == 0)
            {
                if (errno == EINTR)
                    continue;
                if (feof(pipe) == 0) // read file error.
                {
                    FKLog("Error while execute command %s: %s\n", command.c_str(), strerror(errno));
                }
                break;
            }
            result += buffer;
        }
    }
    catch (...)
    {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

bool PlatformDevice::ScreenCapture(std::string& outSavePath)
{
    return false;
}
NS_FK_END

#endif // OF #if (PLATFORM_MAC)



