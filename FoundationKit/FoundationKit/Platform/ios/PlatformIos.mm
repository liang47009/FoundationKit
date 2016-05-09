/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
 #include "FoundationKit/GenericPlatformMacros.h"
 #if TARGET_PLATFORM == PLATFORM_IOS

#include "Platform.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <mach/machine.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h> 
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/time.h>
#include <sched.h>  // maybe the BSD time and sched.h need to be independent, but they are normally available together

#include <unordered_map>
#import <UIKit/UIKit.h>
#import <AdSupport/AdSupport.h>
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Crypto/md5.hpp"

NS_FK_BEGIN


namespace PlatformHelper {
    
    std::string getHardwareProperty(const char* name)
    {
        size_t size;
        sysctlbyname(name, NULL, &size, NULL, 0);
        char *machine = new char[size+1];
        memset(machine, 0, size+1);
        sysctlbyname(name, machine, &size, NULL, 0);
        std::string platformName = machine;
        delete machine;
        return platformName;
    }
    
    //code from https://github.com/fahrulazmi/UIDeviceHardware
    std::string platformString()
    {
        std::string platformName = getHardwareProperty("hw.machine");
        if (platformName == "iPhone1,1")    return "iPhone 1G";
        if (platformName == "iPhone1,2")    return "iPhone 3G";
        if (platformName == "iPhone2,1")    return "iPhone 3GS";
        if (platformName == "iPhone3,1")    return "iPhone 4 (GSM)";
        if (platformName == "iPhone3,3")    return "iPhone 4 (CDMA)";
        if (platformName == "iPhone4,1")    return "iPhone 4S";
        if (platformName == "iPhone5,1")    return "iPhone 5 (GSM)";
        if (platformName == "iPhone5,2")    return "iPhone 5 (CDMA)";
        if (platformName == "iPhone5,3")    return "iPhone 5c";
        if (platformName == "iPhone5,4")    return "iPhone 5c";
        if (platformName == "iPhone6,1")    return "iPhone 5s";
        if (platformName == "iPhone6,2")    return "iPhone 5s";
        if (platformName == "iPhone7,1")    return "iPhone 6 Plus";
        if (platformName == "iPhone7,2")    return "iPhone 6";
        if (platformName == "iPhone8,1")    return "iPhone 6s";
        if (platformName == "iPhone8,2")    return "iPhone 6s Plus";
        
        if (platformName == "iPod1,1")      return "iPod Touch 1G";
        if (platformName == "iPod2,1")      return "iPod Touch 2G";
        if (platformName == "iPod3,1")      return "iPod Touch 3G";
        if (platformName == "iPod4,1")      return "iPod Touch 4G";
        if (platformName == "iPod5,1")      return "iPod Touch 5G";
        if (platformName == "iPod7,1")      return "iPod Touch 6G";
        
        if (platformName == "iPad1,1")      return "iPad";
        if (platformName == "iPad2,1")      return "iPad 2 (WiFi)";
        if (platformName == "iPad2,2")      return "iPad 2 (GSM)";
        if (platformName == "iPad2,3")      return "iPad 2 (CDMA)";
        if (platformName == "iPad2,4")      return "iPad 2 (WiFi)";
        if (platformName == "iPad2,5")      return "iPad Mini (WiFi)";
        if (platformName == "iPad2,6")      return "iPad Mini (GSM)";
        if (platformName == "iPad2,7")      return "iPad Mini (CDMA)";
        if (platformName == "iPad3,1")      return "iPad 3 (WiFi)";
        if (platformName == "iPad3,2")      return "iPad 3 (CDMA)";
        if (platformName == "iPad3,3")      return "iPad 3 (GSM)";
        if (platformName == "iPad3,4")      return "iPad 4 (WiFi)";
        if (platformName == "iPad3,5")      return "iPad 4 (GSM)";
        if (platformName == "iPad3,6")      return "iPad 4 (CDMA)";
        if (platformName == "iPad4,1")      return "iPad Air (WiFi)";
        if (platformName == "iPad4,2")      return "iPad Air (GSM)";
        if (platformName == "iPad4,3")      return "iPad Air (CDMA)";
        if (platformName == "iPad4,4")      return "iPad Mini Retina (WiFi)";
        if (platformName == "iPad4,5")      return "iPad Mini Retina (Cellular)";
        if (platformName == "iPad4,7")      return "iPad Mini 3 (WiFi)";
        if (platformName == "iPad4,8")      return "iPad Mini 3 (Cellular)";
        if (platformName == "iPad4,9")      return "iPad Mini 3 (Cellular)";
        if (platformName == "iPad5,1")      return "iPad Mini 4 (WiFi)";
        if (platformName == "iPad5,2")      return "iPad Mini 4 (Cellular)";
        if (platformName == "iPad5,3")      return "iPad Air 2 (WiFi)";
        if (platformName == "iPad5,4")      return "iPad Air 2 (Cellular)";
        if (platformName == "iPad6,7")      return "iPad Pro (WiFi)";
        if (platformName == "iPad6,8")      return "iPad Pro (Cellular)";
        
        if (platformName == "i386")         return getHardwareProperty("hw.model");
        if (platformName == "x86_64")       return getHardwareProperty("hw.model");
        
        return platformName;
    }

}


