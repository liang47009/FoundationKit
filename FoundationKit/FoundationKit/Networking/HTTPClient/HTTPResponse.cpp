
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"
NS_FK_BEGIN
namespace network{
HTTPResponse::HTTPResponse(RequestPtr pRequest)
    : _request(pRequest)
    , _contentData()
    , _contentSize(0)
    , _responseCode(0)
    , _headers()
    , _responseMessage()
    , _bIsReady(false)
    , _bSucceeded(false)
    , _cookies()
    , _errorMsg()
{

}

HTTPResponse::~HTTPResponse()
{

}

HTTPResponse::Pointer HTTPResponse::create(RequestPtr pRequest)
{
    HTTPResponse::Pointer shared_response(new (std::nothrow) HTTPResponse(pRequest));
    return shared_response;
}

std::string HTTPResponse::getURL()
{
    if (auto request = _request.lock())
    {
        return request->getURL();
    }
    return "";
}

std::string HTTPResponse::getURLParameter(const std::string& parameterName)
{
    if (auto request = _request.lock())
    {
        return request->getURLParameter(parameterName);
    }
    return "";
}

std::string HTTPResponse::getHeader(const std::string& headerName)
{
    std::string result;
    if (!_bIsReady)
    {
        LOG_WARN("Can't get cached header [%s]. Response still processing. %p", headerName.c_str(), &_request);
    }
    else
    {
        auto headerIter = _headers.find(headerName);
        if (headerIter != _headers.end())
        {
            result = headerIter->second;
        }
    }
    return result;
}

std::vector<std::string> HTTPResponse::getAllHeaders()
{
    std::vector<std::string> result;
    if (!_bIsReady)
    {
        LOG_WARN("Can't get cached headers. Response still processing. %p", &_request);
    }
    else
    {
        for (auto headerIter : _headers)
        {
            result.push_back(headerIter.first + ":" + headerIter.second);
        }
    }
    return result;
}

std::string HTTPResponse::getContentType()
{
    return getHeader("Content-Type");
}

size_t HTTPResponse::getContentSize()
{
    return _contentSize;
}

std::vector<uint8>& HTTPResponse::getContentData()
{
    return _contentData;
}

long HTTPResponse::getResponseCode()
{
    return _responseCode;
}


std::string HTTPResponse::getCookies()
{
    return _cookies;
}

std::string HTTPResponse::getResponseMsg()
{
    return _responseMessage;
}

std::string HTTPResponse::getErrorMsg()
{
    return _errorMsg;
}

bool HTTPResponse::isReady()
{
    return _bIsReady;
}

bool HTTPResponse::isSucceeded()
{
    return _bSucceeded;
}

void HTTPResponse::dumpInfo()
{
    std::string responseInfo;
    responseInfo += "URL:";  responseInfo += getURL();
    responseInfo += "\n";
    responseInfo += "Headers:\n";
    responseInfo += StringUtils::join("\n", getAllHeaders());
    responseInfo += "\n";
    responseInfo += "ResponseCode:";  responseInfo += StringUtils::to_string(getResponseCode());
    responseInfo += "\n";
    responseInfo += "Cookies:";  responseInfo += getCookies();
    responseInfo += "\n";
    responseInfo += "ResponseMeg:";  responseInfo += getResponseMsg();
    responseInfo += "\n";
    responseInfo += "ErrorMsg:";  responseInfo += getErrorMsg();
    responseInfo += "\n";
    responseInfo += "ContentType:";  responseInfo += getContentType();
    responseInfo += "\n";
    responseInfo += "ContentSize:";  responseInfo += StringUtils::to_string(getContentSize());
    responseInfo += "\n";
    responseInfo += "ContentData:"; 
    if (!_contentData.empty())
    {
        responseInfo += (char*)(&_contentData[0]);
    }

    LOG_INFO("---------------------Dump Response-----------------------\n");
    LOG_INFO(responseInfo.c_str());
    LOG_INFO("---------------------Dump Response End-----------------------\n");
}

HTTPResponse& HTTPResponse::setHeader(const std::string& headerName, const std::string& headerValue)
{
    _headers.insert(std::make_pair(headerName, headerValue));
    return (*this);
}

HTTPResponse& HTTPResponse::setContentSize(int contentSize)
{
    _contentSize = contentSize;
    return (*this);
}

HTTPResponse& HTTPResponse::setCookies(const std::string& cookies)
{
    _cookies = cookies;
    return (*this);
}

HTTPResponse& HTTPResponse::setReady(bool bReady)
{
    _bIsReady = bReady;
    return (*this);
}

HTTPResponse& HTTPResponse::setSucceeded(bool bSucceeded)
{
    _bSucceeded = bSucceeded;
    return (*this);
}

HTTPResponse& HTTPResponse::setResponseCode(long responseCode)
{
    _responseCode = responseCode;
    return (*this);
}

HTTPResponse& HTTPResponse::setResponseMsg(const std::string& responseMsg)
{
    _responseMessage = responseMsg;
    return (*this);
}

HTTPResponse& HTTPResponse::setErrorMsg(const std::string& errorMsg)
{
    _errorMsg = errorMsg;
    return (*this);

}

} //namespace network
NS_FK_END




