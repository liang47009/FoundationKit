/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_HTTPREQUEST_H
#define LOSEMYMIND_HTTPREQUEST_H

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include "FoundationKit/GenericPlatformMacros.h"
NS_FK_BEGIN

#ifdef DELETE
#undef DELETE
#endif

class HttpResponse;
class HttpRequest
{
    HttpRequest();
public:
    typedef std::vector<std::string>                     Headers;
    typedef std::unordered_map<std::string, std::string> Fields;
    typedef std::shared_ptr<HttpRequest>                 Pointer;
    typedef std::shared_ptr<HttpResponse>                ResponsePtr;

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

    enum class EncodeType
    {
        Identity,
        Gzip,
        Deflate,
    };

public:
    virtual~HttpRequest();

    static Pointer createWithUrl( const char* szURL, Type method = Type::GET);


    /** @brief Add a custom header to the request. */
    void addHeader(const char *header);
    void setHeaders(Headers headers);
    Headers& getHeaders();


    /** @brief Add a POST variable to the request, POST only. */
    void addPOSTValue(const char *key, const char *value);
    Fields& getPOSTValues();

    void addFormFile(const char *name, const char *filePath);
    Fields& getFormFiles();

    void addFormContents(const char *name, const char *value);
    Fields& getFormContents();

    void   setRequestData(const char* buf, size_t len);
    char*  getRequestData();
    size_t getRequestDataSize();

    /** @brief Set/Get cookie string. */
    void setRequestCookies(const char *cookies);
    const std::string& getRequestCookies(void);

    /** @brief Set accept encoding. */
    void setAcceptEncoding(EncodeType acceptEncoding);
    EncodeType getAcceptEncoding();

    void setContentEncoding(EncodeType contentEncoding);
    EncodeType getContentEncoding();

    std::string getEncodingString(EncodeType encodeType);

    /** @brief Set request url. */
    inline void setRequestUrl(const char *url){ _url = url; }

    /** @brief Get request url. */
    inline const std::string getRequestUrl(void){ return _url; }

    inline void setRequestType(Type type){ _requestType = type;}

    inline Type getRequestType() {  return _requestType;}

    inline bool isMultipart(){ return _isMultipart; }

    // if callback in thread, used asynchronous request, 
    // the onRequestFinished is called in the thread.
    inline void callbackInThread(){ _callbackInThread = true; }
    inline bool isCallbackInThread(){ return _callbackInThread; }

    RequestDelegate         onRequestFinished;
    RequestProgressDelegate onProgress;

private:
    Type                _requestType;
    EncodeType          _acceptEncoding;
    EncodeType          _contentEncoding;
    Headers             _headers;
    Fields              _postFields;
    Fields              _fileFields;
    Fields              _contentFields;
    std::string         _url;
    std::string         _requestCookies;
    bool                _isMultipart;
    bool                _callbackInThread;
    std::vector<char>   _requestData;    /// used for POST
};

NS_FK_END

#endif // LOSEMYMIND_HTTPREQUEST_H

