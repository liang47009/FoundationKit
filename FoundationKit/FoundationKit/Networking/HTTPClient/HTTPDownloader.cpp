#include <algorithm>
#include "HTTPDownloader.hpp"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Networking/libcurl_init.hpp"

static const int MAX_WAIT_MSECS = 30 * 1000; /* Wait max. 30 seconds */

static size_t writeFuncForAdapter(void *ptr, size_t size, size_t nmemb, void* userdata)
{
    UNUSED_ARG(ptr);
    UNUSED_ARG(size);
    UNUSED_ARG(userdata);
    return nmemb;
}


NS_FK_BEGIN
namespace network{

HTTPDownloader::HTTPDownloader()
{

}

HTTPDownloader::~HTTPDownloader()
{
    m_quit = true;
    m_downloadThread->join();
    if (nullptr != m_multiHandle)
    {
        curl_multi_cleanup(m_multiHandle);
        m_multiHandle = nullptr;
    }

    if (nullptr != m_shareHandle)
    {
        curl_share_cleanup(m_shareHandle);
        m_shareHandle = nullptr;
    }
}

bool HTTPDownloader::initialize(bool bDebug)
{
    if (m_multiHandle != NULL)
    {
        LOG_WARN("Already initialized multi handle");
        return false;
    }
    m_lastCode = libcurl_init::code();
    m_bDebug = bDebug;
    if (libcurl_init::succeeded())
    {
        curl_version_info_data * versionInfo = curl_version_info(CURLVERSION_NOW);
        if (versionInfo)
        {
            LOG_INFO("Using libcurl %s", versionInfo->version);
            LOG_INFO(" - built for %s", versionInfo->host);

            if (versionInfo->features & CURL_VERSION_SSL)
            {
                LOG_INFO(" - supports SSL with %s", versionInfo->ssl_version);
            }
            else
            {
                // No SSL
                LOG_INFO(" - NO SSL SUPPORT!");
            }

            if (versionInfo->features & CURL_VERSION_LIBZ)
            {
                LOG_INFO(" - supports HTTP deflate (compression) using libz %s", versionInfo->libz_version);
            }

            LOG_INFO(" - other features:");

#define printCurlFeature(Feature)	             \
			if (versionInfo->features & Feature) \
            {                                    \
			    LOG_INFO("     %s", #Feature);	 \
            }
            printCurlFeature(CURL_VERSION_SSL);
            printCurlFeature(CURL_VERSION_LIBZ);
            printCurlFeature(CURL_VERSION_DEBUG);
            printCurlFeature(CURL_VERSION_IPV6);
            printCurlFeature(CURL_VERSION_ASYNCHDNS);
            printCurlFeature(CURL_VERSION_LARGEFILE);
            printCurlFeature(CURL_VERSION_IDN);
            printCurlFeature(CURL_VERSION_CONV);
            printCurlFeature(CURL_VERSION_TLSAUTH_SRP);
#undef printCurlFeature
        }

        m_multiHandle = curl_multi_init();
        if (m_multiHandle == nullptr)
        {
            LOG_ERROR("Could not initialize create libcurl multi handle! HTTP transfers will not function properly.");
        }

        m_shareHandle = curl_share_init();
        if (m_shareHandle == nullptr)
        {
            LOG_ERROR("Could not initialize libcurl share handle!");
        }
        else
        {
            curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
            curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
            curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Could not initialize libcurl (result=%d), HTTP transfers will not function properly.", (int32)libcurl_init::code());
    }

    auto threadFun = std::bind(&HTTPDownloader::performDownload, this);
    m_downloadThread = std::make_shared<std::thread>(threadFun);
    return true;
}

HTTPDownloader& HTTPDownloader::setProxy(const std::string& proxyAddr, const std::string& userPwd)
{
    m_proxyaddr    = proxyAddr;
    m_proxyUserPwd = userPwd;
    return (*this);
}

HTTPDownloader& HTTPDownloader::setEnableVerifyPeer(bool enableVerifyPeer)
{
    m_bEnableVerifyPeer = enableVerifyPeer;
    return (*this);
}

HTTPDownloader& HTTPDownloader::setReuseConnections(bool reuse)
{
    m_bReuseConnections = reuse;
    return (*this);
}

HTTPDownloader& HTTPDownloader::setCertBundlePath(const std::string& bundlePath)
{
    m_certBundlePath = bundlePath;
    return (*this);
}



void HTTPDownloader::downloadToFile(const std::string& url, const std::string& storagePath, const DownloadToFileCallback& callback)
{
    std::string checkedStoragePath = storagePath;
    if (checkedStoragePath.back() != '/' && checkedStoragePath.back() != '\\')
    {
        checkedStoragePath.push_back('/');
    }

    size_t pos = url.find_last_of("/");
    if (pos != std::string::npos)
    {
        checkedStoragePath += url.substr(pos + 1);
    }

    DownloadRequestPointer request = std::make_shared<Request>();
    request->requestUrl = url;
    request->dataWriter = new DownloadDataFileWriter(checkedStoragePath, callback);
    request->isRequestHeader = true;
    buildRequestForHeader(request);
    std::lock_guard<std::mutex>  lock(m_requestMutex);
    request->addToMultiResult = curl_multi_add_handle(m_multiHandle, request->easyHandle);
    m_requestMap.insert(std::make_pair(request->easyHandle, request));
}

void HTTPDownloader::downloadToMemory(const std::string& url, const DownloadToMemoryCallback& callback)
{
    DownloadRequestPointer request = std::make_shared<Request>();
    request->requestUrl = url;
    request->dataWriter = new DownloadDataBufferWriter(callback);
    request->isRequestHeader = true;
    buildRequestForHeader(request);
    std::lock_guard<std::mutex>  lock(m_requestMutex);
    request->addToMultiResult = curl_multi_add_handle(m_multiHandle, request->easyHandle);
    m_requestMap.insert(std::make_pair(request->easyHandle, request));
}

bool HTTPDownloader::buildRequestForData(DownloadRequestPointer pDownloadRequest)
{
    CURL* easyHandle = curl_easy_init();
    buildRequestGeneric(easyHandle);

    // receive error messages
    curl_easy_setopt(easyHandle, CURLOPT_ERRORBUFFER, pDownloadRequest->errorBuffer);
    // associate with this just in case
    curl_easy_setopt(easyHandle, CURLOPT_PRIVATE, pDownloadRequest.get());

    // set up write function to receive response payload
    curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, pDownloadRequest.get());
    curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, staticReceiveBodyCallback);

    // set progress function
    curl_easy_setopt(easyHandle, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(easyHandle, CURLOPT_XFERINFODATA, pDownloadRequest.get());
    curl_easy_setopt(easyHandle, CURLOPT_XFERINFOFUNCTION, staticProgressCallback);
    curl_easy_setopt(easyHandle, CURLOPT_URL, pDownloadRequest->requestUrl.c_str());
    curl_easy_setopt(easyHandle, CURLOPT_HTTPGET, 1L);
    pDownloadRequest->easyHandle = easyHandle;
    return true;
}

bool HTTPDownloader::buildRequestForHeader(DownloadRequestPointer pDownloadRequest)
{
    void *easyHandle = curl_easy_init();
    buildRequestGeneric(easyHandle);
    curl_easy_setopt(easyHandle, CURLOPT_URL, pDownloadRequest->requestUrl.c_str());
    curl_easy_setopt(easyHandle, CURLOPT_HEADER, 1);
    curl_easy_setopt(easyHandle, CURLOPT_NOBODY, 1);
    // in win32 platform, if not set the writeFunction, it will return CURLE_WRITE_ERROR
    curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, writeFuncForAdapter);
    pDownloadRequest->easyHandle = easyHandle;
    return true;
}

bool HTTPDownloader::buildRequestGeneric(CURL* curlHandle)
{
    curl_easy_setopt(curlHandle, CURLOPT_SHARE, m_shareHandle);

    curl_easy_setopt(curlHandle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);

    // allow http redirects to be followed
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);

