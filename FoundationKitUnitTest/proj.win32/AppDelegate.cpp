#include "tracey.hpp"
#include <string>
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
#include "FoundationKit/stdextensions/aligned_ptr.hpp"
#include "FoundationKit/stdextensions/any.hpp"
#include "FoundationKit/stdextensions/function_traits.hpp"
#include "FoundationKit/stdextensions/optional.hpp"
#include "FoundationKit/stdextensions/utility.hpp"
#include "FoundationKit/stdextensions/utility.hpp"
#include "FoundationKit/Foundation/FunctionCenter.hpp"
#include "FoundationKit/stdextensions/delegate.h"
#include "FoundationKit/Base/CompilerMacros.h"
#include "FoundationKit/Base/Timer.h"

//Test header
#include "FoundationKit/HttpDownloader/DownloaderTest.h"
#include "../../HttpClient/HttpClientTest.h"

#include <stdint.h>

#include <chrono>
using namespace std::chrono;

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
    //TestHttpClientWithUploadDumpInfo();


    std::vector<std::string> files;
    FileUtils::getInstance()->getFilesFromDir("D:/UAF/UniversalApplicationFramework_C99/libapplicationkit/ApplicationKit", files, true);
    FileUtils::getInstance()->getFilesFromDir("D:/UAF/UniversalApplicationFramework_C99/libapplicationkit/FoundationKit", files, true);

    std::string result;
    std::string ext = ".cpp";
    std::string fileExt;
    for (auto& str : files)
    {
        fileExt = FileUtils::getInstance()->getFileExtension(str);
        if (fileExt == ext || fileExt == ".c")
        {
            size_t npos = str.find("t/");
            result.append(str.substr(npos + 2, str.size()));
            result.push_back('\n');
        }
    }

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
