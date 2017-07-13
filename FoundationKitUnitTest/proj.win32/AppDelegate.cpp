
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

static std::string compressFile(const std::string& source)
{
    size_t findPos = source.find_last_of(".");
    if (findPos == std::string::npos)
    {
        return source;
    }

    std::string destFile = source.substr(0, findPos + 1);
    destFile += "gz";
    if (Compression::CompressFile(source, destFile))
    {
        return destFile;
    }
    return source;
}

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

    HTTPDownloadRequest::Pointer downloadRequest = HTTPDownloadRequest::Create();
    downloadRequest->SetURL("https://download.sublimetext.com/Sublime%20Text%20Build%203126%20x64%20Setup.exe");
    FILE* fp = fopen("E:\\tmp\\SublimeSetup.exe", "wb");
    downloadRequest->SetFileHandle(fp);
    downloadRequest->OnRequestCompleted = [](HTTPRequest::Pointer request, HTTPResponse::Pointer response)
    {
        if (response->IsSucceeded())
        {
        }
        HTTPDownloadRequest::Pointer requestdownload = std::dynamic_pointer_cast<HTTPDownloadRequest>(request);
        fclose(requestdownload->GetFileHandle());
    };
    downloadRequest->OnRequestProgress = [](HTTPRequest::Pointer request, int64 nowSize, int64 totalSize)
    {
        FKLog("=========== %lld/%lld, %0.2f%%", nowSize, totalSize, nowSize*1.0f / totalSize*100);
    };
    HTTPClient::GetInstance()->PostRequest(downloadRequest);
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
