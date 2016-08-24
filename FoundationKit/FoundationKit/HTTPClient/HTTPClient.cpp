/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/

#include "HttpClient.h"
#include "MimeTypes.h"
#include <sstream>
#include <iostream>
#include "FoundationKit/Foundation/Logger.h"

NS_FK_BEGIN

// ==========================================================================
// ==                            CURL CALLBACK                             ==
// ==========================================================================
// Callback function used by libcurl for collect response data
static size_t onWriteData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::vector<char> *recvBuffer = (std::vector<char>*)stream;
    size_t sizes = size * nmemb;
    // add data to the end of recvBuffer
    // write data maybe called more than once in a single request
    recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr + sizes);

    return sizes;
}

// Callback function used by libcurl for collect header data
static size_t onWriteHeaderData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::vector<char> *recvBuffer = (std::vector<char>*)stream;
    size_t sizes = size * nmemb;
    // add data to the end of recvBuffer
    // write data maybe called more than once in a single request
    recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr + sizes);

    return sizes;
}


// @param[1]   HttpRequest pointer.
// @param[2]   Total download size.
// @param[3]   Now   download size.
// @param[4]   Total upload size.
// @param[5]   Now   upload size.
static int onProgress(void *userdata, double dltotal, double dlnow, double ultotal, double ulnow)
{
    HTTPRequest* pHttpRequest = reinterpret_cast<HTTPRequest*>(userdata);
    if (pHttpRequest && pHttpRequest->onProgress)
    {
        pHttpRequest->onProgress(dltotal, dlnow, ultotal, ulnow);
    }
    return 0;
}

static int onDebug(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr)
{
    HTTPClient* pHttpClient = reinterpret_cast<HTTPClient*>(userptr);
    LOG_INFO(">>>>> HttpClient:%p CURL Handle:%p", pHttpClient, handle);
    switch (type)
    {
    case CURLINFO_DATA_IN:
        LOG_INFO(">>>>> DATA_IN:%s SIZE:%d", data, size);
        break;
    case CURLINFO_DATA_OUT:
        LOG_INFO(">>>>> DATA_OUT:%s SIZE:%d", data, size);
        break;
    case CURLINFO_HEADER_IN:
        LOG_INFO(">>>>> HEADER_IN:%s SIZE:%d", data, size);
        break;
    case CURLINFO_HEADER_OUT:
        LOG_INFO(">>>>> HEADER_OUT:%s SIZE:%d", data, size);
        break;
    case CURLINFO_SSL_DATA_IN:
        LOG_INFO(">>>>> SSL_DATA_IN:%s SIZE:%d", data, size);
        break;
    case CURLINFO_SSL_DATA_OUT:
        LOG_INFO(">>>>> SSL_DATA_OUT:%s SIZE:%d", data, size);
        break;
    case CURLINFO_TEXT:
        LOG_INFO(">>>>> TEXT:%s SIZE:%d", data, size);
        break;
    default:
        break;
    }
    return 0;
}
// ==========================================================================
// ==                          CURL CALLBACK END                           ==
// ==========================================================================

HTTPClient::HTTPClient()
{
    _timeoutForConnect = 30;
    _timeoutForRead = 30;
    _running = false;
    _isDebug = false;
    _bCallbackInUpdateThread = false;
    curl_global_init(CURL_GLOBAL_ALL);
}

HTTPClient::~HTTPClient()
{
    _running = false;
    _requestQueueNotEmpty.notify_all();
    for (auto& threadPtr : _threadList)
    {
        threadPtr->join();
    }
    curl_global_cleanup();
}

std::once_flag HTTPClient::_callOnceFlag;
static HTTPClient* shared_HttpClientInstance = nullptr;
HTTPClient* HTTPClient::getInstance()
{
    std::call_once(_callOnceFlag, []()
    {
        static HTTPClient  httpClient;
        httpClient.lazyInitThread();
        shared_HttpClientInstance = &httpClient;
    });
    return shared_HttpClientInstance;
}

bool HTTPClient::lazyInitThread()
{
    _running = true;
    size_t numThreads = std::thread::hardware_concurrency();
    for (size_t i = 0; i < numThreads; ++i)
    {
        _threadList.push_back(std::make_shared<std::thread>(&HTTPClient::networkThread, this));
    }
    return true;
}

