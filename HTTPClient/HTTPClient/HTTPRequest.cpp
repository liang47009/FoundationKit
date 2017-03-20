#include <algorithm>
#include <limits>
#include <cmath>
#include <algorithm>
#include "HTTPRequest.hpp"
#include "HTTPClient.hpp"
#include "HTTPResponse.hpp"
#include "MimeTypes.h"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"


NS_FK_BEGIN
namespace network{

static const long LOW_SPEED_LIMIT = 1;
static const long LOW_SPEED_TIME = 5;

HTTPRequest::HTTPRequest(bool enableDebug/* = false*/)
: onRequestCompleteDelegate(nullptr)
, onRequestProgressDelegate(nullptr)
, _easyHandle(nullptr)
, _headerList(nullptr)
, _requestType(MethodType::GET)
, _bCanceled(false)
, _bCompleted(false)
, _completionResult(CURLE_OK)
, _addToMultiResult(CURLM_OK)
, _bytesSent(0)
, _requestStatus(HttpStatusCode::NotStarted)
, _enableDebug(enableDebug)
, _url()
, _headers()
, _requestPayload()
, _response(nullptr)
, _bTimeout(false)
, _httpTimeOut(5.0f) // 5 seconds timeout
, _elapsedTime(0.0f)
, _bFinished(false)
, _formPost(nullptr)
{
    memset(_errorBuffer, 0, CURL_ERROR_SIZE + 1);
    _easyHandle = curl_easy_init();

    curl_easy_setopt(_easyHandle, CURLOPT_SHARE, HTTPClient::_G_shareHandle);


    curl_easy_setopt(_easyHandle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);

    // allow http redirects to be followed
    curl_easy_setopt(_easyHandle, CURLOPT_FOLLOWLOCATION, 1L);

    // required for all multi-threaded handles
    curl_easy_setopt(_easyHandle, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(_easyHandle, CURLOPT_FOLLOWLOCATION, 1L);

    // receive error messages
    curl_easy_setopt(_easyHandle, CURLOPT_ERRORBUFFER, _errorBuffer);

    // associate with this just in case
    curl_easy_setopt(_easyHandle, CURLOPT_PRIVATE, this);

    curl_easy_setopt(_easyHandle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    // set certificate verification (disable to allow self-signed certificates)
    if (HTTPClient::HTTPRequestOptions.EnableVerifyPeer)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(_easyHandle, CURLOPT_SSL_VERIFYHOST, 1L);
        
    }
    else
    {
        curl_easy_setopt(_easyHandle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(_easyHandle, CURLOPT_SSL_VERIFYHOST, 0L);
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

    // set progress function
    curl_easy_setopt(_easyHandle, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(_easyHandle, CURLOPT_XFERINFODATA, this);
    curl_easy_setopt(_easyHandle, CURLOPT_XFERINFOFUNCTION, staticProgressCallback);
    curl_easy_setopt(_easyHandle, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
    curl_easy_setopt(_easyHandle, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);
    //curl_easy_setopt(_easyHandle, CURLOPT_MAX_SEND_SPEED_LARGE, 1024 * 100); // limit send speed 100kb/s
    //curl_easy_setopt(_easyHandle, CURLOPT_MAX_RECV_SPEED_LARGE, 1024 * 100); // limit recv speed 100kb/s
}


HTTPRequest::~HTTPRequest()
{
    if (_easyHandle)
    {
        if (_formPost)
        {
            curl_formfree(_formPost);
        }

        // cleanup the handle first (that order is used in howtos)
        curl_easy_cleanup(_easyHandle);
        // destroy headers list
        if (_headerList)
        {
            curl_slist_free_all(_headerList);
        }
    }
}

HTTPRequest::Pointer HTTPRequest::create(bool enableDebug/* = false*/)
{
    HTTPRequest::Pointer shared_httprequest = std::make_shared<HTTPRequest>(enableDebug);
    return shared_httprequest;
}

HTTPRequest& HTTPRequest::setURL(const std::string& url)
{
    _url = url;
    return (*this);
}

HTTPRequest& HTTPRequest::setMethod(MethodType method)
{
    _requestType = method;
    return (*this);
}


HTTPRequest& HTTPRequest::setHeader(const std::string& headerName, const std::string& headerValue)
{
    _headers[headerName] = headerValue;
    return (*this);
}

HTTPRequest& HTTPRequest::setPostField(const std::string& name, const std::string& value)
{
    _postFields[name] = value;
    return (*this);
}

HTTPRequest& HTTPRequest::setContentField(const std::string& name, const std::string& value)
{
    _contentFields[name] = value;
    return (*this);
}


HTTPRequest& HTTPRequest::setFileField(const std::string& name, const std::string& fullPath)
{
    _fileFields[name] = fullPath;
    return (*this);
}

HTTPRequest& HTTPRequest::setRequestPayload(const std::vector<uint8>& data)
{
    _requestPayload = data;
    return (*this);
}

HTTPRequest& HTTPRequest::setTimeOut(float seconds)
{
    _httpTimeOut = seconds;
    return (*this);
}

//----------------------------------------------------------------

std::string HTTPRequest::getURL() const
{
    return _url;
}

std::string HTTPRequest::getURLParameter(const std::string& parameterName)
{
    std::string result;
    auto urlParams = StringUtils::split(_url, '&');
    for (auto& paramItem : urlParams)
    {
        auto paramKeyValue = StringUtils::split(paramItem, '=');
        if (paramKeyValue[0] == parameterName)
        {
            int outSize = 0;
            char*unescapedValue = curl_easy_unescape(_easyHandle, paramKeyValue[1].c_str(), static_cast<int>(paramKeyValue[1].size()), &outSize);
            result = unescapedValue;
            curl_free(unescapedValue);
        }
    }
    return result;
}


std::string HTTPRequest::getHeader(const std::string& headerName)
{
    std::string result;
    auto headerIter = _headers.find(headerName);
    if (headerIter != _headers.end())
    {
        result = headerIter->second;
    }
    return result;
}

std::vector<std::string> HTTPRequest::getAllHeaders()
{
    std::vector<std::string> result;
    for (auto headerIter : _headers)
    {
        result.push_back(headerIter.first + ":" + headerIter.second);
    }
    return result;
}

HTTPRequest::KeyValueMap& HTTPRequest::getPostFields()
{
    return _postFields;
}

HTTPRequest::KeyValueMap& HTTPRequest::getContentFields()
{
    return _contentFields;
}

HTTPRequest::KeyValueMap& HTTPRequest::getFileFields()
{
    return _fileFields;
}

std::vector<uint8>& HTTPRequest::getRequestPayload()
{
    return _requestPayload;
}

CURL* HTTPRequest::getEasyHandle()
{
    return _easyHandle;
}

HttpStatusCode HTTPRequest::getRequestStatus()
{
    return _requestStatus;
}

HTTPRequest::ResponsePtr HTTPRequest::getResponse()
{
    return _response;
}

float HTTPRequest::getTimeOut()
{
    return _httpTimeOut;
}

void HTTPRequest::cancel()
{
    _bCanceled = true;
}

bool HTTPRequest::isFinished()
{
    return _bFinished;
}

void HTTPRequest::dumpInfo()
{
    std::string requestInfo;
    requestInfo += getURL();
    requestInfo += "\n";
    requestInfo += StringUtils::join("\n", getAllHeaders());
    requestInfo += "\n";
    requestInfo += "------PostFields---------------------\n";
    for (auto iter: _postFields)
    {
        requestInfo += iter.first;
        requestInfo += ":";
        requestInfo += iter.second;
        requestInfo += "\n";
    }

    requestInfo += "------ContentFields---------------------\n";
    for (auto iter : _contentFields)
    {
        requestInfo += iter.first;
        requestInfo += ":";
        requestInfo += iter.second;
        requestInfo += "\n";
    }

    requestInfo += "------FileFields---------------------\n";
    for (auto iter : _fileFields)
    {
        requestInfo += iter.first;
        requestInfo += ":";
        requestInfo += iter.second;
        requestInfo += "\n";
    }
    requestInfo += "------PostFields String---------------------\n";
    if (_requestPayload.empty())
    {
        requestInfo.append("");
    }
    else
    {
        requestInfo.append((char*)&_requestPayload[0], _requestPayload.size());
    }

    requestInfo += "\n";
    requestInfo += "Request Status:";
    requestInfo += StringUtils::to_string(static_cast<int>(_requestStatus));
    LOG_INFO("---------------------Dump Request-----------------------\n");
    LOG_INFO(requestInfo.c_str());
    LOG_INFO("---------------------Dump Request End-----------------------\n");
}

void HTTPRequest::tick(float deltaTime)
{
    _elapsedTime += deltaTime;
    if (_elapsedTime >= _httpTimeOut)
    {
        _bTimeout = true;
    }

    if (_bCompleted || _bCanceled || _bTimeout)
    {
        finishedRequest();
    }

}

bool HTTPRequest::isTickable()const
{
    return true;
}

void HTTPRequest::setCompleted(CURLcode completionResult)
{
    _bCompleted = true;
    _completionResult = completionResult;
}

void HTTPRequest::setAddToMultiResult(CURLMcode result)
{
    _addToMultiResult = result;
}

void HTTPRequest::setRequestStatus(HttpStatusCode status)
{
    _requestStatus = status;
}

bool HTTPRequest::build()
{
    ASSERTED(_easyHandle, "");

    _bCompleted = false;
    _bCanceled = false;
    _bFinished = false;
    _addToMultiResult = CURLM_OK;
    curl_slist_free_all(_headerList);
    _headerList = nullptr;

    // Prevent overlapped requests using the same instance
    if (_requestStatus == HttpStatusCode::Processing)
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

    buildPostPayload();
    // set up method
    if (_requestType == MethodType::POST)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_POST, 1L);
        // If we don't pass any other Content-Type, RequestPayload is assumed to be URL-encoded by this time
        // (if we pass, don't check here and trust the request)
        if (!_requestPayload.empty())
        {
            curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDS, &_requestPayload.front());
            curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDSIZE, _requestPayload.size());
        }
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
        if (_requestPayload.size()>0)
        {
            curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDS, &_requestPayload.front());
            curl_easy_setopt(_easyHandle, CURLOPT_POSTFIELDSIZE, _requestPayload.size());
        }
    }
    else
    {
        LOG_ERROR("Unsupported method '%d', can be perhaps added with CURLOPT_CUSTOMREQUEST", _requestType);
    }

    buildFormPayload();

    // set up headers
    if (getHeader("User-Agent").empty())
    {
        setHeader("User-Agent", StringUtils::format("author=%s, client=%s, version=%s", "libo", "HTTPClient", HTTPClientVersionString));
    }

    // content-length should be present http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.4
    if (getHeader("Content-Length").empty())
    {
        //setHeader("Content-Length", StringUtils::format("%d", _requestPayload.size()));
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

    // Mark as in-flight to prevent overlapped requests using the same object
    _requestStatus = HttpStatusCode::Processing;
    // Response object to handle data that comes back after starting this request
    _response = HTTPResponse::create(shared_from_this());
    return true;
}

void HTTPRequest::buildPostPayload()
{
    std::stringbuf buf;
    bool needAnd = (!_requestPayload.empty());
    for (const auto &postField : _postFields)
    {
        if (needAnd)
            buf.sputc('&');
        char *part = curl_easy_escape(_easyHandle, postField.first.c_str(), 0);
        buf.sputn(part, strlen(part));
        buf.sputc('=');
        curl_free(part);
        part = curl_easy_escape(_easyHandle, postField.second.c_str(), 0);
        buf.sputn(part, strlen(part));
        curl_free(part);
        needAnd = true;
    }
    std::string strPostFeilds = buf.str();
    _requestPayload.reserve(_requestPayload.size() + strPostFeilds.size());
    _requestPayload.insert(_requestPayload.end(), strPostFeilds.begin(), strPostFeilds.end());
}

void HTTPRequest::buildFormPayload()
{
    if (!_fileFields.empty() || !_contentFields.empty())
    {
        setHeader("Content-Type", "multipart/form-data;charset=utf-8");
    }
    //set form files
    if (_formPost)
    {
        curl_formfree(_formPost);
    }
    _formPost = nullptr;
    curl_httppost *lastPost = nullptr;
    for (const auto &fileItem : _fileFields)
    {
        curl_formadd(&_formPost, &lastPost,
            CURLFORM_COPYNAME, fileItem.first.c_str(),
            CURLFORM_FILE, fileItem.second.c_str(),
            CURLFORM_CONTENTTYPE, MimeTypes::getMimeType(fileItem.second).c_str(),
            CURLFORM_END);
    }

    //set form contents
    for (const auto &contentItem : _contentFields)
    {
        curl_formadd(&_formPost, &lastPost,
            CURLFORM_COPYNAME, contentItem.first.c_str(),
            CURLFORM_COPYCONTENTS, contentItem.second.c_str(),
            CURLFORM_END);
    }
    if (_formPost)
    {
        curl_easy_setopt(_easyHandle, CURLOPT_HTTPPOST, _formPost);
    }
}

void HTTPRequest::finishedRequest()
{
    if (!_response)
    {
        _response = HTTPResponse::create(shared_from_this());
    }

    std::string operatingCurlMsg;
    // if completed, get more info
    if (_bCompleted)
    {
        long httpCode = 0;
        curl_easy_getinfo(_easyHandle, CURLINFO_RESPONSE_CODE, &httpCode);
        operatingCurlMsg = curl_easy_strerror(_completionResult);
        operatingCurlMsg += ":";
        double contentLengthDownload = 0.0;
        curl_easy_getinfo(_easyHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLengthDownload);
        curl_slist *curlCookies = NULL;
        curl_easy_getinfo(_easyHandle, CURLINFO_COOKIELIST, &curlCookies);
        std::string responseCookies;
        if (curlCookies)
        {
            struct curl_slist *nc = curlCookies;
            std::stringbuf buf;
            while (nc)
            {
                buf.sputn(nc->data, strlen(nc->data));
                buf.sputc('\n');
                nc = nc->next;
            }
            responseCookies = buf.str();
            curl_slist_free_all(curlCookies);
            curlCookies = nullptr;
        }
        _response->setSucceeded(CURLE_OK == _completionResult)
            .setResponseCode(httpCode)
            .setContentSize(static_cast<int32>(contentLengthDownload))
            .setCookies(responseCookies);

        // Mark last request attempt as completed but failed
        switch (_completionResult)
        {
        case CURLE_OK:break;
        case CURLE_COULDNT_CONNECT:
        case CURLE_COULDNT_RESOLVE_PROXY:
        case CURLE_COULDNT_RESOLVE_HOST:
            // report these as connection errors (safe to retry)
            setRequestStatus(HttpStatusCode::Failed_ConnectionError);
            break;
        default:
            setRequestStatus(HttpStatusCode::Failed);
        }
    }
    else if(_bCanceled)
    {
        setRequestStatus(HttpStatusCode::Canceled);
        std::string errMsg = "The request has been canceled";
        operatingCurlMsg += "The request has been canceled";;
    }
    else if (_bTimeout)
    {
        setRequestStatus(HttpStatusCode::TimeOut);
        operatingCurlMsg += "The request time-out,";
    }
    else
    {
        ASSERTED(false, "Operation not supported,");
    }

    if (_response->isSucceeded())
    {
        const bool bDebugServerResponse = _response->getResponseCode() >= 500 && _response->getResponseCode() <= 505;
        // log info about error responses to identify failed downloads
        if (bDebugServerResponse)
        {
            LOG_WARN("%p: request has been successfully processed. URL: %s, HTTP code: %d, content length: %d, actual payload size: %d",
                this, getURL().c_str(), _response->getResponseCode(), _response->getContentSize(), _response->getContentData().size());
        }
        // Mark last request attempt as completed successfully
        setRequestStatus(HttpStatusCode::Succeeded);
    }
    operatingCurlMsg += _errorBuffer;
    _response->setReady(true).setErrorMsg(operatingCurlMsg);

    // Call delegate with valid request/response objects
    if (onRequestCompleteDelegate)
    {
        onRequestCompleteDelegate(shared_from_this(), _response, true);
    }

    _bFinished = true;
}

size_t HTTPRequest::staticUploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(buffer, "HTTPRequest::staticUploadCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticUploadCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->uploadCallback(buffer, sizeInBlocks, blockSizeInBytes);
}

size_t HTTPRequest::staticReceiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(buffer, "HTTPRequest::staticReceiveResponseHeaderCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticReceiveResponseHeaderCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->receiveResponseHeaderCallback(buffer, sizeInBlocks, blockSizeInBytes);
}

size_t HTTPRequest::staticReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(buffer, "HTTPRequest::staticReceiveResponseBodyCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticReceiveResponseBodyCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->receiveResponseBodyCallback(buffer, sizeInBlocks, blockSizeInBytes);
}

int HTTPRequest::staticProgressCallback(void *userData, curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
{
    ASSERTED(userData, "HTTPRequest::staticProgressCallback param [userData] is null.");
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->progressCallback(totalDownload, nowDownload, totalUpload, nowUpload);
}

size_t HTTPRequest::staticDebugCallback(CURL* handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData)
{
    ASSERTED(handle, "HTTPRequest::staticDebugCallback param [handle] is null.");
    ASSERTED(userData, "HTTPRequest::staticDebugCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->debugCallback(handle, debugInfoType, debugInfo, debugInfoSize);
}

size_t HTTPRequest::uploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    _elapsedTime = 0.0f;
    size_t sizeToSend = _requestPayload.size() - _bytesSent;
    size_t sizeToSendThisTime = 0;
    if (sizeToSend != 0)
    {
        sizeToSendThisTime = (std::min)(sizeToSend, sizeInBlocks * blockSizeInBytes);
        if (sizeToSendThisTime != 0)
        {
            // static cast just ensures that this is uint8* in fact
            ::memcpy(buffer, static_cast<uint8*>(&_requestPayload[_bytesSent]), sizeToSendThisTime);
            _bytesSent += sizeToSendThisTime;
        }
    }
    return sizeToSendThisTime;
}


size_t HTTPRequest::receiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    ASSERTED(_response, "");
    _elapsedTime = 0.0f;
    if (_response)
    {
        size_t headerSize = sizeInBlocks * blockSizeInBytes;
        if (headerSize > 0 && headerSize <= CURL_MAX_HTTP_HEADER)
        {
            std::string headers(reinterpret_cast<char*>(buffer));
            // kill \n\r
            std::replace(headers.begin(), headers.end(), '\n', ' ');
            std::replace(headers.begin(), headers.end(), '\r', ' ');
            if (_enableDebug)
            {
                LOG_INFO("%p: Received response header '%s'.", this, headers.c_str());
            }
            auto headerKeyValue = StringUtils::split(headers, ':');

            if (headerKeyValue.size() == 2)
            {
                std::string headerKey = headerKeyValue[0];
                std::string headerValue = headerKeyValue[1];
                if (!headerKey.empty() && !headerValue.empty())
                {
                    std::string previousValue = _response->getHeader(headerKey);
                    std::string newValue;
                    if (!previousValue.empty())
                    {
                        newValue = previousValue + ", ";
                    }
                    newValue += StringUtils::trim(headerValue);
                    _response->setHeader(headerKey, newValue);

                    //Store the content length so OnRequestProgress() delegates have something to work with
                    if (headerKey == ("Content-Length"))
                    {
                        _response->setContentSize(std::atoi(StringUtils::trim(headerValue).c_str()));
                    }
                }
            }
            return headerSize;
        }
        else
        {
            LOG_WARN("%p: Could not process response header for request - header size (%d) is invalid.", this, headerSize);
        }
    }
    else
    {
        LOG_WARN("%p: Could not download response header for request - response not valid.", this);
    }
    return 0;
}

size_t HTTPRequest::receiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    ASSERTED(_response, "");
    _elapsedTime = 0.0f;
    if (_response)
    {
        size_t sizeToDownload = sizeInBlocks * blockSizeInBytes;
        if (_enableDebug)
        {
            LOG_INFO("%p: ReceiveResponseBodyCallback: %d bytes out of %d received. (SizeInBlocks=%d, BlockSizeInBytes=%d, Response->TotalBytesRead=%d, Response->GetContentLength()=%d, SizeToDownload=%d (<-this will get returned from the callback))",
                this,
                static_cast<int32>(_response->getContentData().size() + sizeToDownload), _response->getContentSize(),
                static_cast<int32>(sizeInBlocks), static_cast<int32>(blockSizeInBytes), _response->getContentData().size(), _response->getContentSize(), static_cast<int32>(sizeToDownload)
                );
        }

        // note that we can be passed 0 bytes if file transmitted has 0 length
        if (sizeToDownload > 0)
        {
            auto& responseData = _response->getContentData();
            responseData.insert(responseData.end(), (char*)buffer, (char*)buffer + sizeToDownload);
            return sizeToDownload;
        }
    }
    else
    {
        LOG_WARN("%p: Could not download response data for request - response not valid.", this);
    }

    return 0;	// request will fail with write error if we had non-zero bytes to download
}


int HTTPRequest::progressCallback(curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
{
    _elapsedTime = 0.0f;
    if (onRequestProgressDelegate)
    {
        onRequestProgressDelegate(shared_from_this(), totalDownload, nowDownload, totalUpload, nowUpload);
    }
    return CURLE_OK;
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
        std::replace(debugText.begin(), debugText.end(), '\n', ' ');
        std::replace(debugText.begin(), debugText.end(), '\r', ' ');
        LOG_INFO("TEXT %p:%s", this, debugText.c_str());
    }
    break;
    default:
        LOG_INFO("%p: DebugCallback: Unknown DebugInfoType=%d debugInfo [%s](DebugInfoSize: %d bytes)", this, (int32)debugInfoType, debugInfo, debugInfoSize);
        break;
    }
    return 0;
}

} // namespace network
NS_FK_END







