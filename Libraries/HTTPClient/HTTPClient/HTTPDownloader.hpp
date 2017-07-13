#include <vector>
#include <unordered_map>
#include "HTTPRequest.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Base/multiple_buffer.hpp"
NS_FK_BEGIN


class HTTPDownloadRequest : public HTTPRequest
{
public:
    typedef std::shared_ptr<HTTPDownloadRequest> Pointer;
    HTTPDownloadRequest(bool enableDebug = false);
    virtual ~HTTPDownloadRequest();
    static Pointer           Create(bool enableDebug = false);
    HTTPDownloadRequest&     SetFileHandle(FILE*  pFile);
    HTTPDownloadRequest&     SetEnableRange(bool enableRange);
    FILE*                    GetFileHandle();
    int64                    GetDownladedSize();
   
protected:
    virtual bool   Build()override;
    virtual bool   OnFinishedRequest()override;

   /**
    * Method called when libcurl wants us to receive response body (see CURLOPT_WRITEFUNCTION)
    *
    * @param buffer buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks size of above buffer, in 'blocks'
    * @param blockSizeInBytes size of a single block
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    virtual size_t ReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)override;
public:
    /** Offset into file where the file is stored.*/
    int64           Offset;
    /** Serialized file size. */
    int64           Size;
    /** Current write offset.*/
    int64           WriteOffset;
protected:
    FILE*           FileHandle;
    /** Size of downloaded*/
    int64           DownladedSize;
    /** Has Accept-Ranges*/
    bool            EnableRange;
};

// ==================================================================================
// ============================= For experimental ===================================
// ==================================================================================

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

    DownloadListener()
        : OnErrorCallback(nullptr)
        , OnProgressCallback(nullptr)
        , OnSucceedCallback(nullptr)
    {}
    virtual~DownloadListener(){};
    ErrorCallback     OnErrorCallback;
    ProgressCallback  OnProgressCallback;
    SucceedCallback   OnSucceedCallback;
};

typedef std::vector<HTTPDownloadRequest::Pointer>  DownloadRequestVec;
struct DownloaderEntry
{
    std::string URL;
    std::string FilePath;
    FILE*       FileHandle;
    int64       ContentSize;
    DownloadListener   CallbackListener;
    DownloadRequestVec DownloadRequests;
    DownloaderEntry()
        : FileHandle(nullptr)
        , ContentSize(0)
    {}
};



class HTTPDownloader : public Singleton<HTTPDownloader>
{
    friend Singleton<HTTPDownloader>;
    HTTPDownloader();
public:
    typedef std::shared_ptr<DownloaderEntry>  DownloaderEntryPointer;
    typedef std::unordered_map<std::string, DownloaderEntryPointer>  DownloaderEntryMap;
    ~HTTPDownloader();
    void  Tick(float deltaTime);
    void  DownloadToFile(const std::string& url, const std::string& storagePath, const DownloadListener& listener);
    void  DownloadToMemory(const std::string& url, mutable_buffer memoryBuffer, const DownloadListener& listener);

protected:
    DownloaderEntryMap         DownloaderEntryPool;

};



NS_FK_END


