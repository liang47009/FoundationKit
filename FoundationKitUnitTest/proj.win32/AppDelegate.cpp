
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>

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
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/Base/Dictionary.h"
#include "FoundationKit/Base/MathEx.h"
#include "FoundationKit/Base/Timer.h"
#include "FoundationKit/Crypto/Base64.h"
#include "FoundationKit/Foundation/Compression.h"
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/unique_id.hpp"
//Test header
#include "FoundationKit/HttpDownloader/DownloaderTest.h"
#include "../../HttpClient/HttpClientTest.h"

#include "FoundationKit/Base/DateTime.h"

#include "FoundationKit/experimental/memory.h"
#include "FoundationKit/Networking/network.hpp"
#include "FoundationKit/Platform/PlatformTLS.h"
#include "FoundationKit/Networking/ip/address.hpp"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/Base/TimerManager.hpp"
#include <iostream>

using namespace std;
USING_NS_FK;

static Scheduler* shared_Scheduler = nullptr;
static TimerManager* shared_TimerManager = nullptr;
static bool bExitApp = false;

static std::thread clientThread;
static  std::thread serverThread;
void runServer();
void runClient();


Timer  _mainTimer;


AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}


void AppDelegate::applicationDidLaunching()
{
	shared_Scheduler = Scheduler::getInstance();
    shared_TimerManager = new TimerManager();

}


bool AppDelegate::applicationDidFinishLaunching() 
{
    LOG_INFO(" AppDelegate::applicationDidFinishLaunching()  ");
    
    _mainTimer.onTimedEvent = [](int deltaTime)
    {
        LOG_INFO("====== _mainTimer:%d", deltaTime);
    };

    _mainTimer.setInterval(1000).setTimeScale(1.0f).start();

    /*
    std::string strResourceRoot = FileUtils::getInstance()->getResourceRootPath();
    std::string strConfigFilePath = strResourceRoot + "/CopyFilesConfig.txt";
    std::string strFilesRecord = strResourceRoot + "/CopyFilesRecord.txt";
    std::vector<std::string> allConfigFileLines = FileUtils::getInstance()->readAllLines(strConfigFilePath);
    std::string strSrcRoot = allConfigFileLines[0];
    std::string strDesRoot = allConfigFileLines[1];
    std::vector<std::string> allRecordFileLines = FileUtils::getInstance()->readAllLines(strFilesRecord);
    for (auto filePath : allRecordFileLines)
    {
        FileUtils::getInstance()->copyFile(convertPathFormatToUnixStyle(strSrcRoot + filePath),
            convertPathFormatToUnixStyle(strDesRoot + filePath));
    }
    LOG_INFO(">>>>>>>>Completed files:%d", allRecordFileLines.size());
    */

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

	//memAnalyzer->displayAllocations(true, true);
	//memAnalyzer->displayStatTable();
	//std::cin.get();
	//LOG_INFO(" >>>>>>>>>>>>>>>>CurrentBlocks: %lld \n", memAnalyzer->getCurrentBlocks());
	//LOG_INFO(" >>>>>>>>>>>>>>>>CurrentMemory: %u \n", memAnalyzer->getCurrentMemory());
	//LOG_INFO(" >>>>>>>>>>>>>>>>PeakBlocks: %lld \n", memAnalyzer->getPeakBlocks());
	//LOG_INFO(" >>>>>>>>>>>>>>>>PeakMemory: %u \n", memAnalyzer->getPeakMemory());

    SAFE_DELETE(shared_TimerManager);
}

void AppDelegate::mainLoop()
{
	shared_Scheduler->update(1/60.f);
    shared_TimerManager->update(1 / 60.f);
    _mainTimer.update(1 / 60.f*1000);
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
        //clientSocket.set_non_blocking(true);
        ip::tcp::endpoint endpoint(network::ip::address::from_string("127.0.0.1"), 4215);
        std::error_code ec;
        clientSocket.connect(endpoint, ec);
        if (ec)
        {
            LOG_ERROR("Connect: %s", ec.message().c_str());
        }
        else
        {
            clientList.push_back(std::move(clientSocket));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}