/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/

#include "HttpClient.h"
#include "HttpResponse.h"
#include "MimeTypes.h"
#include <sstream>
#include <iostream>
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/Scheduler.h"

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
    HttpRequest* pHttpRequest = reinterpret_cast<HttpRequest*>(userdata);
    if (pHttpRequest && pHttpRequest->onProgress)
    {
        pHttpRequest->onProgress(dltotal, dlnow, ultotal, ulnow);
    }
    return 0;
}

static int onDebug(CURL *handle,curl_infotype type, char *data, size_t size, void *userptr)
{
    HttpClient* pHttpClient = reinterpret_cast<HttpClient*>(userptr);
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

HttpClient::HttpClient()
{
    _timeoutForConnect = 30;
    _timeoutForRead = 30;
    _isDebug = false;
    curl_global_init(CURL_GLOBAL_ALL);
}

HttpClient::~HttpClient()
{
    _running = false;
    _requestQueueNotEmpty.notify_all();
    for (auto& threadPtr : _threadList)
    {
        threadPtr->join();
    }
    curl_global_cleanup();
}

std::once_flag HttpClient::_callOnceFlag;
static HttpClient* shared_HttpClientInstance = nullptr;
HttpClient* HttpClient::getInstance()
{
    std::call_once(_callOnceFlag, []()
    {
        static HttpClient  httpClient;
        httpClient.lazyInitThread();
        shared_HttpClientInstance = &httpClient;
    });
    return shared_HttpClientInstance;
}

bool HttpClient::lazyInitThread()
{
    _running = true;
    size_t numThreads = std::thread::hardware_concurrency();
    for (size_t i = 0; i < numThreads; ++i)
    {
        _threadList.push_back(std::make_shared<std::thread>(&HttpClient::networkThread, this));
    }
    return true;
}

void HttpClient::networkThread()
{
    while (_running)
    {
        HttpRequest::Pointer requestPtr = getRequest();
        if (requestPtr != nullptr)
        {
            HttpResponse::Pointer response = processRequest(requestPtr);
            _responseQueueMutex.lock();
            _responseQueue.push_back(response);
            _responseQueueMutex.unlock();

            Scheduler::getInstance()->performFunctionInMainThread([&]
            {
                if (!_responseQueue.empty())
                {
                    _responseQueueMutex.lock();
                    auto temp = _responseQueue;
                    _responseQueue.clear();
                    _responseQueueMutex.unlock();
                    for (const auto &responsePtr : temp)
                    {
                        auto fun = responsePtr->getHttpRequest()->onRequestFinished;
                        if (fun)
                            fun(responsePtr);
                    }
                }
            });
        }
        else
        {
            break;
        }
    }
}

HttpRequest::Pointer HttpClient::getRequest()
{
    std::unique_lock<std::mutex> lock(_requestQueueMutex);
    _requestQueueNotEmpty.wait(lock, [&]{return !_running || !_requestQueue.empty(); });
    HttpRequest::Pointer requestPtr(nullptr);
    if (!_requestQueue.empty())
    {
        requestPtr = _requestQueue.front();
        _requestQueue.pop_front();
    }
    return requestPtr;
}



void HttpClient::setTimeoutForConnect(int value)
{
    _timeoutForConnect = value;
}

int HttpClient::getTimeoutForConnect()
{
    return _timeoutForConnect;
}

void HttpClient::setTimeoutForRead(int value)
{
    _timeoutForRead = value;
}

int HttpClient::getTimeoutForRead()
{
    return _timeoutForRead;
}

HttpResponse::Pointer HttpClient::sendRequest(HttpRequest::Pointer requestPtr)
{
    return processRequest(requestPtr);
}

void HttpClient::sendRequestAsync(HttpRequest::Pointer requestPtr)
{
    std::lock_guard<std::mutex> lock(_requestQueueMutex);
    _requestQueue.push_back(requestPtr);
    _requestQueueNotEmpty.notify_one();
}

HttpResponse::Pointer HttpClient::processRequest(HttpRequest::Pointer requestPtr)
{
    CURL *_curl = curl_easy_init();
    HttpResponse::Pointer response = HttpResponse::create(requestPtr);
    //设置写入数据的函数
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, onWriteData);
    //设置接收数据的对象
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, response->getResponseData());
    //设置写入response header的函数
    curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, onWriteHeaderData);
    //设置接收header数据的对象
    curl_easy_setopt(_curl, CURLOPT_WRITEHEADER, response->getResponseHeader());
    //允许上报进度
    curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0L);
    //设置上报进度的函数
    curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, onProgress);
    //上报进度函数中UserData
    curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, requestPtr.get());
    //设置请求的URL
    curl_easy_setopt(_curl, CURLOPT_URL, requestPtr->getRequestUrl().c_str());
    curl_easy_setopt(_curl, CURLOPT_USERAGENT, "libcurl");
    //设置连接超时时间(单位:秒)
    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, this->getTimeoutForConnect());
    //设置读取超时时间(单位:秒)
    curl_easy_setopt(_curl, CURLOPT_TIMEOUT, this->getTimeoutForRead());
    curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(_curl, CURLOPT_COOKIEFILE, "");
    //允许连接编码
    curl_easy_setopt(_curl, CURLOPT_HTTP_CONTENT_DECODING, 1L);
    char errorBuffer[RESPONSE_BUFFER_SIZE] = { 0 };
    curl_easy_setopt(_curl, CURLOPT_ERRORBUFFER, errorBuffer);

    // set cookies
    std::string strCookie = requestPtr->getRequestCookies();
    curl_easy_setopt(_curl, CURLOPT_COOKIE, strCookie.c_str());

    // set accept encoding type
    std::string strAcceptEncode = requestPtr->getAcceptEncodingString();
    curl_easy_setopt(_curl, CURLOPT_ACCEPT_ENCODING, strAcceptEncode.c_str());

    if (_isDebug)
    {
        curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_DEBUGFUNCTION, onDebug);
        curl_easy_setopt(_curl, CURLOPT_DEBUGDATA, this);
    }

    // GET/POST/PUT/DELETE OPT
    switch (requestPtr->getRequestType())
    {
    case HttpRequest::Type::GET:
        curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
        break;
    case HttpRequest::Type::POST:
        curl_easy_setopt(_curl, CURLOPT_POST, 1L);
        break;
    case HttpRequest::Type::PUT:
        curl_easy_setopt(_curl, CURLOPT_PUT, 1L);
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
        break;
    case HttpRequest::Type::DELETE:
        curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    default:
        break;
    }

    // set headers
    curl_slist *curlHeaders = nullptr;

    if (requestPtr->isMultipart())
    {
        std::string contentType = "Content-Type:multipart/form-data";
        curlHeaders = curl_slist_append(curlHeaders, contentType.c_str());
        curlHeaders = curl_slist_append(curlHeaders, "Expect:");
    }

    HttpRequest::Headers& requestHeaders = requestPtr->getHeaders();
    for (const auto &header : requestHeaders)
    {
        curlHeaders = curl_slist_append(curlHeaders, header.c_str());
    }
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, curlHeaders);

    //set form files
    curl_httppost      *formPost = nullptr;
    curl_httppost      *lastPost = nullptr;
    HttpRequest::Fields& fileFields = requestPtr->getFormFiles();
    for (const auto &fileItem : fileFields)
    {
        curl_formadd(&formPost, &lastPost,
            CURLFORM_COPYNAME, fileItem.first.c_str(),
            CURLFORM_FILE, fileItem.second.c_str(),
            CURLFORM_CONTENTTYPE, MimeTypes::getMimeType(fileItem.second).c_str(),
            CURLFORM_END);
    }

    //set form contents
    HttpRequest::Fields& contentFields = requestPtr->getFormContents();
    for (const auto &contentItem : contentFields)
    {
        curl_formadd(&formPost, &lastPost,
            CURLFORM_COPYNAME, contentItem.first.c_str(),
            CURLFORM_COPYCONTENTS, contentItem.second.c_str(),
            CURLFORM_END);
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPPOST, formPost);

    // set post fields
    HttpRequest::Fields& postFields = requestPtr->getPOSTValues();
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
    //curl_multi_cleanup(_multi_handle);
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


void HttpClient::setDebugMode(bool debugMode)
{
    _isDebug = debugMode;
}


NS_FK_END


