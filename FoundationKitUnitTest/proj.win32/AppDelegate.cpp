
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>

#include "AppDelegate.h"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/DataStream.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/ApplyTuple.hpp"
#include "FoundationKit/Foundation/TupleApply.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/std/memory_aligned.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Platform/FileUtils.hpp"
#include "FoundationKit/Foundation/TimerQueue.hpp"

#include "excel/BasicExcel.hpp"


#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

using namespace std;
USING_NS_FK;

static TimerQueue  G_TimerQueue;
static bool bExitApp = false;

static std::thread clientThread;
static  std::thread serverThread;
void runServer();
void runClient();

void ParseAppleDeviceData();

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}


void AppDelegate::applicationDidLaunching()
{

}



bool AppDelegate::applicationDidFinishLaunching() 
{
    std::error_code ec;
    std::string strErr = ec.message();
    FileUtils* pFileUtils = FileUtils::getInstance();
    pFileUtils->setDefaultResourceRootPath("F:/temp/com.netease.zztj/files/NeoX/");

    auto is_npk_file = [](const std::string& path)
    {
        return (path.find(".npk") != std::string::npos);
    };

    auto is_npkheader_file = [](const std::string& path)
    {
        return (path.find(".npk.header") != std::string::npos);
    };

    auto is_npkmap_file = [](const std::string& path)
    {
        return (path.find(".npk.map") != std::string::npos);
    };

    std::vector<std::string>  npkFilesPath;
    std::vector<std::string>  npkMapsPath;
    std::vector<std::string>  npkHeadersPath;

    std::vector<std::string> filelist = pFileUtils->readAllLines(pFileUtils->fullPathForFilename("filelist.txt"));
    for (auto& fileline : filelist)
    {
        std::vector<std::string> lineArgs = StringUtils::split(fileline, "\t");
        if (lineArgs.size()==0) continue;

        if (is_npkheader_file(lineArgs[0]))
        {
            npkHeadersPath.push_back(lineArgs[0]);
            continue;
        }

        if (is_npkmap_file(lineArgs[0]))
        {
            npkMapsPath.push_back(lineArgs[0]);
            continue;
        }

        if (is_npk_file(lineArgs[0]))
        {
            npkFilesPath.push_back(lineArgs[0]);
        }
    }

    std::string fileName111 = pFileUtils->getFileNameWithoutExtension(pFileUtils->fullPathForFilename("filelist.txt"));
    std::string releaseRootPath = pFileUtils->getResourceRootPath() + "Documents/";
    for (auto& npkFile : npkFilesPath)
    {
        std::string npkfullpath = pFileUtils->fullPathForFilename(npkFile);
        std::string npkMapfullpath = pFileUtils->fullPathForFilename(npkFile+".map");
        std::string fileName = pFileUtils->getFileNameWithoutExtension(npkFile);
        std::string finallyPath = releaseRootPath + fileName+"/";
        pFileUtils->createDirectory(finallyPath);
        std::vector<std::string> npkMapLines = pFileUtils->readAllLines(npkMapfullpath);
        if (npkMapLines.size() == 0)continue;
        FILE* npkfp = fopen(npkfullpath.c_str(), "rb");
        for (auto& npkmapline:npkMapLines)
        {
            std::vector<std::string> lineArgs = StringUtils::split(npkmapline, "\t");
            if (lineArgs.size() != 7) continue;
            std::string filePath = lineArgs[0];
            int     compressSize = atoi(lineArgs[2].c_str());
            if (compressSize == 0) continue;
            int     uncompressSize = atoi(lineArgs[4].c_str());
            int     offset = atoi(lineArgs[6].c_str());
            char* fileData = new char[compressSize];
            fseek(npkfp, offset, SEEK_CUR);
            fread(fileData, 1, compressSize, npkfp);
            pFileUtils->createDirectory(pFileUtils->getDirPath(finallyPath + filePath));
            pFileUtils->writeDataToFile(mutable_buffer(fileData, compressSize), finallyPath + filePath);
            delete fileData;
        }
        fclose(npkfp);
    }

    //std::vector<std::string>  files;
    //FileUtils::getInstance()->getFilesFromDir("E:\\GitHub\\FoundationKit\\FoundationKit\\FoundationKit", files, true);
    //std::string strMakefile;
    //auto fileutils = FileUtils::getInstance();
    //for (auto filepath:files)
    //{
    //    if (fileutils->getFileExtension(filepath) == ".cpp" 
    //        || fileutils->getFileExtension(filepath) == ".c"
    //        || fileutils->getFileExtension(filepath) == ".cc")
    //    {
    //        strMakefile += filepath;
    //        strMakefile += "\n";
    //    }
    //}


    //clientThread = std::thread([]()
    //{
    //    runClient();
    //});

    //serverThread = std::thread([]()
    //{
    //    runServer();
    //});
    
	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() 
{

}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() 
{

}

void AppDelegate::applicationWillTerminate()
{
    bExitApp = true;

    if (clientThread.joinable())
    {
        clientThread.join();
    }

    if (serverThread.joinable())
    {
        serverThread.join();
    }
}

void AppDelegate::mainLoop()
{

}

void ParseAppleDeviceData()
{
    YExcel::BasicExcel           _basicExcel;
    YExcel::BasicExcelWorksheet* _workSheet;
    _basicExcel.Load("E:\\temp\\DeviceInfo.xls");
    _workSheet = _basicExcel.GetWorksheet(0);

    int rowIndex = 2;
    std::string serialization;
    auto GetExcelString = [&](int row, int col)
    {
        std::string result;
        const char* value = _workSheet->Cell(row, col)->GetString();
        if (value)
        {
            result = value;
        }
        else
        {
            const wchar_t* wvalue = _workSheet->Cell(row, col)->GetWString();
            if (wvalue)
            {
                result = StringUtils::wstring2string(wvalue);
            }
        }
        return result;
    };
    while (true)
    {
        std::string deviceModel = GetExcelString(rowIndex, 1);
        if (deviceModel.empty())
        {
            break;
        }

        auto deviceModelList = StringUtils::split(deviceModel, " \" ");
        size_t modelIndex = 0;
        for (auto& strDeviceModel : deviceModelList)
        {
            std::string realModelName = strDeviceModel;
            if (modelIndex > 0)
            {
                std::string tmpModel = deviceModelList[0];
                realModelName = tmpModel.substr(0, tmpModel.size() - 3);
                realModelName = StringUtils::trim(realModelName);
                realModelName += strDeviceModel;
            }
            ++modelIndex;
            serialization += "{\"";
            serialization += realModelName;
            serialization += "\",{";

            serialization += "\"";
            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 4)->GetInteger()); //RAM
            serialization += "\",";

            serialization += "\"";
            serialization += GetExcelString(rowIndex, 7); //CPU NAME
            serialization += "\",";

            serialization += "\"";
            serialization += GetExcelString(rowIndex, 8); //CPU arch
            serialization += "\",";

            serialization += "\"";
            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 10)->GetInteger()); //CPU cores
            serialization += "\",";

            serialization += "\"";
            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 11)->GetInteger()); //CPU clocks
            serialization += "\",";

            serialization += "\"";
            serialization += GetExcelString(rowIndex, 16); //GPU NAME
            serialization += "\",";

            serialization += "\"";
            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 17)->GetInteger()); //GPU cores
            serialization += "\",";

            serialization += "\"";
            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 18)->GetInteger()); //GPU clocks
            serialization += "\",";

            serialization += "\"";
            serialization += GetExcelString(rowIndex, 19); //  Screen Res 
            serialization += "\",";

            serialization += "\"";
            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 20)->GetInteger()); //  PPI 
            serialization += "\",";

            serialization += "\"";
            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 21)->GetDouble()); //  Screen Size (inches)
            serialization += "\"";

            serialization += "}},\n";
        }
        ++rowIndex;
    }
}




