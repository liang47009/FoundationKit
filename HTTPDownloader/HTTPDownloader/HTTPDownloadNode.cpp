#include "HTTPDownloadNode.hpp"
#include "HTTPDownloader.hpp"
NS_FK_BEGIN
namespace network
{


RequestBase::RequestBase()
    : curlHandle(nullptr)
    , curlCode(CURLE_OK)
    , curlMessage()
    , headerList(NULL)
    , addToMultiResult(CURLM_OK)
    , responseCode(0)
    , requestUrl()
{
    memset(errorBuffer, 0, CURL_ERROR_SIZE);
}

RequestBase::~RequestBase()
{
    // cleanup the handle first (that order is used in howtos)
    if (curlHandle)
    {
        curl_easy_cleanup(curlHandle);
    }
    // destroy headers list
    if (headerList)
    {
        curl_slist_free_all(headerList);
    }
}

HeaderRequest::HeaderRequest()
    : pHTTPDownloadNode(nullptr)
{

}

HeaderRequest::~HeaderRequest()
{

}

ContentRequest::ContentRequest()
    : rangeFrom(0)
    , rangeTo(0)
    , retryCount(50)
    , pHTTPDownloadNode(nullptr)
{

}

ContentRequest::~ContentRequest()
{

}

DownloadListener::DownloadListener()
    : onErrorCallback(nullptr)
    , onProgressCallback(nullptr)
    , onSucceedCallback(nullptr)
{

}

DownloadListener::~DownloadListener()
{

}

HTTPDownloadNode::HTTPDownloadNode(const std::string& requestUrl
    , const std::string& fileFullPath
    , const DownloadListener& listener
    , HTTPDownloader* pHTTPDownloader)
    : _requestUrl(requestUrl)
    , _fileFullPath(fileFullPath)
    , _contentType()
    , _contentSize(0)
    , _contentRequests()
    , _contentRequestMutex()
    , _downloadListener(listener)
    , _fileHandle(nullptr)
    , _pHTTPDownloader(pHTTPDownloader)
    , _nowDownlaod(0)
{

}

HTTPDownloadNode::~HTTPDownloadNode()
{
    if (_fileHandle)
    {
        fclose(_fileHandle);
    }
    _fileHandle = nullptr;
}

void HTTPDownloadNode::pause()
{
    for (auto requestIter : _contentRequests)
    {
        CURLcode ret = curl_easy_pause(requestIter.first, CURLPAUSE_ALL);
        if (ret != CURLE_OK)
        {
            reportError(requestIter.second.get());
            break;
        }
    }
}

void HTTPDownloadNode::resume()
{
    for (auto requestIter : _contentRequests)
    {
        CURLcode ret = curl_easy_pause(requestIter.first, CURLPAUSE_CONT);
        if (ret != CURLE_OK)
        {
            reportError(requestIter.second.get());
            break;
        }
    }
}

bool HTTPDownloadNode::headerRequestCompleted(HeaderRequestPointer headerRequest)
{
    bool bSuccess = false;
    if (headerRequest->curlCode == CURLE_OK)
    {
        _fileHandle = fopen(_fileFullPath.c_str(), "wb");
        CURL* curlHandle = headerRequest->curlHandle;
        char *effectiveUrl;
        char *contentType;
        double contentSize;
        curl_easy_getinfo(curlHandle, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
        curl_easy_getinfo(curlHandle, CURLINFO_CONTENT_TYPE, &contentType);
        curl_easy_getinfo(curlHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentSize);
        curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &headerRequest->responseCode);
        _contentType = contentType;
        _effectiveUrl = effectiveUrl;
        _contentSize = static_cast<size_t>(contentSize);

        size_t rangeNum   = 0;
        size_t chunkSize  = CHUNK_LENGTH_LIMIT;
        if (_contentSize <= (CHUNK_LENGTH_LIMIT*CHUNK_NUMBER_LIMIT))
        {
            rangeNum  = _contentSize / CHUNK_LENGTH_LIMIT;
        }
        else
        {
            rangeNum  = CHUNK_NUMBER_LIMIT;
            chunkSize = _contentSize / CHUNK_NUMBER_LIMIT;
        }

        for (size_t i = 0; i <= rangeNum; i++)
        {
            HTTPDownloadNode::ContentRequestPointer contentRequest = std::make_shared<ContentRequest>();
            contentRequest->requestUrl = _effectiveUrl;
            contentRequest->pHTTPDownloadNode = headerRequest->pHTTPDownloadNode;
            if (i < rangeNum)
            {
                contentRequest->rangeFrom = i * chunkSize;
                contentRequest->rangeTo = (i + 1) * chunkSize - 1;
            }
            else
            {
                if ((_contentSize % chunkSize) != 0)
                {
                    contentRequest->rangeFrom = i * chunkSize;
                    contentRequest->rangeTo = _contentSize - 1;
                }
            }
            addContentRequest(contentRequest);
        }
        bSuccess = true;
    }
    else
    {
        reportError(headerRequest.get());
    }
    return bSuccess;
}

bool HTTPDownloadNode::contentRequestCompleted(ContentRequestPointer contentRequest)
{
    std::unique_lock<std::mutex> lock(_contentRequestMutex);
    auto requestIter = _contentRequests.find(contentRequest->curlHandle);
    if (requestIter != _contentRequests.end())
    {
        _contentRequests.erase(contentRequest->curlHandle);
    }
    if (contentRequest->curlCode != CURLE_OK)
    {
        // 这里有个问题，如果有一个请求的重试次数为0了，那么将会取消全部请求，
        // 这是一个需要优化的地方，以后再优化。

        if (contentRequest->retryCount == 0)
        {
            _failedRequests.push_back(contentRequest);
            reportError(contentRequest.get());
            for (auto request : _contentRequests)
            {
                _pHTTPDownloader->removeContentRequest(request.second);
            }
            _contentRequests.clear();

            return true;
        }
        else
        {
            --contentRequest->retryCount;
            lock.unlock();
            addContentRequest(contentRequest);
            lock.lock();
        }
    }

    if (_contentRequests.empty() && _failedRequests.empty())
    {
        // download succeed.
        if (_fileHandle)
        {
            fclose(_fileHandle);
        }
        _fileHandle = nullptr;

        if (_downloadListener.onSucceedCallback)
        {
            _downloadListener.onSucceedCallback(_requestUrl, _fileFullPath);
        }
        else
        {
            FKLog("****** Download:%s to:%s Succeed.", _requestUrl.c_str(), _fileFullPath.c_str());
        }
    }

    return _contentRequests.empty();
}

size_t HTTPDownloadNode::writeData(ContentRequest* contentRequest, void* buffer, size_t size)
{
    size_t writeSize = 0;
    if (_fileHandle)
    {
        fseek(_fileHandle, contentRequest->rangeFrom, SEEK_SET);
        writeSize = fwrite(buffer, 1, size, _fileHandle);
    }
    contentRequest->rangeFrom += writeSize;
    _nowDownlaod += writeSize;

    if (_downloadListener.onProgressCallback)
    {
        _downloadListener.onProgressCallback(contentRequest->requestUrl, _nowDownlaod, _contentSize);
    }
    else
    {
        FKLog("===== PROCESS:%0.2f", _nowDownlaod*1.0f / _contentSize*100.0f);
    }
    return writeSize;
}

void HTTPDownloadNode::addContentRequest(ContentRequestPointer contentRequest)
{
    std::lock_guard<std::mutex> lock(_contentRequestMutex);
    if (_pHTTPDownloader->addContentRequest(contentRequest))
    {
        _contentRequests.insert(std::make_pair(contentRequest->curlHandle, contentRequest));
    }
    else
    {
        std::string curlPerformMessage = curl_multi_strerror(contentRequest->addToMultiResult);
        contentRequest->curlMessage = curlPerformMessage;
        if (_downloadListener.onErrorCallback)
        {
            _downloadListener.onErrorCallback(contentRequest->requestUrl, curlPerformMessage);
        }
        else
        {
            FKLog("****** Request url header error:%s,Error:%s", contentRequest->requestUrl.c_str(), curlPerformMessage.c_str());
        }
    }
}

void HTTPDownloadNode::reportError(RequestBase* request)
{
    std::string curlPerformMessage = curl_easy_strerror(request->curlCode);
    request->curlMessage = curlPerformMessage;
    curlPerformMessage += "[";
    curlPerformMessage += request->errorBuffer;
    curlPerformMessage += "]";
    if (_downloadListener.onErrorCallback)
    {
        _downloadListener.onErrorCallback(request->requestUrl, curlPerformMessage);
    }
    else
    {
        FKLog("****** Request url header error:%s,Error:%s", request->requestUrl.c_str(), curlPerformMessage.c_str());
    }
}



} // namespace network
NS_FK_END