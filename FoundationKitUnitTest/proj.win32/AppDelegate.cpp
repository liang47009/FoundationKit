
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>

#include "AppDelegate.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Base/DataStream.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Foundation/Scheduler.h"
#include "FoundationKit/Foundation/Version.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/LuaEngine/LuaEngine.h"
#include "FoundationKit/Foundation/ApplyTuple.h"
#include "FoundationKit/Foundation/TupleApply.h"
#include "FoundationKit/Platform/Platform.h"
#include "FoundationKit/std/memory_aligned.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/std/any.hpp"
#include "FoundationKit/std/optional.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/unique_id.hpp"
#include "FoundationKit/Base/DateTime.h"
#include "FoundationKit/Platform/PlatformTLS.h"
#include "FoundationKit/Base/basic_streambuf.hpp"
#include "FoundationKit/Networking/ip/address.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/Base/Dictionary.h"
#include "FoundationKit/Base/MathEx.h"
#include "FoundationKit/Crypto/Base64.h"
#include "FoundationKit/Base/ElapsedTimer.h"
#include "FoundationKit/Base/Timer.h"
#include "FoundationKit/Foundation/Compression.h"
#include "FoundationKit/Platform/FileUtils.h"
#include "FoundationKit/Base/TimerQueue.h"
#include "FoundationKit/Networking/network.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPClient.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPRequest.hpp"
#include "FoundationKit/Networking/HTTPClient/HTTPResponse.hpp"
#include "FoundationKit/Networking/HTTPDownloader/HTTPDownloader.hpp"

#include "excel/BasicExcel.hpp"

using namespace std;
USING_NS_FK;
using namespace network;

static Scheduler* shared_Scheduler = nullptr;
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

enum DeviceDataFeild
{
    RAM,
    CPU_MODEL,
    CPU_ARCH,
    CPU_CORES,
    CPU_CLOCK,
    GPU_MODEL,
    GPU_CORES,
    GPU_CLOCK,
    SCREEN_PPI
};

