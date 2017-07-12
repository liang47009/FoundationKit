
#include "HTTPRequest.hpp"
NS_FK_BEGIN

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
    ErrorCallback     OnErrorCallback;
    ProgressCallback  OnProgressCallback;
    SucceedCallback   OnSucceedCallback;
};

class HTTPDownloadRequest : public HTTPRequest
{
public:
    typedef std::shared_ptr<HTTPDownloadRequest> Pointer;

    HTTPDownloadRequest(bool enableDebug = false);

    virtual ~HTTPDownloadRequest();

    static Pointer           Create(bool enableDebug = false);

    virtual void             Tick(float deltaTime) override;
protected:
   /**
    * Method called when libcurl wants us to receive response body (see CURLOPT_WRITEFUNCTION)
    *
    * @param buffer buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks size of above buffer, in 'blocks'
    * @param blockSizeInBytes size of a single block
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    virtual size_t ReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)override;

   /**
    * Method called progress info about libcurl activities (see CURLOPT_XFERINFOFUNCTION)
    *
    * @param totalDownload Size of total download.
    * @param nowDownload Size of current downloaded.
    * @param totalUpload Size of total upload.
    * @param nowUpload   Size of current uploaded.
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    virtual int ProgressCallback(curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload) override;
};




NS_FK_END


