/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef NETFRAMEWORK_HTTPDOWNLOADER_HPP
#define NETFRAMEWORK_HTTPDOWNLOADER_HPP
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include <vector>
#include <functional>
#include "HTTPClient/HTTPRequest.hpp"
#include "HTTPClient/HTTPResponse.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Base/mutablebuf.hpp"
NS_FK_BEGIN


class HTTPDownloadRequest : public HTTPRequest
{
public:
    typedef std::shared_ptr<HTTPDownloadRequest> Pointer;
    HTTPDownloadRequest(bool enableDebug = false);
    virtual ~HTTPDownloadRequest();
    static Pointer           Create(bool enableDebug = false);
    HTTPDownloadRequest&     SetFileHandle(FILE*  pFile);
    HTTPDownloadRequest&     SetEnableRange(bool enableRange);
    FILE*                    GetFileHandle();
    int64                    GetDownladedSize();
   
protected:
    virtual bool   Build()override;
    virtual bool   OnFinished()override;

   /**
    * Method called when libcurl wants us to receive response body (see CURLOPT_WRITEFUNCTION)
    *
    * @param buffer buffer to copy data to (allocated and managed by libcurl)
    * @param sizeInBlocks size of above buffer, in 'blocks'
    * @param blockSizeInBytes size of a single block
    * @return number of bytes actually processed, error is triggered if it does not match number of bytes passed
    */
    virtual size_t ReceiveResponseBodyCallback(void* buffer, size_t sizeInBlocks, size_t blockSizeInBytes)override;
public:
    /** Offset into file where the file is stored.*/
    int64           Offset;
    /** Serialized file size. */
    int64           Size;
    /** Current write offset.*/
    int64           WriteOffset;
protected:
    FILE*           FileHandle;
    /** Size of downloaded*/
    int64           DownladedSize;
    /** Has Accept-Ranges*/
    bool            EnableRange;
};


// Experimental HTTPDownloader
class HTTPDownloader :public std::enable_shared_from_this<HTTPDownloader>
{

public:
    typedef std::shared_ptr<HTTPDownloader> Pointer;
    typedef std::vector<HTTPDownloadRequest::Pointer>   HTTPDownloadRequestList;
    typedef std::function<void(HTTPDownloader::Pointer, const std::string&)> HTTPDownloaderErrorMethod;
    typedef std::function<void(HTTPDownloader::Pointer, int64, int64)>       HTTPDownloaderProgressMethod;
    typedef std::function<void(HTTPDownloader::Pointer)> HTTPDownloaderCompletedMethod;
    HTTPDownloader();
    ~HTTPDownloader();
    static Pointer Create(const std::string& url, const std::string& path);
    void           Tick(float deltaTime);
    std::string    GetURL()const;
    std::string    GetFilePath()const;
    HTTPDownloaderErrorMethod         OnErrorHandler;
    HTTPDownloaderProgressMethod      OnProgressHandler;
    HTTPDownloaderCompletedMethod     OnCompletedHandler;

public:
    void OnCompleted(HTTPRequest::Pointer request, HTTPResponse::Pointer response);
protected:
    bool Init();
    void Clear();
private:
    std::string              URL;
    std::string              FilePath;
    FILE*                    FileHandle;
    int64                    ContentSize;
    int64                    DownloadedSize;
    HTTPDownloadRequestList  Requests;
    size_t                   RetryCount;
    bool                     bCompleted;
};

NS_FK_END
//#include "HTTPClient/HTTPDownloader.ipp"
#endif // END OF NETFRAMEWORK_HTTPDOWNLOADER_HPP
