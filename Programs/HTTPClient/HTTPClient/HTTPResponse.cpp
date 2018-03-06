/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPRESPONSE_CPP
#define NETFRAMEWORK_HTTPRESPONSE_CPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "HTTPClient/HTTPRequest.hpp"
#include "HTTPClient/HTTPResponse.hpp"

NS_FK_BEGIN

HTTPResponse::HTTPResponse(RequestWeakPtr pRequest)
    : RequestInstance(pRequest)
    , ContentData()
    , ContentSize(0)
    , ResponseCode(0)
    , Headers()
    , ResponseMessage()
    , bIsReady(false)
    , bIsSucceeded(false)
    , Cookies()
    , ErrorMsg()
{

}

HTTPResponse::~HTTPResponse()
{

}

HTTPResponse::Pointer HTTPResponse::Create(RequestWeakPtr pRequest)
{
    HTTPResponse::Pointer shared_response(new (std::nothrow) HTTPResponse(pRequest));
    return shared_response;
}

std::string HTTPResponse::GetURL()
{
    if (auto request = RequestInstance.lock())
    {
        return request->GetURL();
    }
    return "";
}

std::string HTTPResponse::GetEffectiveURL()
{
    return EffectiveUrl;
}

std::string HTTPResponse::GetURLParameter(const std::string& parameterName)
{
    if (auto request = RequestInstance.lock())
    {
        return request->GetURLParameter(parameterName);
    }
    return "";
}

std::string HTTPResponse::GetHeader(const std::string& headerName)
{
    std::string result;
    auto headerIter = Headers.find(headerName);
    if (headerIter != Headers.end())
    {
        result = headerIter->second;
    }
    return result;
}

std::vector<std::string> HTTPResponse::GetAllHeaders()
{
    std::vector<std::string> result;
    for (auto headerIter : Headers)
    {
        result.push_back(headerIter.first + ":" + headerIter.second);
    }
    return result;
}

std::string HTTPResponse::GetContentType()
{
    return GetHeader("Content-Type");
}

size_t HTTPResponse::GetContentSize()
{
    return ContentSize > 0 ? ContentSize : ContentData.size();
}

std::vector<uint8>& HTTPResponse::GetContentData()
{
    return ContentData;
}

HTTPCode HTTPResponse::GetResponseCode()
{
    return ResponseCode;
}


std::string HTTPResponse::GetCookies()
{
    return Cookies;
}

std::string HTTPResponse::GetResponseMsg()
{
    return ResponseMessage;
}

std::string HTTPResponse::GetErrorMsg()
{
    return ErrorMsg;
}

bool HTTPResponse::IsReady()
{
    return bIsReady;
}

bool HTTPResponse::IsSucceeded()
{
    return bIsSucceeded;
}

void HTTPResponse::DumpInfo()
{
    std::string responseInfo;
    responseInfo += "URL:";  responseInfo += GetURL();
    responseInfo += "\n";
    responseInfo += "Headers:\n";
    responseInfo += StringUtils::Join("\n", GetAllHeaders());
    responseInfo += "\n";
    responseInfo += "ResponseCode:";  responseInfo += GetResponseCode().ToString();
    responseInfo += "\n";
    responseInfo += "Cookies:";  responseInfo += GetCookies();
    responseInfo += "\n";
    responseInfo += "ResponseMeg:";  responseInfo += GetResponseMsg();
    responseInfo += "\n";
    responseInfo += "ErrorMsg:";  responseInfo += GetErrorMsg();
    responseInfo += "\n";
    responseInfo += "ContentType:";  responseInfo += GetContentType();
    responseInfo += "\n";
    responseInfo += "ContentSize:";  responseInfo += StringUtils::Tostring(GetContentSize());
    responseInfo += "\n";
    responseInfo += "ContentData:"; 
    if (!ContentData.empty())
    {
        responseInfo += (char*)(&ContentData[0]);
    }
    responseInfo += "\n";
    FKLog("---------------------Dump Response-----------------------\n");
    responseInfo += "\n";
    FKLog(responseInfo.c_str());
    FKLog("---------------------Dump Response End-----------------------\n");
}

HTTPResponse& HTTPResponse::SetHeader(const std::string& headerName, const std::string& headerValue)
{
    Headers.insert(std::make_pair(headerName, headerValue));
    return (*this);
}

HTTPResponse& HTTPResponse::SetContentSize(int contentSize)
{
    ContentSize = contentSize;
    return (*this);
}

HTTPResponse& HTTPResponse::SetCookies(const std::string& cookies)
{
    Cookies = cookies;
    return (*this);
}

HTTPResponse& HTTPResponse::SetReady(bool bReady)
{
    bIsReady = bReady;
    return (*this);
}

HTTPResponse& HTTPResponse::SetSucceeded(bool bSucceeded)
{
    bIsSucceeded = bSucceeded;
    return (*this);
}

HTTPResponse& HTTPResponse::SetResponseCode(long responseCode)
{
    ResponseCode = responseCode;
    return (*this);
}

HTTPResponse& HTTPResponse::SetResponseCode(const HTTPCode& responseCode)
{
    ResponseCode = responseCode;
    return (*this);
}

HTTPResponse& HTTPResponse::SetResponseMsg(const std::string& responseMsg)
{
    ResponseMessage = responseMsg;
    return (*this);
}

HTTPResponse& HTTPResponse::SetErrorMsg(const std::string& errorMsg)
{
    ErrorMsg = errorMsg;
    return (*this);

}

HTTPResponse& HTTPResponse::SetEffectiveURL(const std::string& effectiveUrl)
{
    EffectiveUrl = effectiveUrl;
    return (*this);
}

NS_FK_END

#endif // END OF NETFRAMEWORK_HTTPRESPONSE_CPP
