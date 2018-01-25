/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPCODE_HPP
#define NETFRAMEWORK_HTTPCODE_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN

class HTTPCode
{
public:
    long        code;
	const char *message;

	HTTPCode();
    HTTPCode(long httpcode);
    HTTPCode(long httpcode, const char * msg);
    void Assign(long httpcode);
    std::string ToString();
    HTTPCode& operator= (long httpcode)
    {
        Assign(httpcode);
        return (*this);
    }

    bool operator ==(long httpcode)
	{
        return (code == httpcode);
	}

    bool operator !=(long httpcode)
	{
        return (code != httpcode);
	}

    bool operator <=(long httpcode)
    {
        return (code <= httpcode);
    }

    bool operator >=(long httpcode)
    {
        return (code >= httpcode);
    }

    bool operator <(long httpcode)
    {
        return (code < httpcode);
    }

    bool operator >(long httpcode)
    {
        return (code > httpcode);
    }

	// 1XX informational
	static const HTTPCode SHUTDOWN;
	// 1XX informational
	static const HTTPCode CONTINUE;
	static const HTTPCode SWITCH_PROTOCOLS;
	// 2XX success
	static const HTTPCode OK;
	static const HTTPCode CREATED;
	static const HTTPCode ACCEPTED;
	static const HTTPCode PARTIAL;
	static const HTTPCode NO_CONTENT;
	static const HTTPCode RESET_CONTENT;
	static const HTTPCode PARTIAL_CONTENT;
	static const HTTPCode WEBDAV_MULTI_STATUS;
	// 3XX redirection
	static const HTTPCode AMBIGUOUS;
	static const HTTPCode MOVED;
	static const HTTPCode REDIRECT;
	static const HTTPCode REDIRECT_METHOD;
	static const HTTPCode NOT_MODIFIED;
	static const HTTPCode USE_PROXY;
	static const HTTPCode REDIRECT_KEEP_VERB;
	// 4XX client's fault
	static const HTTPCode BAD_REQUEST;
	static const HTTPCode DENIED;
	static const HTTPCode PAYMENT_REQ;
	static const HTTPCode FORBIDDEN;
	static const HTTPCode NOT_FOUND;
	static const HTTPCode BAD_METHOD;
	static const HTTPCode NONE_ACCEPTABLE;
	static const HTTPCode PROXY_AUTH_REQ;
	static const HTTPCode REQUEST_TIMEOUT;
	static const HTTPCode CONFLICT;
	static const HTTPCode GONE;
	static const HTTPCode LENGTH_REQUIRED;
	static const HTTPCode PRECOND_FAILED;
	static const HTTPCode REQUEST_TOO_LARGE;
	static const HTTPCode URI_TOO_LONG;
	static const HTTPCode UNSUPPORTED_MEDIA;
	static const HTTPCode RETRY_WITH;
	// 5XX server's fault
	static const HTTPCode SERVER_ERROR;
	static const HTTPCode NOT_SUPPORTED;
	static const HTTPCode BAD_GATEWAY;
	static const HTTPCode SERVICE_UNAVAIL;
	static const HTTPCode GATEWAY_TIMEOUT;
	static const HTTPCode VERSION_NOT_SUP;
};


inline bool operator==(const HTTPCode& hcp, long httpCode)
{
    return (hcp.code == httpCode);
}

inline bool operator!=(const HTTPCode& hcp, long httpCode)
{
    return (hcp.code != httpCode);
}

NS_FK_END
//#include "HTTPClient/HTTPCode.ipp"
#endif // END OF NETFRAMEWORK_HTTPCODE_HPP
