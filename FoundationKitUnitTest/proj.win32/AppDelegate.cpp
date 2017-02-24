
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
#include "FoundationKit/std/any.hpp"
#include "FoundationKit/std/optional.hpp"
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
#include "FoundationKit/Platform/FileUtils.h"
#include "FoundationKit/Base/TimerQueue.h"
#include "FoundationKit/Networking/network.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPClient.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPRequest.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPResponse.hpp"
#include "FoundationKit/Networking/HTTPDownloader/HTTPDownloader.hpp"

using namespace std;
USING_NS_FK;
using namespace network;

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

    static DownloadListener  downloadListener;
    static Timer::pointer   processTimer = Timer::create(1000);
    static int64 g_nowDownlaod=0; 
    static int64 g_totalDownload = 0;
    static int64 g_preDownlaod = 0;
    
    processTimer->onTimedEvent = [&](const TimedEventArgs& timerArgs)
    {
        if (g_totalDownload !=0)
        {
            double speed = (g_nowDownlaod - g_preDownlaod*1.0f)/1024/1024;
			LOG_INFO(" ********** PROCESS:%0.2f, Speed:%0.2f MB [%lld/%lldMB]", g_nowDownlaod*1.0f / g_totalDownload*100.0f, speed, g_nowDownlaod/1024/1024, g_totalDownload/1024/1024);
        }
        g_preDownlaod = g_nowDownlaod;
    };
    processTimer->startInThread();
    downloadListener.onErrorCallback = [](const std::string& url, const std::string& errMsg)
    {
        LOG_INFO(" ********** URL:%s, ERRMESSAGE:%s", url.c_str(), errMsg.c_str());
    };

    downloadListener.onProgressCallback = [&](const std::string&url, int64 nowDownlaod, int64 totalDownload)
    {
        g_nowDownlaod = nowDownlaod;
        g_totalDownload = totalDownload;
    };

    downloadListener.onSucceedCallback = [](const std::string& url, const std::string& filePath)
    {
        LOG_INFO(" ********** SUCCEED URL:%s , %s", url.c_str(), filePath.c_str());
    };

    static HTTPDownloader  hd;
    hd.initialize(false);
    //hd.downloadToFile("https://www.python.org/ftp/python/2.7.13/python-2.7.13.msi", "E:\\temp", downloadListener);
    hd.downloadToFile("https://dl.google.com/android/repository/android-ndk-r13b-windows-x86_64.zip", "E:\\temp", downloadListener);
    //hd.downloadToFile("http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz", "E:\\temp", downloadListener);
    //hd.downloadToFile("http://10.206.2.142/Ark/repair/data_2017010311430002.rp", "E:\\temp", downloadListener);
    //hd.downloadToFile("http://10.206.2.142/Ark/repair/data_2017010311430055.rp", "E:\\temp", downloadListener);

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




