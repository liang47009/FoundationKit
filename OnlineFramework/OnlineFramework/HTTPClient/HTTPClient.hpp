/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_HTTPCLIENT_HPP
#define FOUNDATIONKIT_HTTPCLIENT_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "curl.h"
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

NS_FK_BEGIN

#define HTTPClientVersionString "1.0.0"

class HTTPClient : public Singleton<HTTPClient>
{
	friend Singleton < HTTPClient >;
	HTTPClient();
public:
    /** Global multi handle */
    static CURLM*  G_multiHandle;
    static CURLSH* G_shareHandle;

	~HTTPClient();
	void        Initialize();
    void        Tick(float deltaTime);
    void        PostRequest(HTTPRequest::Pointer request);
    void        RemoveRequest(HTTPRequest::Pointer request);
    HTTPResponse::Pointer SendRequest(HTTPRequest::Pointer request);
	static struct RequestOptions
	{
		RequestOptions()
			: EnableVerifyPeer(false)
			, CertBundlePath()
			, IsUseHttpProxy(false)
			, HttpProxyAddress()
			, HttpProxyAcount()
			, IsDontReuseConnections(false)
		{}

		/** Prints out the options to the log */
		void DumpOptions();

		/** Whether or not should verify peer certificate (disable to allow self-signed certs) */
		bool EnableVerifyPeer;

		/** A path to certificate bundle */
		std::string CertBundlePath;

		/** Whether or not should use HTTP proxy */
		bool IsUseHttpProxy;

		/** Address of the HTTP proxy */
		std::string HttpProxyAddress;

		/* "user:password" to use with proxy. */
		/*example: HTTPRequestOptions::HttpProxyAcount="UserName:Password"*/
		std::string HttpProxyAcount;

		/** Forbid reuse connections (for debugging purposes, since normally it's faster to reuse) */
		bool IsDontReuseConnections;
	}
	HTTPRequestOptions;
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
	typedef std::unordered_map<CURL*, HTTPRequest::Pointer> RequestMap;
    typedef std::vector<HTTPRequest::Pointer>               RequestVector;
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

#endif // END OF FOUNDATIONKIT_HTTPCLIENT_HPP
