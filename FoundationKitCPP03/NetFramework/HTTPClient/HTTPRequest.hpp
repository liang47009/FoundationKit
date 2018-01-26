/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPREQUEST_HPP
#define NETFRAMEWORK_HTTPREQUEST_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include "curl.h"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Base/noncopyable.hpp"

NS_FK_BEGIN

enum RequestStatus
{
	/** Has not been started via ProcessRequest() */
	NotStarted,
	/** Finished and was successful */
	Succeeded,
	/** Currently being ticked and processed */
	Processing,
	/** The request has been canceled*/
	Canceled,
	/** The request time-out*/
	TimeOut,
	/** Finished but failed */
	Failed,
	/** Failed because it was unable to connect (safe to retry) */
	Failed_ConnectionError

};

#ifdef DELETE
#undef DELETE
#endif
enum RequestMethodType
{
	GET,
	POST,
	PUT,
	HEAD,
	DELETE,
	UNKNOWN,
};

class HTTPResponse;
class HTTPRequest : public noncopyable, public std::enable_shared_from_this<HTTPRequest>
{
public:
    typedef std::map<std::string, std::string> KeyValueMap;
    typedef std::shared_ptr<HTTPRequest>       Pointer;
    typedef std::shared_ptr<HTTPResponse>      ResponsePtr;


    // Function prototype: void(HTTPRequest::Pointer request, int64 nowSize, int64 totalSize)
    using HTTPRequestProgressMethod = std::function < void(HTTPRequest::Pointer, int64, int64) >;

   /**
    * Delegate called when an Http request completes
    *
    * Function prototype: void(HTTPRequest::Pointer request, HTTPResponse::Pointer response)
    * first parameter - original Http request that started things
    * second parameter - response received from the server if a successful connection was established
    * third parameter - indicates whether or not the request was able to connect successfully
    */
    using HTTPRequestCompletedMethod = std::function < void(HTTPRequest::Pointer, ResponsePtr) >;
    //typedef std::function < void(HTTPRequest::Pointer, ResponsePtr, bool) > HttpRequestCompleteDelegate;

    HTTPRequestProgressMethod  OnRequestProgress;

    HTTPRequestCompletedMethod OnRequestCompleted;

public:
    HTTPRequest(bool enableDebug = false);
    virtual ~HTTPRequest();

    static Pointer           Create(bool enableDebug = false);
    HTTPRequest&             SetURL(const std::string& url);
    HTTPRequest&             SetMethod(RequestMethodType  method);
    HTTPRequest&             SetHeader(const std::string& headerName, const std::string& headerValue);
    HTTPRequest&             SetPostField(const std::string& name, const std::string& value);
    HTTPRequest&             SetTimeOut(int seconds);
    HTTPRequest&             SetContentField(const std::string& name, const std::string& value);
    HTTPRequest&             SetFileField(const std::string& name, const std::string& fullPath);
    HTTPRequest&             SetRecvSpeed(int64 bytesPerSecond);
    HTTPRequest&             SetSendSpeed(int64 bytesPerSecond);

    std::string              GetURL()const;
    std::string              GetURLParameter(const std::string& parameterName);
    std::string              GetHeader(const std::string& headerName);
    std::string              GetPostFieldsAsString();
    std::vector<std::string> GetAllHeaders();
    KeyValueMap&             GetContentFields();
    KeyValueMap&             GetFileFields();
    CURL*                    GetEasyHandle();
    RequestStatus            GetRequestStatus();
    ResponsePtr              GetResponse();
    int                      GetTimeOut();
    bool                     IsFinished();
    void                     Cancel();
protected:
    virtual bool             Build();
    virtual void             OnTick(float deltaTime);
    virtual bool             OnFinished();

   /**
    * Method called when libcurl wants us to supply more data (see CURLOPT_READFUNCTION)
    *
    * @param buffer buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks size of above buffer, in 'blocks'
    * @param blockSizeInBytes size of a single block
    * @return number of bytes actually written to buffer, or CURL_READFUNC_ABORT to abort the operation
    */
    virtual size_t UploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes);

   /**
    * Method called when libcurl wants us to receive response header (see CURLOPT_HEADERFUNCTION). Headers will be passed
    * line by line (i.e. this callback will be called with a full line), not necessarily zero-terminated. This callback will
    * be also passed any intermediate headers, not only final response's ones.
    *
    * @param buffer buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks size of above buffer, in 'blocks'
    * @param blockSizeInBytes size of a single block
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    virtual size_t ReceiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes);

   /**
    * Method called when libcurl wants us to receive response body (see CURLOPT_WRITEFUNCTION)
    *
    * @param buffer buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks size of above buffer, in 'blocks'
    * @param blockSizeInBytes size of a single block
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    virtual size_t ReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes);

   /**
    * Method called progress info about libcurl activities (see CURLOPT_XFERINFOFUNCTION)
    *
    * @param totalDownload Size of total download.
    * @param nowDownload Size of current downloaded.
    * @param totalUpload Size of total upload.
    * @param nowUpload   Size of current uploaded.
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    virtual int    ProgressCallback(curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload);

   /**
    * Method called with debug information about libcurl activities (see CURLOPT_DEBUGFUNCTION)
    *
    * @param handle handle to which the debug information applies
    * @param debugInfoType type of information (CURLINFO_*)
    * @param debugInfo debug information itself (may NOT be text, may NOT be zero-terminated)
    * @param debugInfoSize exact size of debug information
    * @return must return 0
    */
    virtual size_t DebugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize);
