//
//  AppDelegate.m
//  FoundationKitTest
//
//  Created by libo on 15/9/18.
//  Copyright (c) 2015年 libo. All rights reserved.
//

#import "AppDelegate.h"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/Rect.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/Platform/FileUtils.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include "FoundationKit/Base/type_cast.hpp"
#include "FoundationKit/Base/types.hpp"
#import <Foundation/Foundation.h>

USING_NS_FK;

using namespace FoundationKit;

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    std::string strLogPaht = FileUtils::GetInstance()->GetWritablePath();
    strLogPaht+= "FoundationKit.log";
    Logger::GetInstance()->Initialize(strLogPaht);
    LOG_INFO("==== Thread id:%d", PlatformTLS::GetCurrentThreadId());
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    DateTime dateTime = DateTime::Now();
    std::string fileName = dateTime.ToString();
    fileName += ".jpg";
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    FoundationKit::Rect rc(0,0, screenRect.size.width, screenRect.size.height);
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
