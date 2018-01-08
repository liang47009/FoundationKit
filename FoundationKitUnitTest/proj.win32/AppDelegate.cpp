
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>
#include <utility>

#include "AppDelegate.h"

#include "FoundationKit/forward.hpp"
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/LanguageFeatures.hpp"

#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Crypto/sha1.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Foundation/ITickable.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/MathContent.hpp"
#include "FoundationKit/Foundation/MathUtil.hpp"
#include "FoundationKit/Foundation/Matrix.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Quaternion.hpp"
#include "FoundationKit/Foundation/Rect.hpp"
#include "FoundationKit/Foundation/Runnable.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/Size.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/ThreadTimer.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/Vector2.hpp"
#include "FoundationKit/Foundation/Vector3.hpp"
#include "FoundationKit/Foundation/Vector4.hpp"
#include "FoundationKit/Foundation/Version.hpp"

#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"


#include "HTTPClient/HTTPClient.hpp"



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

    std::string APP_ID = "324d4b61-df14-11e7-b817-e0accb778420";
    std::string APP_SECURE = "pdjudrmnf9";
    std::string date = DateTime::Now().ToIso8601();
    std::string Auth = MD5::md5_hash_hex(APP_ID+APP_SECURE+ date);
    HTTPClient::GetInstance()->Initialize();
    HTTPRequest::Pointer requestAuth = HTTPRequest::Create(true);
    requestAuth->SetURL("http://qos.189.cn/t1?appid=324d4b61-df14-11e7-b817-e0accb778420");
    requestAuth->SetHeader("X-Request-At", date);
    requestAuth->SetHeader("X-Application-Id", APP_ID);
    requestAuth->SetHeader("X-Application-Auth", Auth);
    requestAuth->OnRequestCompleted = [](HTTPRequest::Pointer pRequest, HTTPResponse::Pointer pResponse)
    {
        if (pResponse->IsSucceeded())
        {
            HTTPRequest::Pointer request = HTTPRequest::Create(true);
            request->SetMethod(RequestMethodType::POST);
            request->SetURL("http://qos.189.cn/api/mobile/speeding");
            request->SetPostField("Security_token", "0001:1::1515059140_324d4b61-df14-11e7-b817-e0accb778420_xD6GAk/HHXZkncyyKGTvyvRT4ZI3ZQCtXFmbcRWpFg+yeuHIrAD14UQnFMaJfVDt9SxP3vaSKu8B7zx66oU9HgHxED1sAZDPkEZAnn76B3jS/zNE/aRrGfQbNrt4LfnZ");
            request->SetPostField("dst_info", "192.168.0.1:80");
            request->SetPostField("src_info", "172.14.44.80:80");
            request->SetPostField("user_id", "snailtest;7");
            request->SetPostField("product_id", "wnts");
            request->OnRequestCompleted = [](HTTPRequest::Pointer pRequest, HTTPResponse::Pointer pResponse)
            {
                pResponse->DumpInfo();
                auto responseData = pResponse->GetContentData();
                std::string pdata((char*)&responseData[0], responseData.size());
                std::u16string ustr;
                bool ret = StringUtils::UTF8ToUTF16(pdata, ustr);
                if (pResponse->IsSucceeded())
                {

                }
            };
            HTTPClient::GetInstance()->SendRequest(request);
        }
    };
    HTTPClient::GetInstance()->SendRequest(requestAuth);



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
    HTTPClient::GetInstance()->Tick(0.016);
}

void AppDelegate::TestTupleArgs(int a, const std::string & str, const char * str1)
{
}