    // required for all multi-threaded handles
    curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curlHandle, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_LIMIT, 1); //限制最低速度1个字节
    curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_TIME, 5);  //如果5秒内没有字节读取，则超时
    curl_easy_setopt(curlHandle, CURLOPT_MAX_SEND_SPEED_LARGE, 1024 * 100); // limit send speed 100kb/s
    curl_easy_setopt(curlHandle, CURLOPT_MAX_RECV_SPEED_LARGE, 1024 * 100); // limit recv speed 100kb/s

    // set certificate verification (disable to allow self-signed certificates)
    if (m_bEnableVerifyPeer)
    {
        curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 1L);
    }
    else
    {
        curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    if (!m_proxyaddr.empty())
    {
        // guaranteed to be valid at this point
        curl_easy_setopt(curlHandle, CURLOPT_PROXY, m_proxyaddr.c_str());
    }

    if (!m_proxyUserPwd.empty())
    {
        curl_easy_setopt(curlHandle, CURLOPT_PROXYUSERPWD, m_proxyUserPwd.c_str());
    }

    if (m_bReuseConnections)
    {
        curl_easy_setopt(curlHandle, CURLOPT_FORBID_REUSE, 1L);
    }

    if (!m_certBundlePath.empty())
    {
        curl_easy_setopt(curlHandle, CURLOPT_CAINFO, m_certBundlePath.c_str());
    }

    if (m_bDebug)
    {
        // set debug functions (FIXME: find a way to do it only if LogHttp is >= Verbose)
        curl_easy_setopt(curlHandle, CURLOPT_DEBUGDATA, this);
        curl_easy_setopt(curlHandle, CURLOPT_DEBUGFUNCTION, staticDebugCallback);
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
    }

    std::vector<std::string> allHeaders;
    allHeaders.push_back("Pragma:no-cache");
    allHeaders.push_back("Expect:");
    allHeaders.push_back(StringUtils::format("User-Agent:author=%s,client=%s,version=%s", "libo", "HTTPDownloader", HTTPDownloaderVersionString));

    /** List of custom headers to be passed to CURL */
    curl_slist*	    headerList=nullptr;
    for (auto& header : allHeaders)
    {
        headerList = curl_slist_append(headerList, header.c_str());
    }

    if (headerList)
    {
        curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headerList);
    }
    return true;
}

