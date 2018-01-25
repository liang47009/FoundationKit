/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_LIBCURL_INIT_HPP
#define NETFRAMEWORK_LIBCURL_INIT_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include "curl.h"
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
class libcurl_init_base
{
protected:
    struct Result
    {
        CURLcode Code;
    };
    static void Startup(Result& result);
    static void Cleanup();

};

class libcurl_init : private libcurl_init_base
{
public:
    libcurl_init()
    {
        Startup(InitResult);
    }
    ~libcurl_init()
    {
        Cleanup();
    }
    static bool Succeeded()
    {
        return (InitResult.Code == CURLE_OK);
    }
    static CURLcode Code()
    {
        return InitResult.Code;
    }
private:
    static Result InitResult;
};

static const libcurl_init& libcurl_init_instance = libcurl_init();
NS_FK_END


//#include "HTTPClient/libcurl_init.ipp"

#endif // END OF NETFRAMEWORK_LIBCURL_INIT_HPP