protected:
    void                     SetCompleted(CURLcode completionResult);
    void                     SetAddToMultiResult(CURLMcode result);
    void                     SetRequestStatus(RequestStatus status);
    void                     FinishedRequest();
private: // ================================ Callback by libcurl ============================

   /**
    * Static callback to be used as read function (CURLOPT_READFUNCTION), will dispatch the call to proper instance
    *
    * @param buffer Buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks Size of above buffer, in 'blocks'
    * @param blockSizeInBytes Size of a single block
    * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
    * @return number of bytes actually written to buffer, or CURL_READFUNC_ABORT to abort the operation
    */
    static size_t StaticUploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

   /**
    * Static callback to be used as header function (CURLOPT_HEADERFUNCTION), will dispatch the call to proper instance
    *
    * @param buffer Buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks Size of above buffer, in 'blocks'
    * @param blockSizeInBytes Size of a single block
    * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    static size_t StaticReceiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

   /**
    * Static callback to be used as write function (CURLOPT_WRITEFUNCTION), will dispatch the call to proper instance
    *
    * @param buffer Buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks Size of above buffer, in 'blocks'
    * @param blockSizeInBytes Size of a single block
    * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    static size_t StaticReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

   /**
    * Static callback to be used as write function (CURLOPT_XFERINFOFUNCTION), will dispatch the call to proper instance
    *
    * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
    * @param totalDownload Size of total download.
    * @param nowDownload Size of current downloaded.
    * @param totalUpload Size of total upload.
    * @param nowUpload   Size of current uploaded.
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    static int    StaticProgressCallback(void *userData, curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload);

   /**
    * Static callback to be used as debug function (CURLOPT_DEBUGFUNCTION), will dispatch the call to proper instance
    *
    * @param handle handle to which the debug information applies
    * @param debugInfoType type of information (CURLINFO_*)
    * @param debugInfo debug information itself (may NOT be text, may NOT be zero-terminated)
    * @param debugInfoSize exact size of debug information
    * @param userData data we associated with request (will be a pointer to HTTPRequest instance)
    * @return must return 0
    */
    static size_t StaticDebugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData);
protected:
    friend class HTTPClient;
    /** Pointer to an easy handle specific to this request */
    CURL*			EasyHandle;
    /** List of custom headers to be passed to CURL */
    curl_slist*	    HeaderList;
    /** Cached request type */
    RequestMethodType  MethodType;
    /** Set to true if request has been canceled */
    bool			bCanceled;
    /** Set to true when request has been completed */
    bool			bCompleted;
    /* Time-out the request operation after this amount of seconds */
    bool            bTimeout;
    /** Operation result code as returned by libcurl */
    CURLcode		CompletionResult;
    /** Set to true if request failed to be added to curl multi */
    CURLMcode		AddToMultiResult;
    /** Number of bytes sent already */
    uint32			BytesSent;
    /** Current status of request being processed */
    RequestStatus   Status;
    /** Set true if enable curl call CURLOPT_DEBUGFUNCTION */
    bool            EnableDebug;
    /** Cached URL */
    std::string     Url;
    /** Mapping of header section to values. */
    KeyValueMap     Headers;
    KeyValueMap     PostFields;
    KeyValueMap     ContentFields;
    KeyValueMap     FileFields;
    curl_httppost*  FormPost;
    /** The response object which we will use to pair with this request */
    ResponsePtr     ResponseInstance;
    char            ErrorBuffer[CURL_ERROR_SIZE];
    /* Time-out the request operation after this amount of seconds */
    int             TimeOutValue;
    float           ElapsedTime;
    bool            bFinished;    
};

NS_FK_END
//#include "HTTPClient/HTTPRequest.ipp"
#endif // END OF NETFRAMEWORK_HTTPREQUEST_HPP
