#include <algorithm>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPClient.hpp"
#include "MimeTypes.hpp"

NS_FK_BEGIN

static const long LOW_SPEED_LIMIT = 1;
static const long LOW_SPEED_TIME = 10;

HTTPRequest::HTTPRequest(bool enableDebug/* = false*/)
    : OnRequestCompleted(nullptr)
    , OnRequestProgress(nullptr)
    , EasyHandle(nullptr)
    , HeaderList(nullptr)
    , MethodType(RequestMethodType::GET)
    , bCanceled(false)
    , bCompleted(false)
    , CompletionResult(CURLE_OK)
    , AddToMultiResult(CURLM_OK)
    , BytesSent(0)
    , Status(RequestStatus::NotStarted)
    , EnableDebug(enableDebug)
    , Url()
    , Headers()
    , FormPost(nullptr)
    , ResponseInstance(nullptr)
    , bTimeout(false)
    , TimeOutValue(5) // 5 seconds timeout
    , ElapsedTime(0.0f)
    , bFinished(false)
{
    memset(ErrorBuffer, 0, CURL_ERROR_SIZE);
    EasyHandle = curl_easy_init();

    curl_easy_setopt(EasyHandle, CURLOPT_SHARE, HTTPClient::G_shareHandle);


    curl_easy_setopt(EasyHandle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);

    // allow http redirects to be followed
    curl_easy_setopt(EasyHandle, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(EasyHandle, CURLOPT_MAXREDIRS, 10);

    curl_easy_setopt(EasyHandle, CURLOPT_BUFFERSIZE, CURL_MAX_WRITE_SIZE);

    curl_easy_setopt(EasyHandle, CURLOPT_AUTOREFERER, true);

    // required for all multi-threaded handles
    curl_easy_setopt(EasyHandle, CURLOPT_NOSIGNAL, 1L);

    // receive error messages
    curl_easy_setopt(EasyHandle, CURLOPT_ERRORBUFFER, ErrorBuffer);

    // associate with this just in case
    curl_easy_setopt(EasyHandle, CURLOPT_PRIVATE, this);

    curl_easy_setopt(EasyHandle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    // set certificate verification (disable to allow self-signed certificates)
    if (HTTPClient::HTTPRequestOptions.EnableVerifyPeer)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(EasyHandle, CURLOPT_SSL_VERIFYHOST, 1L);
    }
    else
    {
        curl_easy_setopt(EasyHandle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(EasyHandle, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    if (HTTPClient::HTTPRequestOptions.IsUseHttpProxy)
    {
        // guaranteed to be valid at this point
        curl_easy_setopt(EasyHandle, CURLOPT_PROXY, HTTPClient::HTTPRequestOptions.HttpProxyAddress.c_str());
    }

    if (HTTPClient::HTTPRequestOptions.IsDontReuseConnections)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_FORBID_REUSE, 1L);
    }

    if (!HTTPClient::HTTPRequestOptions.CertBundlePath.empty())
    {
        curl_easy_setopt(EasyHandle, CURLOPT_CAINFO, HTTPClient::HTTPRequestOptions.CertBundlePath.c_str());
    }

    if (EnableDebug)
    {
        // set debug functions (FIXME: find a way to do it only if LogHttp is >= Verbose)
        curl_easy_setopt(EasyHandle, CURLOPT_DEBUGDATA, this);
        curl_easy_setopt(EasyHandle, CURLOPT_DEBUGFUNCTION, StaticDebugCallback);
        curl_easy_setopt(EasyHandle, CURLOPT_VERBOSE, 1L);
    }

    // set up header function to receive response headers
    curl_easy_setopt(EasyHandle, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(EasyHandle, CURLOPT_HEADERFUNCTION, StaticReceiveResponseHeaderCallback);

    // set up write function to receive response payload
    curl_easy_setopt(EasyHandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(EasyHandle, CURLOPT_WRITEFUNCTION, StaticReceiveResponseBodyCallback);

    // set progress function
    curl_easy_setopt(EasyHandle, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(EasyHandle, CURLOPT_XFERINFODATA, this);
    curl_easy_setopt(EasyHandle, CURLOPT_XFERINFOFUNCTION, StaticProgressCallback);
}


HTTPRequest::~HTTPRequest()
{
    if (EasyHandle)
    {
        // cleanup the handle first (that order is used in howtos)
        curl_easy_cleanup(EasyHandle);
        // destroy headers list
        if (HeaderList)
        {
            curl_slist_free_all(HeaderList);
            HeaderList = nullptr;
        }

        if (FormPost)
        {
            curl_formfree(FormPost);
            FormPost = nullptr;
        }
    }
}

HTTPRequest::Pointer HTTPRequest::Create(bool enableDebug/* = false*/)
{
    HTTPRequest::Pointer shared_httprequest = std::make_shared<HTTPRequest>(enableDebug);
    return shared_httprequest;
}

HTTPRequest& HTTPRequest::SetURL(const std::string& url)
{
    Url = url;
    return (*this);
}

HTTPRequest& HTTPRequest::SetMethod(RequestMethodType method)
{
    MethodType = method;
    return (*this);
}

HTTPRequest& HTTPRequest::SetHeader(const std::string& headerName, const std::string& headerValue)
{
    Headers[headerName] = headerValue;
    return (*this);
}

HTTPRequest& HTTPRequest::SetPostField(const std::string& name, const std::string& value)
{
    PostFields[name] = value;
    return (*this);
}

HTTPRequest& HTTPRequest::SetTimeOut(int seconds)
{
    TimeOutValue = seconds;
    return (*this);
}

HTTPRequest& HTTPRequest::SetContentField(const std::string& name, const std::string& value)
{
    ContentFields[name] = value;
    return (*this);
}


HTTPRequest& HTTPRequest::SetFileField(const std::string& name, const std::string& fullPath)
{
    FileFields[name] = fullPath;
    return (*this);
}


HTTPRequest& HTTPRequest::SetRecvSpeed(int64 bytesPerSecond)
{
    CURLcode resultCode = curl_easy_setopt(EasyHandle, CURLOPT_MAX_RECV_SPEED_LARGE, bytesPerSecond);
    if (resultCode != CURLE_OK)
    {
        FKLog("HTTPRequest::SetRecvSpeed Failed %d", resultCode);
    }
    return (*this);
}

HTTPRequest& HTTPRequest::SetSendSpeed(int64 bytesPerSecond)
{
    CURLcode resultCode = curl_easy_setopt(EasyHandle, CURLOPT_MAX_SEND_SPEED_LARGE, bytesPerSecond);
    if (resultCode != CURLE_OK)
    {
        FKLog("HTTPRequest::SetSendSpeed Failed %d", resultCode);
    }
    return (*this);
}


std::string HTTPRequest::GetURL() const
{
    return Url;
}

std::string HTTPRequest::GetURLParameter(const std::string& parameterName)
{
    std::string result;
    auto urlParams = StringUtils::Split(Url, '&');
    for (auto& paramItem : urlParams)
    {
        auto paramKeyValue = StringUtils::Split(paramItem, '=');
        if (paramKeyValue[0] == parameterName)
        {
            int outSize = 0;
            char*unescapedValue = curl_easy_unescape(EasyHandle, paramKeyValue[1].c_str(), static_cast<int>(paramKeyValue[1].size()), &outSize);
            result = unescapedValue;
            curl_free(unescapedValue);
        }
    }
    return result;
}


std::string HTTPRequest::GetHeader(const std::string& headerName)
{
    std::string result;
    auto headerIter = Headers.find(headerName);
    if (headerIter != Headers.end())
    {
        result = headerIter->second;
    }
    return result;
}

std::string HTTPRequest::GetPostFieldsAsString()
{
    std::stringbuf buf;
    bool needAnd = false;
    for (const auto &postField : PostFields)
    {
        if (needAnd)
            buf.sputc('&');
        char *part = curl_easy_escape(EasyHandle, postField.first.c_str(), 0);
        buf.sputn(part, strlen(part));
        buf.sputc('=');
        curl_free(part);
        part = curl_easy_escape(EasyHandle, postField.second.c_str(), 0);
        buf.sputn(part, strlen(part));
        curl_free(part);
        needAnd = true;
    }
    return buf.str();
}

std::vector<std::string> HTTPRequest::GetAllHeaders()
{
    std::vector<std::string> result;
    for (auto headerIter : Headers)
    {
        result.push_back(headerIter.first + ":" + headerIter.second);
    }
    return result;
}

HTTPRequest::KeyValueMap& HTTPRequest::GetContentFields()
{
    return ContentFields;
}

HTTPRequest::KeyValueMap& HTTPRequest::GetFileFields()
{
    return FileFields;
}

CURL* HTTPRequest::GetEasyHandle()
{
    return EasyHandle;
}

RequestStatus HTTPRequest::GetRequestStatus()
{
    return Status;
}

HTTPRequest::ResponsePtr HTTPRequest::GetResponse()
{
    return ResponseInstance;
}

int HTTPRequest::GetTimeOut()
{
    return TimeOutValue;
}

void HTTPRequest::Cancel()
{
    bCanceled = true;
}

bool HTTPRequest::IsFinished()
{
    return bFinished;
}

void HTTPRequest::SetCompleted(CURLcode completionResult)
{
    bCompleted = true;
    CompletionResult = completionResult;
}

void HTTPRequest::SetAddToMultiResult(CURLMcode result)
{
    AddToMultiResult = result;
}

void HTTPRequest::SetRequestStatus(RequestStatus status)
{
    Status = status;
}

bool HTTPRequest::Build()
{
    ASSERTED(EasyHandle, "");

    // Prevent overlapped requests using the same instance
    if (Status == RequestStatus::Processing)
    {
        FKLog("ProcessRequest failed. Still processing last request.");
        return false;
    }
    // Nothing to do without a valid URL
    else if (Url.empty())
    {
        FKLog("Cannot process HTTP request: URL is empty");
        return false;
    }

    bCompleted = false;
    bCanceled = false;
    bFinished = false;
    AddToMultiResult = CURLM_OK;
    curl_easy_setopt(EasyHandle, CURLOPT_URL, Url.c_str());
    curl_easy_setopt(EasyHandle, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
    curl_easy_setopt(EasyHandle, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);
    //curl_easy_setopt(EasyHandle, CURLOPT_CONNECTTIMEOUT, TimeOutValue);
    //curl_easy_setopt(EasyHandle, CURLOPT_TIMEOUT, TimeOutValue);
    // set up method
    if (MethodType == RequestMethodType::POST)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_POST, 1L);
        std::string requestPayload = GetPostFieldsAsString();
        if (!requestPayload.empty())
        {
            curl_easy_setopt(EasyHandle, CURLOPT_POSTFIELDS, requestPayload.c_str());
            curl_easy_setopt(EasyHandle, CURLOPT_POSTFIELDSIZE, requestPayload.size());
        }

    }
    else if (MethodType == RequestMethodType::PUT)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_UPLOAD, 1L);
        // this pointer will be passed to read function
        curl_easy_setopt(EasyHandle, CURLOPT_READDATA, this);
        curl_easy_setopt(EasyHandle, CURLOPT_READFUNCTION, StaticUploadCallback);
    }
    else if (MethodType == RequestMethodType::GET)
    {
        // technically might not be needed unless we reuse the handles
        curl_easy_setopt(EasyHandle, CURLOPT_HTTPGET, 1L);
    }
    else if (MethodType == RequestMethodType::HEAD)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_NOBODY, 1L);
    }
    else if (MethodType == RequestMethodType::DELETE)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    else
    {
        FKLog("Unsupported method '%d', can be perhaps added with CURLOPT_CUSTOMREQUEST", static_cast<int>(MethodType));
    }
    // set up headers
    if (GetHeader("User-Agent").empty())
    {
        SetHeader("User-Agent", StringUtils::Format("author=%s, client=%s, version=%s", "libo", "HTTPClient", HTTPClientVersionString));
    }
    // Add "Pragma: no-cache" to mimic WinInet behavior
    if (GetHeader("Pragma").empty())
    {
        SetHeader("Pragma", "no-cache");
    }
    // Remove "Expect: 100-continue" since this is supposed to cause problematic behavior on Amazon ELB (and WinInet doesn't send that either)
    // (also see http://www.iandennismiller.com/posts/curl-http1-1-100-continue-and-multipartform-data-post.html , http://the-stickman.com/web-development/php-and-curl-disabling-100-continue-header/ )
    if (GetHeader("Expect").empty())
    {
        SetHeader("Expect", "");
    }
    curl_slist_free_all(HeaderList);
    HeaderList = nullptr;
    auto allHeaders = GetAllHeaders();
    for (auto& header : allHeaders)
    {
        HeaderList = curl_slist_append(HeaderList, header.c_str());
    }

    if (HeaderList)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_HTTPHEADER, HeaderList);
    }
    // ============== build form data =====================
    if (!FileFields.empty() || !ContentFields.empty())
    {
        SetHeader("Content-Type", "multipart/form-data;charset=utf-8");
    }
    //set form files
    if (FormPost)
    {
        curl_formfree(FormPost);
    }
    FormPost = nullptr;
    curl_httppost *lastPost = nullptr;
    for (const auto &fileItem : FileFields)
    {
        curl_formadd(&FormPost, &lastPost,
            CURLFORM_COPYNAME, fileItem.first.c_str(),
            CURLFORM_FILE, fileItem.second.c_str(),
            CURLFORM_CONTENTTYPE, MimeTypes::GetMimeType(fileItem.second).c_str(),
            CURLFORM_END);
    }

    //set form contents
    for (const auto &contentItem : ContentFields)
    {
        curl_formadd(&FormPost, &lastPost,
            CURLFORM_COPYNAME, contentItem.first.c_str(),
            CURLFORM_COPYCONTENTS, contentItem.second.c_str(),
            CURLFORM_END);
    }
    if (FormPost)
    {
        curl_easy_setopt(EasyHandle, CURLOPT_HTTPPOST, FormPost);
    }

    Status = RequestStatus::Processing;
    ResponseInstance = HTTPResponse::Create(shared_from_this());
    return true;
}