void HTTPDownloader::performDownload()
{
    while (!m_quit)
    {
        if (getRequestSize()> 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            int still_running = 0;
            // Query multi perform
            CURLMcode curlm_code = CURLM_CALL_MULTI_PERFORM;
            bool failed = false;
            int rc;
            int maxfd = -1;
#if (TARGET_PLATFORM == PLATFORM_LINUX)
            // set a suitable timeout to play around with
            struct timeval select_tv;
            long curl_timeo = -1;
            select_tv.tv_sec = 1;
            select_tv.tv_usec = 0;

            curl_multi_timeout(m_multiHandle, &curl_timeo);
            if (curl_timeo >= 0)
            {
                select_tv.tv_sec = curl_timeo / 1000;
                if (select_tv.tv_sec > 1)
                    select_tv.tv_sec = 1;
                else
                    select_tv.tv_usec = (curl_timeo % 1000) * 1000;
            }
            fd_set fdread;
            fd_set fdwrite;
            fd_set fdexcep;
            FD_ZERO(&fdread);
            FD_ZERO(&fdwrite);
            FD_ZERO(&fdexcep);
            curl_multi_fdset(m_multiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
            rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &select_tv);
#else
            rc = curl_multi_wait(m_multiHandle, nullptr, 0, MAX_WAIT_MSECS, &maxfd);
#endif

            switch (rc)
            {
            case -1:
                failed = true;
                break;
            case 0:
            default:
                curlm_code = CURLM_CALL_MULTI_PERFORM;
                while (CURLM_CALL_MULTI_PERFORM == curlm_code)
                {
                    curlm_code = curl_multi_perform(m_multiHandle, &still_running);
                }
                if (curlm_code != CURLM_OK)
                {
                    std::string errMsg = std::string("Unable to continue the download process: [curl error]") + curl_multi_strerror(curlm_code);
                    //errorCallback(errMsg, curlm_code, "");
                }
                break;
            }


            if (still_running == 0 || still_running != getRequestSize())
            {
                for (;;)
                {
                    int msgsStillInQueue = 0;	// may use that to impose some upper limit we may spend in that loop
                    CURLMsg * curlMsg = curl_multi_info_read(m_multiHandle, &msgsStillInQueue);
                    BREAK_IF(curlMsg == nullptr);
                    // find out which requests have completed
                    if (curlMsg->msg == CURLMSG_DONE)
                    {
                        CURL* completedHandle = curlMsg->easy_handle;
                        curl_multi_remove_handle(m_multiHandle, completedHandle);
                        // set request is completed.
                        requestCompleted(completedHandle);
                    }
                }
            }
        }
    } //while (!m_quit)
}


