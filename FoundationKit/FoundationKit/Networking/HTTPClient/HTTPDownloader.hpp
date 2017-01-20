
#include <functional>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include "curl.h"
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Base/multiple_buffer.hpp"
#include "FoundationKit/Base/error_code.hpp"

NS_FK_BEGIN
namespace network{

#define HTTPDownloaderVersionString "1.0.0"


struct DownloadRequest;
class HTTPDownloader
{
public:

    struct DownloadError
    {
        int         httpCode;
        std::string message;
    };

    enum class DownloadType
    {
        T_NONE,
        T_FILE,
        T_BUFFER,
    };

    struct Response
    {
        bool         valid;
        std::string  url;
        std::string  contentType;
        double       contentSize;
        long         responseCode;
    };

    struct Request
    {
        Request();
        ~Request();
        curl_slist*	                  headerList;
        CURL*                         easyHandle;
        std::string                   requestUrl;
        char                          errorBuffer[CURL_ERROR_SIZE];
        HTTPDownloader::DownloadType  type;
        CURLMcode                     addToMultiResult;
        Response                      response;
    };

    typedef std::function<void(const std::string&, int, const std::string&)> ErrorCallback;
    using DownloadRequestPointer = std::shared_ptr < Request >;
    using RequestMap = std::unordered_map < CURL*, DownloadRequestPointer > ;
    using ThreadPointer = std::shared_ptr < std::thread > ;
public:
    HTTPDownloader();
    virtual~HTTPDownloader();
    bool   initialize(bool bDebug);

   /**
    * Set proxy info
    * @param proxyAddr host:port to use with proxy.
    * @param userPwd   "user:password" to use with proxy.
    * example: 
    * HTTPDownloader::setProxy("proxy-host.com:8080","UserName:Password");
    */
    HTTPDownloader& setProxy(const std::string& proxyAddr, const std::string& userPwd);
    HTTPDownloader& setEnableVerifyPeer(bool enableVerifyPeer);
    HTTPDownloader& setReuseConnections(bool reuse);
    HTTPDownloader& setCertBundlePath(const std::string& bundlePath);


    void  download(const std::string& url, const std::string& storagePath);
    void  download(const std::string& url, mutable_buffer& buffer);

private:
    bool   buildRequestForData(DownloadRequestPointer pDownloadRequest);
    bool   buildRequestForHeader(DownloadRequestPointer pDownloadRequest);
    bool   buildRequestGeneric(CURL* curlHandle);
    void   performDownload();
    size_t getRequestSize();
    void   requestCompleted(CURL* curlHandle);
private: // ================================ Callback by libcurl ============================
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

   /**
    * Static callback to be used as header function (CURLOPT_HEADERFUNCTION), will dispatch the call to proper instance
    *
    * @param buffer Buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks Size of above buffer, in 'blocks'
    * @param blockSizeInBytes Size of a single block
    * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    static size_t staticReceiveHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

   /**
    * Static callback to be used as write function (CURLOPT_WRITEFUNCTION), will dispatch the call to proper instance
    *
    * @param buffer Buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks Size of above buffer, in 'blocks'
    * @param blockSizeInBytes Size of a single block
    * @param userData Data we associated with request (will be a pointer to HTTPRequest instance)
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    static size_t staticReceiveBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData);

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


protected:
    CURLcode    m_lastCode;
    CURLM*      m_multiHandle;
    CURLSH*     m_shareHandle;
    bool        m_bDebug;
    std::string m_proxyaddr;
    std::string m_proxyUserPwd;
    bool        m_bEnableVerifyPeer;
    bool        m_bReuseConnections;
    std::string m_certBundlePath;
    RequestMap m_requestMap;
    std::mutex       m_requestMutex;
    std::condition_variable m_requestCond;
    ThreadPointer           m_downloadThread;
    std::atomic_bool        m_quit;
};



//int            httpCode;
//std::string    httpMessage;
//
//mutable_buffer buffer;
//std::string    storagePath;
//mutable void*  fileHandle;
//mutable bool   resumeDownload;
//mutable double downloaded;
//mutable double totalToDownload;
//mutable void*  reserved;

} // namespace network
NS_FK_END

