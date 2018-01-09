
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>
#include <utility>

#include "AppDelegate.h"

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
#include "FoundationKit/Foundation/Version.hpp"

#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"


#include "HTTPClient/HTTPClient.hpp"



using namespace std;
USING_NS_FK;


static bool bExitApp = false;

AppDelegate::AppDelegate() 
{

}

AppDelegate::~AppDelegate() 
{

}



void AppDelegate::applicationDidLaunching()
{

}


enum LinkerInitialized 
{
    kLinkerInitialized = 0,
};


bool AppDelegate::applicationDidFinishLaunching() 
{
	std::error_code ec;
	std::string strErr = ec.message();

    std::string strPath = "E:\\tmp\\ÖØÃüÃû.bat";
    DateTime dt0 = File::GetCreationTime(strPath);
    DateTime dt1 = File::GetCreationTimeUtc(strPath);
    DateTime dt2 = File::GetLastAccessTime(strPath);
    DateTime dt3 = File::GetLastAccessTimeUtc(strPath);
    DateTime dt4 = File::GetLastWriteTime(strPath);
    DateTime dt5 = File::GetLastWriteTimeUtc(strPath);

    FKLog("dt0=%s", dt0.ToString().c_str());
    FKLog("dt1=%s", dt1.ToString().c_str());
    FKLog("dt2=%s", dt2.ToString().c_str());
    FKLog("dt3=%s", dt3.ToString().c_str());
    FKLog("dt4=%s", dt4.ToString().c_str());
    FKLog("dt5=%s", dt5.ToString().c_str());




	int im_a_breakpoint = 0;
	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() 
{

}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() 
{

}

void AppDelegate::applicationWillTerminate()
{
	bExitApp = true;
}

void AppDelegate::mainLoop()
{
    HTTPClient::GetInstance()->Tick(0.016);
}

void AppDelegate::TestTupleArgs(int a, const std::string & str, const char * str1)
{
}
