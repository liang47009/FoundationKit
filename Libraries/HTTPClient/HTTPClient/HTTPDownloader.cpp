#include "FoundationKit/GenericPlatformMacros.hpp"
#include "HTTPDownloader.hpp"
#include "HTTPResponse.hpp"
#include "HTTPClient.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
NS_FK_BEGIN

HTTPDownloadRequest::HTTPDownloadRequest(bool enableDebug /*= false*/)
: HTTPRequest(enableDebug)
, DownladedSize(0)
, EnableRange(false)
, Offset(0)
, Size(0)
, WriteOffset(0)
{

}

HTTPDownloadRequest::~HTTPDownloadRequest()
{

}

HTTPDownloadRequest::Pointer HTTPDownloadRequest::Create(bool enableDebug /*= false*/)
{
    HTTPDownloadRequest::Pointer shared_httprequest = std::make_shared<HTTPDownloadRequest>(enableDebug);
    shared_httprequest->SetMethod(RequestMethodType::GET);
    return shared_httprequest;
}

HTTPDownloadRequest& HTTPDownloadRequest::SetFileHandle(FILE* pFile)
{
    FileHandle = pFile;
    return (*this);
}

HTTPDownloadRequest& HTTPDownloadRequest::SetEnableRange(bool enableRange)
{
    EnableRange = enableRange;
    return (*this);
}

FILE* HTTPDownloadRequest::GetFileHandle()
{
    return FileHandle;
}

int64 HTTPDownloadRequest::GetDownladedSize()
{
    return DownladedSize;
}

bool HTTPDownloadRequest::Build()
{
    bool result = HTTPRequest::Build();
    if (EnableRange && Size != 0)
    {
        WriteOffset = Offset;
        char range[64] = { 0 };
        int errcode = sprintf_s(range, 64, "%lld-%lld", Offset, Offset + Size);
        curl_easy_setopt(EasyHandle, CURLOPT_RANGE, range);
    }
    return result;
}

bool HTTPDownloadRequest::OnFinished()
{
    return false;
}

size_t HTTPDownloadRequest::ReceiveResponseBodyCallback(void* buffer, size_t size, size_t nmemb)
{
    size_t writeSize = 0;
    if (FileHandle)
    {
        if (fseek(FileHandle, static_cast<long>(WriteOffset), SEEK_SET) == 0)
        {
            writeSize = fwrite(buffer, size, nmemb, FileHandle);
        }
        else
        {
            Cancel();
            std::string strMessage = "Write buffer to file error.";
            memcpy(ErrorBuffer, strMessage.c_str(), strMessage.size());
            return 0;
        }
    }
    WriteOffset += writeSize;
    DownladedSize += writeSize;
    return writeSize;
}

// ==================================================================================
// ============================= For experimental ===================================
// ==================================================================================

//=========================== HTTPDownloader ======================
HTTPDownloader::HTTPDownloader()
{

}

HTTPDownloader::~HTTPDownloader()
{

}

static ElapsedTimer  ElapsedTimerTick;
static int64 PreDeownloadSize = 0;
void HTTPDownloader::Tick(float deltaTime)
{
    for (auto& DownloaderEntryPair : DownloaderEntryPool)
    {
        int64 tempDownloadSize = 0;
        size_t FinishedCount = 0;
        for (auto downloadRequest : DownloaderEntryPair.second->DownloadRequests)
        {
            tempDownloadSize += downloadRequest->GetDownladedSize();
            if (downloadRequest->IsFinished())
                ++FinishedCount;
        }
        if (DownloaderEntryPair.second->CallbackListener.OnProgressCallback)
        {
            DownloaderEntryPair.second->CallbackListener.OnProgressCallback(DownloaderEntryPair.second->URL, tempDownloadSize, DownloaderEntryPair.second->ContentSize);
        }
        if (FinishedCount!=0&&FinishedCount == DownloaderEntryPair.second->DownloadRequests.size())
        {
            fclose(DownloaderEntryPair.second->FileHandle);
        }

        if (ElapsedTimerTick.Seconds() >= 1)
        {

            FKLog("======== %s, download: %lld/%lld, Speed:%lld KB, Proc:%0.2f %%", 
                DownloaderEntryPair.second->URL.c_str(), 
                tempDownloadSize, 
                DownloaderEntryPair.second->ContentSize,
                (tempDownloadSize - PreDeownloadSize)/1024,
                tempDownloadSize*1.0f / DownloaderEntryPair.second->ContentSize *100);
            ElapsedTimerTick.Reset();
            PreDeownloadSize = tempDownloadSize;
        }
    }
}

