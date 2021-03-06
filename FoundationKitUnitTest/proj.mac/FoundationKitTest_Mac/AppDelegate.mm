//
//  AppDelegate.m
//  FoundationKitTest_Mac
//
//  Created by libo on 2017/5/11.
//  Copyright © 2017年 libo. All rights reserved.
//

#import "AppDelegate.h"
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
#import <IOKit/IOKitLib.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>


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
#include "FoundationKit/Foundation/FunctionBox.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
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
#include "FoundationKit/Platform/PlatformTLS.hpp"


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

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    PlatformDevice::DumpDeviceInfo();
    char SwapUsage[1024] = {0};
    size_t Size = sizeof(SwapUsage);
    sysctlbyname("machdep.cpu.brand_string", SwapUsage, &Size, NULL, 0);

    std::string BundleID = [[[[NSBundle mainBundle] infoDictionary]objectForKey:@"CFBundleIdentifier"] UTF8String];

    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
