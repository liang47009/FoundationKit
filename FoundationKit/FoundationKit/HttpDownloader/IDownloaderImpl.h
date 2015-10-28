/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef FoundationKit_IDownloaderImpl_H
#define FoundationKit_IDownloaderImpl_H
#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include "FoundationKit/FoundationKitDefines.h"

NS_FK_BEGIN

namespace network
{
    struct HeaderInfo
    {
        bool valid;
        std::string url;
        std::string contentType;
        double contentSize;
        long responseCode;
    };

    struct DownloadUnit
    {
        // info provided by the user
        std::string srcUrl;
        std::string storagePath;
        std::string customId;

        // additional info created by CCDownloader
        mutable void* fp;
        mutable bool resumeDownload;
        mutable double downloaded;
        mutable double totalToDownload;
        mutable void *_reserved;
    };

    typedef std::unordered_map<std::string, DownloadUnit> DownloadUnits;

    class IDownloaderImpl
    {
    public:
        typedef std::function<int(void* ptr, size_t, size_t, void* userdata)> WriterCallback;
        typedef std::function<int(void* userdata, double, double)> ProgressCallback;
        typedef std::function<void(const std::string&, int, const std::string&)> ErrorCallback;

        IDownloaderImpl(){}
        virtual ~IDownloaderImpl(){}
        virtual bool init() = 0;

        enum class Options 
        {
            RESUME
        };

        // methods that must be overriden
        virtual int performDownload(DownloadUnit* unit,
                                    const WriterCallback& writerCallback,
                                    const ProgressCallback& progressCallback
                                    ) = 0;

        virtual int performBatchDownload(const DownloadUnits& units,
                                         const WriterCallback& writerCallback,
                                         const ProgressCallback& progressCallback,
                                         const ErrorCallback& errorCallback
                                         ) = 0;

        virtual int getHeader(const std::string& url, HeaderInfo* headerInfo) = 0;
        virtual std::string getStrError() const = 0;
        virtual void setConnectionTimeout(int timeout) = 0;
    };
}

NS_FK_END
#endif // FoundationKit_IDownloaderImpl_H

