#ifndef LOSEMYMIND_HTTPCLIENT_H
#define LOSEMYMIND_HTTPCLIENT_H


#include <string>
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Foundation/Singleton.h"

NS_FK_BEGIN


#define HTTPClientVersionString "1.0.0"

class HTTPClient : public Singleton<HTTPClient>
{
    friend Singleton < HTTPClient > ;
    HTTPClient();
public:




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

private:
    /** Global multi handle */
    static CURLM* _G_multiHandle;
    /** multi handle that groups all the requests - not owned by this class */
    CURLM* _multiHandle;
};



NS_FK_END



#endif // LOSEMYMIND_HTTPCLIENT_H
