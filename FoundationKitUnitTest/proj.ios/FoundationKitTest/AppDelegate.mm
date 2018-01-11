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
#include "FoundationKit/LanguageFeatures.hpp"

#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Crypto/sha1.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Foundation/ITickable.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/MathContent.hpp"
#include "FoundationKit/Foundation/MathUtil.hpp"
#include "FoundationKit/Foundation/Matrix.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Quaternion.hpp"
#include "FoundationKit/Foundation/Rect.hpp"
#include "FoundationKit/Foundation/Runnable.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/Size.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/ThreadTimer.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/Vector2.hpp"
#include "FoundationKit/Foundation/Vector3.hpp"
#include "FoundationKit/Foundation/Vector4.hpp"
#include "FoundationKit/Foundation/Value.hpp"

#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "HTTPClient/HTTPClient.hpp"

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


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    FKLog("==== Thread id:%d", PlatformTLS::GetCurrentThreadId());
    std::string APP_ID = "324d4b61-df14-11e7-b817-e0accb778420";
    std::string APP_SECURE = "pdjudrmnf9";
    std::string date = DateTime::Now().ToISO8601();
    std::string Auth = MD5::md5_hash_hex(APP_ID+APP_SECURE+ date);
    HTTPClient::GetInstance()->Initialize();
    
    HTTPRequest::Pointer requestAuth = HTTPRequest::Create(true);
    requestAuth->SetURL("http://qos.189.cn/t1?appid=324d4b61-df14-11e7-b817-e0accb778420");
    requestAuth->SetHeader("X-Request-At", date);
    requestAuth->SetHeader("X-Application-Id", APP_ID);
    requestAuth->SetHeader("X-Application-Auth", Auth);
    requestAuth->OnRequestCompleted = [](HTTPRequest::Pointer pRequest, HTTPResponse::Pointer pResponse)
    {
        if (pResponse->IsSucceeded())
        {
            auto reponseData = pResponse->GetContentData();
            std::string auth((char*)&reponseData[0], reponseData.size());
            HTTPRequest::Pointer request = HTTPRequest::Create(true);
            request->SetMethod(RequestMethodType::POST);
            request->SetURL("http://qos.189.cn/api/mobile/speeding/");
            request->SetPostField("Security_token", auth);
            request->SetPostField("dst_info", "192.168.0.1:80");
            request->SetPostField("src_info", "172.14.44.80:80");
            request->SetPostField("user_id", "snailtest;7");
            request->SetPostField("product_id", "wnts");
            request->OnRequestCompleted = [](HTTPRequest::Pointer pRequest, HTTPResponse::Pointer pResponse)
            {
                pResponse->DumpInfo();
                auto responseData = pResponse->GetContentData();
                std::string pdata((char*)&responseData[0], responseData.size());
                std::u16string ustr;
                bool ret = StringUtils::UTF8ToUTF16(pdata, ustr);
                if (pResponse->IsSucceeded())
                {
                    
                }
            };
            HTTPClient::GetInstance()->SendRequest(request);
        }
    };
    HTTPClient::GetInstance()->SendRequest(requestAuth);
    
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