const size_t CHUNK_LENGTH_LIMIT = 1024 * 1024 * 5; //5M
const size_t CHUNK_NUMBER_LIMIT = 20;
void HTTPDownloader::DownloadToFile(const std::string& url, const std::string& storagePath, const DownloadListener& listener)
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

    HTTPRequest::Pointer  HeadRequest = HTTPRequest::Create();
    HeadRequest->SetMethod(RequestMethodType::HEAD);
    HeadRequest->SetURL(url);
    FILE* fp = nullptr;
    int errcode = fopen_s(&fp, checkedStoragePath.c_str(), "wb");
    DownloaderEntryPointer  downlaoderEntry = std::make_shared<DownloaderEntry>();
    downlaoderEntry->CallbackListener = listener;
    downlaoderEntry->URL = url;
    downlaoderEntry->FilePath = checkedStoragePath;
    downlaoderEntry->FileHandle = fp;
    DownloaderEntryPool.insert(std::make_pair(url, downlaoderEntry));
    HeadRequest->OnRequestCompleted = [=](HTTPRequest::Pointer request, HTTPResponse::Pointer response)
    {
        if (response->IsSucceeded())
        {
            size_t ContentSize = response->GetContentSize();
            downlaoderEntry->ContentSize = ContentSize;
            std::string strEffectiveUrl = response->GetEffectiveURL();
            if (strEffectiveUrl.empty())
            {
                strEffectiveUrl = url;
            }

            std::string AcceptRanges = response->GetHeader("Accept-Ranges");
            if (AcceptRanges.empty() || AcceptRanges == "none")
            {
                HTTPDownloadRequest::Pointer ContentRequest = HTTPDownloadRequest::Create();
                ContentRequest->SetURL(strEffectiveUrl);
                ContentRequest->SetFileHandle(fp);
                ContentRequest->SetEnableRange(false);
                downlaoderEntry->DownloadRequests.push_back(ContentRequest);
                HTTPClient::GetInstance()->PostRequest(ContentRequest);
            }
            else
            {
                size_t rangeNum = 0;
                size_t chunkSize = CHUNK_LENGTH_LIMIT;
                if (ContentSize <= (CHUNK_LENGTH_LIMIT*CHUNK_NUMBER_LIMIT))
                {
                    rangeNum = ContentSize / CHUNK_LENGTH_LIMIT;
                }
                else
                {
                    rangeNum = CHUNK_NUMBER_LIMIT;
                    chunkSize = ContentSize / CHUNK_NUMBER_LIMIT;
                }

                for (size_t i = 0; i <= rangeNum; i++)
                {
                    HTTPDownloadRequest::Pointer ContentRequest = HTTPDownloadRequest::Create();
                    ContentRequest->SetEnableRange(true);
                    ContentRequest->SetURL(strEffectiveUrl);
                    if (i < rangeNum)
                    {
                        ContentRequest->Offset = i * chunkSize;
                        ContentRequest->Size = chunkSize - 1;
                    }
                    else
                    {
                        if ((ContentSize % chunkSize) != 0)
                        {
                            ContentRequest->Offset = i * chunkSize;
                            ContentRequest->Size = ContentSize - ContentRequest->Offset - 1;
                        }
                    }
                    ContentRequest->SetFileHandle(fp);
                    downlaoderEntry->DownloadRequests.push_back(ContentRequest);
                    HTTPClient::GetInstance()->PostRequest(ContentRequest);
                }
            }
        }
        else
        {
            if (listener.OnErrorCallback)
            {
                std::string strError = response->GetErrorMsg();
                strError += "(";
                strError += response->GetResponseCode().ToString();
                strError += ")";
                listener.OnErrorCallback(url, strError);
            }
        }
    };
    HTTPClient::GetInstance()->PostRequest(HeadRequest);
}


void HTTPDownloader::DownloadToMemory(const std::string& url, mutable_buffer memoryBuffer, const DownloadListener& listener)
{

}

NS_FK_END








