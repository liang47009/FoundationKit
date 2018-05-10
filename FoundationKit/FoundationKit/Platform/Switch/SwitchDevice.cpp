/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_SWITCH
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sstream>
#include "FoundationKit/Base/mutablebuf.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Math/Math.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"

NS_FK_BEGIN


std::string PlatformDevice::GetDeviceId()
{
    std::string strDeviceId;
    return strDeviceId;
}

std::string PlatformDevice::GetProduct()
{
    return "";
}

std::string PlatformDevice::GetHardware()
{
    return "";
}

std::string PlatformDevice::GetDevice()
{
    return "";
}

std::string PlatformDevice::GetBrandName()
{
    return "";
}

std::string PlatformDevice::GetModel()
{
    return "";
}

std::string PlatformDevice::GetManufacturer()
{
    return "";
}

std::string PlatformDevice::GetSystemVersion()
{
    return "";
}

std::string PlatformDevice::GetSDKVersion()
{
    return "";
}

std::string PlatformDevice::GetCPUBrand()
{
    static std::string CPUHardwareStr;
    return CPUHardwareStr;
}

std::string PlatformDevice::GetCPUVendor()
{
    return "";
}

std::string PlatformDevice::GetCPUArch()
{
    return 0;
}

int PlatformDevice::GetCPUCoreCount()
{
    return 8;
}

int PlatformDevice::GetCPUFrequency()
{
    return 0;
}

std::string PlatformDevice::GetGPUBrand()
{
    return "";
}


enum NetworkType
{
    NONE,
    NWIFI,
    N2G,
    N3G,
    N4G
};
int PlatformDevice::GetNetworkType()
{
    return NetworkType::NONE;;
}

std::string PlatformDevice::GetIpAddressV4()
{
    std::string ipaddressv4;
    return ipaddressv4;
}

std::string PlatformDevice::GetIpAddressV6()
{
    std::string ipaddressv6;
    return ipaddressv6;
}

std::string PlatformDevice::GetMacAddress()
{
    std::string MacAddressResult;
    return MacAddressResult;
}

PlatformDevice::string_list PlatformDevice::GetDNS()
{
    std::vector<std::string> dnslist;

    return dnslist;
}

long long PlatformDevice::GetTotalMemory()
{
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    long long physical_memory = memInfo.totalram * memInfo.mem_unit;
    return physical_memory;
}

long long PlatformDevice::GetAvailableMemory()
{
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    long long AvailableMemory = memInfo.freeram   * memInfo.mem_unit;
    AvailableMemory          += memInfo.freeswap  * memInfo.mem_unit;
    AvailableMemory          += memInfo.bufferram * memInfo.mem_unit;
    return AvailableMemory;
}

struct DisplayInfo
{
    bool bInit = false;
    Size appSize;
    Size realSize;
    int densityDpi;
    float fps;
    float nativeScale;
    float xdpi;
    float ydpi;
};
static DisplayInfo  GLOBAL_DisplayInfo;
static void lazyGetScreenResolution()
{
    if (!GLOBAL_DisplayInfo.bInit)
    {
        GLOBAL_DisplayInfo.bInit = true;
    }
}

Rect PlatformDevice::GetScreenResolution()
{
    lazyGetScreenResolution();
    return Rect(static_cast<float>(0)
        , static_cast<float>(0)
        , static_cast<float>(GLOBAL_DisplayInfo.appSize.Width)
        , static_cast<float>(GLOBAL_DisplayInfo.appSize.Height));
}

Rect PlatformDevice::GetScreenNativeResolution()
{
    lazyGetScreenResolution();
     return Rect(static_cast<float>(0)
        , static_cast<float>(0)
        , static_cast<float>(GLOBAL_DisplayInfo.realSize.Width)
        , static_cast<float>(GLOBAL_DisplayInfo.realSize.Height));
}

float PlatformDevice::GetScreenDPI()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.densityDpi;
}

float PlatformDevice::GetScreenXDPI()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.xdpi;
}

float PlatformDevice::GetScreenYDPI()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.ydpi;
}

float PlatformDevice::GetRefreshRate()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.fps;
}

float PlatformDevice::GetNativeScale()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.nativeScale;
}

PlatformMemoryConstants& PlatformDevice::GetMemoryConstants()
{
    static PlatformMemoryConstants MemoryConstants;

    // Gather platform memory stats.
    struct sysinfo SysInfo;
    unsigned long long MaxPhysicalRAMBytes = 0;
    unsigned long long MaxVirtualRAMBytes = 0;

    if (0 == sysinfo(&SysInfo))
    {
        MaxPhysicalRAMBytes = static_cast<unsigned long long>(SysInfo.mem_unit) * static_cast<unsigned long long>(SysInfo.totalram);
        MaxVirtualRAMBytes = static_cast<unsigned long long>(SysInfo.mem_unit) * static_cast<unsigned long long>(SysInfo.totalswap);
    }

    MemoryConstants.TotalPhysical = MaxPhysicalRAMBytes;
    MemoryConstants.TotalVirtual = MaxVirtualRAMBytes;
    MemoryConstants.PageSize = sysconf(_SC_PAGESIZE);
    // sanitize stats as sometimes peak < used for some reason
    MemoryConstants.PeakUsedVirtual = Math::Max(MemoryConstants.PeakUsedVirtual, MemoryConstants.UsedVirtual);
    MemoryConstants.PeakUsedPhysical = Math::Max(MemoryConstants.PeakUsedPhysical, MemoryConstants.UsedPhysical);
    return MemoryConstants;
}

