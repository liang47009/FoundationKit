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
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/FileUtils.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/Path.hpp"
#import <IOKit/IOKitLib.h>
#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#include <stdlib.h>

USING_NS_FK;

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    static PlatformMemoryConstants MemoryConstants = PlatformDevice::GetMemoryConstants();
    auto lines = Environment::GetCommandLineArgs();
    auto envs = Environment::GetEnvironmentVariables();
    auto path = Environment::GetEnvironmentVariable("PATH");
    NSDictionary<NSString *, NSString *>* envs1 = [[NSProcessInfo processInfo]environment];
    auto AppPath = FileUtils::GetInstance()->GetResourceRootPath();
    auto temp = Path::GetDocumentsPath();

}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end