void HTTPRequest::OnTick(float deltaTime)
{
    ElapsedTime += deltaTime;
    if (ElapsedTime >= TimeOutValue)
    {
        bTimeout = true;
    }

    if (bCompleted || bCanceled || bTimeout)
    {
        FinishedRequest();
    }
}

bool HTTPRequest::OnFinished()
{
    // Derived can change some data of the HTTPResponse.
    return false;
}

void HTTPRequest::FinishedRequest()
{
    std::string operatingCurlMsg;
    // if completed, get more info
    if (bCompleted)
    {
        long httpCode = 0;
        curl_easy_getinfo(EasyHandle, CURLINFO_RESPONSE_CODE, &httpCode);
        operatingCurlMsg = curl_easy_strerror(CompletionResult);
        operatingCurlMsg += ":";
        double contentLengthDownload = 0.0;
        curl_easy_getinfo(EasyHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLengthDownload);
        char* effective_url;
        curl_easy_getinfo(EasyHandle, CURLINFO_EFFECTIVE_URL, &effective_url);
        curl_slist *curlCookies = NULL;
        curl_easy_getinfo(EasyHandle, CURLINFO_COOKIELIST, &curlCookies);
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
        ResponseInstance->SetSucceeded(CURLE_OK == CompletionResult)
            .SetResponseCode(httpCode)
            .SetContentSize(static_cast<int32>(contentLengthDownload))
            .SetCookies(responseCookies)
            .SetEffectiveURL(effective_url ? effective_url : "");

        // Mark last request attempt as completed but failed
        switch (CompletionResult)
        {
        case CURLE_OK:
            break;
        case CURLE_COULDNT_CONNECT:
        case CURLE_COULDNT_RESOLVE_PROXY:
        case CURLE_COULDNT_RESOLVE_HOST:
            // report these as connection errors (safe to retry)
            SetRequestStatus(RequestStatus::Failed_ConnectionError);
            break;
        default:
            SetRequestStatus(RequestStatus::Failed);
        }
    }
    else if (bCanceled)
    {
        SetRequestStatus(RequestStatus::Canceled);
        std::string errMsg = "The request has been canceled";
        operatingCurlMsg += "The request has been canceled";;
    }
    else if (bTimeout)
    {
        SetRequestStatus(RequestStatus::TimeOut);
        operatingCurlMsg += "The request time-out,";
    }
    else
    {
        ASSERTED(false, "Operation not supported,");
    }
    
    if (ResponseInstance->IsSucceeded())
    {
        // Mark last request attempt as completed successfully
        SetRequestStatus(RequestStatus::Succeeded);
    }
    operatingCurlMsg += ErrorBuffer;
    ResponseInstance->SetReady(true).SetErrorMsg(operatingCurlMsg);
    bFinished = true;

    // Call delegate with valid request/response objects
    if (!OnFinished() && OnRequestCompleted)
    {
        OnRequestCompleted(shared_from_this(), ResponseInstance);
    }

    const bool bDebugServerResponse = ResponseInstance->GetResponseCode() >= 500 && ResponseInstance->GetResponseCode() <= 505;
    // log info about error responses to identify failed downloads
    if (bDebugServerResponse)
    {
        FKLog("%p: request has been successfully processed. URL: %s, HTTP code: %d, content length: %d, actual payload size: %d",
            (void*)this,
            GetURL().c_str(),
            ResponseInstance->GetResponseCode(),
            ResponseInstance->GetContentSize(),
            ResponseInstance->GetContentData().size());
    }
}

