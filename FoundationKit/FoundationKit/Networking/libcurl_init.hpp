#ifndef LOSEMYMIND_LIBCURL_INIT_HPP
#define LOSEMYMIND_LIBCURL_INIT_HPP

#include "FoundationKit/GenericPlatformMacros.h"
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


class libcurl_init : public libcurl_init_base
{
public:
    libcurl_init();
    ~libcurl_init();
    static bool succeeded();
    static CURLcode code();
private:
    static Result _result;
};

static libcurl_init libcurl_init_instance = libcurl_init();

NS_FK_END

#include "libcurl_init.ipp"



#endif // LOSEMYMIND_LIBCURL_INIT_HPP
