/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_HTTPUPLOADER_HPP
#define FOUNDATIONKIT_HTTPUPLOADER_HPP

#include <functional>
#include <memory>
#include "HTTPRequest.hpp"

NS_FK_BEGIN
/**
Example:
HTTPUploadRequest::Pointer UploadRequest = HTTPUploadRequest::Create(true);
UploadRequest->SetURL("https://www.baidu.com");
UploadRequest->SetMethod(RequestMethodType::PUT);
UploadRequest->SetFilePath("E:\\GitHub\\FoundationKit\\Win32\\dump.dmp");
UploadRequest->OnRequestCompleted = OnHttpCallback;
UploadRequest->OnRequestProgress = [](HTTPUploadRequest::Pointer request, int64 nowUpload, int64 totalUpload)
{

};
HTTPClient::GetInstance()->PostRequest(UploadRequest);
*/ 
class HTTPUploadRequest : public HTTPRequest
{
public:
    typedef std::shared_ptr<HTTPUploadRequest> Pointer;

    HTTPUploadRequest(bool enableDebug = false);
    virtual~HTTPUploadRequest();
    static Pointer     Create(bool enableDebug = false);
    HTTPUploadRequest& SetFilePath(const std::string& fileFullPath);
    std::string        GetFilePath();

protected:
    virtual bool   Build()override;
    virtual bool   OnFinishedRequest();
    virtual size_t UploadCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)override;
    virtual int    ProgressCallback(curl_off_t totalDownload, curl_off_t nowDownload, curl_off_t totalUpload, curl_off_t nowUpload)override;
    std::string FilePath;
    FILE*       FileHandle;
};

NS_FK_END

#endif // FOUNDATIONKIT_HTTPUPLOADER_HPP