size_t HTTPRequest::StaticUploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(buffer, "HTTPRequest::staticUploadCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticUploadCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    request->ElapsedTime = 0.0f;
    return request->UploadCallback(buffer, sizeInBlocks, blockSizeInBytes);
}

size_t HTTPRequest::StaticReceiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(buffer, "HTTPRequest::staticReceiveResponseHeaderCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticReceiveResponseHeaderCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    request->ElapsedTime = 0.0f;
    return request->ReceiveResponseHeaderCallback(buffer, sizeInBlocks, blockSizeInBytes);
}

size_t HTTPRequest::StaticReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    ASSERTED(buffer, "HTTPRequest::staticReceiveResponseBodyCallback param [ptr] is null.");
    ASSERTED(userData, "HTTPRequest::staticReceiveResponseBodyCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    request->ElapsedTime = 0.0f;
    return request->ReceiveResponseBodyCallback(buffer, sizeInBlocks, blockSizeInBytes);
}

int HTTPRequest::StaticProgressCallback(void *userData, curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
{
    ASSERTED(userData, "HTTPRequest::staticProgressCallback param [userData] is null.");
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    request->ElapsedTime = 0.0f;
    return request->ProgressCallback(totalDownload, nowDownload, totalUpload, nowUpload);
}

size_t HTTPRequest::StaticDebugCallback(CURL* handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData)
{
    ASSERTED(handle, "HTTPRequest::staticDebugCallback param [handle] is null.");
    ASSERTED(userData, "HTTPRequest::staticDebugCallback param [userData] is null.");
    // dispatch
    HTTPRequest* request = reinterpret_cast<HTTPRequest*>(userData);
    return request->DebugCallback(handle, debugInfoType, debugInfo, debugInfoSize);
}

size_t HTTPRequest::UploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    return 0;
}


