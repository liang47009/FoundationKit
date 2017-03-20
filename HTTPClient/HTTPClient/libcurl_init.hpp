#ifndef LOSEMYMIND_LIBCURL_INIT_HPP
#define LOSEMYMIND_LIBCURL_INIT_HPP
# pragma once
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "curl.h"

NS_FK_BEGIN

class libcurl_init_base
{
protected:
    struct Result
    {
        CURLcode                     _initCode;
    };
    static void startup(Result& result);
    static void cleanup();

};

class libcurl_init : private libcurl_init_base
{
public:
    libcurl_init()
    {
        startup(_result);
    }
    ~libcurl_init()
    {
        cleanup();
    }
    static bool succeeded()
    {
        return (_result._initCode == CURLE_OK);
    }
    static CURLcode code()
    {
        return _result._initCode;
    }
private:
    static Result _result;
};

//libcurl_init_base::Result libcurl_init::_result;

static const libcurl_init& libcurl_init_instance = libcurl_init();

NS_FK_END

//#include "libcurl_init.ipp"

#endif // LOSEMYMIND_LIBCURL_INIT_HPP



