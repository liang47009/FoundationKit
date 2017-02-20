/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef FoundationKit_DownloaderImpl_H
#define FoundationKit_DownloaderImpl_H
#pragma once

#include <string>
#include <vector>
#include "IDownloaderImpl.h"

NS_FK_BEGIN

namespace network
{
    class DownloaderImpl : public IDownloaderImpl
    {
    public:
        DownloaderImpl();
        virtual ~DownloaderImpl();

        bool init() override;

        // Overrides
        int performDownload(DownloadUnit* unit,
                            const WriterCallback& writerCallback,
                            const ProgressCallback& progressCallback
                            ) override;

        int performBatchDownload(const DownloadUnits& units,
                                 const WriterCallback& writerCallback,
                                 const ProgressCallback& progressCallback,
                                 const ErrorCallback& errorCallback                                         
                                 ) override;

        int getHeader(const std::string& url, HeaderInfo* headerInfo) override;
        std::string getStrError() const override;
        void setConnectionTimeout(int timeout) override;

        bool supportsResume(const std::string& url);

        //
        const WriterCallback& getWriterCallback() const { return _writerCallback; }
        const ProgressCallback& getProgressCallback() const { return _progressCallback; }
        
    private:

        int _connectionTimeout;
        WriterCallback _writerCallback;
        ProgressCallback _progressCallback;

        void* _curlHandle;
        int _lastErrCode;
        bool _initialized;
    };

} // namespace network

NS_FK_END
#endif // FoundationKit_DownloaderImpl_H