size_t HTTPRequest::ReceiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    ASSERTED(ResponseInstance, "");
    if (ResponseInstance)
    {
        size_t headerSize = sizeInBlocks * blockSizeInBytes;
        if (headerSize > 0 && headerSize <= CURL_MAX_HTTP_HEADER)
        {
            std::string headers(reinterpret_cast<char*>(buffer));
            // kill \n\r
            std::replace(headers.begin(), headers.end(), '\n', ' ');
            std::replace(headers.begin(), headers.end(), '\r', ' ');
            if (EnableDebug)
            {
                FKLog("%p: Received response header '%s'.", (void*)this, headers.c_str());
            }
            auto headerKeyValue = StringUtils::Split(headers, ':');

            if (headerKeyValue.size() == 2)
            {
                std::string headerKey = headerKeyValue[0];
                std::string headerValue = headerKeyValue[1];
                if (!headerKey.empty() && !headerValue.empty())
                {
                    std::string previousValue = ResponseInstance->GetHeader(headerKey);
                    std::string newValue;
                    if (!previousValue.empty())
                    {
                        newValue = previousValue + ", ";
                    }
                    newValue += StringUtils::Trim(headerValue);
                    ResponseInstance->SetHeader(headerKey, newValue);

                    //Store the content length so OnRequestProgress() delegates have something to work with
                    if (headerKey == ("Content-Length"))
                    {
                        ResponseInstance->SetContentSize(std::atoi(StringUtils::Trim(headerValue).c_str()));
                    }
                }
            }
            return headerSize;
        }
        else
        {
            FKLog("%p: Could not process response header for request - header size (%d) is invalid.", (void*)this, headerSize);
        }
    }
    else
    {
        FKLog("%p: Could not download response header for request - response not valid.", (void*)this);
    }
    return 0;
}