void HTTPClient::networkThread()
{
    while (_running)
    {
        HTTPRequest::Pointer requestPtr = getRequest();
        if (requestPtr != nullptr)
        {
            HTTPResponse::Pointer response = processRequest(requestPtr);
            if (_bCallbackInUpdateThread && requestPtr->onRequestFinished)
            {
                requestPtr->onRequestFinished(response);
            }
            else
            {
                _responseQueueMutex.lock();
                _responseQueue.push_back(response);
                _responseQueueMutex.unlock();
            }

            if (_isDebug)
            {
                std::vector<char> vdata = *response->getResponseHeader();
                std::string strHeader(&vdata[0], vdata.size());
                int errorCode = response->getResponseCode();
                LOG_INFO("=== CODE:%d, HEADER:%s", errorCode, strHeader.c_str());
                vdata = *response->getResponseData();
                std::string strData(&vdata[0], vdata.size());
                LOG_INFO("=== Msg:%s, Data:%s", response->getResponseMessage(), strData.c_str());
            }
        }
    }
}

void HTTPClient::setTimeoutForConnect(int value)
{
    _timeoutForConnect = value;
}

int HTTPClient::getTimeoutForConnect()
{
    return _timeoutForConnect;
}

void HTTPClient::setTimeoutForRead(int value)
{
    _timeoutForRead = value;
}

int HTTPClient::getTimeoutForRead()
{
    return _timeoutForRead;
}

void HTTPClient::setDebugMode(bool debugMode)
{
    _isDebug = debugMode;
}

void HTTPClient::update(float deltaTime)
{
    UNUSED_ARG(deltaTime);

    if (!_responseQueue.empty())
    {
        _responseQueueMutex.lock();
        auto temp = _responseQueue;
        _responseQueue.clear();
        _responseQueueMutex.unlock();
        for (const auto &responsePtr : temp)
        {
            auto fun = responsePtr->getHTTPRequest()->onRequestFinished;
            if (fun)
                fun(responsePtr);
        }
    }
}

HTTPResponse::Pointer HTTPClient::sendRequest(HTTPRequest::Pointer requestPtr)
{
    return processRequest(requestPtr);
}

void HTTPClient::sendRequestAsync(HTTPRequest::Pointer requestPtr, bool callbackInUpdateThread/* = false*/)
{
    std::lock_guard<std::mutex> lock(_requestQueueMutex);
    _requestQueue.push_back(requestPtr);
    _requestQueueNotEmpty.notify_one();
    _bCallbackInUpdateThread = callbackInUpdateThread;
}

HTTPRequest::Pointer HTTPClient::getRequest()
{
    std::unique_lock<std::mutex> lock(_requestQueueMutex);
    _requestQueueNotEmpty.wait(lock, [&]{return !_running || !_requestQueue.empty(); });
    HTTPRequest::Pointer requestPtr(nullptr);
    if (!_requestQueue.empty())
    {
        requestPtr = _requestQueue.front();
        _requestQueue.pop_front();
    }
    return requestPtr;
}

