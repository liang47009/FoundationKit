#include "HTTPClient.hpp"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Platform/FileUtils.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
extern std::string GExternalFilePath;
#endif

NS_FK_BEGIN

namespace
{
    /**
     * A callback that libcurl will use to allocate memory
     *
     * @param Size size of allocation in bytes
     * @return Pointer to memory chunk or NULL if failed
     */
    void* custom_curl_malloc(size_t size)
    {
        return ::malloc(size);
    }

    /**
     * A callback that libcurl will use to free memory
     *
     * @param Ptr pointer to memory chunk (may be NULL)
     */
    void custom_curl_free(void* ptr)
    {
        ::free(ptr);
    }

    /**
     * A callback that libcurl will use to reallocate memory
     *
     * @param Ptr pointer to existing memory chunk (may be NULL)
     * @param Size size of allocation in bytes
     * @return Pointer to memory chunk or NULL if failed
     */
    void* custom_curl_realloc(void* ptr, size_t size)
    {
        return ::realloc(ptr, size);
    }

    /**
     * A callback that libcurl will use to duplicate a string
     *
     * @param ZeroTerminatedString pointer to string (ANSI or UTF-8, but this does not matter in this case)
     * @return Pointer to a copy of string
     */
    char* custom_curl_strdup(const char * zeroTerminatedString)
    {
        char * pCopy = NULL;
        if (zeroTerminatedString)
        {
            size_t strLen = strlen(zeroTerminatedString);
            pCopy = reinterpret_cast<char*>(::malloc(strLen + 1));
            if (pCopy)
            {
                ::strcpy(pCopy, zeroTerminatedString);
                ASSERTED(strcmp(pCopy, zeroTerminatedString)==0,"");
            }
        }
        return pCopy;
    }

    /**
    * A callback that libcurl will use to allocate zero-initialized memory
    *
    * @param NumElems number of elements to allocate (may be 0, then NULL should be returned)
    * @param ElemSize size of each element in bytes (may be 0)
    * @return Pointer to memory chunk, filled with zeroes or NULL if failed
    */
    void* custom_curl_calloc(size_t numElems, size_t elemSize)
    {
        void* ret = nullptr;
        const size_t size = numElems * elemSize;
        if (size)
        {
            ret = ::malloc(size);

            if (ret)
            {
                memset(ret, 0, size);
            }
        }
        return ret;
    }
}



