/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/HttpClient/HttpClient.h"
#include "FoundationKit/Foundation/Logger.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

NS_FK_BEGIN

static void TestHttpClient()
{
    LOG_INFO("======= TestHttpClient =======");
    //HttpClient::getInstance()->setDebugMode(true);
    HTTPRequest::Pointer request = HTTPRequest::create("http://wiki.mysnail.com/", HTTPRequest::Type::POST);
    request->addPostField("username", "libo");
    request->addContent("upload", "test.txt");
    request->addFile("googlebreakpad.zip", "C:/Users/libo/Downloads/googlebreakpad.zip");
    request->onRequestFinished = [](HTTPResponse::Pointer responsePtr)
    {
        if (responsePtr->isSucceed())
        {
            std::vector<char> vdata = *responsePtr->getResponseHeader();
            std::string strHeader(&vdata[0], vdata.size());
            int errorCode = responsePtr->getResponseCode();
            LOG_INFO("=== CODE:%d, HEADER:%s", errorCode, strHeader.c_str());
        }
        else
        {
            const char* errorBuf = responsePtr->getErrorBuffer();
            LOG_INFO("=== ERROR:%s", errorBuf);
        }


    };
    request->onProgress = [](double dlTotal, double dlNow, double ulTotal, double ulNow)
    {
        //LOG_INFO("=========onProgress: %f/%f, %f/%f ", dlTotal, dlNow, ulTotal, ulNow);
    };

    HttpClient::getInstance()->sendRequestAsync(request);
    HttpClient::getInstance()->sendRequestAsync(request);
    HttpClient::getInstance()->sendRequestAsync(request);
    HttpClient::getInstance()->sendRequestAsync(request);
    HTTPResponse::Pointer response = HttpClient::getInstance()->sendRequest(request);
    if (response->getHTTPRequest()->onRequestFinished)
    {
        response->getHTTPRequest()->onRequestFinished(response);
    }
}


static void TestHttpClientWithUploadDumpInfo()
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("channelId"); writer.String("channelId");
    writer.Key("guid"); writer.String("guid");
    writer.Key("device"); writer.String("device");
    writer.Key("ptime"); writer.String("ptime");
    writer.Key("version"); writer.String("version");
    writer.Key("osver"); writer.String("osver");
    writer.Key("fmemory"); writer.String("fmemory");
    writer.Key("gameId"); writer.String("66");
    writer.Key("arch"); writer.String("arch");
    writer.Key("lifespan"); writer.String("lifespan");
    writer.Key("sceneid"); writer.String("sceneid");
    writer.Key("userAccount"); writer.String("userAccount");
    writer.Key("type"); writer.String("type");
    writer.Key("sdkVersion"); writer.String("sdkVersion");
    writer.Key("totalMemory"); writer.String("totalMemory");
    writer.Key("isRoot"); writer.Int(1);
    writer.Key("crashPackage"); writer.String("crashPackage");
    writer.EndObject();
    std::string jsonString = sb.GetString();

    HttpClient::getInstance()->setDebugMode(true);
    // http://172.19.10.135:8080/bgmgt/api/comm/cpp2
    // http://172.19.10.135:8080/bgmgt/api/comm/cpp
    HTTPRequest::Pointer request = HTTPRequest::create("http://172.19.10.135:8080/bgmgt/api/comm/cpp", HTTPRequest::Type::POST);
    //request->addPOSTValue("params", jsonString.c_str());
    request->addContent("params", jsonString.c_str());
    request->addFile("file", "E:/dump.txt");
    request->addFile("traceFile", "E:/tracelog.log");
    request->setContentEncoding(HTTPRequest::DataEncodingType::Gzip);

    request->onProgress = [](double dlTotal, double dlNow, double ulTotal, double ulNow)
    {
        LOG_INFO("=========onProgress: %f/%f, %f/%f ", dlTotal, dlNow, ulTotal, ulNow);
    };

    HTTPResponse::Pointer response = HttpClient::getInstance()->sendRequest(request);
    if (response->isSucceed())
    {
        std::vector<char> vdata = *response->getResponseHeader();
        std::string strHeader(&vdata[0], vdata.size());
        int errorCode = response->getResponseCode();
        LOG_INFO("=== CODE:%d, HEADER:%s", errorCode, strHeader.c_str());
    }
    else
    {
        const char* errorBuf = response->getErrorBuffer();
        LOG_INFO("=== ERROR:%s", errorBuf);
    }
}


NS_FK_END

