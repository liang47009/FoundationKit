/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPCODE_IPP
#define NETFRAMEWORK_HTTPCODE_IPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include "FoundationKit/Base/lexical_cast.hpp"
#include "HTTPClient/HTTPCode.hpp"
NS_FK_BEGIN
extern const HTTPCode* HTTPCodeList[];

const HTTPCode HTTPCode::SHUTDOWN = { 0, "Shutdown requested" };
const HTTPCode HTTPCode::CONTINUE = { 100, "The request can be continued." };
const HTTPCode HTTPCode::SWITCH_PROTOCOLS = { 101, "The server has switched protocols in an upgrade header." };
const HTTPCode HTTPCode::OK = { 200, "The request completed successfully." };
const HTTPCode HTTPCode::CREATED = { 201, "The request has been fulfilled and resulted in the creation of a new resource." };
const HTTPCode HTTPCode::ACCEPTED = { 202, "The request has been accepted for processing, but the processing has not been completed." };
const HTTPCode HTTPCode::PARTIAL = { 203, "The returned meta information in the entity-header is not the definitive set available from the originating server." };
const HTTPCode HTTPCode::NO_CONTENT = { 204, "The server has fulfilled the request, but there is no new information to send back." };
const HTTPCode HTTPCode::RESET_CONTENT = { 205, "The request has been completed, and the client program should reset the document view that caused the request to be sent to allow the user to easily initiate another input action." };
const HTTPCode HTTPCode::PARTIAL_CONTENT = { 206, "The server has fulfilled the partial GET request for the resource." };
const HTTPCode HTTPCode::WEBDAV_MULTI_STATUS = { 207, "During a World Wide Web Distributed Authoring and Versioning (WebDAV) operation, this indicates multiple status const HTTPCodes for a single response. The response body contains Extensible Markup Language (XML) that describes the status const HTTPCodes. For more information, see HTTP Extensions for Distributed Authoring." };
const HTTPCode HTTPCode::AMBIGUOUS = { 300, "The requested resource is available at one or more locations." };
const HTTPCode HTTPCode::MOVED = { 301, "The requested resource has been assigned to a new permanent Uniform Resource Identifier (URI), and any future references to this resource should be done using one of the returned URIs." };
const HTTPCode HTTPCode::REDIRECT = { 302, "The requested resource resides temporarily under a different URI." };
const HTTPCode HTTPCode::REDIRECT_METHOD = { 303, "The response to the request can be found under a different URI and should be retrieved using a GET HTTP verb on that resource." };
const HTTPCode HTTPCode::NOT_MODIFIED = { 304, "The requested resource has not been modified." };
const HTTPCode HTTPCode::USE_PROXY = { 305, "The requested resource must be accessed through the proxy given by the location field." };
const HTTPCode HTTPCode::REDIRECT_KEEP_VERB = { 307, "The redirected request keeps the same HTTP verb. HTTP/1.1 behavior." };
const HTTPCode HTTPCode::BAD_REQUEST = { 400, "The request could not be processed by the server due to invalid syntax." };
const HTTPCode HTTPCode::DENIED = { 401, "The requested resource requires user authentication." };
const HTTPCode HTTPCode::PAYMENT_REQ = { 402, "Not implemented in the HTTP protocol." };
const HTTPCode HTTPCode::FORBIDDEN = { 403, "The server understood the request, but cannot fulfill it." };
const HTTPCode HTTPCode::NOT_FOUND = { 404, "The server has not found anything that matches the requested URI." };
const HTTPCode HTTPCode::BAD_METHOD = { 405, "The HTTP verb used is not allowed." };
const HTTPCode HTTPCode::NONE_ACCEPTABLE = { 406, "No responses acceptable to the client were found." };
const HTTPCode HTTPCode::PROXY_AUTH_REQ = { 407, "Proxy authentication required." };
const HTTPCode HTTPCode::REQUEST_TIMEOUT = { 408, "The server timed out waiting for the request." };
const HTTPCode HTTPCode::CONFLICT = { 409, "The request could not be completed due to a conflict with the current state of the resource. The user should resubmit with more information." };
const HTTPCode HTTPCode::GONE = { 410, "The requested resource is no longer available at the server, and no forwarding address is known." };
const HTTPCode HTTPCode::LENGTH_REQUIRED = { 411, "The server cannot accept the request without a defined content length." };
const HTTPCode HTTPCode::PRECOND_FAILED = { 412, "The precondition given in one or more of the request header fields evaluated to false when it was tested on the server." };
const HTTPCode HTTPCode::REQUEST_TOO_LARGE = { 413, "The server cannot process the request because the request entity is larger than the server is able to process." };
const HTTPCode HTTPCode::URI_TOO_LONG = { 414, "The server cannot service the request because the request URI is longer than the server can interpret." };
const HTTPCode HTTPCode::UNSUPPORTED_MEDIA = { 415, "The server cannot service the request because the entity of the request is in a format not supported by the requested resource for the requested method." };
const HTTPCode HTTPCode::RETRY_WITH = { 449, "The request should be retried after doing the appropriate action." };
const HTTPCode HTTPCode::SERVER_ERROR = { 500, "The server encountered an unexpected condition that prevented it from fulfilling the request." };
const HTTPCode HTTPCode::NOT_SUPPORTED = { 501, "The server does not support the functionality required to fulfill the request." };
const HTTPCode HTTPCode::BAD_GATEWAY = { 502, "The server, while acting as a gateway or proxy, received an invalid response from the upstream server it accessed in attempting to fulfill the request." };
const HTTPCode HTTPCode::SERVICE_UNAVAIL = { 503, "The service is temporarily overloaded." };
const HTTPCode HTTPCode::GATEWAY_TIMEOUT = { 504, "The request was timed out waiting for a gateway." };
const HTTPCode HTTPCode::VERSION_NOT_SUP = { 505, "The server does not support the HTTP protocol version that was used in the request message." };

