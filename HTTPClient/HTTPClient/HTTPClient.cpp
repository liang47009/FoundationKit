#include "HTTPClient.hpp"
#include "FoundationKit/Platform/FileUtils.hpp"
#include "libcurl_init.hpp"

#if TARGET_PLATFORM == PLATFORM_ANDROID
// in AndroidJNI::initializeJavaEnv
extern std::string GExternalFilePath;
#endif

NS_FK_BEGIN

namespace network{

HTTPClient::RequestOptions HTTPClient::HTTPRequestOptions;
CURLM*  HTTPClient::_G_multiHandle = nullptr;
CURLSH* HTTPClient::_G_shareHandle = nullptr;

void HTTPClient::RequestOptions::dumpOptions()
{
    FKLog(" CurlRequestOptions (configurable via config and command line):");
    FKLog(" - bVerifyPeer = %s  - Libcurl will %sverify peer certificate",
        EnableVerifyPeer ? ("true") : ("false"),
        EnableVerifyPeer ? ("") : ("NOT ")
        );

    FKLog(" - CertBundlePath = %s  - Libcurl will %s",
        CertBundlePath.empty() ? "nullptr": CertBundlePath.c_str() ,
        CertBundlePath.empty() ? "use whatever was configured at build time.":"set CURLOPT_CAINFO to it"
        );

    FKLog(" - IsUseHttpProxy = %s  - Libcurl will %suse HTTP proxy",
        IsUseHttpProxy ? ("true") : ("false"),
        IsUseHttpProxy ? ("") : ("NOT ")
        );
    if (IsUseHttpProxy)
    {
        FKLog(" - HttpProxyAddress = '%s'", HttpProxyAddress.c_str());
        FKLog(" - HttpProxyAcount = '%s'", HttpProxyAcount.c_str());
    }

    FKLog(" - IsDontReuseConnections = %s  - Libcurl will %sreuse connections",
        IsDontReuseConnections ? ("true") : ("false"),
        IsDontReuseConnections ? ("NOT ") : ("")
        );


}

void HTTPClient::initialize()
{
    if (_G_multiHandle != NULL)
    {
        FKLog("Already initialized multi handle");
        return;
    }

    if (libcurl_init::succeeded())
    {
        curl_version_info_data * versionInfo = curl_version_info(CURLVERSION_NOW);
        if (versionInfo)
        {
            FKLog("Using libcurl %s", versionInfo->version);
            FKLog(" - built for %s", versionInfo->host);

            if (versionInfo->features & CURL_VERSION_SSL)
            {
                FKLog(" - supports SSL with %s", versionInfo->ssl_version);
            }
            else
            {
                // No SSL
                FKLog(" - NO SSL SUPPORT!");
            }

            if (versionInfo->features & CURL_VERSION_LIBZ)
            {
                FKLog(" - supports HTTP deflate (compression) using libz %s", versionInfo->libz_version);
            }

            FKLog(" - other features:");

#define printCurlFeature(Feature)	             \
			if (versionInfo->features & Feature) \
            {                                    \
			    FKLog("     %s", #Feature);	 \
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
            FKLog("Could not initialize create libcurl multi handle! HTTP transfers will not function properly.");
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
            FKLog("Could not initialize libcurl share handle!");
        }
    }
    else
    {
        FKLog("Could not initialize libcurl (result=%d), HTTP transfers will not function properly.", (int32)libcurl_init::code());
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
            FKLog(" Libcurl: checking if '%s' exists", fileName.c_str());

            if (FileUtils::getInstance()->isFileExist(fileName))
            {
                HTTPRequestOptions.CertBundlePath = *currentBundle;
                break;
            }
        }
        if (HTTPRequestOptions.CertBundlePath.empty())
        {
            FKLog(" Libcurl: did not find a cert bundle in any of known locations, TLS may not work");
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
            FKLog(" Libcurl: using generated PEM file: '%s'", PEMFilename.c_str());
        }
        else
        {
            HTTPRequestOptions.CertBundlePath = PEMFilename;
            FKLog(" Libcurl: using existing PEM file: '%s'", PEMFilename.c_str());
        }

        if (HTTPRequestOptions.CertBundlePath.empty())
        {
            FKLog(" Libcurl: failed to generate a PEM cert bundle, TLS may not work");
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
    if (NULL != _G_multiHandle)
    {
        curl_multi_cleanup(_G_multiHandle);
        _G_multiHandle = NULL;
    }

    if (NULL != _G_shareHandle)
    {
        curl_share_cleanup(_G_shareHandle);
        _G_shareHandle = NULL;
    }
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

    // update HTTPRequest status.
    for (auto& requestIter : _requests)
    {
        requestIter.second->tick(deltaTime);
    }

    // Remove finished HTTPRequest.
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
    }
    else
    {
        FKLog("Failed to add easy handle %p to muti handle with code %d", requestHandle, (int)addResult);
    }
}

//void HTTPClient::removeRequest(HTTPRequest::Pointer request)
//{
//    std::lock_guard<std::mutex> lock(_mutex);
//    auto requestIter = _requests.find(request->getEasyHandle());
//    if (requestIter != _requests.end())
//    {
//        curl_multi_remove_handle(_multiHandle, request->getEasyHandle());
//        _requests.erase(requestIter);
//    }
//}

} //namespace network
NS_FK_END


