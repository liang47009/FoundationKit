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
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "HTTPRequest.hpp"

NS_FK_BEGIN


#define HTTPClientVersionString "1.0.0"

namespace network{

/*
 * simple http client
 * First must be call HTTPClient::initialize() to initialize http client.
 */
class HTTPClient : public Singleton<HTTPClient>
{
    friend Singleton < HTTPClient > ;
    HTTPClient();
public:
    ~HTTPClient();
    static void  initialize();
    void         sendRequestAsync(HTTPRequest::Pointer request);
    void tick(float deltaTime);
public:

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
        void dumpOptions();

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

public:
    /** Global multi handle */
    static CURLM*  _G_multiHandle;
    static CURLSH* _G_shareHandle;
private:
    typedef std::vector<HTTPRequest::Pointer>               RequestVector;
    typedef std::unordered_map<CURL*, HTTPRequest::Pointer> RequestMap;
    /** multi handle that groups all the requests - not owned by this class */
    CURLM*                       _multiHandle;
    std::mutex                   _mutex;
    RequestMap                   _requests;
};

} // namespace network

NS_FK_END


#endif // FOUNDATIONKIT_HTTPCLIENT_HPP
