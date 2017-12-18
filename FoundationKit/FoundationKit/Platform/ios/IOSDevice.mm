/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_IOS)

#include <mach/machine.h>
#include <sys/sysctl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <unistd.h>
#include <malloc/malloc.h>
#include <unordered_map>
#include <sstream>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "detail/FCUUID.h"
#include "detail/UICKeyChainStore.h" //Based on https://github.com/kishikawakatsumi/UICKeyChainStore
#include "detail/UIDeviceHardware.h" //Based on https://github.com/fahrulazmi/UIDeviceHardware
#include "detail/Reachability_libo.h"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"

NS_FK_BEGIN

namespace detail
{
    enum DeviceDataFeild
    {
        RAM,
        CPU_NAME,
        CPU_ARCH,
        CPU_CORES,
        CPU_CLOCK,
        GPU_NAME,
        GPU_CORES,
        GPU_CLOCK,
        SCREEN_RES,
        SCREEN_PPI,
        SCREEN_SIZE,//inches
    };
    // https://support.hockeyapp.net/kb/client-integration-ios-mac-os-x-tvos/ios-device-types
    // Data source:http://www.blakespot.com/ios_device_specifications_grid.html
    std::unordered_map<std::string, std::vector<std::string>> AppleDeviceData =
    {
        { "iPhone1,1", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "412", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
        { "iPod1,1", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "412", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
        { "iPhone1,2", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "412", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
        { "iPod2,1", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "532", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
        { "iPhone2,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "600", "PowerVR SGX535", "1", "150", "480 x 320", "163", "3.50" } },
        { "iPod3,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "600", "PowerVR SGX535", "1", "0", "480 x 320", "163", "3.50" } },
        { "iPad1,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "1000", "PowerVR SGX535", "1", "200", "1024 x 768", "132", "9.70" } },
        { "iPhone3,1", { "512", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
        { "iPhone3,2", { "512", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
        { "iPhone3,3", { "512", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
        { "iPod4,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
        { "iPad2,1", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "132", "9.70" } },
        { "iPad2,2", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "132", "9.70" } },
        { "iPad2,3", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "132", "9.70" } },
        { "iPhone4,1", { "512", "ARM Cortex-A9", "ARMv7", "2", "800", "PowerVR SGX543MP2", "2", "250", "960 x 640", "326", "3.50" } },
        { "iPad3,1", { "1024", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP4", "4", "250", "2048 x 1536", "264", "9.70" } },
        { "iPad3,2", { "1024", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP4", "4", "250", "2048 x 1536", "264", "9.70" } },
        { "iPad3,3", { "1024", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP4", "4", "250", "2048 x 1536", "264", "9.70" } },
        { "iPhone5,1", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
        { "iPhone5,2", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
        { "iPod5,1", { "512", "ARM Cortex-A9", "ARMv7", "2", "800", "PowerVR SGX543MP2", "2", "250", "1136 x 640", "326", "4.00" } },
        { "iPad3,4", { "1024", "Swift (Apple)", "ARMv7s", "2", "1400", "PowerVR SGX554MP4", "4", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad3,5", { "1024", "Swift (Apple)", "ARMv7s", "2", "1400", "PowerVR SGX554MP4", "4", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad3,6", { "1024", "Swift (Apple)", "ARMv7s", "2", "1400", "PowerVR SGX554MP4", "4", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad2,5", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "163", "7.90" } },
        { "iPad2,6", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "163", "7.90" } },
        { "iPad2,7", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "163", "7.90" } },
        { "iPhone5,3", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
        { "iPhone5,4", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
        { "iPhone6,1", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "1136 x 640", "326", "4.00" } },
        { "iPhone6,2", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "1136 x 640", "326", "4.00" } },
        { "iPhone6,3", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "1136 x 640", "326", "4.00" } },
        { "iPad4,1", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1400", "PowerVR G6430", "4", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad4,2", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1400", "PowerVR G6430", "4", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad4,3", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1400", "PowerVR G6430", "4", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad4,4", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
        { "iPad4,5", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
        { "iPad4,6", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
        { "iPhone7,2", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "1334 x 750", "326", "4.70" } },
        { "iPhone7,1", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "1920 x 1080", "401", "5.50" } },
        { "iPad5,3", { "2048", "Typhoon (Apple)", "ARMv8", "3", "1500", "PowerVR GXA6850", "8", "450", "2048 x 1536", "264", "9.70" } },
        { "iPad5,4", { "2048", "Typhoon (Apple)", "ARMv8", "3", "1500", "PowerVR GXA6850", "8", "450", "2048 x 1536", "264", "9.70" } },
        { "iPad4,7", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
        { "iPad4,8", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
        { "iPad4,9", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
        { "iPod7,1", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1100", "PowerVR GX6450", "4", "450", "1334 x 750", "326", "4.70" } },
        { "iPad6,7", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2260", "PowerVR Series7", "12", "0", "2732 x 2048", "264", "12.90" } },
        { "iPad6,8", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2260", "PowerVR Series7", "12", "0", "2732 x 2048", "264", "12.90" } },
        { "iPad5,1", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "2048 x 1536", "264", "7.90" } },
        { "iPad5,2", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "2048 x 1536", "264", "7.90" } },
        { "iPhone8,1", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "6", "0", "1334 x 750", "326", "4.70" } },
        { "iPhone8,2", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "6", "0", "1920 x 1080", "401", "5.50" } },
        { "iPhone8,4", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "6", "0", "1136 x 640", "326", "4.00" } },
        { "iPad6,3", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2160", "PowerVR Series7", "12", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad6,4", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2160", "PowerVR Series7", "12", "0", "2048 x 1536", "264", "9.70" } },
        { "iPhone9,1", { "2048", "Twister (Apple)", "ARMv8", "4", "2340", "PowerVR GT7600 Plus", "6", "0", "1334 x 750", "326", "4.70" } },
        { "iPhone9,2", { "2048", "Twister (Apple)", "ARMv8", "4", "2340", "PowerVR GT7600 Plus", "6", "0", "1334 x 750", "326", "4.70" } },
        { "iPhone9,3", { "2048", "Twister (Apple)", "ARMv8", "4", "2340", "PowerVR GT7600 Plus", "6", "0", "1334 x 750", "326", "4.70" } },
        { "iPhone9,4", { "3072", "Twister (Apple)", "ARMv8", "4", "2340", "PowerVR GT7600 Plus", "6", "0", "1920 x 1080", "401", "5.50" } },
        { "iPhone9,5", { "3072", "Twister (Apple)", "ARMv8", "4", "2340", "PowerVR GT7600 Plus", "6", "0", "1920 x 1080", "401", "5.50" } },
        { "iPhone9,6", { "3072", "Twister (Apple)", "ARMv8", "4", "2340", "PowerVR GT7600 Plus", "6", "0", "1920 x 1080", "401", "5.50" } },
        { "iPad6,11", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "8", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad6,12", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "8", "0", "2048 x 1536", "264", "9.70" } },
        { "iPad7,4",  { "4096", "|Hurricane + Zephyr","ARMv8-A","6","2360", "N/A",         "12","0", "2224 x 1668", "264", "10.5" } },
    };
    
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
} // namespace detail

std::string PlatformDevice::GetDeviceId()
{
    NSString* nsDeviceId = [FCUUID uuidForDevice];
    std::string strDeviceId ="N/A";
    if(nsDeviceId)
        strDeviceId = [nsDeviceId UTF8String];
    return strDeviceId;
}

std::string PlatformDevice::GetProduct()
{
    return "Apple";
}

std::string PlatformDevice::GetHardware()
{
    return "Apple";
}

std::string PlatformDevice::GetDevice()
{
    NSString* model = [UIDeviceHardware platformString];
    return [model UTF8String];
}

std::string PlatformDevice::GetBrandName()
{
    return "Apple";
}

std::string PlatformDevice::GetModel()
{
    size_t size = 0;
    std::string strModel;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = new char[size+1];
    memset(machine, 0, size+1);
    if(sysctlbyname("hw.machine", machine, &size, NULL, 0) >= 0)
        strModel = machine;
    delete[] machine;
    return strModel;
}

std::string PlatformDevice::GetManufacturer()
{
    return "Apple Inc.";
}

std::string PlatformDevice::GetSystemVersion()
{
    NSString* systemVersion= [UIDevice currentDevice].systemVersion;
    std::string strVersion = [systemVersion UTF8String];
    int mib[2] = {CTL_KERN, KERN_OSVERSION};
    u_int nameLen = sizeof(mib)/ sizeof(mib[0]);
    size_t bufferSize= 0;
    sysctl(mib, nameLen, nullptr, &bufferSize, NULL, 0);
    char *sysVersion = new char[bufferSize+1];
    memset(sysVersion, 0, bufferSize+1);
    int result = sysctl(mib, nameLen, sysVersion, &bufferSize, NULL, 0);
    if (result >= 0)
    {
        strVersion += "(";
        strVersion += sysVersion;
        strVersion += ")";
    }
    delete[] sysVersion;
    return strVersion;
}

std::string PlatformDevice::GetSDKVersion()
{
    NSString* systemVersion= [UIDevice currentDevice].systemVersion;
    return [systemVersion UTF8String];
}

//std::string PlatformDevice::GetRendererVersion()
//{
//    char* szOpenGLVersionString = (char*)glGetString(GL_VERSION);
//    return szOpenGLVersionString?szOpenGLVersionString:"";
//}

std::string PlatformDevice::GetCPUArch()
{
    cpu_type_t  type;
    cpu_subtype_t sub_type;
    size_t type_size = sizeof(type);
    sysctlbyname("hw.cputype", &type, &type_size, NULL, 0);
    type_size = sizeof(sub_type);
    sysctlbyname("hw.cpusubtype", &sub_type, &type_size, NULL, 0);
    std::string strCpuArch;
    switch (type) {
        case CPU_TYPE_X86_64:
            strCpuArch = "x86_64";
            break;
        case CPU_TYPE_X86:
            strCpuArch = "x86";
			if(sub_type ==CPU_TYPE_X86_64|| sub_type == CPU_SUBTYPE_X86_64_H)
			{
				strCpuArch += "_64";
			}
            break;
        case CPU_TYPE_ARM:
            strCpuArch = "arm";
            switch (sub_type) {
                case CPU_SUBTYPE_ARM_V6:
                    strCpuArch += "v6";
                    break;
                case CPU_SUBTYPE_ARM_V8:
                    strCpuArch += "v8";
                    break;
                default:
                    strCpuArch += "v7s";
                    break;
            }
            break;
        case CPU_TYPE_ARM64:
            strCpuArch += "arm64";
            break;
        default:
            strCpuArch += "N/A";
            break;
    }
    return strCpuArch;
}

int PlatformDevice::GetCPUCoreCount()
{
    return static_cast<int>([[NSProcessInfo processInfo] processorCount]);
}

int PlatformDevice::GetCPUFrequency()
{
    int cpuFreq = 0;
    std::string model = [[UIDeviceHardware platform] UTF8String];
    auto modelIter =detail::AppleDeviceData.find(model);
    if(modelIter != detail::AppleDeviceData.end())
    {
        cpuFreq = ::atoi(modelIter->second[detail::DeviceDataFeild::CPU_CLOCK].c_str());
        
    }
    return cpuFreq;
}

int PlatformDevice::GetNetworkType()
{
    Reachability_libo* reachability = [Reachability_libo reachabilityForInternetConnection];
    [reachability startNotifier];
    NetworkStatus status = [reachability currentReachabilityStatus];
    int result = 0;
    switch (status)
    {
        case ReachableViaWWAN:
        {
            CTTelephonyNetworkInfo *info = [[CTTelephonyNetworkInfo alloc] init];
            NSString *radioAccessTechnology = info.currentRadioAccessTechnology;
            if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyGPRS]) {
                result = 2;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyEdge]) {
                result =  2;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyWCDMA]) {
                result =  3;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyHSDPA]) {
                result =  3;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyHSUPA]) {
                result =  3;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyCDMA1x]) {
                result =  3;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyCDMAEVDORev0]) {
                result =  3;;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyCDMAEVDORevA]) {
                result =  3;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyCDMAEVDORevB]) {
                result =  3;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyeHRPD]) {
                result =  3;
            } else if ([radioAccessTechnology isEqualToString:CTRadioAccessTechnologyLTE]) {
                result =  4;
            }
            [info release];
            break;
        }
        case ReachableViaWiFi:
        {
            result =  1;
            break;
        }
        default:
            break;
    }
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
    auto scale =  [[UIScreen mainScreen] scale];
    CGSize renderedSize =[UIScreen mainScreen].bounds.size;
    Size resolution = Size(renderedSize.width*scale , renderedSize.height*scale);
    Rect bounds;
    bounds.size = resolution;
    bounds.origin.Set(0.0f, 0.0f);
    return bounds;
}

Rect PlatformDevice::GetScreenNativeResolution()
{
    Rect bounds;
    if ([[UIScreen mainScreen] respondsToSelector:@selector(nativeScale)])
    {
        auto scale =  [[UIScreen mainScreen] nativeScale];
        CGRect physicalRect =[UIScreen mainScreen].bounds;
        bounds.size.SetSize(physicalRect.size.width*scale , physicalRect.size.height*scale);
        bounds.origin.Set(physicalRect.origin.x, physicalRect.origin.y);
    }
    else
    {
        bounds = GetScreenResolution();
    }
    return bounds;
}

float PlatformDevice::GetScreenDPI()
{
    float ppi = 0.0f;
    std::string model = [[UIDeviceHardware platform] UTF8String];
    auto modelIter =detail::AppleDeviceData.find(model);
    if(modelIter != detail::AppleDeviceData.end())
    {
        ppi = ::atof(modelIter->second[detail::DeviceDataFeild::SCREEN_PPI].c_str());
        
    }
    return ppi;
}

float PlatformDevice::GetRefreshRate()
{
    return 60.0f;
}

float PlatformDevice::GetScreenXDPI()
{
    return GetScreenDPI();
}

float PlatformDevice::GetScreenYDPI()
{
    return GetScreenDPI();
}

float PlatformDevice::GetNativeScale()
{
    if ([[UIScreen mainScreen] respondsToSelector:@selector(nativeScale)])
        return [[UIScreen mainScreen] nativeScale];
    else
        return [[UIScreen mainScreen] scale];
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
    vm_statistics64 Stats;
    mach_msg_type_number_t StatsCount = HOST_VM_INFO64_COUNT;
    host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&Stats, &StatsCount);
    
//    int64 TotalPhysical = 0;
//    size_t Length = sizeof(int64);
//    int Mib[] = {CTL_HW, HW_MEMSIZE};
//    sysctl(Mib, 2, &TotalPhysical, &Length, NULL, 0);
//    sysctlbyname("hw.memsize", NULL, &Length, NULL, 0);
//    sysctlbyname("hw.memsize", &TotalPhysical, &Length, NULL, 0);
    
    mach_task_basic_info_data_t TaskInfo;
    mach_msg_type_number_t TaskInfoCount = MACH_TASK_BASIC_INFO_COUNT;
    task_info( mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&TaskInfo, &TaskInfoCount );
    
    uint64_t FreeMem = Stats.free_count * PageSize;
    uint64_t UsedMem = (Stats.active_count + Stats.inactive_count + Stats.wire_count) * PageSize;
    uint64_t TotalPhys = FreeMem + UsedMem;

    static mach_vm_size_t PeakUsedPhysical = 0;
    static mach_vm_size_t PeakUsedVirtual = 0;
    if (TaskInfo.resident_size > PeakUsedPhysical)
    {
        PeakUsedPhysical = TaskInfo.resident_size;
    }
    if (TaskInfo.virtual_size > PeakUsedPhysical)
    {
        PeakUsedVirtual = TaskInfo.virtual_size;
    }
    
    MemoryConstants.TotalPhysical     = TotalPhys;
    MemoryConstants.TotalVirtual      = TotalPhys + SwapUsage.xsu_total;
    MemoryConstants.PageSize          = PageSize;
    MemoryConstants.AvailablePhysical = FreeMem;
    MemoryConstants.AvailableVirtual  = FreeMem + SwapUsage.xsu_avail;
    MemoryConstants.UsedPhysical      = TaskInfo.resident_size;
    MemoryConstants.UsedVirtual       = TaskInfo.virtual_size;
    MemoryConstants.PeakUsedPhysical  = PeakUsedPhysical;
    MemoryConstants.PeakUsedVirtual   = PeakUsedVirtual;
    return MemoryConstants;
}

void PlatformDevice::DumpDeviceInfo()
{
    FKLog("============ Device Info===============");
    std::ostringstream ss;
    ss << "GetDeviceId:" << GetDeviceId() << "\n";
    ss << "GetDeviceId:" << GetProduct() << "\n";
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
    char buffer[256+1]={0};
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe))
        {
            if (fgets(buffer, 256, pipe) != NULL)
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


bool PlatformDevice::ScreenShot(std::string& outSavePath)
{
    NSMutableArray *windows = [UIApplication sharedApplication].windows.mutableCopy;
    UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;
    if (![windows containsObject:keyWindow])
    {
        [windows addObject:keyWindow];
    }
    if (windows.count == 0)
    {
        
        NSLog(@"***** Could not take screenshot.  No windows were available.");
        [windows release];
        return false;
    }
    
    UIGraphicsBeginImageContextWithOptions([[windows objectAtIndex:0] bounds].size, YES, 0);
    for (UIWindow *window in windows)
    {
        if (window.hidden)
        {
            continue;
        }
        
        if ([window respondsToSelector:@selector(drawViewHierarchyInRect:afterScreenUpdates:)])
        {
            [window drawViewHierarchyInRect:window.bounds afterScreenUpdates:YES];
        }
        else
        {
            [window.layer renderInContext:UIGraphicsGetCurrentContext()];
        }
    }
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    [windows release];
    std::string filename = "ScreenShot_";
    DateTime  dateNow = DateTime::Now();
    filename +=  dateNow.ToString("%Y%m%d%H%M%S");
    filename += ".jpg";
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *savePath = [paths objectAtIndex : 0];
    NSString *imageName = [NSString stringWithUTF8String:filename.c_str()];
    savePath = [savePath stringByAppendingPathComponent:imageName];
    NSData * imageData = UIImageJPEGRepresentation(image,1.0);
    [imageData writeToFile:savePath atomically:YES];
    outSavePath = [savePath UTF8String];
    NSLog(@"***** ScreenShot completed, path:%@", savePath);
    return true;
}


NS_FK_END

#endif // OF #if (PLATFORM_IOS)