HTTPResponse::Pointer HTTPClient::processRequest(HTTPRequest::Pointer requestPtr)
{
    CURL *_curl = curl_easy_init();
    HTTPResponse::Pointer response = HTTPResponse::create(requestPtr);
    // 设置写入数据的函数
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, onWriteData);
    // 设置接收数据的对象
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, response->getResponseData());
    // 设置写入response header的函数
    curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, onWriteHeaderData);
    // 设置接收header数据的对象
    curl_easy_setopt(_curl, CURLOPT_WRITEHEADER, response->getResponseHeader());
    // 允许上报进度
    curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0L);
    // 设置上报进度的函数
    curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, onProgress);
    // 上报进度函数中UserData
    curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, requestPtr.get());
    // 设置请求的URL
    curl_easy_setopt(_curl, CURLOPT_URL, requestPtr->getRequestUrl().c_str());
    curl_easy_setopt(_curl, CURLOPT_USERAGENT, "libcurl");
    // 设置连接超时时间(单位:秒)
    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, this->getTimeoutForConnect());
    // 设置读取超时时间(单位:秒)
    curl_easy_setopt(_curl, CURLOPT_TIMEOUT, this->getTimeoutForRead());
    curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
    // 允许连接编码
    curl_easy_setopt(_curl, CURLOPT_HTTP_CONTENT_DECODING, 1L);
    // 保存错误信息
    char errorBuffer[RESPONSE_BUFFER_SIZE] = { 0 };
    curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorBuffer);

    // set accept encoding type
    curl_easy_setopt(_curl, CURLOPT_ACCEPT_ENCODING, requestPtr->getEncodingAsString(requestPtr->getAcceptEncoding()).c_str());

    // set cookies
    std::string strCookie = requestPtr->getRequestCookies();
    curl_easy_setopt(_curl, CURLOPT_COOKIE, strCookie.c_str());
    curl_easy_setopt(_curl, CURLOPT_COOKIEFILE, "");
    if (_isDebug)
    {
        curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_DEBUGFUNCTION, onDebug);
        curl_easy_setopt(_curl, CURLOPT_DEBUGDATA, this);
    }

    // GET/POST/PUT/DELETE OPT
    switch (requestPtr->getRequestType())
    {
    case HTTPRequest::Type::GET:
        curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
        break;
    case HTTPRequest::Type::POST:
        curl_easy_setopt(_curl, CURLOPT_POST, 1L);
        if (requestPtr->getRequestDataSize() > 0)
        {
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, requestPtr->getRequestData());
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, requestPtr->getRequestDataSize());
        }
        break;
    case HTTPRequest::Type::PUT:
        curl_easy_setopt(_curl, CURLOPT_PUT, 1L);
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
        break;
    case HTTPRequest::Type::DELETE:
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    default:
        break;
    }

    HTTPRequest::Fields& fileFields = requestPtr->getFiles();
    HTTPRequest::Fields& contentFields = requestPtr->getContents();

    // set headers
    curl_slist *curlHeaders = nullptr;

    if (!fileFields.empty() || !contentFields.empty())
    {
        std::string contentType = "Content-Type:multipart/form-data";
        curlHeaders = curl_slist_append(curlHeaders, contentType.c_str());
        curlHeaders = curl_slist_append(curlHeaders, "Expect:");
    }

    HTTPRequest::Headers& requestHeaders = requestPtr->getHeaders();
    for (const auto &header : requestHeaders)
    {
        curlHeaders = curl_slist_append(curlHeaders, header.c_str());
    }
    // set content encoding type
    std::string contentEncoding = "Content-Encoding: ";
    contentEncoding += requestPtr->getEncodingAsString(requestPtr->getContentEncoding());
    curlHeaders = curl_slist_append(curlHeaders, contentEncoding.c_str());

    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, curlHeaders);

    //set form files
    curl_httppost      *formPost = nullptr;
    curl_httppost      *lastPost = nullptr;
    for (const auto &fileItem : fileFields)
    {
        curl_formadd(&formPost, &lastPost,
            CURLFORM_COPYNAME, fileItem.first.c_str(),
            CURLFORM_FILE, fileItem.second.c_str(),
            CURLFORM_CONTENTTYPE, MimeTypes::getMimeType(fileItem.second).c_str(),
            CURLFORM_END);
    }

    //set form contents
    for (const auto &contentItem : contentFields)
    {
        curl_formadd(&formPost, &lastPost,
            CURLFORM_COPYNAME, contentItem.first.c_str(),
            CURLFORM_COPYCONTENTS, contentItem.second.c_str(),
            CURLFORM_END);
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPPOST, formPost);

    // set post fields
    HTTPRequest::Fields& postFields = requestPtr->getPostFields();
    if (postFields.size() > 0)
    {
        std::stringbuf buf;
        bool needAnd = false;
        for (const auto &postField : postFields)
        {
            if (needAnd)
                buf.sputc('&');
            char *part = curl_easy_escape(_curl, postField.first.c_str(), 0);
            buf.sputn(part, strlen(part));
            buf.sputc('=');
            curl_free(part);
            part = curl_easy_escape(_curl, postField.second.c_str(), 0);
            buf.sputn(part, strlen(part));
            curl_free(part);
            needAnd = true;
        }
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, buf.str().c_str());
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, buf.str().size());
    }

    // Commit request.
    CURLcode retCode;
    retCode = curl_easy_perform(_curl);
    long responseCode;
    curl_slist *curlCookies = NULL;
    // get response data
    curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &responseCode);
    curl_easy_getinfo(_curl, CURLINFO_COOKIELIST, &curlCookies);
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
    response->setResponseCode(responseCode);
    response->setResponseCookies(responseCookies.c_str());
    response->setErrorBuffer(errorBuffer);
    response->setResponseMessage(curl_easy_strerror(retCode));

    /* always cleanup */
    curl_easy_cleanup(_curl);
    _curl = nullptr;
    if (formPost)
    {
        /* then cleanup the formpost chain */
        curl_formfree(formPost);
        formPost = nullptr;
    }
    /* free slist */
    curl_slist_free_all(curlHeaders);
    response->setSucceed(retCode == CURLE_OK);
    return response;
}




NS_FK_END


