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
    : m_multiHandle(nullptr)
    , m_shareHandle(nullptr)
    , m_bDebug(false)
    , m_proxyaddr()
    , m_proxyUserPwd()
    , m_bEnableVerifyPeer(false)
    , m_bReuseConnections(false)
    , m_certBundlePath()
    , m_requestThread(nullptr)
    , m_headerRequests()
    , m_headerRequestMutex()
    , m_contentRequests()
    , m_contentRequestMutex()
{
    m_quit = true;
}

HTTPDownloader::~HTTPDownloader()
{
    m_quit = true;
    m_requestThread->join();
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

    for (auto downloadNode : m_httpDownloadNodes)
    {
        delete downloadNode.second;
    }
    m_httpDownloadNodes.clear();
}

bool HTTPDownloader::initialize(bool bDebug)
{
    if (m_multiHandle != NULL)
    {
        LOG_WARN("Already initialized multi handle");
        return false;
    }
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
        m_shareHandle = curl_share_init();
        if (m_multiHandle == nullptr || m_shareHandle == nullptr)
        {
            if (!m_multiHandle)
                LOG_ERROR("Could not initialize create libcurl multi handle! HTTP transfers will not function properly.");

            if (!m_shareHandle)
                LOG_ERROR("Could not initialize libcurl share handle!");
            return false;
        }
        curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_TOTAL_CONNECTIONS, 100);
        curl_multi_setopt(m_multiHandle, CURLMOPT_MAXCONNECTS, 100);
        curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_HOST_CONNECTIONS, 50);
        curl_multi_setopt(m_multiHandle, CURLMOPT_PIPELINING, 1);
        curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_PIPELINE_LENGTH, 50);
        
        
        
        curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
        curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
    }
    else
    {
        LOG_ERROR("Could not initialize libcurl (result=%d), HTTP transfers will not function properly.", (int32)libcurl_init::code());
        return false;
    }
    m_quit = false;
    auto threadFun = std::bind(&HTTPDownloader::performRequest, this);
    m_requestThread = std::make_shared<std::thread>(threadFun);
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

void HTTPDownloader::downloadToFile(const std::string& url, const std::string& storagePath, const DownloadListener& listener)
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
    std::lock_guard<std::mutex> downlaodNodeLock(m_httpDownloadNodeMutex);
    HTTPDownloadNode* pHTTPDownloadNode = new HTTPDownloadNode(url, checkedStoragePath, listener, this);
    m_httpDownloadNodes.insert(std::make_pair(url, pHTTPDownloadNode));

    std::lock_guard<std::mutex> headerLock(m_headerRequestMutex);
    HTTPDownloadNode::HeaderRequestPointer headerRequest = std::make_shared<HeaderRequest>();
    headerRequest->requestUrl = url;
    headerRequest->pHTTPDownloadNode = pHTTPDownloadNode;
    buildRequestForHeader(headerRequest);
    headerRequest->addToMultiResult = curl_multi_add_handle(m_multiHandle, headerRequest->curlHandle);
    m_headerRequests.insert(std::make_pair(headerRequest->curlHandle, headerRequest));

}

bool HTTPDownloader::buildRequestForHeader(const HTTPDownloadNode::HeaderRequestPointer& headerRequest)
{
    void *easyHandle = curl_easy_init();
    headerRequest->curlHandle = easyHandle;
    buildRequestGeneric(headerRequest.get());
    curl_easy_setopt(easyHandle, CURLOPT_URL, headerRequest->requestUrl.c_str());
    curl_easy_setopt(easyHandle, CURLOPT_HEADER, 1);
    curl_easy_setopt(easyHandle, CURLOPT_NOBODY, 1);
    // in win32 platform, if not set the writeFunction, it will return CURLE_WRITE_ERROR
    curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, writeFuncForAdapter);
    return true;
}

bool HTTPDownloader::buildRequestForData(const HTTPDownloadNode::ContentRequestPointer& contentRequest)
{
    CURL* easyHandle = curl_easy_init();
    contentRequest->curlHandle = easyHandle;
    buildRequestGeneric(contentRequest.get());

    // associate with this just in case
    curl_easy_setopt(easyHandle, CURLOPT_PRIVATE, contentRequest.get());

    // set up write function to receive response payload
    curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, contentRequest.get());
    curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, staticReceiveBodyCallback);

    // set progress function
    curl_easy_setopt(easyHandle, CURLOPT_NOPROGRESS, 1);
    //curl_easy_setopt(easyHandle, CURLOPT_XFERINFODATA, contentRequest.get());
    //curl_easy_setopt(easyHandle, CURLOPT_XFERINFOFUNCTION, staticProgressCallback);
    curl_easy_setopt(easyHandle, CURLOPT_URL, contentRequest->requestUrl.c_str());
    curl_easy_setopt(easyHandle, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(easyHandle, CURLOPT_CONNECTTIMEOUT, 60);
    return true;
}

bool HTTPDownloader::buildRequestGeneric(RequestBase* request)
{
    CURL* curlHandle = request->curlHandle;
    curl_easy_setopt(curlHandle, CURLOPT_SHARE, m_shareHandle);

    curl_easy_setopt(curlHandle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);

    // allow http redirects to be followed
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);

    // required for all multi-threaded handles
    curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(curlHandle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curlHandle, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curlHandle, CURLOPT_AUTOREFERER, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_LIMIT, 1); //限制最低速度1个字节
    curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_TIME, 30);  //如果5秒内没有字节读取，则超时
    curl_easy_setopt(curlHandle, CURLOPT_MAX_SEND_SPEED_LARGE, 1024 * 1024*10); // limit send speed 10MB/s
    curl_easy_setopt(curlHandle, CURLOPT_MAX_RECV_SPEED_LARGE, 1024 * 1024*10); // limit recv speed 10MB/s

    // receive error messages
    curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, request->errorBuffer);

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
    curl_easy_setopt(curlHandle, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_SESSIONID_CACHE, 1L);
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
    request->headerList = headerList;
    return true;
}

