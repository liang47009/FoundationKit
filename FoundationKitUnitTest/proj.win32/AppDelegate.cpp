
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>

#include "AppDelegate.h"
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
#include "FoundationKit/Foundation/DelegateManager.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Base/type_cast.hpp"
#include "FoundationKit/Base/scope_guard.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"
#include "HTTPClient/HTTPClient.hpp"
#include "HTTPClient/HTTPCode.hpp"



using namespace std;
USING_NS_FK;

static TimerQueue  G_TimerQueue;
static bool bExitApp = false;

static std::thread clientThread;
static  std::thread serverThread;
void runServer();
void runClient();

void ParseAppleDeviceData();

AppDelegate::AppDelegate() 
{

}

AppDelegate::~AppDelegate() 
{

}


void AppDelegate::applicationDidLaunching()
{

}

template<typename T>
void printType()
{
    std::cout << typeid(T).name() << std::endl;

}

float(*castfunc)(std::string, int);
float free_function(const std::string&a, int b)
{
    return (float)a.size() / b;

}

struct AA
{
    int f(int a, int b) volatile{ return a + b; }
    int operator()(int)const { return 0; }

};

void TestFunctionTraits()
{
    FKLog("======= TestFunctionTraits =======");
    std::function<int(int)> fn = [](int a){ return a; };
    printType<function_traits<std::function<int(int)>>::function_type>();
    printType<function_traits<std::function<int(int)>>::args<0>::type>();
    printType<function_traits<decltype(fn)>::function_type>();
    printType<function_traits<decltype(free_function)>::function_type>();
    printType<function_traits<decltype(castfunc)>::function_type>();
    printType<function_traits<AA>::function_type>();
    using T = decltype(&AA::f);
    printType<T>();
    printType<function_traits<decltype(&AA::f)>::function_type>();

}


void TupleTest()
{

}

void TupleTestArgs(int a, const std::string& str, char* str1)
{

}


bool AppDelegate::applicationDidFinishLaunching() 
{
    std::error_code ec;
    std::string strErr = ec.message();
    std::unique_ptr<AppDelegate>  AppDelegatePtr;
    DelegateManager::GetInstance()->AddObserver("TupleTest", BindFunctionHandler(&TupleTest));
    DelegateManager::GetInstance()->Invoke("TupleTest", ArgumentList());
    ArgumentList args;
    args.emplace_back(10);
    args.emplace_back("Fuck you");
    args.emplace_back("Fuck you again");
    DelegateManager::GetInstance()->AddObserver("TupleTestArgs", BindFunctionHandler(&TupleTestArgs));
    DelegateManager::GetInstance()->Invoke("TupleTestArgs", args);


    int im_a_breakpoint = 0;

    //unsigned char cipherbuf[256];
    //unsigned char plainbuf[256];
    //unsigned char plaintext[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // our plaintext to encrypt
    //unsigned char secret[] = "A SECRET PASSWORD"; // 16 bytes long
    //int written = AES::aesEncrypt(cipherbuf, 256, plaintext, 26, secret);
    //written = AES::aesDecrypt(plainbuf, 256, cipherbuf, written, secret);

    //std::vector<std::string>  files;
    //FileUtils::GetInstance()->GetFilesFromDir("E:\\WorkSpace\\GameToolsGroup\\PAL\\Source\\Runtime", files, true);
    //std::string strMakefile;
    //auto fileutils = FileUtils::GetInstance();
    //for (auto filepath:files)
    //{
    //    if (fileutils->GetFileExtension(filepath) == ".cpp" 
    //        || fileutils->GetFileExtension(filepath) == ".c"
    //        || fileutils->GetFileExtension(filepath) == ".cc")
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
//
//void ParseAppleDeviceData()
//{
//    YExcel::BasicExcel           _basicExcel;
//    YExcel::BasicExcelWorksheet* _workSheet;
//    _basicExcel.Load("E:\\temp\\DeviceInfo.xls");
//    _workSheet = _basicExcel.GetWorksheet(0);
//
//    int rowIndex = 2;
//    std::string serialization;
//    auto GetExcelString = [&](int row, int col)
//    {
//        std::string result;
//        const char* value = _workSheet->Cell(row, col)->GetString();
//        if (value)
//        {
//            result = value;
//        }
//        else
//        {
//            const wchar_t* wvalue = _workSheet->Cell(row, col)->GetWString();
//            if (wvalue)
//            {
//                result = StringUtils::wstring2string(wvalue);
//            }
//        }
//        return result;
//    };
//    while (true)
//    {
//        std::string deviceModel = GetExcelString(rowIndex, 1);
//        if (deviceModel.empty())
//        {
//            break;
//        }
//
//        auto deviceModelList = StringUtils::split(deviceModel, " \" ");
//        size_t modelIndex = 0;
//        for (auto& strDeviceModel : deviceModelList)
//        {
//            std::string realModelName = strDeviceModel;
//            if (modelIndex > 0)
//            {
//                std::string tmpModel = deviceModelList[0];
//                realModelName = tmpModel.substr(0, tmpModel.size() - 3);
//                realModelName = StringUtils::trim(realModelName);
//                realModelName += strDeviceModel;
//            }
//            ++modelIndex;
//            serialization += "{\"";
//            serialization += realModelName;
//            serialization += "\",{";
//
//            serialization += "\"";
//            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 4)->GetInteger()); //RAM
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += GetExcelString(rowIndex, 7); //CPU NAME
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += GetExcelString(rowIndex, 8); //CPU arch
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 10)->GetInteger()); //CPU cores
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 11)->GetInteger()); //CPU clocks
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += GetExcelString(rowIndex, 16); //GPU NAME
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 17)->GetInteger()); //GPU cores
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 18)->GetInteger()); //GPU clocks
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += GetExcelString(rowIndex, 19); //  Screen Res 
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 20)->GetInteger()); //  PPI 
//            serialization += "\",";
//
//            serialization += "\"";
//            serialization += StringUtils::to_string(_workSheet->Cell(rowIndex, 21)->GetDouble()); //  Screen Size (inches)
//            serialization += "\"";
//
//            serialization += "}},\n";
//        }
//        ++rowIndex;
//    }
//}
//
//
//