size_t HTTPDownloader::getRequestSize()
{
    std::lock_guard<std::mutex>  lock(m_requestMutex);
    return m_requestMap.size();
}

void HTTPDownloader::requestCompleted(CURL* curlHandle)
{
    DownloadRequestPointer request;
    {
        std::lock_guard<std::mutex>  lock(m_requestMutex);
        auto requestIter = m_requestMap.find(curlHandle);
        if (requestIter != m_requestMap.end())
        {
            if (requestIter->second->isRequestHeader)
            {
                Response& response = requestIter->second->response;
                char *effectiveUrl;
                char *contentType;
                curl_easy_getinfo(curlHandle, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
                curl_easy_getinfo(curlHandle, CURLINFO_CONTENT_TYPE, &contentType);
                curl_easy_getinfo(curlHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &response.contentSize);
                curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &response.responseCode);
                response.url = effectiveUrl;
                response.contentType = contentType;
                response.valid = true;
                requestIter->second->isRequestHeader = false;
                requestIter->second->requestUrl = response.url;
                requestIter->second->dataWriter->init(response.contentSize);
                request = requestIter->second;
            }
            else
            {
                requestIter->second->dataWriter->flush();
            }
            m_requestMap.erase(requestIter);
        }
    }
    if (request)
    {
        buildRequestForData(request);
        std::lock_guard<std::mutex>  lock(m_requestMutex);
        request->addToMultiResult = curl_multi_add_handle(m_multiHandle, request->easyHandle);
        m_requestMap.insert(std::make_pair(request->easyHandle, request));
    }
}


