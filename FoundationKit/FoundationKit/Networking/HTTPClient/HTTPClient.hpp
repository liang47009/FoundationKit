#ifndef LOSEMYMIND_HTTPCLIENT_H
#define LOSEMYMIND_HTTPCLIENT_H


#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "curl.h"
#include "FoundationKit/Base/ITickable.h"
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Foundation/Singleton.h"
#include "HTTPRequest.hpp"

NS_FK_BEGIN


#define HTTPClientVersionString "1.0.0"

namespace network{

/*
 * simple http client
 * First must be call HTTPClient::initialize() to initialize http client.
 */
class HTTPClient : public ITickable, public Singleton<HTTPClient>
{
    friend Singleton < HTTPClient > ;
    HTTPClient();
public:
    ~HTTPClient();
    static void  initialize();
    void         loopInThread();
    void         sendRequestAsync(HTTPRequest::Pointer request);
    virtual void tick(float deltaTime) override;
    virtual bool isTickable() const override;
public:

    static struct RequestOptions
    {
        RequestOptions()
            : EnableVerifyPeer(true)
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
    void         removeRequest(HTTPRequest::Pointer request);
    typedef std::vector<HTTPRequest::Pointer>               RequestVector;
    typedef std::unordered_map<CURL*, HTTPRequest::Pointer> RequestMap;
    /** multi handle that groups all the requests - not owned by this class */
    CURLM*                       _multiHandle;
    std::mutex                   _mutex;
    RequestMap                   _requests;
    std::condition_variable      _threadRequestNotEmpty;
    std::shared_ptr<std::thread> _workThread;
    bool                         _threadRunning;
};

} // namespace network

NS_FK_END



#endif // LOSEMYMIND_HTTPCLIENT_H







