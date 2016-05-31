/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#include "HttpResponse.h"
#include "HttpRequest.h"

NS_FK_BEGIN

HttpRequest::HttpRequest()
    : _requestType(Type::GET)
    , _acceptEncoding(EncodeType::Identity)
    , _isMultipart(false)
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
    _isMultipart = true;
}

HttpRequest::Fields& HttpRequest::getPOSTValues()
{
    return _postFields;
}


void HttpRequest::addFormFile(const char *name, const char *filePath)
{
    _fileFields[std::string(name)] = std::string(filePath ? filePath : "");
    _isMultipart = true;
}

HttpRequest::Fields& HttpRequest::getFormFiles()
{
    return _fileFields;
}


void HttpRequest::addFormContents(const char *name, const char *value)
{
    _contentFields[std::string(name)] = std::string(value ? value : "");
    _isMultipart = true;
}

HttpRequest::Fields& HttpRequest::getFormContents()
{
    return _contentFields;
}

void HttpRequest::setRequestData(const char* buf, size_t len)
{
    _requestData.assign(buf, buf + len);
}

void HttpRequest::setRequestCookies(const char *cookie)
{
    _requestCookies = cookie;
}

char* HttpRequest::getRequestData()
{
    if (_requestData.size() != 0)
        return &(_requestData.front());
    return nullptr;
}

size_t HttpRequest::getRequestDataSize()
{
    return _requestData.size();
}

const std::string& HttpRequest::getRequestCookies(void)
{
    return _requestCookies;
}

void HttpRequest::setAcceptEncoding(EncodeType acceptEncoding)
{
    _acceptEncoding = acceptEncoding;
}

HttpRequest::EncodeType HttpRequest::getAcceptEncoding()
{
    return _acceptEncoding;
}

void HttpRequest::setContentEncoding(EncodeType contentEncoding)
{
    _contentEncoding = contentEncoding;
}

HttpRequest::EncodeType HttpRequest::getContentEncoding()
{
    return _contentEncoding;
}

std::string HttpRequest::getEncodingString(EncodeType encodeType)
{
    std::string strEncoding = "identity";
    switch (encodeType)
    {
    case FoundationKit::HttpRequest::EncodeType::Identity:
        strEncoding = "identity";
        break;
    case FoundationKit::HttpRequest::EncodeType::Gzip:
        strEncoding = "gzip";
        break;
    case FoundationKit::HttpRequest::EncodeType::Deflate:
        strEncoding = "deflate";
        break;
    default:
        break;
    }
    return strEncoding;
}

NS_FK_END