std::unordered_map<std::string, std::vector<std::string>>  AppleDeviceData = 
{
    { "iPhone1,1", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "412", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
    { "iPod1,1", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "412", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
    { "iPhone1,2", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "412", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
    { "iPod2,1", { "128", "ARM1176JZ(F)-S v1.0", "ARMv6", "1", "532", "PowerVR MBX Lite", "1", "60", "480 x 320", "163", "3.50" } },
    { "iPhone2,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "600", "PowerVR SGX535", "1", "150", "480 x 320", "163", "3.50" } },
    { "iPod3,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "600", "PowerVR SGX535", "1", "0", "480 x 320", "163", "3.50" } },
    { "iPad1,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "1000", "PowerVR SGX535", "1", "200", "1024 x 768", "132", "9.70" } },
    { "iPhone3,1", { "512", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
    { "iPhone3,2", { "512", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
    { "iPhone3,3", { "512", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
    { "iPod4,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "960 x 640", "326", "3.50" } },
    { "AppleTV2,1", { "256", "ARM Cortex-A8", "ARMv7", "1", "800", "PowerVR SGX535", "1", "200", "..", "0", "0.00" } },
    { "iPad2,1", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "132", "9.70" } },
    { "iPad2,2", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "132", "9.70" } },
    { "iPad2,3", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "132", "9.70" } },
    { "iPhone4,1", { "512", "ARM Cortex-A9", "ARMv7", "2", "800", "PowerVR SGX543MP2", "2", "250", "960 x 640", "326", "3.50" } },
    { "iPad 3,1", { "1024", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP4", "4", "250", "2048 x 1536", "264", "9.70" } },
    { "iPad3,2", { "1024", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP4", "4", "250", "2048 x 1536", "264", "9.70" } },
    { "iPad3,3", { "1024", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP4", "4", "250", "2048 x 1536", "264", "9.70" } },
    { "AppleTV3,1", { "512", "ARM Cortex-A9", "ARMv7", "1", "0", "PowerVR SGX543MP2", "2", "250", "..", "0", "0.00" } },
    { "iPhone5,1", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
    { "iPhone5,2", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
    { "iPod5,1", { "512", "ARM Cortex-A9", "ARMv7", "2", "800", "PowerVR SGX543MP2", "2", "250", "1136 x 640", "326", "4.00" } },
    { "iPad3,4", { "1024", "Swift (Apple)", "ARMv7s", "2", "1400", "PowerVR SGX554MP4", "4", "0", "2048 x 1536", "264", "9.70" } },
    { "iPad3,5", { "1024", "Swift (Apple)", "ARMv7s", "2", "1400", "PowerVR SGX554MP4", "4", "0", "2048 x 1536", "264", "9.70" } },
    { "iPad3,6", { "1024", "Swift (Apple)", "ARMv7s", "2", "1400", "PowerVR SGX554MP4", "4", "0", "2048 x 1536", "264", "9.70" } },
    { "iPad2,5", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "163", "7.90" } },
    { "iPad2,6", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "163", "7.90" } },
    { "iPad2,7", { "512", "ARM Cortex-A9", "ARMv7", "2", "1000", "PowerVR SGX543MP2", "2", "250", "1024 x 768", "163", "7.90" } },
    { "AppleTV3,1", { "512", "ARM Cortex-A9", "ARMv7", "1", "0", "PowerVR SGX543MP2", "2", "250", "..", "0", "0.00" } },
    { "iPhone5,3", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
    { "iPhone5,4", { "1024", "Swift (Apple)", "ARMv7s", "2", "1300", "PowerVR SGX543MP3", "3", "266", "1136 x 640", "326", "4.00" } },
    { "iPhone6,1", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "1136 x 640", "326", "4.00" } },
    { "iPhone6,2", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "1136 x 640", "326", "4.00" } },
    { "iPhone6,3", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "1136 x 640", "326", "4.00" } },
    { "iPad4,1", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1400", "PowerVR G6430", "4", "0", "2048 x 1536", "264", "9.70" } },
    { "iPad4,2", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1400", "PowerVR G6430", "4", "0", "2048 x 1536", "264", "9.70" } },
    { "iPad4,3", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1400", "PowerVR G6430", "4", "0", "2048 x 1536", "264", "9.70" } },
    { "iPad4,4", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
    { "iPad4,5", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
    { "iPad4,6", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
    { "iPhone7,2", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "1334 x 750", "326", "4.70" } },
    { "iPhone7,1", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "1920 x 1080", "401", "5.50" } },
    { "iPad5,3", { "2048", "Typhoon (Apple)", "ARMv8", "3", "1500", "PowerVR GXA6850", "8", "450", "2048 x 1536", "264", "9.70" } },
    { "iPad5,4", { "2048", "Typhoon (Apple)", "ARMv8", "3", "1500", "PowerVR GXA6850", "8", "450", "2048 x 1536", "264", "9.70" } },
    { "iPad4,7", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
    { "iPad4,8", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
    { "iPad4,9", { "1024", "Cyclone (Apple)", "ARMv8", "2", "1300", "PowerVR G6430", "4", "0", "2048 x 1536", "326", "7.90" } },
    { "iPod7,1", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1100", "PowerVR GX6450", "4", "450", "1334 x 750", "326", "4.70" } },
    { "AppleTV5,3", { "2048", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "..", "0", "0.00" } },
    { "iPad6,7", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2260", "PowerVR Series7", "12", "0", "2732 x 2048", "264", "12.90" } },
    { "iPad6,8", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2260", "PowerVR Series7", "12", "0", "2732 x 2048", "264", "12.90" } },
    { "iPad5,1", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "2048 x 1536", "264", "7.90" } },
    { "iPad5,2", { "1024", "Typhoon (Apple)", "ARMv8", "2", "1400", "PowerVR GX6450", "4", "450", "2048 x 1536", "264", "7.90" } },
    { "iPhone8,1", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "6", "0", "1334 x 750", "326", "4.70" } },
    { "iPhone8,2", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "6", "0", "1920 x 1080", "401", "5.50" } },
    { "iPhone8,4", { "2048", "Twister (Apple)", "ARMv8", "2", "1850", "PowerVR GT7600", "6", "0", "1136 x 640", "326", "4.00" } },
    { "iPad6,3", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2160", "PowerVR Series7", "12", "0", "2048 x 1536", "264", "9.70" } },
    { "iPad6,4", { "4096", "Twister (Apple)", "ARMv8-A", "2", "2160", "PowerVR Series7", "12", "0", "2048 x 1536", "264", "9.70" } },
    { "iPhone9,1", { "2048", "Twister (Apple)", "ARMv8", "0", "2340", "PowerVR GT7600 Plus", "6", "0", "1334 x 750", "326", "4.70" } },
    { "iPhone9,2", { "2048", "Twister (Apple)", "ARMv8", "0", "2340", "PowerVR GT7600 Plus", "6", "0", "1334 x 750", "326", "4.70" } },
    { "iPhone9,3", { "2048", "Twister (Apple)", "ARMv8", "0", "2340", "PowerVR GT7600 Plus", "6", "0", "1334 x 750", "326", "4.70" } },
    { "iPhone9,4", { "3072", "Twister (Apple)", "ARMv8", "0", "2340", "PowerVR GT7600 Plus", "6", "0", "1920 x 1080", "401", "5.50" } },
    { "iPhone9,5", { "3072", "Twister (Apple)", "ARMv8", "0", "2340", "PowerVR GT7600 Plus", "6", "0", "1920 x 1080", "401", "5.50" } },
    { "iPhone9,6", { "3072", "Twister (Apple)", "ARMv8", "0", "2340", "PowerVR GT7600 Plus", "6", "0", "1920 x 1080", "401", "5.50" } },

};


void AppDelegate::applicationDidLaunching()
{
	shared_Scheduler = Scheduler::getInstance();
    HTTPClient::initialize();

}

bool AppDelegate::applicationDidFinishLaunching() 
{
    std::error_code ec;
    std::string strErr = ec.message();
    ParseAppleDeviceData();
    auto modelIter = AppleDeviceData.find("iPhone7,2");
    if (modelIter != AppleDeviceData.end())
    {
        int cpufreq = atoi(modelIter->second[DeviceDataFeild::CPU_CLOCK].c_str());
    }


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
 	shared_Scheduler->update(1/60.f);
    HTTPClient::getInstance()->tick(1 / 60.f);
}


void runServer()
{

    network::ip::tcp::socket serverSocket;
    serverSocket.open(network::ip::tcp::v4());
    serverSocket.set_non_blocking(true);
    serverSocket.bind(network::ip::tcp::endpoint(ip::tcp::v4(), 4215));
    serverSocket.listen();

    std::vector<network::ip::tcp::socket> clientList;
    while (!bExitApp)
    {
        std::error_code ec;
        if (serverSocket.has_pending_accept(ec))
        {
            ip::tcp::endpoint endpoint;
            network::ip::tcp::socket clientSocket = serverSocket.accept(endpoint);
            LOG_INFO("===== Client: %d, ip:%s", clientSocket.native_handle(), endpoint.address().to_string().c_str());

            std::string sendtoClient = "Server Accept Client.";
            clientSocket.send(const_buffer(sendtoClient.c_str(), sendtoClient.size()));

            while (true)
            {
                if (clientSocket.available()>0)
                {
                    mutable_buffer mb;
                    mb.reallocate(128);
                    std::error_code ec;
                    clientSocket.receive(mb, ec);
                    if (ec)
                    {
                        LOG_ERROR("%s", ec.message().c_str());
                    }
                    LOG_INFO("Client Msg:%s", mb.c_str());
                    break;
                }

            }
            clientList.push_back(std::move(clientSocket));

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

}

void runClient()
{

    std::vector<network::ip::tcp::socket> clientList;
    while (!bExitApp)
    {
        network::ip::tcp::socket clientSocket;

        clientSocket.open(network::ip::tcp::v4());
        clientSocket.set_non_blocking(true);
        ip::tcp::endpoint endpoint(network::ip::address::from_string("127.0.0.1"), 4215);
        std::error_code ec;
        clientSocket.connect(endpoint, ec);
        if (ec)
        {
            LOG_ERROR("Connect: %s", ec.message().c_str());
        }
        else
        {
            while (true)
            {
                if (clientSocket.available()>0)
                {
                    mutable_buffer mb;
                    mb.reallocate(128);
                    clientSocket.receive(mb);
                    LOG_INFO("Server Msg:%s", mb.c_str());
                    std::string sendtoServer = " I receive your message.";
                    clientSocket.send(const_buffer(sendtoServer.c_str(), sendtoServer.size()));
                    break;
                }

            }
            clientList.push_back(std::move(clientSocket));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

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




