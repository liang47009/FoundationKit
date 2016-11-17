#ifndef LOSEMYMIND_HTTPREQUEST_H
#define LOSEMYMIND_HTTPREQUEST_H


#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include "curl.h"

#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Base/Types.h"
#include "HTTPError.hpp"

NS_FK_BEGIN

#ifdef DELETE
#undef DELETE
#endif

#define REQUIRED
#define OPTIONAL

enum class HttpStatusCode
{
    /** Has not been started via ProcessRequest() */
    NotStarted,
    /** Currently being ticked and processed */
    Processing,
    /** Finished but failed */
    Failed,
    /** Failed because it was unable to connect (safe to retry) */
    Failed_ConnectionError,
    /** Finished and was successful */
    Succeeded
};

struct HttpStatusObject
{
    HttpStatusCode _Errcode;
    const char *_Name;
};

static const HttpStatusObject _Map_HttpErrorTab[] =
{	// table of Windows code/name pairs
    HttpStatusCode::NotStarted, "Has not been started.",
    HttpStatusCode::Processing, "Currently being ticked and processed.",
    HttpStatusCode::Failed, "Finished but failed.",
    HttpStatusCode::Failed_ConnectionError, "Failed because it was unable to connect (safe to retry).",
    HttpStatusCode::Succeeded, "Finished and was successful",
    (HttpStatusCode)0, 0
};

const char * httpStatusToString(HttpStatusCode statusCode)
{	// convert to name of generic error
    const HttpStatusObject *_Ptr = &_Map_HttpErrorTab[0];
    for (; _Ptr->_Name != 0; ++_Ptr)
        if (_Ptr->_Errcode == statusCode)
            return (_Ptr->_Name);
    return ("unknown status");
}

class HTTPResponse;
class HTTPRequest
{
public:
    typedef std::unordered_map<std::string, std::string> KeyValueMap;
    typedef std::shared_ptr<HTTPRequest>                 Pointer;
    typedef std::shared_ptr<HTTPResponse>                ResponsePtr;

    /**
     * Delegate called when an Http request completes
     *
     * @param first parameter - original Http request that started things
     * @param second parameter - response received from the server if a successful connection was established
     * @param third parameter - indicates whether or not the request was able to connect successfully
     */
    using HttpRequestCompleteDelegate = std::function < void(HTTPRequest::Pointer, ResponsePtr, bool) >;
    //typedef std::function < void(HTTPRequest::Pointer, ResponsePtr, bool) > HttpRequestCompleteDelegate;

    /**
     * Delegate called per tick to update an Http request upload or download size progress
     *
     * @param first parameter - original Http request that started things
     * @param second parameter - the number of bytes sent / uploaded in the request so far.
     * @param third parameter - the number of bytes received / downloaded in the response so far.
     */
    using HttpRequestProgressDelegate = std::function < void(HTTPRequest::Pointer, int32, int32) > ;
    //typedef std::function < void(HTTPRequest::Pointer, int32, int32) > HttpRequestProgressDelegate;

    enum class MethodType
    {
        GET,
        POST,
        PUT,
        HEAD,
        DELETE,
        UNKNOWN,
    };
public:
    // implementation friends
    friend class HTTPResponse;

    /** Delegate that will get called once request completes or on any error */
    HttpRequestCompleteDelegate onRequestCompleteDelegate;
    /** Delegate that will get called once per tick with total bytes uploaded and downloaded so far */
    HttpRequestProgressDelegate onRequestProgressDelegate;

    /**
     * Constructor
     */
    HTTPRequest(CURLSH* inShareHandle, bool enableDebug = false);

    /**
     * Destructor. Clean up any connection/request handles
     */
    virtual ~HTTPRequest();


    HTTPRequest&             setURL(const std::string& url);
    HTTPRequest&             setMethod(MethodType  method);
    HTTPRequest&             setHeader(const std::string& headerName, const std::string& headerValue);



    std::string              getURL()const;
    std::string              getURLParameter(const std::string& parameterName);
    std::string              getHeader(const std::string& headerName);
    std::vector<std::string> getAllHeaders();

