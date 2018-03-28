
#include <string>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>
#include <utility>

#include "AppDelegate.h"

#include "FoundationKit/forward.hpp"
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/GenericPlatformMacros.hpp"

#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/Base58.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/crc.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Crypto/sha1.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/BitStream.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/Version.hpp"

#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"

#include "FoundationKit/Math/Math.hpp"
#include "FoundationKit/Math/MathContent.hpp"
#include "FoundationKit/Math/MathUtil.hpp"
#include "FoundationKit/Math/Matrix.hpp"
#include "FoundationKit/Math/Quaternion.hpp"
#include "FoundationKit/Math/Rect.hpp"
#include "FoundationKit/Math/Size.hpp"
#include "FoundationKit/Math/Vector2.hpp"
#include "FoundationKit/Math/Vector3.hpp"
#include "FoundationKit/Math/Vector4.hpp"

using namespace std;
USING_NS_FK;


static bool bExitApp = false;

AppDelegate::AppDelegate() 
{

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
    PlatformDevice::DumpDeviceInfo();
    std::string DocumentsPath = Path::GetDocumentsPath();
    FKLog("windows 平台支持OpenGL 和OpenGL ES,如果使用OpenGL，请链接opengl32.lib，如果使用OpenGL ES，请链接 libGLESv2.lib");
    FKLog(">>> applicationDidFinishLaunching end.");
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
}

void AppDelegate::mainLoop()
{

}

void AppDelegate::TestTupleArgs(int a, const std::string & str, const char * str1)
{
}



void RunGuetzli()
{
    auto LaunchArgs = Environment::GetCommandLineArgs();
    if (LaunchArgs.size() < 3)return;
    std::string SrcPath = LaunchArgs[1];
    std::string DesPath = LaunchArgs[2];
    Directory::GetFiles(SrcPath, [&](const std::string& fileFullPath)
    {
        std::string FileExtension = Path::GetExtension(fileFullPath);
        if (FileExtension.compare(".jpg") == 0 ||
            FileExtension.compare(".jpeg") == 0 ||
            FileExtension.compare(".png") == 0)
        {
            std::string RelativePath = Path::GetRelativePath(SrcPath, fileFullPath);
            std::string DesFullPath = DesPath + RelativePath;
            std::string DesDir = Path::GetPath(DesFullPath);
            Directory::Create(DesDir);
            std::string command = "cd E:\\GitHub\\FoundationKit\\Win32\\Release\\ && .\\guetzli.exe --cuda --quality 80 ";
            command += fileFullPath;
            command += " ";
            command += DesFullPath;
            auto result = PlatformDevice::ExecuteSystemCommand(command.c_str());
            FKLog("%s", result.c_str());
        }
        return false;
    }, Directory::ESearchOption::AllDirectories);
}
