
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>

#include "AppDelegate.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Base/DataStream.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Foundation/Scheduler.h"
#include "FoundationKit/Foundation/Version.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/LuaEngine/LuaEngine.h"
#include "FoundationKit/Foundation/ApplyTuple.h"
#include "FoundationKit/Foundation/TupleApply.h"
#include "FoundationKit/Platform/Platform.h"
#include "FoundationKit/std/memory_aligned.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/experimental/any.hpp"
#include "FoundationKit/experimental/optional.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/unique_id.hpp"
#include "FoundationKit/Base/DateTime.h"
#include "FoundationKit/Platform/PlatformTLS.h"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/Networking/ip/address.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/Base/Dictionary.h"
#include "FoundationKit/Base/MathEx.h"
#include "FoundationKit/Crypto/Base64.h"
#include "FoundationKit/Base/ElapsedTimer.h"
#include "FoundationKit/Base/Timer.h"
#include "FoundationKit/Foundation/Compression.h"
#include "FoundationKit/Base/TimerQueue.h"
#include "FoundationKit/HttpDownloader/DownloaderTest.h"
#include "FoundationKit/Networking/network.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPClient.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPRequest.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPResponse.hpp"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

using namespace std;
USING_NS_FK;

static Scheduler* shared_Scheduler = nullptr;
static TimerQueue  G_TimerQueue;
static bool bExitApp = false;

static std::thread clientThread;
static  std::thread serverThread;
void runServer();
void runClient();




AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}


void AppDelegate::applicationDidLaunching()
{
	shared_Scheduler = Scheduler::getInstance();
    HTTPClient::initialize();

}


bool AppDelegate::applicationDidFinishLaunching() 
{
    std::error_code ec;
    std::string strErr = ec.message();

    // http client Test
    ///*
    HTTPRequest::Pointer request = HTTPRequest::create(false);
    request->setURL("http://dl2.youme.im/release/youme-rtc-2.4.1.2442_android.cn.zip");
    //request->setMethod(HTTPRequest::MethodType::POST);
    //request->setURL("https://crashlogs1.woniu.com/crashlogs/api/comm/cpp");
    request->onRequestCompleteDelegate = [](HTTPRequest::Pointer pRequest, HTTPResponse::Pointer pResponse, bool ableConn)
    {
        pRequest->dumpInfo();
        pResponse->dumpInfo();
        if (pResponse->isSucceeded())
        {
            auto responseData = pResponse->getContentData();
            mutable_buffer data(&responseData[0], responseData.size());
            FileUtils::getInstance()->writeDataToFile(data, "E:\\temp\\result.html");
        }
        else
        {
            auto responseData = pResponse->getContentData();
            if (responseData.size()>0)
            {
                mutable_buffer data(&responseData[0], responseData.size());
                FileUtils::getInstance()->writeDataToFile(data, "E:\\temp\\result.html");
            }
        }
    };

    static ElapsedTimer downloadET;
    static int64 currentDownloadSize = 0;
    request->onRequestProgressDelegate = [&](HTTPRequest::Pointer pRequest
        , int64 totalDownload
        , int64 nowDownload
        , int64 totalUpload
        , int64 nowUpload)
    {
        if (nowDownload == totalDownload && totalDownload != 0)
        {
            LOG_INFO("Download speed:%0.2fMB/s, process:%0.2f%%", 0.0f, 100.0f);
        }
        else if (downloadET.seconds()>=1)
        {
            double downloadSpeed = (nowDownload - currentDownloadSize) / 1024.f/1024.f;
            double downloadRate = nowDownload* 100.0f / totalDownload;
            LOG_INFO("Download speed:%0.2fMB/s, process:%0.2f%%", downloadSpeed, downloadRate);
            currentDownloadSize = nowDownload;
            downloadET.reset();
        }
    };
    HTTPClient::getInstance()->sendRequestAsync(request);
    return true;



    std::unordered_map<std::string, std::string >g_uploadParameters;
    g_uploadParameters["channelId"] = "10000";
    g_uploadParameters["lifespan"] = "0000-0000-0000-0000-0000";
    g_uploadParameters["sceneid"] = "23";
    g_uploadParameters["userAccount"] = "libo";
    g_uploadParameters["version"] = "1.0.0";
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
        writer.Key(iter.first.c_str()); writer.String(iter.second.c_str());
    }
    writer.EndObject();
    std::string jsonString = sb.GetString();
    request->setContentField("params", jsonString);
    request->setFileField("file", "E:\\temp\\crash.dmp");
    request->setFileField("traceFile", "E:\\temp\\trace.log");
    HTTPClient::getInstance()->sendRequestAsync(request);
    //*/


    //clientThread = std::thread([]()
    //{
    //    runClient();
    //});

    //serverThread = std::thread([]()
    //{
    //    runServer();
    //});
    
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

    if (clientThread.joinable())
    {
        clientThread.join();
    }

    if (serverThread.joinable())
    {
        serverThread.join();
    }
}

void AppDelegate::mainLoop()
{
 	shared_Scheduler->update(1/60.f);
    HTTPClient::getInstance()->tick(1 / 60.f);
}


void runServer()
{

    network::ip::tcp::socket serverSocket;
    serverSocket.open(network::ip::tcp::v4());
    serverSocket.set_non_blocking(true);
    serverSocket.bind(network::ip::tcp::endpoint(ip::tcp::v4(), 4215));
    serverSocket.listen();

    std::vector<network::ip::tcp::socket> clientList;
    while (!bExitApp)
    {
        std::error_code ec;
        if (serverSocket.has_pending_accept(ec))
        {
            ip::tcp::endpoint endpoint;
            network::ip::tcp::socket clientSocket = serverSocket.accept(endpoint);
            LOG_INFO("===== Client: %d, ip:%s", clientSocket.native_handle(), endpoint.address().to_string().c_str());

            std::string sendtoClient = "Server Accept Client.";
            clientSocket.send(const_buffer(sendtoClient.c_str(), sendtoClient.size()));

            while (true)
            {
                if (clientSocket.available()>0)
                {
                    mutable_buffer mb;
                    mb.reallocate(128);
                    std::error_code ec;
                    clientSocket.receive(mb, ec);
                    if (ec)
                    {
                        LOG_ERROR("%s", ec.message().c_str());
                    }
                    LOG_INFO("Client Msg:%s", mb.c_str());
                    break;
                }

            }
            clientList.push_back(std::move(clientSocket));

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

}

void runClient()
{

    std::vector<network::ip::tcp::socket> clientList;
    while (!bExitApp)
    {
        network::ip::tcp::socket clientSocket;

        clientSocket.open(network::ip::tcp::v4());
        clientSocket.set_non_blocking(true);
        ip::tcp::endpoint endpoint(network::ip::address::from_string("127.0.0.1"), 4215);
        std::error_code ec;
        clientSocket.connect(endpoint, ec);
        if (ec)
        {
            LOG_ERROR("Connect: %s", ec.message().c_str());
        }
        else
        {
            while (true)
            {
                if (clientSocket.available()>0)
                {
                    mutable_buffer mb;
                    mb.reallocate(128);
                    clientSocket.receive(mb);
                    LOG_INFO("Server Msg:%s", mb.c_str());
                    std::string sendtoServer = " I receive your message.";
                    clientSocket.send(const_buffer(sendtoServer.c_str(), sendtoServer.size()));
                    break;
                }

            }
            clientList.push_back(std::move(clientSocket));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}
