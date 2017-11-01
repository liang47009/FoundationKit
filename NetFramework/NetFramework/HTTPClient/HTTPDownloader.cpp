#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "HTTPDownloader.hpp"
#include "HTTPResponse.hpp"
#include "HTTPClient.hpp"
#include <thread>
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
#if PLATFORM_WINDOWS
        sprintf_s(range, 64, "%lld-%lld", Offset, Offset + Size);
#else
		sprintf(range, "%lld-%lld", Offset, Offset + Size);
#endif // PLATFORM_ANDROID
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



HTTPDownloader::HTTPDownloader()
	: FileHandle(nullptr)
	, ContentSize(0)
	, OnErrorHandler(nullptr)
	, OnProgressHandler(nullptr)
	, OnCompletedHandler(nullptr)
	, RetryCount(-1)
	, bCompleted(true)
{

}

HTTPDownloader::~HTTPDownloader()
{
	Clear();
}

HTTPDownloader::Pointer HTTPDownloader::Create(const std::string& url, const std::string& path)
{
	HTTPDownloader::Pointer shared_downloader = std::make_shared<HTTPDownloader>();
	shared_downloader->URL = url;
	shared_downloader->FilePath = path;
	if (shared_downloader->Init())
	{
		return shared_downloader;
	}
	return nullptr;
}

void HTTPDownloader::Tick(float deltaTime)
{
	if (bCompleted) return;

	if (OnProgressHandler)
	{
		for (auto& request : Requests)
		{
			DownloadedSize += request->GetDownladedSize();
		}
		OnProgressHandler(shared_from_this(), DownloadedSize, ContentSize);
		DownloadedSize = 0;
	}

	bool successed = true;
	for (auto& request : Requests)
	{
		successed = (successed && (request->GetRequestStatus() == RequestStatus::Succeeded));
	}
	if (!Requests.empty() && successed)
	{
		fclose(FileHandle);
		FileHandle = nullptr;
		if (OnCompletedHandler)
		{
			OnCompletedHandler(shared_from_this());
		}
		Requests.clear();
		bCompleted = true;
	}
}

static int64  MIN_BLOCK_SIZE = 1024 * 1024 * 10;

bool HTTPDownloader::Init()
{
	std::string checkedStoragePath = FilePath;
	if (checkedStoragePath.back() != '/' && checkedStoragePath.back() != '\\')
	{
		checkedStoragePath.push_back('/');
	}

	size_t pos = URL.find_last_of("/");
	if (pos != std::string::npos)
	{
		checkedStoragePath += URL.substr(pos + 1);
	}

#if PLATFORM_WINDOWS
    int errcode = fopen_s(&FileHandle, checkedStoragePath.c_str(), "wb");
    if (errcode != 0 || FileHandle == nullptr) return false;

#else
    FileHandle = fopen(checkedStoragePath.c_str(), "wb");
    if (FileHandle == nullptr) return false;
#endif // PLATFORM_ANDROID
	uint32 concurrency = std::thread::hardware_concurrency();
	HTTPRequest::Pointer  HeadRequest = HTTPRequest::Create();
	HeadRequest->SetMethod(RequestMethodType::HEAD);
	HeadRequest->SetURL(URL);
	HeadRequest->OnRequestCompleted = [=](HTTPRequest::Pointer request, HTTPResponse::Pointer response)
	{
		if (response->IsSucceeded())
		{
			ContentSize = response->GetContentSize();
			std::string strEffectiveUrl = response->GetEffectiveURL();
			if (strEffectiveUrl.empty())
			{
				strEffectiveUrl = URL;
			}

			std::string AcceptRanges = response->GetHeader("Accept-Ranges");
			uint32      RangeNum = 1;
			bool bAcceptRanges = (!AcceptRanges.empty() && AcceptRanges != "none");
			if (bAcceptRanges)
			{
				RangeNum = static_cast<int32>(ContentSize / MIN_BLOCK_SIZE);
				if (RangeNum > concurrency)
					RangeNum = concurrency;
				if (RangeNum < 1)
					RangeNum = 1;
			}
			int64 chunkSize = ContentSize / RangeNum;
			for (uint32 i = 0; i <= RangeNum; ++i)
			{
				HTTPDownloadRequest::Pointer ContentRequest = HTTPDownloadRequest::Create();
				ContentRequest->SetURL(strEffectiveUrl);
				ContentRequest->SetFileHandle(FileHandle);
				ContentRequest->SetEnableRange(bAcceptRanges);
				ContentRequest->OnRequestCompleted = FUN_BIND_2(HTTPDownloader::OnCompleted, this);
				if (i < RangeNum)
				{
					ContentRequest->Offset = i * chunkSize;
					ContentRequest->Size = chunkSize;
					HTTPClient::GetInstance()->PostRequest(ContentRequest);
					Requests.push_back(ContentRequest);
				}
				else
				{
					if ((ContentSize % chunkSize) != 0)
					{
						ContentRequest->Offset = i * chunkSize;
						ContentRequest->Size = ContentSize - ContentRequest->Offset;
						HTTPClient::GetInstance()->PostRequest(ContentRequest);
						Requests.push_back(ContentRequest);
					}
				}
			}
			bCompleted = false;
		}
		else
		{
			if (OnErrorHandler)
			{
				std::string errMsg = response->GetResponseMsg();
				errMsg += "(";
				errMsg += response->GetErrorMsg();
				errMsg += ")";
				OnErrorHandler(shared_from_this(), errMsg);
			}
			Clear();
		}
	};
	HTTPClient::GetInstance()->PostRequest(HeadRequest);
	return true;
}

void HTTPDownloader::OnCompleted(HTTPRequest::Pointer request, HTTPResponse::Pointer response)
{
	if (!response->IsSucceeded())
	{
		if (RetryCount > 0)
		{
			HTTPDownloadRequest::Pointer downloadRequest = std::dynamic_pointer_cast<HTTPDownloadRequest>(request);
			downloadRequest->Size = downloadRequest->Size - (downloadRequest->WriteOffset - downloadRequest->Offset);
			downloadRequest->Offset = downloadRequest->WriteOffset;
			HTTPClient::GetInstance()->PostRequest(downloadRequest);
			RetryCount -= 1;
		}
		else
		{
			if (OnErrorHandler)
			{
				std::string errMsg = response->GetResponseMsg();
				errMsg += "(";
				errMsg += response->GetErrorMsg();
				errMsg += ")";
				OnErrorHandler(shared_from_this(), errMsg);
			}
			Clear();
			return;
		}

	}
}

std::string HTTPDownloader::GetURL() const
{
	return URL;
}

std::string HTTPDownloader::GetFilePath() const
{
	return FilePath;
}

void HTTPDownloader::Clear()
{
	for (auto& request : Requests)
	{
		HTTPClient::GetInstance()->RemoveRequest(request);
		request->OnRequestCompleted = nullptr;
		request->OnRequestProgress = nullptr;
	}
	Requests.clear();
	bCompleted = true;
}

NS_FK_END








