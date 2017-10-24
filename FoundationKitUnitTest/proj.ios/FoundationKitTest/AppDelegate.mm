//
//  AppDelegate.m
//  FoundationKitTest
//
//  Created by libo on 15/9/18.
//  Copyright (c) 2015年 libo. All rights reserved.
//

#import "AppDelegate.h"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/Rect.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Base/function_traits.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#import <Foundation/Foundation.h>

#include <sys/sysctl.h>

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


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    FKLog("==== Thread id:%d", PlatformTLS::GetCurrentThreadId());
    auto lines = Environment::GetCommandLineArgs();
    auto envs = Environment::GetEnvironmentVariables();
    auto path = Environment::GetEnvironmentVariable("PATH");
    auto envs1 = [[NSProcessInfo processInfo]environment];
    auto appPath = Path::GetApplicationPath();
    PlatformDevice::DumpDeviceInfo();
    auto strValue = GetOSStringData(CTL_HW, HW_MODEL);
    auto intValue = GetOSIntData(CTL_HW, HW_CPU_FREQ);
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
    Platform::ScreenShot(path);
}

@end
