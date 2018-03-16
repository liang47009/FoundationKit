//
//  AppDelegate.m
//  FoundationKitTest
//
//  Created by libo on 15/9/18.
//  Copyright (c) 2015年 libo. All rights reserved.
//

#import "AppDelegate.h"
#import <Foundation/Foundation.h>
#include <sys/sysctl.h>

#include "FoundationKit/forward.hpp"
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/GenericPlatformMacros.hpp"

#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Crypto/sha1.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/BitStream.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/Version.hpp"

#include "FoundationKit/Math/Math.hpp"
#include "FoundationKit/Math/MathContent.hpp"
#include "FoundationKit/Math/MathUtil.hpp"
#include "FoundationKit/Math/Matrix.hpp"
#include "FoundationKit/Math/Quaternion.hpp"
#include "FoundationKit/Math/Rect.hpp"
#include "FoundationKit/Math/Size.hpp"
#include "FoundationKit/Math/Vector2.hpp"
#include "FoundationKit/Math/Vector3.hpp"
#include "FoundationKit/Math/Vector4.hpp"

#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"

//#include "rapidjson/"

USING_NS_FK;

std::string GetOSStringData(int key, int type)
{
    int mib[2] = {key, type};
    u_int nameLen = sizeof(mib)/ sizeof(mib[0]);
    size_t bufferSize= 0;
    sysctl(mib, nameLen, nullptr, &bufferSize, NULL, 0);
    char *sysVersion = new char[bufferSize+1];
    memset(sysVersion, 0, bufferSize+1);
    sysctl(mib, nameLen, sysVersion, &bufferSize, NULL, 0);
    return sysVersion;
}

size_t GetOSIntData(int key, int type)
{
    int mib[2] = {key, type};
    u_int nameLen = sizeof(mib)/ sizeof(mib[0]);
    size_t valueSize= 0;
    sysctl(mib, nameLen, nullptr, &valueSize, NULL, 0);
    size_t kernValue = 0;
    sysctl(mib, nameLen, &kernValue, &valueSize, NULL, 0);
    return kernValue;
}

@interface AppDelegate ()

@end

@implementation AppDelegate

//#define    HW_MACHINE     1        /* string: machine class */
//#define    HW_MODEL     2        /* string: specific machine model */
//#define    HW_NCPU         3        /* int: number of cpus */
//#define    HW_BYTEORDER     4        /* int: machine byte order */
//#define    HW_PHYSMEM     5        /* int: total memory */
//#define    HW_USERMEM     6        /* int: non-kernel memory */
//#define    HW_PAGESIZE     7        /* int: software page size */
//#define    HW_DISKNAMES     8        /* strings: disk drive names */
//#define    HW_DISKSTATS     9        /* struct: diskstats[] */
//#define    HW_EPOCH      10        /* int: 0 for Legacy, else NewWorld */
//#define HW_FLOATINGPT    11        /* int: has HW floating point? */
//#define HW_MACHINE_ARCH    12        /* string: machine architecture */
//#define HW_VECTORUNIT    13        /* int: has HW vector unit? */
//#define HW_BUS_FREQ    14        /* int: Bus Frequency */
//#define HW_CPU_FREQ    15        /* int: CPU Frequency */
//#define HW_CACHELINE    16        /* int: Cache Line Size in Bytes */
//#define HW_L1ICACHESIZE    17        /* int: L1 I Cache Size in Bytes */
//#define HW_L1DCACHESIZE    18        /* int: L1 D Cache Size in Bytes */
//#define HW_L2SETTINGS    19        /* int: L2 Cache Settings */
//#define HW_L2CACHESIZE    20        /* int: L2 Cache Size in Bytes */
//#define HW_L3SETTINGS    21        /* int: L3 Cache Settings */
//#define HW_L3CACHESIZE    22        /* int: L3 Cache Size in Bytes */
//#define HW_TB_FREQ    23        /* int: Bus Frequency */
//#define HW_MEMSIZE    24        /* uint64_t: physical ram size */
//#define HW_AVAILCPU    25        /* int: number of available CPUs */
//#define    HW_MAXID    26        /* number of valid hw ids */
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    FKLog("==== Thread id:%d", PlatformTLS::GetCurrentThreadId());
    auto ipv4 = PlatformDevice::GetIpAddressV4();
    auto ipv6 = PlatformDevice::GetIpAddressV6();
    PlatformDevice::DumpDeviceInfo();
    FKLog("HW_MACHINE:%s", GetOSStringData(CTL_HW, HW_MACHINE).c_str());
    FKLog("HW_MODEL:%s", GetOSStringData(CTL_HW, HW_MODEL).c_str());
    FKLog("HW_AVAILCPU:%d", GetOSIntData(CTL_HW, HW_AVAILCPU));
    FKLog("HW_NCPU:%d", GetOSIntData(CTL_HW, HW_NCPU));
    FKLog("HW_PHYSMEM:%lld", GetOSIntData(CTL_HW, HW_PHYSMEM));
    FKLog("HW_DISKNAMES:%s", GetOSStringData(CTL_HW, HW_DISKNAMES).c_str());
    FKLog("HW_MACHINE_ARCH:%s", GetOSStringData(CTL_HW, HW_MACHINE_ARCH).c_str());
    FKLog("HW_CPU_FREQ:%d", GetOSIntData(CTL_HW, HW_CPU_FREQ));
    FKLog("HW_CACHELINE:%d", GetOSIntData(CTL_HW, HW_CACHELINE));
    FKLog("HW_L1ICACHESIZE:%d", GetOSIntData(CTL_HW, HW_L1ICACHESIZE));
    FKLog("HW_L1DCACHESIZE:%d", GetOSIntData(CTL_HW, HW_L1DCACHESIZE));
    FKLog("HW_L2SETTINGS:%d", GetOSIntData(CTL_HW, HW_L2SETTINGS));
    FKLog("HW_L2CACHESIZE:%d", GetOSIntData(CTL_HW, HW_L2CACHESIZE));
    FKLog("HW_L3SETTINGS:%d", GetOSIntData(CTL_HW, HW_L3SETTINGS));
    FKLog("HW_L3CACHESIZE:%d", GetOSIntData(CTL_HW, HW_L3CACHESIZE));
    FKLog("HW_MEMSIZE:%lld", GetOSIntData(CTL_HW, HW_MEMSIZE));
    
    size_t DeviceIDLen;
    sysctlbyname("hw.machine", NULL, &DeviceIDLen, NULL, 0);
    char* DeviceID = (char*)malloc(DeviceIDLen);
    sysctlbyname("hw.machine", DeviceID, &DeviceIDLen, NULL, 0);
    
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.

}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.

}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    std::string path;
    PlatformDevice::ScreenCapture(path);
}

@end
