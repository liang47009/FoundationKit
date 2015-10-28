/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#include "HttpResponse.h"
#include "HttpRequest.h"

NS_FK_BEGIN

HttpRequest::HttpRequest()
    : _requestType(Type::GET)
    , _acceptEncode(AcceptEncode::Identity)
{

}


HttpRequest::~HttpRequest()
{

}

HttpRequest::Pointer HttpRequest::createWithUrl(const char* szURL, Type method /*= Type::GET*/)
{
    HttpRequest::Pointer shared_httprequest(new HttpRequest());
    shared_httprequest->setRequestUrl(szURL);
    shared_httprequest->setRequestType(method);
    return shared_httprequest;
}

void HttpRequest::addHeader(const char *header)
{
    _headers.push_back(header);
}

void HttpRequest::setHeaders(std::vector<std::string> headers)
{
    _headers = headers;
}

HttpRequest::Headers& HttpRequest::getHeaders()
{
    return _headers;
}

void HttpRequest::addPOSTValue(const char *key, const char *value)
{
    _postFields[std::string(key)] = std::string(value ? value : "");
}

HttpRequest::Fields& HttpRequest::getPOSTValues()
{
    return _postFields;
}


void HttpRequest::addFormFile(const char *name, const char *filePath)
{
    _fileFields[std::string(name)] = std::string(filePath ? filePath : "");
}

HttpRequest::Fields& HttpRequest::getFormFiles()
{
    return _fileFields;
}


void HttpRequest::addFormContents(const char *name, const char *value)
{
    _contentFields[std::string(name)] = std::string(value ? value : "");
}

HttpRequest::Fields& HttpRequest::getFormContents()
{
    return _contentFields;
}


void HttpRequest::setRequestCookies(const char *cookie)
{
    _requestCookies = cookie;
}

const std::string& HttpRequest::getRequestCookies(void)
{
    return _requestCookies;
}

void HttpRequest::setAcceptEncoding(AcceptEncode acceptEncoding)
{
    _acceptEncode = acceptEncoding;
}

std::string HttpRequest::getAcceptEncodingString()
{
    std::string aestr = "identity";
    switch (_acceptEncode)
    {
    case HttpRequest::AcceptEncode::Gzip:
        aestr = "gzip";
        break;
    case HttpRequest::AcceptEncode::Deflate:
        aestr = "deflate";
        break;
    default:
        break;
    }
    return aestr;
}

NS_FK_END


