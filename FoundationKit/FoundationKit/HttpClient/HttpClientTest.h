/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/HttpClient/HttpClient.h"
#include "FoundationKit/Foundation/Logger.h"

NS_FK_BEGIN

static void TestHttpClient()
{
    LOG_INFO("======= TestHttpClient =======");
    //HttpClient::getInstance()->setDebugMode(true);
    HttpRequest::Pointer request = HttpRequest::createWithUrl("wiki.mysnail.com/", HttpRequest::Type::POST);
    request->addPOSTValue("username", "libo");
    request->addFormContents("upload", "test.txt");
    request->addFormFile("googlebreakpad.zip", "C:/Users/libo/Downloads/googlebreakpad.zip");
    request->onRequestFinished = [](HttpResponse::Pointer responsePtr)
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
    HttpResponse::Pointer response = HttpClient::getInstance()->sendRequest(request);
    if (response->getHttpRequest()->onRequestFinished)
    {
        response->getHttpRequest()->onRequestFinished(response);
    }
}

NS_FK_END

