

#include "HTTPRequest.hpp"
#include "HTTPClient.hpp"
#include "HTTPResponse.hpp"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"

NS_FK_BEGIN

HTTPRequest::HTTPRequest(CURLSH* inShareHandle, bool enableDebug/* = false*/)
: onRequestCompleteDelegate(nullptr)
, onRequestProgressDelegate(nullptr)
, _easyHandle(nullptr)
, _headerList(nullptr)
, _requestType(MethodType::GET)
, _bCanceled(false)
, _bCompleted(false)
, _addToMultiResult(CURLM_OK)
, _completionResult(CURLE_OK)
, _bytesSent(0)
, _completionStatus(HttpStatusCode::NotStarted)
, _elapsedTime(0.0f)
, _timeSinceLastResponse(0.0f)
, _enableDebug(enableDebug)
, _url()
, _headers()
, _requestPayload()
, _response(nullptr)
, _errorBuffer{0}
{
    internalInitialize(inShareHandle);
}


HTTPRequest::~HTTPRequest()
{
    if (_easyHandle)
    {
        // cleanup the handle first (that order is used in howtos)
        curl_easy_cleanup(_easyHandle);
        // destroy headers list
        if (_headerList)
        {
            curl_slist_free_all(_headerList);
        }
    }
}

HTTPRequest& HTTPRequest::setURL(const std::string& url)
{
    _url = url;
    return (*this);
}


std::string HTTPRequest::getURL() const
{
    return _url;
}








size_t HTTPRequest::staticUploadCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(ptr,"HTTPRequest::staticUploadCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticUploadCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->uploadCallback(ptr, sizeInBlocks, blockSizeInBytes);
}


size_t HTTPRequest::uploadCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    return 0;
}

size_t HTTPRequest::staticReceiveResponseHeaderCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(ptr, "HTTPRequest::staticReceiveResponseHeaderCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticReceiveResponseHeaderCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->receiveResponseHeaderCallback(ptr, sizeInBlocks, blockSizeInBytes);
}

size_t HTTPRequest::receiveResponseHeaderCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    return 0;
}

size_t HTTPRequest::staticReceiveResponseBodyCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(ptr, "HTTPRequest::staticReceiveResponseBodyCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticReceiveResponseBodyCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->receiveResponseBodyCallback(ptr, sizeInBlocks, blockSizeInBytes);
}

size_t HTTPRequest::receiveResponseBodyCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    return 0;
}