float Platform::getTotalMemory()
{
    long megabyte = 1024*1024;
	int64_t physical_memory;
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE;
	size_t length = sizeof(int64_t);
	sysctl(mib, 2, &physical_memory, &length, NULL, 0);
	return static_cast<int>((physical_memory/megabyte));
}

float Platform::getFreeMemory()
{
    long megabyte = 1024*1024;
	vm_size_t page_size;
	mach_port_t mach_port;
	mach_msg_type_number_t count;
	vm_statistics_data_t vm_stats;
	int64_t free_memory = 0;
	float mb = 0;
	mach_port = mach_host_self();
	count = sizeof(vm_stats) / sizeof(natural_t);
	if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
		KERN_SUCCESS == host_statistics(mach_port, HOST_VM_INFO, 
										(host_info_t)&vm_stats, &count))
	{
		free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;

		mb = static_cast<int>((free_memory/megabyte));
	}
	return mb;
}

float Platform::getProcessMemory()
{
    long megabyte = 1024*1024;
	struct task_basic_info t_info;
	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
	
	if (KERN_SUCCESS != task_info(mach_task_self(),
								  TASK_BASIC_INFO, (task_info_t)&t_info, 
								  &t_info_count))
	{
		return -1;
	}
	return static_cast<int>((t_info.resident_size/megabyte));
}

std::string Platform::getMacAddress()
{
    int                 mgmtInfoBase[6];
    char                *msgBuffer = NULL;
    NSString            *errorFlag = NULL;
    size_t              length;
    
    // Setup the management Information Base (mib)
    mgmtInfoBase[0] = CTL_NET;        // Request network subsystem
    mgmtInfoBase[1] = AF_ROUTE;       // Routing table info
    mgmtInfoBase[2] = 0;
    mgmtInfoBase[3] = AF_LINK;        // Request link layer information
    mgmtInfoBase[4] = NET_RT_IFLIST;  // Request all configured interfaces
    
    // With all configured interfaces requested, get handle index
    if ((mgmtInfoBase[5] = if_nametoindex("en0")) == 0)
        errorFlag = @"if_nametoindex failure";
    // Get the size of the data available (store in len)
    else if (sysctl(mgmtInfoBase, 6, NULL, &length, NULL, 0) < 0)
        errorFlag = @"sysctl mgmtInfoBase failure";
    // Alloc memory based on above call
    else if ((msgBuffer = static_cast<char*>(malloc(length))) == NULL)
        errorFlag = @"buffer allocation failure";
    // Get system information, store in buffer
    else if (sysctl(mgmtInfoBase, 6, msgBuffer, &length, NULL, 0) < 0)
    {
        free(msgBuffer);
        errorFlag = @"sysctl msgBuffer failure";
    }
    else
    {
        // Map msgbuffer to interface message structure
        struct if_msghdr *interfaceMsgStruct = (struct if_msghdr *) msgBuffer;
        
        // Map to link-level socket structure
        struct sockaddr_dl *socketStruct = (struct sockaddr_dl *) (interfaceMsgStruct + 1);
        
        // Copy link layer address data in socket structure to an array
        unsigned char macAddress[6];
        memcpy(&macAddress, socketStruct->sdl_data + socketStruct->sdl_nlen, 6);
        
        // Read from char array into a string object, into traditional Mac address format
        NSString *macAddressString = [NSString stringWithFormat:@"%02X%02X%02X%02X%02X%02X",
                                      macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]];
        NSLog(@"Mac Address: %@", macAddressString);
        
        // Release the buffer memory
        free(msgBuffer);
        
        return [macAddressString UTF8String];
    }
    
    // Error...
    NSLog(@"Error: %@", errorFlag);
    
    return "";
    
}

