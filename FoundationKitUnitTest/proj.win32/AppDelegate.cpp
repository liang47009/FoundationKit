
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
#include "FoundationKit/Base/TimerQueue.hpp"
#include <iostream>

using namespace std;
USING_NS_FK;

class GenericArgument
{
public:
    inline GenericArgument(const char *aName = nullptr, const void *aData = nullptr)
        : _data(aData), _name(aName) {}
    inline void *data() const { return const_cast<void *>(_data); }
    inline const char *name() const { return _name; }

private:
    const void *_data;
    const char *_name;
};

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

}


bool AppDelegate::applicationDidFinishLaunching() 
{
    LOG_INFO(" AppDelegate::applicationDidFinishLaunching()  ");

    TimerOption option(true, 5000, 1.0f, 100, Timer::repeat_forever);
    option.onTimedEvent = [](const TimedEventArgs& args)
    {
        G_TimerQueue.erase(args.timerId);
        LOG_INFO("====== _mainTimer id:%d, deltaTime:%d ms, Date:%s", args.timerId, args.deltaTime, args.signalTime.toString().c_str());
    };
    G_TimerQueue.enqueue(option);
    G_TimerQueue.enqueue(option);
    G_TimerQueue.enqueue(option);
    G_TimerQueue.enqueue(option);
    G_TimerQueue.enqueue(option);



    /*
    clientThread = std::thread([]()
    {
        runClient();
    });

    serverThread = std::thread([]()
    {
        runServer();
    });
    */
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

}

void AppDelegate::mainLoop()
{
	shared_Scheduler->update(1/60.f);
    G_TimerQueue.update(1 / 60.f);
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