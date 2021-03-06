/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPUPLOADER_IPP
#define NETFRAMEWORK_HTTPUPLOADER_IPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include "HTTPClient/HTTPUploader.hpp"

NS_FK_BEGIN


HTTPUploadRequest::HTTPUploadRequest(bool enableDebug /*= false*/)
	: HTTPRequest(enableDebug)
	, FileHandle(nullptr)
{

}

HTTPUploadRequest::~HTTPUploadRequest()
{
	if (FileHandle)
	{
		fclose(FileHandle);
	}
}

HTTPUploadRequest::Pointer HTTPUploadRequest::Create(bool enableDebug /*= false*/)
{
	HTTPUploadRequest::Pointer shared_httprequest = std::make_shared<HTTPUploadRequest>(enableDebug);
	return shared_httprequest;
}


HTTPUploadRequest& HTTPUploadRequest::SetFilePath(const std::string& fileFullPath)
{
	FilePath = fileFullPath;
	return (*this);
}

std::string HTTPUploadRequest::GetFilePath()
{
	return FilePath;
}


bool HTTPUploadRequest::Build()
{
	bool result = false;
	do 
	{
		result = HTTPRequest::Build();
		BREAK_IF(!result);

#if PLATFORM_WINDOWS
        int errcode = fopen_s(&FileHandle, FilePath.c_str(), "rb");
        BREAK_IF(errcode != 0);
#else
		FileHandle = fopen(FilePath.c_str(), "rb");
#endif // PLATFORM_ANDROID
		BREAK_IF(!FileHandle);
		fseek(FileHandle, 0, SEEK_END);
		curl_off_t FileSize = ftell(FileHandle);
		fseek(FileHandle, 0, SEEK_SET);
		/* and give the size of the upload (optional) */
		curl_easy_setopt(EasyHandle, CURLOPT_INFILESIZE_LARGE, FileSize);
	} while (false);
	return result;
}

bool HTTPUploadRequest::OnFinishedRequest()
{
	if (FileHandle)
	{
		fclose(FileHandle);
	}
	return false;
}

size_t HTTPUploadRequest::UploadCallback(void* buffer, size_t size, size_t nmemb)
{
	ElapsedTime = 0.0f;
	return fread(buffer, size, nmemb, FileHandle);
}


int HTTPUploadRequest::ProgressCallback(curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)
{
	if (OnRequestProgress)
	{
		OnRequestProgress(std::dynamic_pointer_cast<HTTPUploadRequest>(shared_from_this()), nowUpload, totalUpload);
	}
	return CURLE_OK;
}

NS_FK_END

#endif // END OF NETFRAMEWORK_HTTPUPLOADER_IPP
