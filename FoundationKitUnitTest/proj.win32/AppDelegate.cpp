
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
#include "HTTPClient/HTTPDownloader.hpp"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Base/string_builder.hpp"
#include "FoundationKit/Base/scope_locale.hpp"
#include "FoundationKit/Base/unique_id.hpp"

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


bool AppDelegate::applicationDidFinishLaunching() 
{
    std::error_code ec;
    std::string strErr = ec.message();
    HTTPClient::GetInstance()->Initialize();

	std::vector<uint64> ids0;
	std::vector<uint64> ids1;
	std::vector<uint64> ids2;
	std::vector<uint64> ids3;

	unique_id  uid;
	std::thread thread0([&]()
	{
		for (int i = 0; i < 500000; ++i)
		{
			uint64 tmpid = uid.generate();
			auto iter = std::find(ids0.begin(), ids0.end(), tmpid);
			if (iter == ids0.end())
			{
				ids0.push_back(uid.generate());
			}
		}
	});

	/*
	std::thread thread1([&]()
	{
		for (int i = 0; i < 500000; ++i)
		{
			uint64 tmpid = uid.generate();
			auto iter = std::find(ids1.begin(), ids1.end(), tmpid);
			if (iter == ids1.end())
			{
				ids1.push_back(uid.generate());
			}
		}
	});

	std::thread thread2([&]()
	{
		for (int i = 0; i < 500000; ++i)
		{
			uint64 tmpid = uid.generate();
			auto iter = std::find(ids2.begin(), ids2.end(), tmpid);
			if (iter == ids2.end())
			{
				ids2.push_back(uid.generate());
			}
		}
	});

	std::thread thread3([&]()
	{
		for (int i = 0; i < 500000; ++i)
		{
			uint64 tmpid = uid.generate();
			auto iter = std::find(ids3.begin(), ids3.end(), tmpid);
			if (iter == ids3.end())
			{
				ids3.push_back(uid.generate());
			}
		}
	});
	*/
	thread0.join();
	//thread1.join();
	//thread2.join();
	//thread3.join();

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
    HTTPDownloader::GetInstance()->Tick(1 / 60.0f);
}
