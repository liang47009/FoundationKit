

#include <sstream>
#include <stdlib.h>
#include <mach/machine.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/Exception.h"
#import <UIKit/UIKit.h>

NS_FK_BEGIN

namespace PlatformHelper {
    
    std::string getHardwareProperty(const char* name)
    {
        size_t size;
        sysctlbyname(name, NULL, &size, NULL, 0);
        char *machine = new char[size+1];
        memset(machine, 0, size+1);
        sysctlbyname(name, machine, &size, NULL, 0);
        std::string platformName = machine;
        delete[] machine;
        return platformName;
    }
    
    //code from https://github.com/fahrulazmi/UIDeviceHardware
    std::string platformString()
    {
        std::string platformName = getHardwareProperty("hw.machine");
        if (platformName == "iPhone1,1")    return "iPhone 1G";
        if (platformName == "iPhone1,2")    return "iPhone 3G";
        if (platformName == "iPhone2,1")    return "iPhone 3GS";
        if (platformName == "iPhone3,1")    return "iPhone 4 (GSM)";
        if (platformName == "iPhone3,3")    return "iPhone 4 (CDMA)";
        if (platformName == "iPhone4,1")    return "iPhone 4S";
        if (platformName == "iPhone5,1")    return "iPhone 5 (GSM)";
        if (platformName == "iPhone5,2")    return "iPhone 5 (CDMA)";
        if (platformName == "iPhone5,3")    return "iPhone 5c";
        if (platformName == "iPhone5,4")    return "iPhone 5c";
        if (platformName == "iPhone6,1")    return "iPhone 5s";
        if (platformName == "iPhone6,2")    return "iPhone 5s";
        if (platformName == "iPhone7,1")    return "iPhone 6 Plus";
        if (platformName == "iPhone7,2")    return "iPhone 6";
        if (platformName == "iPhone8,1")    return "iPhone 6s";
        if (platformName == "iPhone8,2")    return "iPhone 6s Plus";
        
        if (platformName == "iPod1,1")      return "iPod Touch 1G";
        if (platformName == "iPod2,1")      return "iPod Touch 2G";
        if (platformName == "iPod3,1")      return "iPod Touch 3G";
        if (platformName == "iPod4,1")      return "iPod Touch 4G";
        if (platformName == "iPod5,1")      return "iPod Touch 5G";
        if (platformName == "iPod7,1")      return "iPod Touch 6G";
        
        if (platformName == "iPad1,1")      return "iPad";
        if (platformName == "iPad2,1")      return "iPad 2 (WiFi)";
        if (platformName == "iPad2,2")      return "iPad 2 (GSM)";
        if (platformName == "iPad2,3")      return "iPad 2 (CDMA)";
        if (platformName == "iPad2,4")      return "iPad 2 (WiFi)";
        if (platformName == "iPad2,5")      return "iPad Mini (WiFi)";
        if (platformName == "iPad2,6")      return "iPad Mini (GSM)";
        if (platformName == "iPad2,7")      return "iPad Mini (CDMA)";
        if (platformName == "iPad3,1")      return "iPad 3 (WiFi)";
        if (platformName == "iPad3,2")      return "iPad 3 (CDMA)";
        if (platformName == "iPad3,3")      return "iPad 3 (GSM)";
        if (platformName == "iPad3,4")      return "iPad 4 (WiFi)";
        if (platformName == "iPad3,5")      return "iPad 4 (GSM)";
        if (platformName == "iPad3,6")      return "iPad 4 (CDMA)";
        if (platformName == "iPad4,1")      return "iPad Air (WiFi)";
        if (platformName == "iPad4,2")      return "iPad Air (GSM)";
        if (platformName == "iPad4,3")      return "iPad Air (CDMA)";
        if (platformName == "iPad4,4")      return "iPad Mini Retina (WiFi)";
        if (platformName == "iPad4,5")      return "iPad Mini Retina (Cellular)";
        if (platformName == "iPad4,7")      return "iPad Mini 3 (WiFi)";
        if (platformName == "iPad4,8")      return "iPad Mini 3 (Cellular)";
        if (platformName == "iPad4,9")      return "iPad Mini 3 (Cellular)";
        if (platformName == "iPad5,1")      return "iPad Mini 4 (WiFi)";
        if (platformName == "iPad5,2")      return "iPad Mini 4 (Cellular)";
        if (platformName == "iPad5,3")      return "iPad Air 2 (WiFi)";
        if (platformName == "iPad5,4")      return "iPad Air 2 (Cellular)";
        if (platformName == "iPad6,7")      return "iPad Pro (WiFi)";
        if (platformName == "iPad6,8")      return "iPad Pro (Cellular)";
        
        if (platformName == "i386")         return getHardwareProperty("hw.model");
        if (platformName == "x86_64")       return getHardwareProperty("hw.model");
        
        return platformName;
    }
    
} //namespace PlatformHelper


std::string Environment::GetEnvironmentVariable(const std::string& variable)
{
    const char* val = getenv(variable.c_str());
    if (val)
        return std::string(val);
    else
        return "";//throw NotFoundException(variable);
}

bool Environment::HasEnvironmentVariable(const std::string& variable)
{
    return getenv(variable.c_str()) != 0;
}

void Environment::SetEnvironmentVariable(const std::string& variable, const std::string& value)
{
    std::string var = variable;
    var.append("=");
    var.append(value);
    if (putenv((char*)var.c_str()))
    {
        std::string msg = "cannot set environment variable: ";
        msg.append(variable);
        throw SystemException(msg);
    }
}


std::string Environment::GetMachineName()
{
//    struct utsname systemInfo;
//    uname(&systemInfo);
//    std::string devName = systemInfo.machine;
    return PlatformHelper::platformString();
}


std::string Environment::GetOSVersion()
{
    int mib[2] = {CTL_KERN, KERN_OSVERSION};
    u_int nameLen = sizeof(mib)/ sizeof(mib[0]);
    size_t bufferSize= 0;
    sysctl(mib, nameLen, nullptr, &bufferSize, NULL, 0);
    
    NSString* systemVersion= [UIDevice currentDevice].systemVersion;
    std::string strVersion = [systemVersion UTF8String];
    char *sysVersion = new char[bufferSize+1];
    memset(sysVersion, 0, bufferSize+1);
    int result = sysctl(mib, nameLen, sysVersion, &bufferSize, NULL, 0);
    if (result >= 0)
    {
        strVersion += "(";
        strVersion += sysVersion;
        strVersion += ")";
    }
    delete[] sysVersion;
    return strVersion;
}



NS_FK_END