void PlatformDevice::DumpDeviceInfo()
{
    FKLog("============ Device Info===============");
    std::ostringstream ss;
    ss << "GetDeviceId:" << GetDeviceId() << "\n";
    //FKLog("============ GetDeviceId===============");
    ss << "GetProduct:" << GetProduct() << "\n";
    //FKLog("============ GetProduct===============");
    ss << "GetHardware:" << GetHardware() << "\n";
    //FKLog("============ GetHardware===============");
    ss << "GetDevice:" << GetDevice() << "\n";
    //FKLog("============ GetDevice===============");
    ss << "GetModel:" << GetModel() << "\n";
    //FKLog("============ GetModel===============");
    ss << "GetManufacturer:" << GetManufacturer() << "\n";
    //FKLog("============ GetManufacturer===============");
    ss << "GetSystemVersion:" << GetSystemVersion() << "\n";
    //FKLog("============ GetSystemVersion===============");
    ss << "GetSDKVersion:" << GetSDKVersion() << "\n";
    //FKLog("============ GetSDKVersion===============");
    ss << "GetCPUBrand:" << GetCPUBrand() << "\n";
    //FKLog("============ GetCPUBrand===============");
    ss << "GetCPUVendor:" << GetCPUVendor() << "\n";
    //FKLog("============ GetCPUVendor===============");
    ss << "GetCPUCoreCount:" << GetCPUCoreCount() << "\n";
    //FKLog("============ GetCPUCoreCount===============");
    ss << "GetCPUFrequency:" << GetCPUFrequency() << "\n";
    //FKLog("============ GetCPUFrequency===============");
    ss << "GetNetworkType:" << GetNetworkType() << " 1 WIFI,2 2G,3 3G,4 4G,0 other. \n";
    //FKLog("============ GetNetworkType===============");
    ss << "GetIpAddressV4:" << GetIpAddressV4() << "\n";
    //FKLog("============ GetIpAddressV4===============");
    ss << "GetIpAddressV6:" << GetIpAddressV6() << "\n";
    //FKLog("============ GetIpAddressV6===============");
    ss << "GetMacAddress:" << GetMacAddress() << "\n";
    //FKLog("============ GetMacAddress===============");
    auto dnss = GetDNS();
    for (auto dns : dnss)
    {
        ss << "GetDNS:" << dns << "\n";
    }
    ss << "GetTotalMemory:" << GetTotalMemory() << " bytes\n";
    ss << "GetAvailableMemory:" << GetAvailableMemory() << "bytes\n";

    /// Get the screen resolution, not including the virtual button area
    Rect Resolution = GetScreenResolution();
    ss << "GetScreenResolution:" << Resolution.size.Width << "*" << Resolution.size.Height << "\n";
    /// Get the screen resolution, including the virtual button area
    Resolution = GetScreenNativeResolution();
    ss << "GetScreenNativeResolution:" << Resolution.size.Width << "*" << Resolution.size.Height << "\n";
    ss << "GetScreenDPI:" << GetScreenDPI() << "\n";
    ss << "GetRefreshRate:" << GetRefreshRate() << "\n";
    ss << "GetScreenXDPI:" << GetScreenXDPI() << "\n";
    ss << "GetScreenYDPI:" << GetScreenYDPI() << "\n";
    ss << "GetNativeScale:" << GetNativeScale() << "\n";

    PlatformMemoryConstants memConstants = GetMemoryConstants();
    ss << "PageSize:" << memConstants.PageSize << "\n";
    ss << "TotalPhysical:" << memConstants.TotalPhysical << "\n";
    ss << "TotalVirtual:" << memConstants.TotalVirtual << "\n";
    ss << "AddressLimit:" << memConstants.AddressLimit << "\n";
    ss << "AvailablePhysical:" << memConstants.AvailablePhysical << "\n";
    ss << "AvailableVirtual:" << memConstants.AvailableVirtual << "\n";
    ss << "UsedPhysical:" << memConstants.UsedPhysical << "\n";
    ss << "PeakUsedPhysical:" << memConstants.PeakUsedPhysical << "\n";
    ss << "UsedVirtual:" << memConstants.UsedVirtual << "\n";
    ss << "PeakUsedVirtual:" << memConstants.PeakUsedVirtual << "\n";
    FKLog(ss.str().c_str());
}

bool PlatformDevice::IsDebuggerPresent()
{
    return false;
}

std::string PlatformDevice::ExecuteSystemCommand(const std::string& command)
{
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        char buffer[256] = { 0 };
        //while (!feof(pipe))
        //{
        //    if (fgets(buffer, 256, pipe) != NULL)
        //        result += buffer;
        //}
        for(;;)
        {
            size_t NumRead = fread(buffer, sizeof(char), sizeof(buffer), pipe);
            if (NumRead == 0)
            {
                if (errno == EINTR)
                    continue;
                if (feof(pipe) == 0) // read file error.
                {
                    FKLog("Error while execute command %s: %s\n", command.c_str(), strerror(errno));
                }
                break;
            }
            result += buffer;
        }
    }
    catch (...)
    {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

bool PlatformDevice::ScreenCapture(std::string& outSavePath)
{
    return false;
}
NS_FK_END

#endif //#if PLATFORM_SWITCH

