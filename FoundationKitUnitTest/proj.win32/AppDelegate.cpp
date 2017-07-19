
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
#include "FoundationKit/Platform/FileUtils.hpp"
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

std::unordered_map<std::string, std::string> g_uploadParameters;

void OnHttpCallback(HTTPRequest::Pointer request, HTTPResponse::Pointer response)
{
    HTTPCode errorCode = response->GetResponseCode();
    if (response->IsSucceeded() && errorCode == 200)
    {
        auto headers = response->GetAllHeaders();
        std::string strHeader;
        for (auto str : headers)
        {
            strHeader += str;
            strHeader += "\n";
        }
        FKLog("=== Request succeed CODE:%s, HEADER:\n %s", errorCode.ToString().c_str(), strHeader.c_str());
    }
    else
    {
        std::string strError = response->GetErrorMsg();
        auto headers = response->GetAllHeaders();
        std::string strHeader;
        for (auto str : headers)
        {
            strHeader += str;
            strHeader += "\n";
        }
        FKLog("=== Request failed CODE:%s, HEADER:\n %s", errorCode.ToString().c_str(), strHeader.c_str());
        FKLog("=== Request failed errorBuf:%s", strError.c_str());
        FKLog("=== Request failed responsemsg:%s", response->GetResponseMsg().c_str());
    }
}



bool AppDelegate::applicationDidFinishLaunching() 
{
    std::error_code ec;
    std::string strErr = ec.message();
    HTTPClient::GetInstance()->Initialize();



    std::string strFilePath1 = "E:/WorkSpace/GameToolsGroup/DeviceMonitor/publish/android/README1.md";
    File::Delete(strFilePath1);

    std::string strFilePath = "E:\\WorkSpace\\GameToolsGroup\\DeviceMonitor\\publish\\android\\readme.txt";
    mutable_buffer filedata = File::ReadAllBytes(strFilePath);
    std::string str = File::ReadAllText(strFilePath);

    std::wstring wstr = StringUtils::string2UTF8wstring(str);

    FILE* FileHandle = FileHandle = fopen(strFilePath.c_str(), "rt");
    fseek(FileHandle, 0, SEEK_END);
    size_t FileSize = ftell(FileHandle);
    fseek(FileHandle, 0, SEEK_SET);
    char* fd = new char[FileSize + 1];
    memset(fd, 0, FileSize + 1);
    fread(fd, 1, FileSize, FileHandle);
    fclose(FileHandle);


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
    HTTPClient::GetInstance()->Tick(1 / 60.0f);
    HTTPDownloader::GetInstance()->Tick(1 / 60.0f);
}
