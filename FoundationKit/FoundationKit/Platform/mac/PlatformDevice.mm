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
#include <unordered_map>
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#import <IOKit/IOKitLib.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>


NS_FK_BEGIN

namespace detail
{
#define IOS_CELLULAR    @"pdp_ip0"
#define IOS_WIFI        @"en0"
#define IOS_VPN         @"utun0"
#define IP_ADDR_IPv4    @"ipv4"
#define IP_ADDR_IPv6    @"ipv6"
    
    bool isValidatIP(const std::string& strIpaddress,bool ipv4)
    {
        if (strIpaddress.size() == 0)
        {
            return NO;
        }
        NSString *IPV4RegEx = @"^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
        "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
        "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
        "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$";
        
//        NSString* IPV6RegEx = @"^\s*((([0-9A-Fa-f]{1,4}:){7}(([0-9A-Fa-f]{1,4})|:))|(([0-9A-Fa-f]{1,4}:){6}(:|((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})|(:[0-9A-Fa-f]{1,4})))|(([0-9A-Fa-f]{1,4}:){5}((:((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){0,1}((:((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){0,2}((:((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}((:((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:)(:[0-9A-Fa-f]{1,4}){0,4}((:((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(:(:[0-9A-Fa-f]{1,4}){0,5}((:((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(((25[0-5]|2[0-4]\d|[01]?\d{1,2})(\.(25[0-5]|2[0-4]\d|[01]?\d{1,2})){3})))(%.+)?\s*$";
        
        NSString* IPV6RegEx = @"^\\s*((([0-9A-Fa-f]{1,4}:){7}(([0-9A-Fa-f]{1,4})|:))|(([0-9A-Fa-f]{1,4}:){6}(:|((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})|(:[0-9A-Fa-f]{1,4})))|(([0-9A-Fa-f]{1,4}:){5}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){0,1}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){0,2}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:)(:[0-9A-Fa-f]{1,4}){0,4}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(:(:[0-9A-Fa-f]{1,4}){0,5}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})))(%.+)?\\s*$";
        
        NSError *error;
        NSString* ipAddress = [NSString stringWithUTF8String:strIpaddress.c_str()];
        NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:ipv4?IPV4RegEx:IPV6RegEx options:0 error:&error];
        if (regex != nil)
        {
            NSTextCheckingResult *firstMatch=[regex firstMatchInString:ipAddress options:0 range:NSMakeRange(0, [ipAddress length])];
            if (firstMatch)
            {
//                NSRange resultRange = [firstMatch rangeAtIndex:0];
//                NSString *result=[ipAddress substringWithRange:resultRange];
                return true;
            }
        }
        return false;
    }
    
    NSDictionary* getIPAddresses()
    {
        NSMutableDictionary *addresses = [NSMutableDictionary dictionaryWithCapacity:8];
        // retrieve the current interfaces - returns 0 on success
        struct ifaddrs *interfaces;
        if(!getifaddrs(&interfaces)) {
            // Loop through linked list of interfaces
            struct ifaddrs *interface;
            for(interface=interfaces; interface; interface=interface->ifa_next) {
                if(!(interface->ifa_flags & IFF_UP) /* || (interface->ifa_flags & IFF_LOOPBACK) */ ) {
                    continue; // deeply nested code harder to read
                }
                const struct sockaddr_in *addr = (const struct sockaddr_in*)interface->ifa_addr;
                char addrBuf[ MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN) ];
                if(addr && (addr->sin_family==AF_INET || addr->sin_family==AF_INET6)) {
                    NSString *name = [NSString stringWithUTF8String:interface->ifa_name];
                    NSString *type = nil;
                    if(addr->sin_family == AF_INET) {
                        if(inet_ntop(AF_INET, &addr->sin_addr, addrBuf, INET_ADDRSTRLEN)) {
                            type = IP_ADDR_IPv4;
                        }
                    } else {
                        const struct sockaddr_in6 *addr6 = (const struct sockaddr_in6*)interface->ifa_addr;
                        if(inet_ntop(AF_INET6, &addr6->sin6_addr, addrBuf, INET6_ADDRSTRLEN))
                        {
                            type = IP_ADDR_IPv6;
                        }
                    }
                    if(type) {
                        NSString *key = [NSString stringWithFormat:@"%@/%@", name, type];
                        addresses[key] = [NSString stringWithUTF8String:addrBuf];
                    }
                }
            }
            // Free memory
            freeifaddrs(interfaces);
        }
        return [addresses count] ? addresses : nil;
    }
    
    std::string GetIPAddressByType(int iptype)
    {
        NSArray *searchArray = iptype == AF_INET ?
        @[ IOS_VPN @"/" IP_ADDR_IPv4, IOS_VPN @"/" IP_ADDR_IPv6, IOS_WIFI @"/" IP_ADDR_IPv4, IOS_WIFI @"/" IP_ADDR_IPv6, IOS_CELLULAR @"/" IP_ADDR_IPv4, IOS_CELLULAR @"/" IP_ADDR_IPv6 ] :
        @[ IOS_VPN @"/" IP_ADDR_IPv6, IOS_VPN @"/" IP_ADDR_IPv4, IOS_WIFI @"/" IP_ADDR_IPv6, IOS_WIFI @"/" IP_ADDR_IPv4, IOS_CELLULAR @"/" IP_ADDR_IPv6, IOS_CELLULAR @"/" IP_ADDR_IPv4 ] ;
        
        NSDictionary *addresses = getIPAddresses();
        
        __block NSString *address;
        [searchArray enumerateObjectsUsingBlock:^(NSString *key, NSUInteger idx, BOOL *stop)
         {
             address = addresses[key];
             //筛选出IP地址格式
             if(address != nil && isValidatIP([address UTF8String], iptype == AF_INET)) *stop = YES;
         } ];
        return address ? [address UTF8String] : "0.0.0.0";
    }
    
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
                
            }
            CFRelease(modelString);
            IOObjectRelease(platformExpert);
        }
        return propertyValue;
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