    void                     cancel();
    void                     update(float deltaTime);

private:
    /**
     * Static callback to be used as read function (CURLOPT_READFUNCTION), will dispatch the call to proper instance
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param blockSizeInBytes size of a single block
     * @param userData data we associated with request (will be a pointer to FCurlHttpRequest instance)
     * @return number of bytes actually written to buffer, or CURL_READFUNC_ABORT to abort the operation
     */
    static size_t staticUploadCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

    /**
     * Method called when libcurl wants us to supply more data (see CURLOPT_READFUNCTION)
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param blockSizeInBytes size of a single block
     * @return number of bytes actually written to buffer, or CURL_READFUNC_ABORT to abort the operation
     */
    size_t uploadCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes);

    /**
     * Static callback to be used as header function (CURLOPT_HEADERFUNCTION), will dispatch the call to proper instance
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param blockSizeInBytes size of a single block
     * @param userData data we associated with request (will be a pointer to FCurlHttpRequest instance)
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    static size_t staticReceiveResponseHeaderCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

    /**
     * Method called when libcurl wants us to receive response header (see CURLOPT_HEADERFUNCTION). Headers will be passed
     * line by line (i.e. this callback will be called with a full line), not necessarily zero-terminated. This callback will
     * be also passed any intermediate headers, not only final response's ones.
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param BlockSizeInBytes size of a single block
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    size_t receiveResponseHeaderCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes);

    /**
     * Static callback to be used as write function (CURLOPT_WRITEFUNCTION), will dispatch the call to proper instance
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param blockSizeInBytes size of a single block
     * @param userData data we associated with request (will be a pointer to FCurlHttpRequest instance)
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    static size_t staticReceiveResponseBodyCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

    /**
     * Method called when libcurl wants us to receive response body (see CURLOPT_WRITEFUNCTION)
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param blockSizeInBytes size of a single block
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    size_t receiveResponseBodyCallback(void* ptr, size_t sizeInBlocks, size_t blockSizeInBytes);

    /**
     * Static callback to be used as debug function (CURLOPT_DEBUGFUNCTION), will dispatch the call to proper instance
     *
     * @param handle handle to which the debug information applies
     * @param debugInfoType type of information (CURLINFO_*)
     * @param debugInfo debug information itself (may NOT be text, may NOT be zero-terminated)
     * @param debugInfoSize exact size of debug information
     * @param userData data we associated with request (will be a pointer to FCurlHttpRequest instance)
     * @return must return 0
     */
    static size_t staticDebugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData);

    /**
     * Method called with debug information about libcurl activities (see CURLOPT_DEBUGFUNCTION)
     *
     * @param handle handle to which the debug information applies
     * @param debugInfoType type of information (CURLINFO_*)
     * @param debugInfo debug information itself (may NOT be text, may NOT be zero-terminated)
     * @param debugInfoSize exact size of debug information
     * @return must return 0
     */
    size_t debugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize);


private:
    void internalInitialize(CURLSH* inShareHandle);
    bool internalSetup();

private:
    /** Pointer to an easy handle specific to this request */
    CURL*			_easyHandle;
    /** List of custom headers to be passed to CURL */
    curl_slist*	    _headerList;
    /** Cached request type */
    MethodType      _requestType;
    /** Set to true if request has been canceled */
    bool			_bCanceled;
    /** Set to true when request has been completed */
    bool			_bCompleted;
    /** Set to true if request failed to be added to curl multi */
    CURLMcode		_addToMultiResult;
    /** Operation result code as returned by libcurl */
    CURLcode		_completionResult;
    /** Number of bytes sent already */
    uint32			_bytesSent;
    /** Current status of request being processed */
    HttpStatusCode              _completionStatus;
    /** Total elapsed time in seconds since the start of the request */
    float                       _elapsedTime;
    /** Elapsed time since the last received HTTP response. */
    float                       _timeSinceLastResponse;
    /** Set true if enable curl call CURLOPT_DEBUGFUNCTION */
    bool                        _enableDebug;
    /** Cached URL */
    std::string                 _url;
    /** Mapping of header section to values. */
    KeyValueMap                 _headers;
    /** BYTE array payload to use with the request. Typically for a POST */
    std::vector<uint8>          _requestPayload;
    /** The response object which we will use to pair with this request */
    ResponsePtr                 _response;
    char                        _errorBuffer[CURL_ERROR_SIZE];
};


NS_FK_END


#endif // LOSEMYMIND_HTTPREQUEST_H
