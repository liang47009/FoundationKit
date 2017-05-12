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
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#import <IOKit/IOKitLib.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

USING_NS_FK;

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    static PlatformMemoryConstants MemoryConstants = PlatformDevice::GetMemoryConstants();
    
    struct ifaddrs* interfaces = nullptr;
    struct ifaddrs* tempaddr = nullptr;
    
    NSInteger success = getifaddrs(&interfaces);
    if(success == 0)
    {
        tempaddr = interfaces;
        while (tempaddr != nullptr)
        {
            if (tempaddr->ifa_addr->sa_family == AF_INET)
            {
                const char* name = tempaddr->ifa_name;
                const char* addr = inet_ntoa(((struct sockaddr_in*)tempaddr->ifa_addr)->sin_addr);
                NSLog(@"===== name:%s, addr:%s", name, addr);
            }
            tempaddr = tempaddr->ifa_next;
        }
    }
    freeifaddrs(interfaces);
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
