#ifndef LOSEMYMIND_HTTPDOWNLOADNODE_HPP
#define LOSEMYMIND_HTTPDOWNLOADNODE_HPP

#include <functional>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include "curl.h"
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Base/Types.h"

NS_FK_BEGIN
namespace network
{

    const size_t CHUNK_LENGTH_LIMIT = 1024*1024*5; //5M
    const size_t CHUNK_NUMBER_LIMIT = 20;

    class HTTPDownloader;
    class HTTPDownloadNode;
    class RequestBase
    {
    public:
        RequestBase();
        virtual~RequestBase();
        CURL*          curlHandle;
        CURLcode       curlCode;
        std::string    curlMessage;
        curl_slist*    headerList;
        CURLMcode      addToMultiResult;
        long           responseCode;
        std::string    requestUrl;
        char           errorBuffer[CURL_ERROR_SIZE];
    };

    class HeaderRequest : public RequestBase
    {
    public:
        HeaderRequest();
        virtual~HeaderRequest();
        HTTPDownloadNode* pHTTPDownloadNode;
    };

    class ContentRequest : public RequestBase
    {
    public:
        ContentRequest();
        virtual~ContentRequest();
        long rangeFrom;
        long rangeTo;
        size_t retryCount;
        HTTPDownloadNode* pHTTPDownloadNode;
    };


    struct DownloadListener
    {
        // param[0]  url
        // param[1]  error message
        typedef std::function<void(const std::string&, const std::string&)> ErrorCallback;

        // param[0]  url
        // param[1]  now download
        // param[2]  total download
        typedef std::function<void(const std::string&, int64, int64)> ProgressCallback;

        // param[0]  url
        // param[1]  file full path
        typedef std::function<void(const std::string&, const std::string&)> SucceedCallback;

        DownloadListener();
        virtual~DownloadListener();
        ErrorCallback     onErrorCallback;
        ProgressCallback  onProgressCallback;
        SucceedCallback   onSucceedCallback;
    };

    class HTTPDownloadNode
    {
    public:
        using HeaderRequestPointer = std::shared_ptr < HeaderRequest >;
        using ContentRequestPointer = std::shared_ptr < ContentRequest >;
        using ContentRequestMap = std::unordered_map <CURL*, ContentRequestPointer >;
        using ContentRequestVector = std::vector <ContentRequestPointer >;

    public:
        HTTPDownloadNode(const std::string& requestUrl, const std::string& fileFullPath, const DownloadListener& listener, HTTPDownloader* pHTTPDownloader);
        virtual~HTTPDownloadNode();
        void pause();
        void resume();
        bool headerRequestCompleted(HeaderRequestPointer headerRequest);
        // if all request is completed,return true otherwise return false.
        bool contentRequestCompleted(ContentRequestPointer contentRequest);
        size_t writeData(ContentRequest* contentRequest, void* buffer, size_t size);

        std::string getRequestUrl(){ return _requestUrl; }

    private:
        void addContentRequest(ContentRequestPointer contentRequest);
        void reportError(RequestBase* request);

    private:
        std::string                _requestUrl;
        std::string                _fileFullPath;
        std::string                _contentType;
        size_t                     _contentSize;
        ContentRequestMap          _contentRequests;
        std::mutex                 _contentRequestMutex;
        DownloadListener           _downloadListener;
        FILE*                      _fileHandle;
        std::string                _effectiveUrl;
        HTTPDownloader*            _pHTTPDownloader;
        size_t                     _nowDownlaod;
        ContentRequestVector       _failedRequests;
    };


} // namespace network
NS_FK_END
#endif // LOSEMYMIND_HTTPDOWNLOADNODE_HPP


