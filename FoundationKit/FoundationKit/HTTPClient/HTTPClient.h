/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_HTTPCLIENT_H
#define LOSEMYMIND_HTTPCLIENT_H

#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <list>
#include <condition_variable>
#include "curl.h"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

NS_FK_BEGIN

class HTTPClient
{
public:
    static const int RESPONSE_BUFFER_SIZE = 256;
    typedef std::list<HTTPRequest::Pointer>          RequestQueue;
    typedef std::list<HTTPResponse::Pointer>         ResponseQueue;
    typedef std::list<std::shared_ptr<std::thread> > ThreadList;

    static HTTPClient* getInstance();


    HTTPClient();
    ~HTTPClient();

    HTTPResponse::Pointer sendRequest(HTTPRequest::Pointer requestPtr);
    void sendRequestAsync(HTTPRequest::Pointer requestPtr, bool callbackInUpdateThread = false);

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
    HTTPRequest::Pointer getRequest();
    HTTPResponse::Pointer processRequest(HTTPRequest::Pointer requestPtr);

private:
    std::atomic_int         _timeoutForConnect;
    std::atomic_int         _timeoutForRead;
    std::atomic_bool        _running;
    std::atomic_bool        _isDebug;
    std::atomic_bool        _bCallbackInUpdateThread;

    RequestQueue            _requestQueue;
    std::mutex              _requestQueueMutex;
    std::condition_variable _requestQueueNotEmpty;

    ResponseQueue           _responseQueue;
    std::mutex              _responseQueueMutex;

    ThreadList              _threadList;

    static std::once_flag   _callOnceFlag;


};

NS_FK_END
#endif // LOSEMYMIND_HTTPCLIENT_H

