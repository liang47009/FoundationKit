
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>

#include "AppDelegate.h"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/ApplyTuple.hpp"
#include "FoundationKit/Foundation/TupleApply.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/std/memory_aligned.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/TimerQueue.hpp"
#include "FoundationKit/Foundation/DelegateManager.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Base/type_cast.hpp"
#include "FoundationKit/Base/scope_guard.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"
#include "HTTPClient/HTTPClient.hpp"
#include "HTTPClient/HTTPRequest.hpp"
#include "HTTPClient/HTTPResponse.hpp"
#include "HTTPClient/HTTPUploader.hpp"
#include "HTTPClient/HTTPDownloader.hpp"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Base/string_builder.hpp"
#include "FoundationKit/Base/scope_locale.hpp"
#include "FoundationKit/Base/unique_id.hpp"

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

enum class Operatoion
{
    Add,
    Delete,
    Modify
};

struct IndexBlock
{
    Operatoion operat;
    int64 offset;
    int64 size;
};

static ElapsedTimer downloadTimer;
static ElapsedTimer requestTime;
static int   requestCount = 100;
static int   completedCount = 0;
static HTTPDownloader::Pointer pDownloader = nullptr;
bool AppDelegate::applicationDidFinishLaunching() 
{
    std::error_code ec;
    std::string strErr = ec.message();
    HTTPClient::GetInstance()->Initialize();

    std::string strurl1 = "http://172.19.10.239:8080/Softwares/sdks/breakpad/breakpad_v2.1.1.zip";
    std::string strurl2 = "https://dl.google.com/android/repository/android-ndk-r14b-windows-x86.zip";
    std::string strurl3 = "https://dl.google.com/android/repository/android-ndk-r14b-darwin-x86_64.zip";
    pDownloader = HTTPDownloader::Create(strurl3, "E:\\tmp\\");
    pDownloader->OnCompletedHandler = [](HTTPDownloader::Pointer request)
    {
        FKLog("Completed:%s  %s", request->GetURL().c_str(), request->GetFilePath().c_str());
    };
    pDownloader->OnErrorHandler = [](HTTPDownloader::Pointer request, const std::string& errmessage)
    {
        FKLog(" Error:%s", errmessage.c_str());
    };
    pDownloader->OnProgressHandler = [](HTTPDownloader::Pointer request, int64 nowSize, int64 TotalSize)
    {
        static int64 PerDownloadSize = 0;
        FKLog(" %lld/%lld, speed:%0.2f KB Rate:%0.2f", nowSize, TotalSize, (nowSize*1.0F - PerDownloadSize)/1024, nowSize*1.0f / TotalSize*100);
        PerDownloadSize = nowSize;
    };


   
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
    pDownloader = nullptr;
}

void AppDelegate::mainLoop()
{
    HTTPClient::GetInstance()->Tick(1 / 60.0f);
    if (pDownloader)
    {
        pDownloader->Tick(1 / 60.0f);
    }
}
