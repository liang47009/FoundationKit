#pragma once

#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/HttpDownloader/Downloader.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Platform/FileUtils.h"

USING_NS_FK;
using namespace FoundationKit::network;

static std::shared_ptr<Downloader> _downloader;
static int DownloaderCount = 0;
static void TestDownloader()
{


    _downloader = std::shared_ptr<Downloader>(new Downloader);
    _downloader->setErrorCallback([](const Downloader::Error& error)
    {
        LOG_INFO("error downloading: %s - %s", error.url.c_str(), error.message.c_str());
    });

    _downloader->setProgressCallback([](double totalToDownload, double nowDownloaded, const std::string& url, const std::string& customId)
    {
        LOG_INFO("download progress: %d%% - %s", (int)((nowDownloaded / totalToDownload) * 100), url.c_str());
    });

    _downloader->setSuccessCallback([&](const std::string& url, const std::string& path, const std::string& customId)
    {
        LOG_INFO("download finished: %s", path.c_str());
        DownloaderCount += 1;
        if (DownloaderCount == 2)
        {
            _downloader = nullptr;
            LOG_INFO("================ TestDownloader Finished.=======");
        }
    });

    std::string path = FileUtils::getInstance()->getResourceRootPath() + "DownloaderTest/ddms_guide0_0_1_sync.zip";
    std::string remote = "http://wiki.mysnail.com/download/attachments/24284168/googlebreakpad+v1.1.0.zip?version=1&modificationDate=1435703452000";
    _downloader->downloadSync(remote, path, "download_async_test");

    path = FileUtils::getInstance()->getResourceRootPath() + "DownloaderTest/ddms_guide0_0_1_async.zip";
    remote = "http://wiki.mysnail.com/download/attachments/24284168/googlebreakpad+v1.1.0.zip?version=1&modificationDate=1435703452000";
    _downloader->downloadAsync(remote, path, "download_async_test");
    LOG_INFO("Downloading '%s' into '%s'", remote.c_str(), path.c_str());
}