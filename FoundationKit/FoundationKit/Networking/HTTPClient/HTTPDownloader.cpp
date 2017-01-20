#include <algorithm>
#include "HTTPDownloader.hpp"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Networking/libcurl_init.hpp"

static const int MAX_WAIT_MSECS = 30 * 1000; /* Wait max. 30 seconds */

static size_t writeFuncForAdapter(void *ptr, size_t size, size_t nmemb, void* userdata)
{
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

void HTTPDownloader::download(const std::string& url, const std::string& storagePath)
{
    DownloadRequestPointer request = std::make_shared<Request>();
    request->requestUrl = url;
    request->type = DownloadType::T_FILE;
    buildRequestForHeader(request);
    std::lock_guard<std::mutex>  lock(m_requestMutex);
    request->addToMultiResult = curl_multi_add_handle(m_multiHandle, request->easyHandle);
    m_requestMap.insert(std::make_pair(request->easyHandle, request));
}

void HTTPDownloader::download(const std::string& url, mutable_buffer& buffer)
{
    DownloadRequestPointer request = std::make_shared<Request>();
    request->requestUrl = url;
    request->type = DownloadType::T_BUFFER;
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
    curl_easy_setopt(easyHandle, CURLOPT_PRIVATE, pDownloadRequest);

    // set up header function to receive response headers
    //curl_easy_setopt(easyHandle, CURLOPT_HEADERDATA, pDownloadRequest);
    //curl_easy_setopt(easyHandle, CURLOPT_HEADERFUNCTION, staticReceiveHeaderCallback);

    // set up write function to receive response payload
    curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, pDownloadRequest);
    curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, staticReceiveBodyCallback);

    // set progress function
    curl_easy_setopt(easyHandle, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(easyHandle, CURLOPT_XFERINFODATA, pDownloadRequest);
    curl_easy_setopt(easyHandle, CURLOPT_XFERINFOFUNCTION, staticProgressCallback);
    curl_easy_setopt(easyHandle, CURLOPT_URL, pDownloadRequest->requestUrl.c_str());
    curl_easy_setopt(easyHandle, CURLOPT_HTTPGET, 1L);
    pDownloadRequest->easyHandle = easyHandle;
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
    std::lock_guard<std::mutex>  lock(m_requestMutex);
    auto headerIter = m_requestMap.begin();
    while (headerIter != m_requestMap.end())
    {
        if (curlHandle == headerIter->second->easyHandle)
        {
            Response& response = headerIter->second->response;
            char *effectiveUrl;
            char *contentType;
            curl_easy_getinfo(curlHandle, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
            curl_easy_getinfo(curlHandle, CURLINFO_CONTENT_TYPE, &contentType);
            curl_easy_getinfo(curlHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &response.contentSize);
            curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &response.responseCode);
            response.url = effectiveUrl;
            response.contentType = contentType;
            m_requestMap.erase(headerIter);
            break;
        }
        ++headerIter;
    }
}


size_t HTTPDownloader::staticDebugCallback(CURL * handle, curl_infotype debugInfoType, char * debugInfo, size_t debugInfoSize, void* userData)
{
    ASSERTED((handle == _easyHandle), "HTTPRequest::debugCallback The handle is not us's");	// make sure it's us
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

size_t HTTPDownloader::staticReceiveHeaderCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{

}

size_t HTTPDownloader::staticReceiveBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes, void* userData)
{

}

int HTTPDownloader::staticProgressCallback(void *userData, curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
{

}

HTTPDownloader::Request::Request()
    : headerList(nullptr)
    , easyHandle(nullptr)
    , requestUrl()
    , type(HTTPDownloader::DownloadType::T_NONE)
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
}

} //namespace network
NS_FK_END