namespace network{


HTTPClient::RequestOptions HTTPClient::HTTPRequestOptions;
CURLM*  HTTPClient::_G_multiHandle = nullptr;
CURLSH* HTTPClient::_G_shareHandle = nullptr;

void HTTPClient::RequestOptions::dumpOptions()
{
    LOG_INFO(" CurlRequestOptions (configurable via config and command line):");
    LOG_INFO(" - bVerifyPeer = %s  - Libcurl will %sverify peer certificate",
        EnableVerifyPeer ? ("true") : ("false"),
        EnableVerifyPeer ? ("") : ("NOT ")
        );

    LOG_INFO(" - CertBundlePath = %s  - Libcurl will %s",
        CertBundlePath.empty() ? "nullptr": CertBundlePath.c_str() ,
        CertBundlePath.empty() ? "use whatever was configured at build time.":"set CURLOPT_CAINFO to it"
        );

    LOG_INFO(" - IsUseHttpProxy = %s  - Libcurl will %suse HTTP proxy",
        IsUseHttpProxy ? ("true") : ("false"),
        IsUseHttpProxy ? ("") : ("NOT ")
        );
    if (IsUseHttpProxy)
    {
        LOG_INFO(" - HttpProxyAddress = '%s'", HttpProxyAddress.c_str());
        LOG_INFO(" - HttpProxyAcount = '%s'", HttpProxyAcount.c_str());
    }

    LOG_INFO(" - IsDontReuseConnections = %s  - Libcurl will %sreuse connections",
        IsDontReuseConnections ? ("true") : ("false"),
        IsDontReuseConnections ? ("NOT ") : ("")
        );


}

void HTTPClient::initialize()
{
    if (_G_multiHandle != NULL)
    {
        LOG_WARN("Already initialized multi handle");
        return;
    }

    CURLcode initResult = curl_global_init_mem(CURL_GLOBAL_ALL, custom_curl_malloc, custom_curl_free, custom_curl_realloc, custom_curl_strdup, custom_curl_calloc);
    if (initResult == 0)
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

        _G_multiHandle = curl_multi_init();
        if (NULL == _G_multiHandle)
        {
            LOG_INFO("Could not initialize create libcurl multi handle! HTTP transfers will not function properly.");
        }

        _G_shareHandle = curl_share_init();
        if (NULL != _G_shareHandle)
        {
            curl_share_setopt(_G_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
            curl_share_setopt(_G_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
            curl_share_setopt(_G_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
        }
        else
        {
            LOG_INFO("Could not initialize libcurl share handle!");
        }
    }
    else
    {
        LOG_INFO("Could not initialize libcurl (result=%d), HTTP transfers will not function properly.", (int32)initResult);
    }

    // discover cert location
#if (TARGET_PLATFORM == PLATFORM_LINUX)	// only relevant to Linux (for now?), not #ifdef'ed to keep the code checked by the compiler when compiling for other platforms
    {
        static const char * knownBundlePaths[] =
        {
            "/etc/pki/tls/certs/ca-bundle.crt",
            "/etc/ssl/certs/ca-certificates.crt",
            "/etc/ssl/ca-bundle.pem",
            nullptr
        };

        for (const char ** currentBundle = knownBundlePaths; *currentBundle; ++currentBundle)
        {
            std::string fileName(*currentBundle);
            LOG_INFO(" Libcurl: checking if '%s' exists", fileName.c_str());

            if (FileUtils::getInstance()->isFileExist(fileName))
            {
                HTTPRequestOptions.CertBundlePath = *currentBundle;
                break;
            }
        }
        if (HTTPRequestOptions.CertBundlePath.empty())
        {
            LOG_INFO(" Libcurl: did not find a cert bundle in any of known locations, TLS may not work");
        }
    }
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    // used #if here to protect against GExternalFilePath only available on Android
    {
        const int32 PathLength = 200;
        static char capath[PathLength] = { 0 };

        // if file does not already exist, create local PEM file with system trusted certificates
        std::string PEMFilename = GExternalFilePath+"/ca-bundle.pem";
        if (!FileUtils::getInstance()->isFileExist(PEMFilename))
        {
            
            // check for override ca-bundle.pem embedded in game content
            std::string OverridePEMFilename =FileUtils::getInstance()->getWritablePath() + "CurlCertificates/ca-bundle.pem";
            std::string Contents = FileUtils::getInstance()->readStringFromFile(OverridePEMFilename);
            if (!Contents.empty())
            {
                FileUtils::getInstance()->writeStringToFile(Contents, PEMFilename, true);
            }
            else
            {
                // gather all the files in system certificates directory
                std::vector<std::string>  files;
                FileUtils::getInstance()->getFilesFromDir("/system/etc/security/cacerts", files);

                for (auto& CertFilename : files)
                {
                    Contents = FileUtils::getInstance()->readStringFromFile(CertFilename);
                    if (!Contents.empty())
                    {
                        FileUtils::getInstance()->writeStringToFile(Contents, PEMFilename, true);
                    }
                }

                // add optional additional certificates
                std::string OptionalPEMFilename = FileUtils::getInstance()->getWritablePath()+("CurlCertificates/ca-additions.pem");
                Contents = FileUtils::getInstance()->readStringFromFile(OptionalPEMFilename);
                if (!Contents.empty())
                {
                    FileUtils::getInstance()->writeStringToFile(Contents, PEMFilename, true);
                }
            }

            HTTPRequestOptions.CertBundlePath = PEMFilename;
            LOG_INFO(" Libcurl: using generated PEM file: '%s'", PEMFilename.c_str());
        }
        else
        {
            HTTPRequestOptions.CertBundlePath = PEMFilename;
            LOG_INFO(" Libcurl: using existing PEM file: '%s'", PEMFilename.c_str());
        }

        if (HTTPRequestOptions.CertBundlePath.empty())
        {
            LOG_INFO(" Libcurl: failed to generate a PEM cert bundle, TLS may not work");
        }
        }
#endif

    // print for visibility
    HTTPRequestOptions.dumpOptions();
}

HTTPClient::HTTPClient()
    : _multiHandle(_G_multiHandle)
{
    ASSERTED(_multiHandle, "First must be call HTTPClient::initialize() to initialize http client.");
}

HTTPClient::~HTTPClient()
{
    _threadRunning = false;
    _threadRequestNotEmpty.notify_all();
    _workThread->join();
    if (NULL != _G_multiHandle)
    {
        curl_multi_cleanup(_G_multiHandle);
        _G_multiHandle = NULL;
    }
    curl_global_cleanup();
}

void HTTPClient::loopInThread()
{
    _threadRunning = true;
    _workThread = std::make_shared<std::thread>([&]()
    {
        while (_threadRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            tick(0.016f);
        }
    });
}

void HTTPClient::tick(float deltaTime)
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (_requests.size() > 0)
    {
        int runningRequests = -1;
        curl_multi_perform(_multiHandle, &runningRequests);
        // read more info if number of requests changed or if there's zero running
        // (note that some requests might have never be "running" from libcurl's point of view)
        if (runningRequests == 0 || runningRequests != static_cast<int>(_requests.size()))
        {
            for (;;)
            {
                int msgsStillInQueue = 0;	// may use that to impose some upper limit we may spend in that loop
                CURLMsg * curlMsg = curl_multi_info_read(_multiHandle, &msgsStillInQueue);
                BREAK_IF(curlMsg == nullptr);

                // find out which requests have completed
                if (curlMsg->msg == CURLMSG_DONE)
                {
                    CURL* completedHandle = curlMsg->easy_handle;
                    curl_multi_remove_handle(_multiHandle, completedHandle);
                    for (auto& requestIter : _requests)
                    {
                        if (requestIter.second->getEasyHandle() == completedHandle)
                        {
                            requestIter.second->setCompleted(curlMsg->data.result);
                            break;
                        }
                    }
                }
            }
        }
    }


    for (auto& requestIter : _requests)
    {
        requestIter.second->tick(deltaTime);
    }

    for (RequestMap::iterator iter = _requests.begin(); iter != _requests.end(); )
    {
        if (iter->second->isFinished())
        {
            iter = _requests.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

}

bool HTTPClient::isTickable() const
{
    return true;
}

void HTTPClient::sendRequestAsync(HTTPRequest::Pointer request)
{
    std::lock_guard<std::mutex> lock(_mutex);
    request->build();
    CURL* requestHandle = request->getEasyHandle();
    CURLMcode addResult = curl_multi_add_handle(_multiHandle, requestHandle);
    request->setAddToMultiResult(addResult);
    if (addResult == CURLM_OK)
    {
        _requests.insert(std::make_pair(requestHandle, request));
        if (_threadRunning)
        {
            _threadRequestNotEmpty.notify_one();
        }
    }
    else
    {
        LOG_ERROR("Failed to add easy handle %p to muti handle with code %d", requestHandle, (int)addResult);
    }
}

void HTTPClient::removeRequest(HTTPRequest::Pointer request)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto requestIter = _requests.find(request->getEasyHandle());
    if (requestIter != _requests.end())
    {
        curl_multi_remove_handle(_multiHandle, request->getEasyHandle());
        _requests.erase(requestIter);
    }
}

} //namespace network
NS_FK_END