std::string PlatformDevice::GetRendererVersion()
{
    char* szOpenGLVersionString = (char*)glGetString(GL_VERSION);
    return szOpenGLVersionString?szOpenGLVersionString:"";
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
            else cpuTypeString.append(StringUtils::to_string(cpuSubType));
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
            cpuTypeString.append(StringUtils::to_string(cpuType) + StringUtils::to_string(cpuSubType));
            break;
    }
    return cpuTypeString;
}

int PlatformDevice::GetCPUCoreCount()
{
    return static_cast<int>([[NSProcessInfo processInfo] processorCount]);
}

int PlatformDevice::GetCPUMaxFreq(int cpuIndex/* = -1*/)
{
    int64 cpufreq = 0;
    int Mib[] = {CTL_HW, HW_CPU_FREQ};
    size_t Length = sizeof(int64);
    sysctl(Mib, 2, &cpufreq, &Length, NULL, 0);
    return static_cast<int>((cpufreq/1000)); //KHz
}

int PlatformDevice::GetCPUCurFreq(int cpuIndex/* = -1*/)
{
    return GetCPUMaxFreq();
}

int PlatformDevice::GetCPUMinFreq(int cpuIndex/* = -1*/)
{
    return GetCPUMaxFreq();
}

int PlatformDevice::GetNetworkType()
{
    int result = 0;
    return result;
}

std::string PlatformDevice::GetIpAddressV4()
{
    return detail::GetIPAddressByType(AF_INET);
}

std::string PlatformDevice::GetIpAddressV6()
{
    return detail::GetIPAddressByType(AF_INET6);
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
        LOG_ERROR("***** Failed to fetch vm statistics");
        return 0;
    }
    //natural_t mem_used = (vm_stat.active_count+vm_stat.inactive_count+vm_stat.wire_count)*pagesize;
    //natural_t mem_total = mem_used +vm_stat.free_count*pagesize;
    return vm_stat.free_count * pagesize;
}

std::string PlatformDevice::GetGPURenderer()
{
    char* szRenderer = (char*)glGetString(GL_RENDERER);
    return szRenderer?szRenderer:"";
}

std::string PlatformDevice::GetGPUVendor()
{
    char* szVendor = (char*)glGetString(GL_VENDOR);
    return szVendor?szVendor:"";
}


Size PlatformDevice::GetScreenResolution()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSRect screenRect = [mainScreen visibleFrame];
    Size resolution = Size(screenRect.size.width, screenRect.size.height);
    return resolution;
}

Size PlatformDevice::GetScreenNativeResolution()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSRect screenRect = [mainScreen frame];
    Size resolution = Size(screenRect.size.width, screenRect.size.height);
    return resolution;
}

float PlatformDevice::GetScreenDPI()
{
    NSScreen* mainScreen = [NSScreen mainScreen];
    NSDictionary* description = [mainScreen deviceDescription];
    NSSize resolution = [[description objectForKey:NSDeviceResolution] sizeValue];
    return (resolution.width+resolution.height) / 2;
}

float PlatformDevice::GetScreenFPS()
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

std::string PlatformDevice::ExecuteSystemCommand(const std::string& command)
{
    char buffer[1024]={0};
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe))
        {
            if (fgets(buffer, 1024, pipe) != NULL)
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

NS_FK_END



