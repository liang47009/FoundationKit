#ifndef LOSEMYMIND_HTTPREQUEST_H
#define LOSEMYMIND_HTTPREQUEST_H


#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include "curl.h"
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Foundation/ITickable.h"
#include "FoundationKit/Base/Types.h"


NS_FK_BEGIN

#ifdef DELETE
#undef DELETE
#endif

#define REQUIRED
#define OPTIONAL

namespace network{
enum class HttpStatusCode
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

class HTTPResponse;
class HTTPRequest : public ITickable, public std::enable_shared_from_this<HTTPRequest>
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
     * @param 1 parameter - original Http request that started things
     * @param 2 parameter - the number of bytes total download in the request so far.
     * @param 3 parameter - the number of bytes current download in the request so far.
     * @param 4 parameter - the number of bytes total upload in the request so far.
     * @param 5 parameter - the number of bytes current upload in the request so far.
     */
    using HttpRequestProgressDelegate = std::function < void(HTTPRequest::Pointer, int64, int64, int64, int64) >;
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
    //friend class HTTPResponse;
    /** Delegate that will get called once request completes or on any error */
    HttpRequestCompleteDelegate onRequestCompleteDelegate;
    /** Delegate that will get called once per tick with total bytes uploaded and downloaded so far */
    HttpRequestProgressDelegate onRequestProgressDelegate;

    /**
     * Constructor
     */
    HTTPRequest(bool enableDebug = false);

    /**
     * Destructor. Clean up any connection/request handles
     */
    virtual ~HTTPRequest();

    static Pointer           create(bool enableDebug = false);
    HTTPRequest&             setURL(const std::string& url);
    HTTPRequest&             setMethod(MethodType  method);
    HTTPRequest&             setHeader(const std::string& headerName, const std::string& headerValue);
    HTTPRequest&             setPostField(const std::string& name, const std::string& value);
    HTTPRequest&             setContentField(const std::string& name, const std::string& value);
    HTTPRequest&             setFileField(const std::string& name, const std::string& fullPath);
    HTTPRequest&             setRequestPayload(const std::vector<uint8>& data);
    HTTPRequest&             setTimeOut(float seconds);


    std::string              getURL()const;
    std::string              getURLParameter(const std::string& parameterName);
    std::string              getHeader(const std::string& headerName);
    std::vector<std::string> getAllHeaders();
    KeyValueMap&             getPostFields();
    KeyValueMap&             getContentFields();
    KeyValueMap&             getFileFields();
    std::vector<uint8>&      getRequestPayload();
    CURL*                    getEasyHandle();
    HttpStatusCode           getRequestStatus();
    ResponsePtr              getResponse();
    float                    getTimeOut();
    void                     cancel();
    bool                     isFinished();

    virtual void tick(float deltaTime) override;
    virtual bool isTickable() const override;
protected:
    bool                     build();
    void                     setCompleted(CURLcode completionResult);
    void                     setAddToMultiResult(CURLMcode result);
    void                     setRequestStatus(HttpStatusCode status);
    void                     finishedRequest();

private:
    void buildRequestPayload();
    void buildFormPayload();

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
    static size_t staticUploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

    /**
     * Static callback to be used as header function (CURLOPT_HEADERFUNCTION), will dispatch the call to proper instance
     *
     * @param buffer Buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks Size of above buffer, in 'blocks'
     * @param blockSizeInBytes Size of a single block
     * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    static size_t staticReceiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

    /**
     * Static callback to be used as write function (CURLOPT_WRITEFUNCTION), will dispatch the call to proper instance
     *
     * @param buffer Buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks Size of above buffer, in 'blocks'
     * @param blockSizeInBytes Size of a single block
     * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    static size_t staticReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

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
    static int    staticProgressCallback(void *userData, curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload);

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
    static size_t staticDebugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData);
   
private:

    /**
     * Method called when libcurl wants us to supply more data (see CURLOPT_READFUNCTION)
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param blockSizeInBytes size of a single block
     * @return number of bytes actually written to buffer, or CURL_READFUNC_ABORT to abort the operation
     */
    size_t uploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes);
    
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
    size_t receiveResponseHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes);

    /**
     * Method called when libcurl wants us to receive response body (see CURLOPT_WRITEFUNCTION)
     *
     * @param ptr buffer to copy data to (allocated and managed by libcurl)
     * @param sizeInBlocks size of above buffer, in 'blocks'
     * @param blockSizeInBytes size of a single block
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    size_t receiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes);

    /**
     * Method called progress info about libcurl activities (see CURLOPT_XFERINFOFUNCTION) 
     *
     * @param totalDownload Size of total download.
     * @param nowDownload Size of current downloaded.
     * @param totalUpload Size of total upload.
     * @param nowUpload   Size of current uploaded.
     * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
     */
    int    progressCallback(curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload);

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
    friend class HTTPClient;
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
    /* Time-out the request operation after this amount of seconds */
    bool            _bTimeout;
    /** Operation result code as returned by libcurl */
    CURLcode		_completionResult;
    /** Set to true if request failed to be added to curl multi */
    CURLMcode		_addToMultiResult;
    /** Number of bytes sent already */
    uint32			_bytesSent;
    /** Current status of request being processed */
    HttpStatusCode              _requestStatus;
    /** Set true if enable curl call CURLOPT_DEBUGFUNCTION */
    bool                        _enableDebug;
    /** Cached URL */
    std::string                 _url;
    /** Mapping of header section to values. */
    KeyValueMap                 _headers;
    KeyValueMap                 _postFields;
    KeyValueMap                 _contentFields;
    KeyValueMap                 _fileFields;
    /** BYTE array payload to use with the request. Typically for a POST */
    std::vector<uint8>          _requestPayload;
    /** The response object which we will use to pair with this request */
    ResponsePtr                 _response;
    char                        _errorBuffer[CURL_ERROR_SIZE];
    /* Time-out the request operation after this amount of seconds */
    float                       _httpTimeOut;
    float                       _elapsedTime;
    bool                        _bFinished;
    curl_httppost*              _formPost;

};

} // namespace network
NS_FK_END


#endif // LOSEMYMIND_HTTPREQUEST_H