size_t HTTPRequest::ReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)
{
    ASSERTED(ResponseInstance, "");
    if (ResponseInstance)
    {
        size_t sizeToDownload = sizeInBlocks * blockSizeInBytes;
        if (EnableDebug)
        {
            FKLog("%p: ReceiveResponseBodyCallback: %d bytes out of %d received. (SizeInBlocks=%d, BlockSizeInBytes=%d, Response->TotalBytesRead=%d, Response->GetContentLength()=%d, SizeToDownload=%d (<-this will get returned from the callback))",
                (void*)this,
                static_cast<int32>(ResponseInstance->GetContentData().size() + sizeToDownload),
                ResponseInstance->GetContentSize(),
                static_cast<int32>(sizeInBlocks), 
                static_cast<int32>(blockSizeInBytes),
                ResponseInstance->GetContentData().size(),
                ResponseInstance->GetContentSize(),
                static_cast<int32>(sizeToDownload)
                );
        }

        // note that we can be passed 0 bytes if file transmitted has 0 length
        if (sizeToDownload > 0)
        {
            auto& responseData = ResponseInstance->GetContentData();
            responseData.insert(responseData.end(), (char*)buffer, (char*)buffer + sizeToDownload);
            return sizeToDownload;
        }
    }
    else
    {
        FKLog("%p: Could not download response data for request - response not valid.", (void*)this);
    }
    return 0;	// request will fail with write error if we had non-zero bytes to download
}