size_t HTTPRequest::staticDebugCallback(CURL* handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData)
{
    ASSERTED(handle, "HTTPRequest::staticDebugCallback param [handle] is null.");
    ASSERTED(userData, "HTTPRequest::staticDebugCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->debugCallback(handle, debugInfoType, debugInfo, debugInfoSize);
}


size_t HTTPRequest::debugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize)
{
    ASSERTED((handle == _easyHandle), "HTTPRequest::debugCallback The handle is not us's");	// make sure it's us
    switch (debugInfoType)
    {
    case CURLINFO_DATA_IN:
        LOG_INFO("DATA_IN %p: Received data [%s] (%d bytes)", this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_DATA_OUT:
        LOG_INFO("DATA_OUT %p: Sent data [%s] (%d bytes)", this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_HEADER_IN:
        LOG_INFO("HEADER_IN %p: Received header [%s] (%d bytes)", this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_HEADER_OUT:
        LOG_INFO("HEADER_OUT %p: Sent header [%s] (%d bytes)", this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_SSL_DATA_IN:
        LOG_INFO("SSL_DATA_IN %p: Received data [%s] (%d bytes)", this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_SSL_DATA_OUT:
        LOG_INFO("SSL_DATA_OUT %p: Sent data [%s] (%d bytes)", this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_TEXT:
    {
        std::string debugText = debugInfo;
        debugText.replace(debugText.begin(), debugText.end(), "\n", "");
        debugText.replace(debugText.begin(), debugText.end(), "\r", "");
        LOG_INFO("TEXT %p:%s", this, debugText.c_str());
    }
        break;
    default:
        LOG_INFO("%p: DebugCallback: Unknown DebugInfoType=%d debugInfo [%s](DebugInfoSize: %d bytes)", this, (int32)debugInfoType, debugInfo, debugInfoSize);
        break;
    }
    return 0;
}


void HTTPRequest::internalInitialize(CURLSH* inShareHandle)
{
    _easyHandle = curl_easy_init();

    curl_easy_setopt(_easyHandle, CURLOPT_SHARE, inShareHandle);

    // allow http redirects to be followed
    curl_easy_setopt(_easyHandle, CURLOPT_FOLLOWLOCATION, 1L);

    // required for all multi-threaded handles
    curl_easy_setopt(_easyHandle, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(_easyHandle, CURLOPT_FOLLOWLOCATION, 1L);

    // receive error messages
    curl_easy_setopt(_easyHandle, CURLOPT_ERRORBUFFER, _errorBuffer);

    // associate with this just in case
    curl_easy_setopt(_easyHandle, CURLOPT_PRIVATE, this);

    // set certificate verification (disable to allow self-signed certificates)
    if (HTTPClient::HTTPRequestOptions.EnableVerifyPeer)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_SSL_VERIFYPEER, 1L);
    }
    else
    {
        curl_easy_setopt(_easyHandle, CURLOPT_SSL_VERIFYPEER, 0L);
    }

    if (HTTPClient::HTTPRequestOptions.IsUseHttpProxy)
    {
        // guaranteed to be valid at this point
        curl_easy_setopt(_easyHandle, CURLOPT_PROXY, HTTPClient::HTTPRequestOptions.HttpProxyAddress.c_str());
    }

    if (HTTPClient::HTTPRequestOptions.IsDontReuseConnections)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_FORBID_REUSE, 1L);
    }

    if (!HTTPClient::HTTPRequestOptions.CertBundlePath.empty())
    {
        curl_easy_setopt(_easyHandle, CURLOPT_CAINFO, HTTPClient::HTTPRequestOptions.CertBundlePath.c_str());
    }

    if (_enableDebug)
    {
        // set debug functions (FIXME: find a way to do it only if LogHttp is >= Verbose)
        curl_easy_setopt(_easyHandle, CURLOPT_DEBUGDATA, this);
        curl_easy_setopt(_easyHandle, CURLOPT_DEBUGFUNCTION, staticDebugCallback);
        curl_easy_setopt(_easyHandle, CURLOPT_VERBOSE, 1L);
    }

    // set up header function to receive response headers
    curl_easy_setopt(_easyHandle, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(_easyHandle, CURLOPT_HEADERFUNCTION, staticReceiveResponseHeaderCallback);

    // set up write function to receive response payload
    curl_easy_setopt(_easyHandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(_easyHandle, CURLOPT_WRITEFUNCTION, staticReceiveResponseBodyCallback);
}

bool HTTPRequest::internalSetup()
{
    ASSERTED(_easyHandle, "");

    _bCompleted = false;
    _bCanceled = false;
    _addToMultiResult = CURLM_OK;
    curl_slist_free_all(_headerList);
    _headerList = nullptr;

    // Prevent overlapped requests using the same instance
    if (_completionStatus == HttpStatusCode::Processing)
    {
        LOG_WARN("ProcessRequest failed. Still processing last request.");
        return false;
    }
    // Nothing to do without a valid URL
    else if (_url.empty())
    {
        LOG_ERROR("Cannot process HTTP request: URL is empty");
        return false;
    }

    curl_easy_setopt(_easyHandle, CURLOPT_URL, _url.c_str());

    // set up method
    if (_requestType == MethodType::POST)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_POST, 1L);
        // If we don't pass any other Content-Type, RequestPayload is assumed to be URL-encoded by this time
        // (if we pass, don't check here and trust the request)
        //check(!GetHeader("Content-Type").IsEmpty() || IsURLEncoded(RequestPayload));
        curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDS, &_requestPayload.front());
        curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDSIZE, _requestPayload.size());
    }
    else if (_requestType == MethodType::PUT)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_UPLOAD, 1L);
        // this pointer will be passed to read function
        curl_easy_setopt(_easyHandle, CURLOPT_READDATA, this);
        curl_easy_setopt(_easyHandle, CURLOPT_READFUNCTION, staticUploadCallback);
        curl_easy_setopt(_easyHandle, CURLOPT_INFILESIZE, _requestPayload.size());
        // reset the counter
        _bytesSent = 0;
    }
    else if (_requestType == MethodType::GET)
    {
        // technically might not be needed unless we reuse the handles
        curl_easy_setopt(_easyHandle, CURLOPT_HTTPGET, 1L);
    }
    else if (_requestType == MethodType::HEAD)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_NOBODY, 1L);
    }
    else if (_requestType == MethodType::DELETE)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
        // If we don't pass any other Content-Type, RequestPayload is assumed to be URL-encoded by this time
        // (if we pass, don't check here and trust the request)
        //check(!GetHeader("Content-Type").IsEmpty() || IsURLEncoded(RequestPayload));
        curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDS, &_requestPayload.front());
        curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDSIZE, _requestPayload.size());
    }
    else
    {
        LOG_ERROR("Unsupported method '%d', can be perhaps added with CURLOPT_CUSTOMREQUEST", _requestType);
    }

    // set up headers
    if (getHeader("User-Agent").empty())
    {
        setHeader("User-Agent", StringUtils::format("author=%s, client=%s, version=%s","libo","HTTPClient",HTTPClientVersionString));
    }

    // content-length should be present http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.4
    if (getHeader("Content-Length").empty())
    {
        setHeader("Content-Length", StringUtils::format("%d", _requestPayload.size()));
    }

    // Add "Pragma: no-cache" to mimic WinInet behavior
    if (getHeader("Pragma").empty())
    {
        setHeader("Pragma", "no-cache");
    }

    // Remove "Expect: 100-continue" since this is supposed to cause problematic behavior on Amazon ELB (and WinInet doesn't send that either)
    // (also see http://www.iandennismiller.com/posts/curl-http1-1-100-continue-and-multipartform-data-post.html , http://the-stickman.com/web-development/php-and-curl-disabling-100-continue-header/ )
    if (getHeader("Expect").empty())
    {
        setHeader("Expect", "");
    }

    auto allHeaders = getAllHeaders();

    for (auto& header : allHeaders)
    {
        _headerList = curl_slist_append(_headerList, header.c_str());
    }

    if (_headerList)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_HTTPHEADER, _headerList);
    }

    return true;
}
NS_FK_END