size_t HTTPDownloader::staticDebugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData)
{
    UNUSED_ARG(userData);
    switch (debugInfoType)
    {
    case CURLINFO_DATA_IN:
        LOG_INFO("DATA_IN %p: Received data [%s] (%d bytes)", handle, debugInfo, debugInfoSize);
        break;
    case CURLINFO_DATA_OUT:
        LOG_INFO("DATA_OUT %p: Sent data [%s] (%d bytes)", handle, debugInfo, debugInfoSize);
        break;
    case CURLINFO_HEADER_IN:
        LOG_INFO("HEADER_IN %p: Received header [%s] (%d bytes)", handle, debugInfo, debugInfoSize);
        break;
    case CURLINFO_HEADER_OUT:
        LOG_INFO("HEADER_OUT %p: Sent header [%s] (%d bytes)", handle, debugInfo, debugInfoSize);
        break;
    case CURLINFO_SSL_DATA_IN:
        LOG_INFO("SSL_DATA_IN %p: Received data [%s] (%d bytes)", handle, debugInfo, debugInfoSize);
        break;
    case CURLINFO_SSL_DATA_OUT:
        LOG_INFO("SSL_DATA_OUT %p: Sent data [%s] (%d bytes)", handle, debugInfo, debugInfoSize);
        break;
    case CURLINFO_TEXT:
    {
        std::string debugText = debugInfo;
        std::replace(debugText.begin(), debugText.end(), '\n', ' ');
        std::replace(debugText.begin(), debugText.end(), '\r', ' ');
        LOG_INFO("TEXT %p:%s", handle, debugText.c_str());
    }
    break;
    default:
        LOG_INFO("%p: DebugCallback: Unknown DebugInfoType=%d debugInfo [%s](DebugInfoSize: %d bytes)", handle, (int32)debugInfoType, debugInfo, debugInfoSize);
        break;
    }
    return 0;
}

size_t HTTPDownloader::staticReceiveBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{
    Request* requestPointer = (Request*)userData;
    return requestPointer->dataWriter->write(buffer, sizeInBlocks*blockSizeInBytes);
}

int HTTPDownloader::staticProgressCallback(void *userData, curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
{
    UNUSED_ARG(totalUpload);
    UNUSED_ARG(nowUpload);
    Request* requestPointer = (Request*)userData;
    if (totalDownload > 0)
    {
        LOG_INFO("===== PROCESS:%0.2f", nowDownload*1.0f / totalDownload*100.0f);
    }
    return CURLE_OK;
}

HTTPDownloader::Request::Request()
    : headerList(nullptr)
    , easyHandle(nullptr)
    , requestUrl()
    , isRequestHeader(false)
    , dataWriter(nullptr)
{
    memset(errorBuffer, 0, CURL_ERROR_SIZE);
}

HTTPDownloader::Request::~Request()
{
    // cleanup the handle first (that order is used in howtos)
    if (easyHandle)
    {
        curl_easy_cleanup(easyHandle);
    }
    // destroy headers list
    if (headerList)
    {
        curl_slist_free_all(headerList);
    }
    SAFE_DELETE(dataWriter);
}


//============== DownloadDataFileWriter====================
DownloadDataFileWriter::DownloadDataFileWriter(const std::string& storagePath, const HTTPDownloader::DownloadToFileCallback& callback)
    : _handle(nullptr)
    , _storagePath(storagePath)
    , _fun(callback)
{
    _handle = fopen(storagePath.c_str(), "wb");
}

void DownloadDataFileWriter::init(size_t dataSize)
{
    UNUSED_ARG(dataSize);
}

size_t DownloadDataFileWriter::write(void* buffer, size_t size)
{
    size_t writeSize = 0;
    if (_handle)
    {
        writeSize = fwrite(buffer, 1, size, _handle);
    }
    return writeSize;
}

void DownloadDataFileWriter::flush()
{
    if (_handle)
    {
        fclose(_handle);
    }
    if (_fun)
    {
        _fun(_storagePath);
    }
}

DownloadDataBufferWriter::DownloadDataBufferWriter(const HTTPDownloader::DownloadToMemoryCallback& callback)
    : _buffer()
    , _writeSize(0)
    , _fun(callback)
{

}

void DownloadDataBufferWriter::init(size_t dataSize)
{
    _buffer.reallocate(dataSize);
}

size_t DownloadDataBufferWriter::write(void* buffer, size_t size)
{
    char* pdst = (char*)_buffer.data();
    pdst += _writeSize;
    memcpy(pdst, buffer, size);
    _writeSize += size;
    return size;
}

void DownloadDataBufferWriter::flush()
{
    if (_fun)
    {
        _fun(_buffer);
    }
}

} //namespace network
NS_FK_END