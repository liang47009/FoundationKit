
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>

#include "AppDelegate.h"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/Base/function_cache.hpp"
#include "FoundationKit/Base/memory_aligned.hpp"
#include "FoundationKit/Base/function_cache.hpp"
#include "FoundationKit/Base/function_traits.hpp"
#include "FoundationKit/Base/scope_guard.hpp"
#include "FoundationKit/Base/hex.hpp"
#include "FoundationKit/Base/string_builder.hpp"
#include "FoundationKit/Base/locale.hpp"
#include "FoundationKit/Base/unique_id.hpp"
#include "FoundationKit/Base/byte_swap.hpp"

#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DelegateManager.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/Value.hpp"

#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/File.hpp"

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

class AAClass
{
public:
    AAClass(int ini, char inc)
        : i(ini)
        , c(inc)
    {}

    int i;
    char c;
};

bool AppDelegate::applicationDidFinishLaunching() 
{
	std::error_code ec;
	std::string strErr = ec.message();
	HTTPClient::GetInstance()->Initialize();
    PlatformDevice::DumpDeviceInfo();

    size_t aab = alignment_of_max_align;

    aligned_ptr<AAClass> apAAClass(100, 'a');


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

void AppDelegate::TestTupleArgs(int a, const std::string & str, const char * str1)
{
}