int HTTPRequest::ProgressCallback(curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
{
    if ((MethodType == RequestMethodType::POST || MethodType == RequestMethodType::PUT) && OnRequestProgress)
    {
        OnRequestProgress(shared_from_this(), nowUpload, totalUpload);
    }
    if (MethodType == RequestMethodType::GET && OnRequestProgress)
    {
        OnRequestProgress(shared_from_this(), nowDownload, totalDownload);
    }
    return CURLE_OK;
}

size_t HTTPRequest::DebugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize)
{
    ASSERTED((handle == EasyHandle), "HTTPRequest::debugCallback The handle is not us's");	// make sure it's us
    switch (debugInfoType)
    {
    case CURLINFO_DATA_IN:
        FKLog("DATA_IN %p: Received data [%s] (%d bytes)", (void*)this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_DATA_OUT:
        FKLog("DATA_OUT %p: Sent data [%s] (%d bytes)", (void*)this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_HEADER_IN:
        FKLog("HEADER_IN %p: Received header [%s] (%d bytes)", (void*)this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_HEADER_OUT:
        FKLog("HEADER_OUT %p: Sent header [%s] (%d bytes)", (void*)this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_SSL_DATA_IN:
        FKLog("SSL_DATA_IN %p: Received data [%s] (%d bytes)", (void*)this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_SSL_DATA_OUT:
        FKLog("SSL_DATA_OUT %p: Sent data [%s] (%d bytes)", (void*)this, debugInfo, debugInfoSize);
        break;
    case CURLINFO_TEXT:
    {
        std::string debugText = debugInfo;
        std::replace(debugText.begin(), debugText.end(), '\n', ' ');
        std::replace(debugText.begin(), debugText.end(), '\r', ' ');
        FKLog("TEXT %p:%s", (void*)this, debugText.c_str());
    }
    break;
    default:
        FKLog("%p: DebugCallback: Unknown DebugInfoType=%d debugInfo [%s](DebugInfoSize: %d bytes)", (void*)this, (int32)debugInfoType, debugInfo, debugInfoSize);
        break;
    }
    return 0;
}

NS_FK_END





