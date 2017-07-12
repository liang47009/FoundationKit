
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

void OnHttpCallback(HTTPRequest::Pointer request, HTTPResponse::Pointer response , bool ret)
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
    g_uploadParameters["channelId"] = "10000";
    g_uploadParameters["lifespan"] = "NONE";
    g_uploadParameters["sceneid"] = "2";
    g_uploadParameters["userAccount"] = "LOSEMYMIND";
    g_uploadParameters["version"] = "NONE";
    g_uploadParameters["gameId"] = "66";
    g_uploadParameters["roleName"] = "NONE";
    g_uploadParameters["guid"] = "NONE";
    g_uploadParameters["device"] = "NONE";
    g_uploadParameters["ptime"] = "NONE";
    g_uploadParameters["osver"] = "NONE";
    g_uploadParameters["fmemory"] = "NONE";
    g_uploadParameters["arch"] = "NONE";
    g_uploadParameters["type"] = "NONE";
    g_uploadParameters["sdkVersion"] = "NONE";
    g_uploadParameters["totalMemory"] = "NONE";
    g_uploadParameters["isRoot"] = "0";
    g_uploadParameters["crashPackage"] = "NONE";


    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();

    for (auto& iter : g_uploadParameters)
    {
        std::string key = iter.first;
        if (key == "isRoot")
        {
            writer.Key(key.c_str()); writer.Int(atoi(iter.second.c_str()));
        }
        else
        {
            writer.Key(key.c_str()); writer.String(iter.second.c_str());
        }
    }
    writer.EndObject();
    std::string jsonString = sb.GetString();

    int reqsize = sizeof(HTTPRequest);

    HTTPRequest::Pointer pRequest = HTTPRequest::Create(false);
    pRequest->SetContentField("params", jsonString);
    pRequest->SetFileField("file", compressFile("E:\\GitHub\\FoundationKit\\Win32\\dump.dmp"));
    pRequest->SetFileField("traceFile", compressFile("E:\\GitHub\\FoundationKit\\Win32\\tracelog.log"));
    pRequest->SetURL("https://crashlogs.woniu.com/crashlogs/api/comm/cpp");
    pRequest->SetMethod(RequestMethodType::POST);
    pRequest->OnRequestCompleted = OnHttpCallback;
    //HTTPClient::GetInstance()->PostRequest(pRequest);

    HTTPUploadRequest::Pointer UploadRequest = HTTPUploadRequest::Create(true);
    UploadRequest->SetURL("http://172.19.10.239:8080/uploadfiles/dump.dmp");
    UploadRequest->SetMethod(RequestMethodType::PUT);
    UploadRequest->SetFilePath("E:\\GitHub\\FoundationKit\\Win32\\dump.dmp");
    UploadRequest->OnRequestCompleted = OnHttpCallback;
    UploadRequest->OnRequestProgress = [](HTTPUploadRequest::Pointer request, int64 nowUpload, int64 totalUpload)
    {
    
    };
    HTTPClient::GetInstance()->PostRequest(UploadRequest);


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
}
