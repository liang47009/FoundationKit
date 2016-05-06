/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_HTTPRESPONSE_H
#define LOSEMYMIND_HTTPRESPONSE_H

#pragma once
#include <vector>
#include <string>
#include <memory>
#include "HttpRequest.h"
#include "FoundationKit/GenericPlatformMacros.h"

NS_FK_BEGIN

class HttpResponse
{
public:

    typedef std::shared_ptr<HttpResponse>                 Pointer;

    static Pointer create(HttpRequest::Pointer request)
    {
        Pointer shared_response(new (std::nothrow) HttpResponse(request));
        return shared_response;
    }

    /**
    * Constructor, it's used by HttpClient internal, users don't need to create HttpResponse manually.
    * @param request the corresponding HttpRequest which leads to this response.
    */
    HttpResponse(HttpRequest::Pointer request)
        :_succeed(false)
        , _responseCode(0)
        , _pHttpRequest(request)
        , _responseMessageString("")
    {
        _responseData.clear();
        _errorBuffer.clear();
    }

    /**
    * Destructor, it will be called in HttpClient internal.
    * Users don't need to desturct HttpResponse object manully.
    */
    virtual ~HttpResponse()
    {

    }


    /**
    * Get the corresponding HttpRequest object which leads to this response.
    * There's no paired setter for it, because it's already setted in class constructor
    * @return HttpRequest* the corresponding HttpRequest object which leads to this response.
    */
    inline HttpRequest::Pointer getHttpRequest()
    {
        return _pHttpRequest;
    }

   /**
    * Set whether the http request is returned successfully or not,
    * This setter is mainly used in HttpClient, users mustn't set it directly
    * @param value the flag represent whether the http request is successful or not.
    */
    inline void setSucceed(bool value)
    {
        _succeed = value;
    };

   /**
    * To see if the http reqeust is returned successfully.
    * Althrough users can judge if (http response code = 200), we want an easier way.
    * If this getter returns false, you can call getResponseCode and getErrorBuffer to find more details.
    * @return bool the flag that represent whether the http request return sucesssfully or not.
    */
    inline bool isSucceed()
    {
        return _succeed;
    };


   /**
    * Set the http response data buffer, it is used by HttpClient.
    * @param data the pointer point to the response data buffer.
    */
    inline void setResponseData(std::vector<char>* data)
    {
        _responseData = *data;
    }

   /**
    * Get the http response data.
    * @return std::vector<char>* the pointer that point to the _responseData.
    * @usage:
        std::vector<char> responseData = *responsePtr->getResponseData();
        std::string strResponseData(&responseData[0], responseData.size());
    */
    inline std::vector<char>* getResponseData()
    {
        return &_responseData;
    }

   /**
    * Set the http response headers buffer, it is used by HttpClient.
    * @param data the pointer point to the response headers buffer.
    */
    inline void setResponseHeader(std::vector<char>* data)
    {
        _responseHeader = *data;
    }


   /**
    * Get the response headers.
    * @return std::vector<char>* the pointer that point to the _responseHeader.
    * @usage:
        std::vector<char> responseHeader = *responsePtr->getResponseHeader();
        std::string strresponseHeader(&responseHeader[0], responseHeader.size());
    */
    inline std::vector<char>* getResponseHeader()
    {
        return &_responseHeader;
    }

   /**
    * Set the http response code.
    * @param value the http response code that represent whether the request is successful or not.
    */
    inline void setResponseCode(long value)
    {
        _responseCode = value;
    }

   /**
    * Get the http response code to judge whether response is sucessful or not.
    * I know that you want to see the _responseCode is 200.
    * If _responseCode is not 200, you should check the meaning for _responseCode by the net.
    * @return long the value of _responseCode
    */
    inline long getResponseCode()
    {
        return _responseCode;
    }

   /**
    * Set the error buffer which will tell you more the reason why http request failed.
    * @param value a string pointer that point to the reason.
    */
    inline void setErrorBuffer(const char* value)
    {
        _errorBuffer.clear();
        _errorBuffer.assign(value);
    };

   /**
    * Get the rror buffer which will tell you more about the reason why http request failed.
    * @return const char* the pointer that point to _errorBuffer.
    */
    inline const char* getErrorBuffer()
    {
        return _errorBuffer.c_str();
    }

    /**
    * Set the response data by the string pointer and the defined size.
    * @param value a string pointer that point to response data buffer.
    * @param n the defined size that the response data buffer would be copied.
    */
    inline void setResponseMessage(const char* value)
    {
        _responseMessageString = value;
    }

    /**
    * Get the string pointer that point to the response data.
    * @return const char* the string pointer that point to the response data.
    */
    inline const char* getResponseMessage()
    {
        return _responseMessageString.c_str();
    }

    inline void setResponseCookies(const char* cookies)
    {
        _responseCookies = cookies;
    }

    inline const char* getResponseCookies()
    {
        return _responseCookies.c_str();
    }

protected:
    // properties
    HttpRequest::Pointer _pHttpRequest;  /// the corresponding HttpRequest pointer who leads to this response 
    bool                 _succeed;       /// to indecate if the http reqeust is successful simply
    std::vector<char>    _responseData;  /// the returned raw data. You can also dump it as a string
    std::vector<char>    _responseHeader;  /// the returned raw header data. You can also dump it as a string
    long                 _responseCode;    /// the status code returned from libcurl, e.g. 200, 404
    std::string          _errorBuffer;   /// if _responseCode != 200, please read _errorBuffer to find the reason
    std::string          _responseMessageString; // the returned raw data. You can also dump it as a string
    std::string          _responseCookies;
};

NS_FK_END

#endif // LOSEMYMIND_HTTPRESPONSE_H


