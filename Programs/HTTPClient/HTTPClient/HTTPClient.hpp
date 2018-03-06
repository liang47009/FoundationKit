/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPCLIENT_HPP
#define NETFRAMEWORK_HTTPCLIENT_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "curl.h"
#include "libcurl_init.hpp"
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "HTTPClient/HTTPRequest.hpp"
#include "HTTPClient/HTTPResponse.hpp"
#include "HTTPClient/HTTPClientOptions.hpp"

NS_FK_BEGIN

#define HTTPClientVersionString "2.0.0"

class HTTPClient : public Singleton<HTTPClient>
{
	friend Singleton < HTTPClient >;
	HTTPClient();
public:
    /** Global multi handle */
    static CURLM*            G_multiHandle;
    static CURLSH*           G_shareHandle;
    static HTTPClientOptions G_HTTPClientOptions;

	~HTTPClient();
	void        Initialize();
    void        Tick(float deltaTime);
    void        PostRequest(HTTPRequest::Pointer request);
    void        RemoveRequest(HTTPRequest::Pointer request);
    HTTPResponse::Pointer SendRequest(HTTPRequest::Pointer request);
    /** Prints out the options to the log */
    void DumpOptions();
private:
    void        InternalPostRequest(HTTPRequest::Pointer request);
    void        InternalRemoveRequest(HTTPRequest::Pointer request);
    void        LazyCreatePerformThread();
    void        AddRequestToFinished(HTTPRequest::Pointer request);
    static void StaticLockCallback(CURL *handle, curl_lock_data data, curl_lock_access access, void*userData);
    static void StaticUnlockCallback(CURL *handle, curl_lock_data data, void *userData);
    void        LockCallback(CURL *handle, curl_lock_data data, curl_lock_access access);
    void        UnlockCallback(CURL *handle, curl_lock_data data);

    typedef std::unordered_map < HTTPRequest::Pointer, bool > RequestTempMap;
	typedef std::unordered_map<CURL*, HTTPRequest::Pointer>   RequestMap;
    typedef std::vector<HTTPRequest::Pointer>                 RequestVector;
	/** multi handle that groups all the requests - not owned by this class */
	CURLM*                       MultiHandle;
    std::mutex                   CURLDataShareMutex;
    std::mutex                   CURLDataCookieMutex;
    std::mutex                   CURLDataDNSMutex;
    std::mutex                   CURLDataSSLSessionMutex;
    std::mutex                   CURLDataConnectMutex;
    RequestTempMap               RequestTempPool;
    std::mutex                   RequestTempMutex;
    std::thread                  ThreadOfPerform;
    RequestMap                   HandlesToRequests;
    std::mutex                   HandlesToRequestsMutex;
    std::condition_variable      HandleRequestCond;
    RequestVector                FinishedRequests;
    std::mutex                   FinishedRequestsMutex;
    bool                         IsRunning;
};

NS_FK_END
//#include "HTTPClient/HTTPClient.ipp"
#endif // END OF NETFRAMEWORK_HTTPCLIENT_HPP
