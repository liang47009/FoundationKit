#include "tracey.hpp"
#include <string>
#include <stdint.h>

#include "AppDelegate.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Base/DataStream.h"
#include "FoundationKit/Base/Data.h"
#include "FoundationKit/Base/Buffer.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Foundation/Scheduler.h"
#include "FoundationKit/Foundation/Version.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/LuaEngine/LuaEngine.h"
#include "FoundationKit/Events/EventDispatcher.h"
#include "FoundationKit/Events/EventCustom.h"
#include "FoundationKit/Events/EventListenerCustom.h"
#include "FoundationKit/Foundation/ApplyTuple.h"
#include "FoundationKit/Foundation/TupleApply.h"
#include "FoundationKit/Platform/Platform.h"
#include "FoundationKit/std/memory_aligned.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/experimental/any.hpp"
#include "FoundationKit/experimental/optional.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/Foundation/FunctionCenter.hpp"
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

#include "FoundationKit/Base/TimeEx.h"
#include "FoundationKit/Base/DateTime.h"

#include "FoundationKit/experimental/memory.h"
#include "FoundationKit/Networking/network.hpp"



using namespace std;
USING_NS_FK;


static Scheduler* shared_Scheduler = nullptr;

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

    network::ip::tcp::socket client;
  
    client.open(network::ip::tcp::v4());
    ip::tcp::endpoint endpoint(ip::address::from_string("172.17.14.116"), 2001);
    client.connect(endpoint);
    char* pbuf = "this is first message";
    mutable_data md(pbuf, strlen(pbuf));
    client.send(md);

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
	shared_Scheduler->update(1/60);
}

