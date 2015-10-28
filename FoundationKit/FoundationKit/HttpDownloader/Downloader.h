/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef FoundationKit_Downloader_H
#define FoundationKit_Downloader_H
#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
#include "DownloaderImpl.h"

NS_FK_BEGIN

namespace network {

class  Downloader : public std::enable_shared_from_this<Downloader>
{
public:

    enum class ErrorCode
    {
        CREATE_FILE,

        NETWORK,

        NO_NEW_VERSION,

        UNCOMPRESS,

        CURL_UNINIT,
        
        CURL_MULTI_ERROR,
        
        CURL_EASY_ERROR,

        INVALID_URL,

        INVALID_STORAGE_PATH,
        
        PREPARE_HEADER_ERROR
    };

    struct Error
    {
        ErrorCode code;
        int curlm_code;
        int curle_code;
        std::string message;
        std::string customId;
        std::string url;
    };

    struct StreamData
    {
        long offset;
        long total;
        unsigned char *buffer;
    };


    typedef std::function<void(const Downloader::Error&)> ErrorCallback;
    typedef std::function<void(double, double, const std::string&, const std::string&)> ProgressCallback;
    typedef std::function<void(const std::string&, const std::string&, const std::string&)> SuccessCallback;

    int getConnectionTimeout();
    void setConnectionTimeout(int timeout);
    
    void setErrorCallback(const ErrorCallback &callback) { _onError = callback; };
    void setProgressCallback(const ProgressCallback &callback) { _onProgress = callback; };
    void setSuccessCallback(const SuccessCallback &callback) { _onSuccess = callback; };

    ErrorCallback getErrorCallback() const { return _onError; };
    ProgressCallback getProgressCallback() const { return _onProgress; };
    SuccessCallback getSuccessCallback() const { return _onSuccess; };

    void downloadToBufferAsync(const std::string& srcUrl, unsigned char *buffer, long size, const std::string& customId = "");
    void downloadToBufferSync(const std::string& srcUrl, unsigned char *buffer, long size, const std::string& customId = "");

    void downloadAsync(const std::string& srcUrl, const std::string& storagePath, const std::string& customId = "");
    void downloadSync(const std::string& srcUrl, const std::string& storagePath, const std::string& customId = "");
    
    void batchDownloadAsync(const DownloadUnits &units, const std::string& batchId = "");
    void batchDownloadSync(const DownloadUnits &units, const std::string& batchId = "");


    HeaderInfo getHeader(const std::string &srcUrl);

    /**
     *  The default constructor.
     */
    Downloader();
    ~Downloader();

protected:


    void prepareDownload(const DownloadUnit& downloadUnit);

    void downloadToBuffer(const std::string& srcUrl, const std::string& customId, unsigned char* buffer, long size);
    void downloadToFP(const std::string& srcUrl, const std::string& customId, const std::string& storagePath);
    void groupBatchDownload(const DownloadUnits& units);

    void notifyError(ErrorCode code, const std::string& msg = "", const std::string& customId = "", int curle_code = 0, int curlm_code = 0);
    void notifyError(const std::string& msg, int curlm_code, const std::string& customId = "");
    void notifyError(const std::string& msg, const std::string& customId, int curle_code);

    // callbacks
    int downloadProgressFunc(void *userdata, double totalToDownload, double nowDownloaded);
    int batchDownloadProgressFunc(void *userdata, double totalToDownload, double nowDownloaded);
    size_t bufferWriteFunc(void *ptr, size_t size, size_t nmemb, void *userdata);
    size_t fileWriteFunc(void *ptr, size_t size, size_t nmemb, void *userdata);

    // callback helpers
    void reportProgressFinished(double totalToDownload, double nowDownloaded, const DownloadUnit* downloadUnit);
    void reportProgressInProgress(double totalToDownload, double nowDownloaded, const DownloadUnit* downloadUnit);
    void reportDownloadFinished(const std::string& url, const std::string&, const std::string& customid);


private:
    std::string getFileNameFromUrl(const std::string& srcUrl);

    ErrorCallback _onError;
    ProgressCallback _onProgress;
    SuccessCallback _onSuccess;

    int _connectionTimeout;
    bool _supportResuming;
    DownloaderImpl* _downloaderImpl;
};

} // namespace network

NS_FK_END

#endif // FoundationKit_Downloader_H

