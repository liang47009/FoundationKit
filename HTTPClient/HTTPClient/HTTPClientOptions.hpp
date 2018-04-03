/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPCLIENTOPTIONS_HPP
#define NETFRAMEWORK_HTTPCLIENTOPTIONS_HPP

#include <string>

struct HTTPClientOptions
{
    HTTPClientOptions()
        : EnableVerifyPeer(false)
        , CertBundlePath()
        , IsUseHttpProxy(false)
        , HttpProxyAddress()
        , HttpProxyAcount()
        , IsDontReuseConnections(false)
    {}
    /** Whether or not should verify peer certificate (disable to allow self-signed certs) */
    bool EnableVerifyPeer;

    /** A path to certificate bundle */
    std::string CertBundlePath;

    /** Whether or not should use HTTP proxy */
    bool IsUseHttpProxy;

    /** Address of the HTTP proxy */
    std::string HttpProxyAddress;

    /* "user:password" to use with proxy. */
    /*example: G_HTTPClientOptions::HttpProxyAcount="UserName:Password"*/
    std::string HttpProxyAcount;

    /** Forbid reuse connections (for debugging purposes, since normally it's faster to reuse) */
    bool IsDontReuseConnections;
};

#endif // END OF NETFRAMEWORK_HTTPCLIENTOPTIONS_HPP