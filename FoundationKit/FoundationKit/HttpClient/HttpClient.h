/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <list>
#include <condition_variable>
#include "curl.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

NS_FK_BEGIN

class HttpClient
{
public:
    static const int RESPONSE_BUFFER_SIZE = 256;
    typedef std::list<HttpRequest::Pointer>          RequestQueue;
    typedef std::list<HttpResponse::Pointer>         ResponseQueue;
    typedef std::list<std::shared_ptr<std::thread> > ThreadList;

    static HttpClient* getInstance();


    HttpClient();
    ~HttpClient();

    HttpResponse::Pointer sendRequest(HttpRequest::Pointer requestPtr);
    void sendRequestAsync(HttpRequest::Pointer requestPtr);

    /**
    * Set the timeout value for connecting.
    *
    * @param value the timeout value for connecting.
    */
    void setTimeoutForConnect(int value);

    /**
    * Get the timeout value for connecting.
    *
    * @return int the timeout value for connecting.
    */
    int getTimeoutForConnect();

    /**
    * Set the timeout value for reading.
    *
    * @param value the timeout value for reading.
    */
    void setTimeoutForRead(int value);

    /**
    * Get the timeout value for reading.
    *
    * @return int the timeout value for reading.
    */
    int getTimeoutForRead();

    void setDebugMode(bool debugMode);

    void update(float deltaTime);

protected:

    bool lazyInitThread();
    void networkThread();
    HttpRequest::Pointer getRequest();
    HttpResponse::Pointer processRequest(HttpRequest::Pointer requestPtr);

private:
    std::atomic_int         _timeoutForConnect;
    std::atomic_int         _timeoutForRead; 
    std::atomic_bool        _running;
    std::atomic_bool        _isDebug;

    RequestQueue            _requestQueue;
    std::mutex              _requestQueueMutex;
    std::condition_variable _requestQueueNotEmpty;

    ResponseQueue           _responseQueue;
    std::mutex              _responseQueueMutex;

    ThreadList              _threadList;

    static std::once_flag   _callOnceFlag;
};

NS_FK_END

