/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_HTTPREQUEST_HPP
#define LOSEMYMIND_HTTPREQUEST_HPP

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include "FoundationKit/GenericPlatformMacros.h"
NS_FK_BEGIN

#ifdef DELETE
#undef DELETE
#endif

class HTTPResponse;
class HTTPRequest
{
    HTTPRequest()
        : _acceptEncodingType(DataEncodingType::Identity)
        , _requestType(Type::GET)
    {}
public:

    typedef std::vector<std::string>                     Headers;
    typedef std::unordered_map<std::string, std::string> Fields;
    typedef std::shared_ptr<HTTPRequest>                 Pointer;
    typedef std::shared_ptr<HTTPResponse>                ResponsePtr;

    using RequestDelegate = std::function < void(ResponsePtr) > ;

    // @param[1]   Total download size.
    // @param[2]   Now   download size.
    // @param[3]   Total upload size.
    // @param[4]   Now   upload size.
    using RequestProgressDelegate = std::function < void(double, double, double, double) > ;

   /**
    * The HttpRequest type enum used in the HttpRequest::setRequestType.
    */
    enum class Type
    {
        GET,
        POST,
        PUT,
        DELETE,
        UNKNOWN,
    };

    enum class DataEncodingType
    {
        Identity,
        Gzip,
        Deflate
    };

public:
    virtual~HTTPRequest()
    {

    }

    static Pointer create(const char* szURL, HTTPRequest::Type method = HTTPRequest::Type::GET)
    {
        HTTPRequest::Pointer shared_httprequest(new HTTPRequest());
        shared_httprequest->setRequestUrl(szURL);
        shared_httprequest->setRequestType(method);
        return shared_httprequest;
    }

    /** @brief Add a custom header to the request. */
    void addHeader(const char *header)
    {
        _headers.push_back(header);
    }

    void setHeaders(Headers headers)
    {
        _headers = headers;
    }

    Headers& getHeaders()
    {
        return _headers;
    }

    /** @brief Add a POST variable to the request, POST only. */
    void addPostField(const std::string& key, const std::string& value)
    {
        _postFields[key] = value;
    }

    Fields& getPostFields()
    {
        return _postFields;
    }

    void addFile(const std::string& name, const std::string& filePath)
    {
        _fileFields[name] = filePath;
    }

    Fields& getFiles()
    {
        return _fileFields;
    }

    void addContent(const std::string& name, const std::string& value)
    {
        _contentFields[name] = value;
    }

    Fields& getContents()
    {
        return _contentFields;
    }

    void   setRequestData(const char* buf, size_t len)
    {
        _requestData.assign(buf, buf + len);
    }

    char*  getRequestData()
    {
        if (_requestData.size() != 0)
            return &(_requestData.front());
        return nullptr;
    }


    size_t getRequestDataSize()
    {
        return _requestData.size();
    }

    /** @brief Set/Get cookie string. */
    void setRequestCookies(const char *cookies)
    {
        _requestCookies = cookies;
    }

    const std::string& getRequestCookies(void)
    {
        return _requestCookies;
    }

    /** @brief Set request url. */
    inline void setRequestUrl(const char *url){ _url = url; }

    /** @brief Get request url. */
    inline const std::string getRequestUrl(void){ return _url; }

    inline void setRequestType(Type type){ _requestType = type; }

    inline Type getRequestType() { return _requestType; }

    void setAcceptEncodingType(DataEncodingType type)
    {
        _acceptEncodingType = type;
    }

    DataEncodingType getAcceptEncoding()
    {
        return _acceptEncodingType;
    }

    void setContentEncoding(DataEncodingType contentEncoding)
    {
        _contentEncoding = contentEncoding;
    }

    DataEncodingType getContentEncoding()
    {
        return _contentEncoding;
    }

    std::string getEncodingAsString(DataEncodingType dataEncodingType)
    {
        std::string strEncoding = "identity";
        switch (dataEncodingType)
        {
        case HTTPRequest::DataEncodingType::Identity:
            strEncoding = "identity";
            break;
        case HTTPRequest::DataEncodingType::Gzip:
            strEncoding = "gzip";
            break;
        case HTTPRequest::DataEncodingType::Deflate:
            strEncoding = "deflate";
            break;
        default:
            break;
        }
        return strEncoding;
    }

    RequestDelegate         onRequestFinished;
    RequestProgressDelegate onProgress;

private:
    DataEncodingType    _acceptEncodingType;
    DataEncodingType    _contentEncoding;
    Type                _requestType;
    Headers             _headers;
    Fields              _postFields;
    Fields              _fileFields;
    Fields              _contentFields;
    std::string         _url;
    std::string         _requestCookies;
    std::vector<char>   _requestData;    /// used for POST
};


NS_FK_END

#endif // LOSEMYMIND_HTTPREQUEST_HPP