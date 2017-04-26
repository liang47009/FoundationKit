
#include <string>
#include <stdint.h>
#include <iostream>

#include "AppDelegate.h"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/DataStream.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/ApplyTuple.hpp"
#include "FoundationKit/Foundation/TupleApply.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Platform/FileUtils.hpp"
#include "FoundationKit/Foundation/TimerQueue.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Base/type_cast.hpp"

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>


using namespace std;
USING_NS_FK;

static TimerQueue  G_TimerQueue;
static bool bExitApp = false;

static std::thread clientThread;
static  std::thread serverThread;
void runServer();
void runClient();

void ParseAppleDeviceData();

AppDelegate::AppDelegate() 
{

}

AppDelegate::~AppDelegate() 
{

}


void AppDelegate::applicationDidLaunching()
{

}

bool AppDelegate::applicationDidFinishLaunching() 
{

    ElapsedTimer et;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    double aaab = et.secondsf();

    std::error_code ec;
    std::string strErr = ec.message();

    std::vector<std::string>  files;
    FileUtils::getInstance()->getFilesFromDir("E:\\GitHub\\FoundationKit\\FoundationKit\\FoundationKit", files, true);
    std::string strMakefile;
    FileUtils* fileutils = FileUtils::getInstance();
    for (std::vector<std::string>::iterator iter = files.begin(); iter != files.end();++iter)
    {
        std::string filepath = *iter;
        if (fileutils->getFileExtension(filepath) == ".cpp" 
            || fileutils->getFileExtension(filepath) == ".c"
            || fileutils->getFileExtension(filepath) == ".cc")
        {
            strMakefile += filepath;
            strMakefile += "\n";
        }
    }


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

}
