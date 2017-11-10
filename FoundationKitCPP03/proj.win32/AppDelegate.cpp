
#include <string>
#include <stdint.h>
#include <iostream>

#include "AppDelegate.h"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Foundation/DelegateManager.hpp"

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>


using namespace std;
USING_NS_FK;

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
void TupleTest(const ArgumentList& args)
{

}

void TupleTestArgs(const ArgumentList& args)
{

}

void AppDelegate::TestTupleArgs(const ArgumentList& args)
{

}

bool AppDelegate::applicationDidFinishLaunching() 
{

    DelegateManager::GetInstance()->AddObserver("TupleTest", BindFunctionHandler(&TupleTest));
    DelegateManager::GetInstance()->Invoke("TupleTest", ArgumentList());
    ArgumentList args;
    args.push_back(Value(10));
    args.push_back(Value("Fuck you"));
    args.push_back(Value("Fuck you again"));
    DelegateManager::GetInstance()->AddObserver("TupleTestArgs", BindFunctionHandler(&TupleTestArgs));
    DelegateManager::GetInstance()->AddObserver("TestTupleArgs", BindFunctionHandler(&AppDelegate::TestTupleArgs, this));
    DelegateManager::GetInstance()->Invoke("TupleTestArgs", args);
    DelegateManager::GetInstance()->Invoke("TestTupleArgs", args);

    std::string val = StringUtils::Format("sfsdfdsf============================ %d, %s", 100, "sfjsdjflsdfjdslfsdlfjslfjdslfjsdlfjsdlfjsdlfjds");

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
