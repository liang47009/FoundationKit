#include "HTTPClient.hpp"
#include "libcurl_init.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
NS_FK_BEGIN

HTTPClient::RequestOptions HTTPClient::HTTPRequestOptions;
CURLM*  HTTPClient::G_multiHandle = nullptr;
CURLSH* HTTPClient::G_shareHandle = nullptr;

void HTTPClient::RequestOptions::DumpOptions()
{
    FKLog(" CurlRequestOptions (configurable via config and command line):");
    FKLog(" - bVerifyPeer = %s  - Libcurl will %sverify peer certificate",
        EnableVerifyPeer ? ("true") : ("false"),
        EnableVerifyPeer ? ("") : ("NOT ")
        );

    FKLog(" - CertBundlePath = %s  - Libcurl will %s",
        CertBundlePath.empty() ? "nullptr" : CertBundlePath.c_str(),
        CertBundlePath.empty() ? "use whatever was configured at build time." : "set CURLOPT_CAINFO to it"
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

void HTTPClient::Initialize()
{
    if (G_multiHandle != NULL)
    {
        FKLog("Already initialized multi handle");
        return;
    }

    if (libcurl_init::Succeeded())
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

#define PrintCurlFeature(Feature)	                                \
			if (versionInfo->features & Feature)                    \
            {                                                       \
			    FKLog("     %s", #Feature);                        	\
            }

            PrintCurlFeature(CURL_VERSION_SSL);
            PrintCurlFeature(CURL_VERSION_LIBZ);
            PrintCurlFeature(CURL_VERSION_DEBUG);
            PrintCurlFeature(CURL_VERSION_IPV6);
            PrintCurlFeature(CURL_VERSION_ASYNCHDNS);
            PrintCurlFeature(CURL_VERSION_LARGEFILE);
            PrintCurlFeature(CURL_VERSION_IDN);
            PrintCurlFeature(CURL_VERSION_CONV);
            PrintCurlFeature(CURL_VERSION_TLSAUTH_SRP);
#undef PrintCurlFeature
        }

        G_multiHandle = curl_multi_init();
        if (NULL == G_multiHandle)
        {
            ASSERTED(false, "Could not initialize create libcurl multi handle! HTTP transfers will not function properly.");
        }
        MultiHandle = G_multiHandle;
        G_shareHandle = curl_share_init();
        if (NULL != G_shareHandle)
        {
            curl_share_setopt(G_shareHandle, CURLSHOPT_USERDATA, this);
            curl_share_setopt(G_shareHandle, CURLSHOPT_LOCKFUNC, StaticLockCallback);
            curl_share_setopt(G_shareHandle, CURLSHOPT_UNLOCKFUNC, StaticUnlockCallback);
            curl_share_setopt(G_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
            curl_share_setopt(G_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
            curl_share_setopt(G_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
            curl_share_setopt(G_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
        }
        else
        {
            ASSERTED(false, "Could not initialize libcurl share handle!");
        }
    }
    else
    {
        FKLog("Could not initialize libcurl (result=%d), HTTP transfers will not function properly.", (int32)libcurl_init::Code());
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

            if (File::IsExists(fileName))
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
        std::string PEMFilename = GExternalFilePath + "/ca-bundle.pem";
        if (!File::IsExists(PEMFilename))
        {

            // check for override ca-bundle.pem embedded in game content
            std::string OverridePEMFilename = Path::GetDocumentsPath() + "CurlCertificates/ca-bundle.pem";
            std::string Contents = File::ReadAllText(OverridePEMFilename);
            if (!Contents.empty())
            {
                File::WriteAllText(PEMFilename, Contents);
            }
            else
            {
                // gather all the files in system certificates directory
                std::vector<std::string>  files;
                Directory::GetFiles("/system/etc/security/cacerts", files);

                for (auto& CertFilename : files)
                {
                    Contents = File::ReadAllText(CertFilename);
                    if (!Contents.empty())
                    {
                        File::WriteAllText(PEMFilename,Contents);
                    }
                }

                // add optional additional certificates
                std::string OptionalPEMFilename = Path::GetDocumentsPath() + ("CurlCertificates/ca-additions.pem");
                Contents = File::ReadAllText(OptionalPEMFilename);
                if (!Contents.empty())
                {
                    File::WriteAllText(PEMFilename,Contents);
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
    HTTPRequestOptions.DumpOptions();
}

HTTPClient::HTTPClient()
    : MultiHandle(nullptr)
{

}

HTTPClient::~HTTPClient()
{
    if (NULL != G_multiHandle)
    {
        curl_multi_cleanup(G_multiHandle);
        G_multiHandle = NULL;
    }

    if (NULL != G_shareHandle)
    {
        curl_share_cleanup(G_shareHandle);
        G_shareHandle = NULL;
    }
    RequestTempPool.clear();
    RequestPool.clear();
}

void HTTPClient::Tick(float deltaTime)
{
    {
        std::lock_guard<std::mutex> lock(RequestTempMutex);
        std::lock_guard<std::mutex> lockRequest(RequestMutex);
        for (auto& requestPair : RequestTempPool)
        {
            if (requestPair.second == true)
            {
                InternalPostRequest(requestPair.first);
            }
            else
            {
                InternalRemoveRequest(requestPair.first);
            }
        }
        RequestTempPool.clear();
    }

    if (GetRequestPoolSize() > 0)
    {
        int runningRequests = -1;
        curl_multi_perform(MultiHandle, &runningRequests);
        // read more info if number of requests changed or if there's zero running
        // (note that some requests might have never be "running" from libcurl's point of view)
        if (runningRequests == 0 || runningRequests != static_cast<int>(GetRequestPoolSize()))
        {
            for (;;)
            {
                int msgsStillInQueue = 0;	// may use that to impose some upper limit we may spend in that loop
                CURLMsg * curlMsg = curl_multi_info_read(MultiHandle, &msgsStillInQueue);
                BREAK_IF(curlMsg == nullptr);

                // find out which requests have completed
                if (curlMsg->msg == CURLMSG_DONE)
                {
                    std::lock_guard<std::mutex> lock(RequestMutex);
                    CURL* completedHandle = curlMsg->easy_handle;
                    RequestMap::iterator completedIter = RequestPool.find(completedHandle);
                    if (completedIter != RequestPool.end())
                    {
                        completedIter->second->SetCompleted(curlMsg->data.result);
                    }
                    completedIter->second->OnTick(deltaTime);
                    RequestPool.erase(completedIter);
                    curl_multi_remove_handle(MultiHandle, completedHandle);
                }
            }
        }
    }

    std::lock_guard<std::mutex> lock(RequestMutex);
    for (RequestMap::iterator requestIter = RequestPool.begin(); requestIter != RequestPool.end(); ++requestIter)
    {
        // Update other HTTPRequest status.
        requestIter->second->OnTick(deltaTime);
        if (requestIter->second->IsFinished())
        {
            // Remove finished HTTPRequest.
            RemoveRequest(requestIter->second);
        }
    }
}


void HTTPClient::PostRequest(HTTPRequest::Pointer request)
{
    std::lock_guard<std::mutex> lock(RequestTempMutex);
    RequestTempPool[request] = true;
}

void FoundationKit::HTTPClient::RemoveRequest(HTTPRequest::Pointer request)
{
    std::lock_guard<std::mutex> lock(RequestTempMutex);
    RequestTempPool[request] = false;
}

HTTPResponse::Pointer HTTPClient::SendRequest(HTTPRequest::Pointer request)
{
    request->Build();
    request->SetCompleted(curl_easy_perform(request->GetEasyHandle()));
    request->FinishedRequest();
    return request->GetResponse();
}


void HTTPClient::InternalPostRequest(HTTPRequest::Pointer request)
{
    request->Build();
    CURL* requestHandle = request->GetEasyHandle();
    RequestMap::iterator requestIter = RequestPool.find(requestHandle);
    if (requestIter == RequestPool.end())
    {
        CURLMcode addResult = curl_multi_add_handle(MultiHandle, requestHandle);
        request->SetAddToMultiResult(addResult);
        if (addResult == CURLM_OK)
        {
            RequestPool.insert(std::make_pair(requestHandle, request));
        }
        else
        {
            FKLog("Failed to add easy handle %p to muti handle with code %d", requestHandle, (int)addResult);
        }
    }
}

void FoundationKit::HTTPClient::InternalRemoveRequest(HTTPRequest::Pointer request)
{
    CURL* requestHandle = request->GetEasyHandle();
    RequestMap::iterator requestIter = RequestPool.find(requestHandle);
    if (requestIter != RequestPool.end())
    {
        RequestPool.erase(requestIter);
        curl_multi_remove_handle(MultiHandle, requestHandle);
    }
}

size_t FoundationKit::HTTPClient::GetRequestPoolSize()
{
    std::lock_guard<std::mutex> lock(RequestMutex);
    return RequestPool.size();
}

void HTTPClient::LazyCreatePerformThread()
{
    static bool bIsCreatePerformThread = false;
    if (!bIsCreatePerformThread)
    {
        ThreadOfPerform = std::thread([&]()
        {
            ElapsedTimer runningTime;
            while (true)
            {
                std::unique_lock<std::mutex> lockRequests(HandleRequestMutex);
                HandleRequestCond.wait(lockRequests, [&](){return !HandlesToRequests.empty(); });
                int runningRequests = 0;
                curl_multi_perform(MultiHandle, &runningRequests);
                // read more info if number of requests changed or if there's zero running
                // (note that some requests might have never be "running" from libcurl's point of view)
                if (runningRequests == 0 || runningRequests != static_cast<int>(HandlesToRequests.size()))
                {
                    for (;;)
                    {
                        int msgsStillInQueue = 0;	// may use that to impose some upper limit we may spend in that loop
                        CURLMsg * curlMsg = curl_multi_info_read(MultiHandle, &msgsStillInQueue);
                        BREAK_IF(curlMsg == nullptr);

                        // find out which requests have completed
                        if (curlMsg->msg == CURLMSG_DONE)
                        {
                            std::lock_guard<std::mutex> lock(RequestMutex);
                            CURL* completedHandle = curlMsg->easy_handle;
                            RequestMap::iterator completedIter = HandlesToRequests.find(completedHandle);
                            if (completedIter != HandlesToRequests.end())
                            {
                                completedIter->second->SetCompleted(curlMsg->data.result);
                            }
                            completedIter->second->OnTick(runningTime.Secondsf());
                            AddRequestToFinished(completedIter->second);
                            HandlesToRequests.erase(completedIter);
                            curl_multi_remove_handle(MultiHandle, completedHandle);
                        }
                    }
                }
            }
        
        });
        bIsCreatePerformThread = true;
    }
}

void HTTPClient::AddRequestToFinished(HTTPRequest::Pointer request)
{
    std::lock_guard<std::mutex> lockRequests(FinishedRequestsMutex);
    FinishedRequests.push_back(request);
}

void HTTPClient::StaticLockCallback(CURL *handle, curl_lock_data data, curl_lock_access access, void*userData)
{
    static_cast<HTTPClient*>(userData)->LockCallback(handle, data, access);
}

void HTTPClient::StaticUnlockCallback(CURL *handle, curl_lock_data data, void *userData)
{
    static_cast<HTTPClient*>(userData)->UnlockCallback(handle, data);
}

void HTTPClient::LockCallback(CURL *handle, curl_lock_data data, curl_lock_access access)
{
    switch (data)
    {
    case CURL_LOCK_DATA_SHARE:
        CURLDataShareMutex.lock();
        break;
    case CURL_LOCK_DATA_COOKIE:
        CURLDataCookieMutex.lock();
        break;
    case CURL_LOCK_DATA_DNS:
        CURLDataDNSMutex.lock();
        break;
    case CURL_LOCK_DATA_SSL_SESSION:
        CURLDataSSLSessionMutex.lock();
        break;
    case CURL_LOCK_DATA_CONNECT:
        CURLDataConnectMutex.lock();
        break;
    default:
        FKLog("Unknown data to lock:%d",data);
        break;
    }
}

void HTTPClient::UnlockCallback(CURL *handle, curl_lock_data data)
{
    switch (data)
    {
    case CURL_LOCK_DATA_SHARE:
        CURLDataShareMutex.unlock();
        break;
    case CURL_LOCK_DATA_COOKIE:
        CURLDataCookieMutex.unlock();
        break;
    case CURL_LOCK_DATA_DNS:
        CURLDataDNSMutex.unlock();
        break;
    case CURL_LOCK_DATA_SSL_SESSION:
        CURLDataSSLSessionMutex.unlock();
        break;
    case CURL_LOCK_DATA_CONNECT:
        CURLDataConnectMutex.unlock();
        break;
    default:
        FKLog("Unknown data to lock:%d", data);
        break;
    }
}

NS_FK_END