void HTTPDownloader::performRequest()
{
    while (!m_quit)
    {
        if (m_quit)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (getRequestSize()> 0)
        {
            int runningRequests = 0;
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
                    curlm_code = curl_multi_perform(m_multiHandle, &runningRequests);
                }
                if (curlm_code != CURLM_OK)
                {
                    std::string errMsg = std::string("Unable to continue the download process: [curl error]") + curl_multi_strerror(curlm_code);
                    //errorCallback(errMsg, curlm_code, "");
                }
                break;
            }

            if (runningRequests == 0 || runningRequests != static_cast<int>(getRequestSize()))
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
                        if (!(requestHeaderCompleted(completedHandle, curlMsg->data.result)))
                        {
                            requestContentCompleted(completedHandle, curlMsg->data.result);
                        }
                    }
                }
            }
        }
    } //while (!m_quit)
}


size_t HTTPDownloader::getRequestSize()
{
    std::lock_guard<std::mutex>  headerLock(m_headerRequestMutex);
    std::lock_guard<std::mutex>  contentLock(m_contentRequestMutex);
    return m_headerRequests.size() + m_contentRequests.size();
}

bool HTTPDownloader::requestHeaderCompleted(CURL* curlHandle, CURLcode completionResult)
{
    bool bFound = false;
    std::lock_guard<std::mutex>  lock(m_headerRequestMutex);
    auto requestIter = m_headerRequests.find(curlHandle);
    if (requestIter != m_headerRequests.end())
    {
        bFound = true;
        HTTPDownloadNode::HeaderRequestPointer headerRequest = requestIter->second;
        headerRequest->curlCode = completionResult;
        headerRequest->curlMessage = "";
        if (headerRequest->pHTTPDownloadNode->headerRequestCompleted(headerRequest) == false)
        {
            std::lock_guard<std::mutex> downlaodNodeLock(m_httpDownloadNodeMutex);
            auto downloadNodeIter = m_httpDownloadNodes.find(headerRequest->requestUrl);
            if (downloadNodeIter != m_httpDownloadNodes.end())
            {
                delete downloadNodeIter->second;
                m_httpDownloadNodes.erase(downloadNodeIter);
            }
        }
        m_headerRequests.erase(requestIter);
    }
    return bFound;
}


bool HTTPDownloader::requestContentCompleted(CURL* curlHandle, CURLcode completionResult)
{
    bool bFound = false;
    std::unique_lock<std::mutex>  lock(m_contentRequestMutex);
    auto requestIter = m_contentRequests.find(curlHandle);
    if (requestIter != m_contentRequests.end())
    {
        bFound = true;
        HTTPDownloadNode::ContentRequestPointer contentRequest = requestIter->second;
        m_contentRequests.erase(requestIter);
        lock.unlock();

        contentRequest->curlCode = completionResult;
        contentRequest->curlMessage = "";
        curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &contentRequest->responseCode);

        if (contentRequest->pHTTPDownloadNode->contentRequestCompleted(contentRequest))
        {
            std::lock_guard<std::mutex> downlaodNodeLock(m_httpDownloadNodeMutex);
            for (auto downloadNodeIter = m_httpDownloadNodes.begin(); downloadNodeIter != m_httpDownloadNodes.end(); ++downloadNodeIter)
            {
                if (downloadNodeIter->second->getRequestUrl() == contentRequest->requestUrl)
                {
                    delete downloadNodeIter->second;
                    m_httpDownloadNodes.erase(downloadNodeIter);
                    break;
                }
            }
        }
    }
    return bFound;
}


bool HTTPDownloader::addContentRequest(HTTPDownloadNode::ContentRequestPointer contentRequest)
{
    std::lock_guard<std::mutex>  lock(m_contentRequestMutex);
    buildRequestForData(contentRequest);
    char range[64] = { 0 };
    sprintf(range, "%ld-%ld", contentRequest->rangeFrom, contentRequest->rangeTo);
    LOG_INFO("========= curl:%p, range[%s]", contentRequest->curlHandle, range);
    curl_easy_setopt(contentRequest->curlHandle, CURLOPT_RANGE, range);
    contentRequest->addToMultiResult = curl_multi_add_handle(m_multiHandle, contentRequest->curlHandle);
    if (contentRequest->addToMultiResult == CURLM_OK)
    {
        m_contentRequests.insert(std::make_pair(contentRequest->curlHandle, contentRequest));
        return true;
    }
    return false;
}

void HTTPDownloader::removeContentRequest(HTTPDownloadNode::ContentRequestPointer contentRequest)
{
    std::lock_guard<std::mutex>  lock(m_contentRequestMutex);
    auto requestIter = m_contentRequests.find(contentRequest->curlHandle);
    if (requestIter != m_contentRequests.end())
    {
        curl_multi_remove_handle(m_multiHandle, requestIter->second->curlHandle);
        m_contentRequests.erase(requestIter);
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
    ContentRequest* contentRequest = (ContentRequest*)userData;
    return contentRequest->pHTTPDownloadNode->writeData(contentRequest, buffer, sizeInBlocks*blockSizeInBytes);
}

//int HTTPDownloader::staticProgressCallback(void *userData, curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
//{
//    return CURLE_OK;
//}



} //namespace network
NS_FK_END