std::string Platform::getDeviceId()
{
    float systemVersion = [[[UIDevice currentDevice] systemVersion] floatValue];
    if(systemVersion < 7.0f)
    {
        return MD5::md5_hash_hex(getMacAddress().c_str());
    }
    else
    {
        NSString* advertisingId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
        return [advertisingId UTF8String];
    }
    return "";
}

std::string Platform::getDeviceName()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    std::string devName = systemInfo.machine;
    
    return PlatformHelper::platformString();
}

std::string Platform::getOperatingSystemVersion()
{
    int mib[2] = {CTL_KERN, KERN_OSVERSION};
    u_int nameLen = sizeof(mib)/ sizeof(mib[0]);
    size_t bufferSize= 0;
    sysctl(mib, nameLen, nullptr, &bufferSize, NULL, 0);
    
    NSString* systemVersion= [UIDevice currentDevice].systemVersion;
    std::string strVersion = [systemVersion UTF8String];
    char *sysVersion = new char[bufferSize+1];
    memset(sysVersion, 0, bufferSize+1);
    int result = sysctl(mib, nameLen, sysVersion, &bufferSize, NULL, 0);
    if (result >= 0)
    {
        strVersion += "(";
        strVersion += sysVersion;
        strVersion += ")";
    }
    delete sysVersion;
    return strVersion;
}

std::string Platform::getCPUArchitecture()
{
    cpu_type_t cpuType;
    cpu_subtype_t cpuSubType;
    size_t size = sizeof(cpuType);
    sysctlbyname("hw.cputype", &cpuType, &size, nullptr, 0);
    
    size = sizeof(cpuSubType);
    sysctlbyname("hw.cputype", &cpuSubType, &size, nullptr, 0);
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

void Platform::systemTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    // convert it to local time
    struct tm localTime;
    localtime_r(&tmVal.tv_sec, &localTime);

    // pull out data/time
    year = localTime.tm_year + 1900;
    month = localTime.tm_mon + 1;
    dayOfWeek = localTime.tm_wday;
    day = localTime.tm_mday;
    hour = localTime.tm_hour;
    min = localTime.tm_min;
    sec = localTime.tm_sec;
    msec = tmVal.tv_usec / 1000;
}

void Platform::utcTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    // convert it to UTC
    struct tm localTime;
    gmtime_r(&tmVal.tv_sec, &localTime);

    // pull out data/time
    year = localTime.tm_year + 1900;
    month = localTime.tm_mon + 1;
    dayOfWeek = localTime.tm_wday;
    day = localTime.tm_mday;
    hour = localTime.tm_hour;
    min = localTime.tm_min;
    sec = localTime.tm_sec;
    msec = tmVal.tv_usec / 1000;
}

int64 Platform::getTickCount()
{
    return (int64)mach_absolute_time();
}

NS_FK_END

#endif //TARGET_PLATFORM == PLATFORM_IOS