HTTPCode::HTTPCode()
	: code(200)
	, message("OK")
{

}

HTTPCode::HTTPCode(long httpcode)
	: code(httpcode)
{
    Assign(httpcode);
}

HTTPCode::HTTPCode(long httpcode, const char * msg)
	: code(httpcode)
	, message(msg)
{

}
void HTTPCode::Assign(long httpcode)
{
    code = httpcode;
    size_t index = 0;
    const HTTPCode* pHTTPCode = HTTPCodeList[index];
    while (pHTTPCode)
    {
        if (pHTTPCode->code == httpcode)
        {
            message = pHTTPCode->message;
            break;
        }
        pHTTPCode = HTTPCodeList[++index];
    }
}

std::string HTTPCode::ToString()
{
    std::string result = lexical_cast<std::string>(code);
    result += ":";
    result += message ? message : "Unknown";
    return result;
}

const HTTPCode* HTTPCodeList[] =
{
	&HTTPCode::SHUTDOWN,
	&HTTPCode::CONTINUE,
	&HTTPCode::SWITCH_PROTOCOLS,
	&HTTPCode::OK,
	&HTTPCode::CREATED,
	&HTTPCode::ACCEPTED,
	&HTTPCode::PARTIAL,
	&HTTPCode::NO_CONTENT,
	&HTTPCode::RESET_CONTENT,
	&HTTPCode::PARTIAL_CONTENT,
	&HTTPCode::WEBDAV_MULTI_STATUS,
	&HTTPCode::AMBIGUOUS,
	&HTTPCode::MOVED,
	&HTTPCode::REDIRECT,
	&HTTPCode::REDIRECT_METHOD,
	&HTTPCode::NOT_MODIFIED,
	&HTTPCode::USE_PROXY,
	&HTTPCode::REDIRECT_KEEP_VERB,
	&HTTPCode::BAD_REQUEST,
	&HTTPCode::DENIED,
	&HTTPCode::PAYMENT_REQ,
	&HTTPCode::FORBIDDEN,
	&HTTPCode::NOT_FOUND,
	&HTTPCode::BAD_METHOD,
	&HTTPCode::NONE_ACCEPTABLE,
	&HTTPCode::PROXY_AUTH_REQ,
	&HTTPCode::REQUEST_TIMEOUT,
	&HTTPCode::CONFLICT,
	&HTTPCode::GONE,
	&HTTPCode::LENGTH_REQUIRED,
	&HTTPCode::PRECOND_FAILED,
	&HTTPCode::REQUEST_TOO_LARGE,
	&HTTPCode::URI_TOO_LONG,
	&HTTPCode::UNSUPPORTED_MEDIA,
	&HTTPCode::RETRY_WITH,
	&HTTPCode::SERVER_ERROR,
	&HTTPCode::NOT_SUPPORTED,
	&HTTPCode::BAD_GATEWAY,
	&HTTPCode::SERVICE_UNAVAIL,
	&HTTPCode::GATEWAY_TIMEOUT,
	&HTTPCode::VERSION_NOT_SUP,
	nullptr
};
NS_FK_END

#endif // END OF NETFRAMEWORK_HTTPCODE_IPP
