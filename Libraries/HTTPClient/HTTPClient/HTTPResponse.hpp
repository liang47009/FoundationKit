/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_HTTPRESPONSE_HPP
#define FOUNDATIONKIT_HTTPRESPONSE_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "HTTPCode.hpp"

NS_FK_BEGIN
class HTTPRequest;
class HTTPResponse
{
    friend class HTTPRequest;
    HTTPResponse() = delete;
public:
    typedef std::unordered_map<std::string, std::string> KeyValueMap;
    typedef std::shared_ptr<HTTPResponse>                Pointer;
    typedef std::weak_ptr<HTTPRequest>                   RequestWeakPtr;

public:
    ~HTTPResponse();
    static Pointer           Create(RequestWeakPtr pRequest);
    std::string              GetURL();
    std::string              GetEffectiveURL();
    std::string              GetURLParameter(const std::string& parameterName);
    std::string              GetHeader(const std::string& headerName);
    std::vector<std::string> GetAllHeaders();
    std::string              GetContentType();
    size_t                   GetContentSize();
    std::vector<uint8>&      GetContentData();
    HTTPCode                 GetResponseCode();
    std::string              GetCookies();
    std::string              GetResponseMsg();
    std::string              GetErrorMsg();
    bool                     IsReady();
    bool                     IsSucceeded();
    void                     DumpInfo();
protected:
    HTTPResponse(RequestWeakPtr pRequest);
    HTTPResponse&            SetHeader(const std::string& headerName, const std::string& headerValue);
    HTTPResponse&            SetContentSize(int contentSize);
    HTTPResponse&            SetCookies(const std::string& cookies);
    HTTPResponse&            SetReady(bool bReady);
    HTTPResponse&            SetSucceeded(bool bSucceeded);
    HTTPResponse&            SetResponseCode(long responseCode);
    HTTPResponse&            SetResponseCode(const HTTPCode& responseCode);
    HTTPResponse&            SetResponseMsg(const std::string& responseMsg);
    HTTPResponse&            SetErrorMsg(const std::string& errorMsg);
    HTTPResponse&            SetEffectiveURL(const std::string& effectiveUrl);

private:
    RequestWeakPtr     RequestInstance;
    /** BYTE array to fill in as the response is read via didReceiveData */
    std::vector<uint8> ContentData;
    /** Cached content length from completed response */
    int32              ContentSize;
    /** Cached code from completed response */
    HTTPCode           ResponseCode;
    /** Cached key/value header pairs. Parsed once request completes */
    KeyValueMap        Headers;
    /** message for http code */
    std::string        ResponseMessage;
    /** True when the response has finished async processing */
    bool volatile      bIsReady;
    /** True if the response was successfully received/processed */
    bool volatile      bIsSucceeded;
    std::string        Cookies;
    std::string        ErrorMsg;
    /** url to verify the connection worked */
    std::string        EffectiveUrl;
};

NS_FK_END

#endif // FOUNDATIONKIT_HTTPRESPONSE_HPP
