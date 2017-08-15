#include "FoundationKit/GenericPlatformMacros.hpp"
#if (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include <windows.h>
#include <WindowsX.h>
#include <psapi.h>
#include <vector>
#include <memory>
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include <array>
#include <d3d9.h>
#pragma   comment(lib,"d3d9.lib")
#pragma   comment(lib,"Psapi.lib")
// for getMacAddressUseGetAdaptersInfo
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


NS_FK_BEGIN

namespace detail
{
    std::vector<uint8> GetMacAddressRaw()
    {
        std::vector<uint8> result;
        result.resize(6);
        IP_ADAPTER_INFO IpAddresses[16];
        ULONG OutBufferLength = sizeof(IP_ADAPTER_INFO) * 16;
        // Read the adapters
        uint32 RetVal = GetAdaptersInfo(IpAddresses, &OutBufferLength);
        if (RetVal == NO_ERROR)
        {
            PIP_ADAPTER_INFO AdapterList = IpAddresses;
            // Walk the set of addresses copying each one
            while (AdapterList)
            {
                // If there is an address to read
                if (AdapterList->AddressLength > 0)
                {
                    //result.resize(AdapterList->AddressLength);
                    std::memcpy(result.data(), AdapterList->Address, result.size());
                    break;
                }
                AdapterList = AdapterList->Next;
            }
        }
        return result;
    }

    std::string GetMacAddress()
    {
        std::vector<uint8> macVec = GetMacAddressRaw();
        std::string result;
        // Copy the data and say we did
        result = StringUtils::Format("%02X%02X%02X%02X%02X%02X"
            , macVec[0]
            , macVec[1]
            , macVec[2]
            , macVec[3]
            , macVec[4]
            , macVec[5]);
        return result;
    }
} // namespace detail


std::string PlatformDevice::GetDeviceId()
{
    return MD5::md5_hash_hex(detail::GetMacAddress().c_str());
}

typedef LONG(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
std::string PlatformDevice::GetProduct()
{
    RTL_OSVERSIONINFOEXW verInfo = { 0 };
    verInfo.dwOSVersionInfoSize = sizeof(verInfo);
    static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
    if (RtlGetVersion != 0)
        RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo);

    switch (verInfo.dwMajorVersion)
    {
    case 10:
        switch (verInfo.dwMinorVersion)
        {
        case 0:
            return verInfo.wProductType == VER_NT_WORKSTATION ? "Windows 10" : "Windows Server 2016";
        }
    case 6:
        switch (verInfo.dwMinorVersion)
        {
        case 0:
            return verInfo.wProductType == VER_NT_WORKSTATION ? "Windows Vista" : "Windows Server 2008";
        case 1:
            return verInfo.wProductType == VER_NT_WORKSTATION ? "Windows 7" : "Windows Server 2008 R2";
        case 2:
            return verInfo.wProductType == VER_NT_WORKSTATION ? "Windows 8" : "Windows Server 2012";
        case 3:
            return verInfo.wProductType == VER_NT_WORKSTATION ? "Windows 8.1" : "Windows Server 2012 R2";
        default:
            return "Unknown";
        }
    case 5:
        switch (verInfo.dwMinorVersion)
        {
        case 0:
            return "Windows 2000";
        case 1:
            return "Windows XP";
        case 2:
            return "Windows Server 2003/Windows Server 2003 R2";
        default:
            return "Unknown";
        }
    default:
        return "Unknown";
    }
}

std::string PlatformDevice::GetHardware()
{
    return "Windows";
}


std::string PlatformDevice::GetDevice()
{
    wchar_t wzComputerName[256] = { 0 };
    DWORD dwNameLenght = sizeof(wzComputerName) / sizeof(wzComputerName[0]);

    if (0 == GetComputerNameW(wzComputerName, &dwNameLenght))
    {
        FKLog("ERROR: GetComputerName failed with %d!\n", GetLastError());
    }
    std::string strComputerName = StringUtils::wstring2UTF8string(wzComputerName);
    // dwNameLenght is the length of wzComputerName without NULL 
    if (dwNameLenght < 0 || dwNameLenght >(sizeof(wzComputerName) / sizeof(wzComputerName[0]) - 1))
    {
        FKLog("ERROR: GetComputerName returned %s with dwNameLenght = %u whereas the passed in buffer size is %d!\n", strComputerName.c_str(), dwNameLenght, sizeof(wzComputerName) / sizeof(wzComputerName[0]));
        return "";
    }
    // dwNameLenght is the length of wzComputerName without NULL
    if (dwNameLenght != wcslen(wzComputerName))
    {
        FKLog("ERROR: GetComputerName returned %s of length %d which is not equal to dwSize = %u!\n", strComputerName.c_str(), wcslen(wzComputerName), dwNameLenght);
        return "";
    }
    return StringUtils::wstring2UTF8string(wzComputerName);
}

std::string PlatformDevice::GetBrandName()
{
    // https://msdn.microsoft.com/en-us/library/hskdteyh.aspx
    // https://en.wikipedia.org/wiki/CPUID
    std::array<int, 4> cpui;
    // Calling __cpuid with 0x80000000 as the function_id argument  
    // gets the number of the highest valid extended ID.  
	__cpuidex(cpui.data(), 0x80000000,0);
    int nExIds = cpui[0];
    char brand[64] = {0};
    std::vector<std::array<int, 4>> extdata;
    for (int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuidex(cpui.data(), i, 0);
        extdata.push_back(cpui);
    }
    // Interpret CPU brand string if reported  
    if (nExIds >= 0x80000004)
    {
        memcpy(brand, extdata[2].data(), sizeof(cpui));
        memcpy(brand + 16, extdata[3].data(), sizeof(cpui));
        memcpy(brand + 32, extdata[4].data(), sizeof(cpui));
    }
    return brand;
}

std::string PlatformDevice::GetModel()
{
    return "";
}

std::string PlatformDevice::GetManufacturer()
{
    // https://msdn.microsoft.com/en-us/library/hskdteyh.aspx
    // https://en.wikipedia.org/wiki/CPUID
    std::array<int, 4> cpui;
	__cpuidex(cpui.data(), 0,0);
    int nIds = cpui[0];
    std::vector<std::array<int, 4>> data;
    for (int i = 0; i <= nIds; ++i)
    {
        __cpuidex(cpui.data(), i, 0);
        data.push_back(cpui);
    }
    char vendor[32] = {0};
    *reinterpret_cast<int*>(vendor) = data[0][1];
    *reinterpret_cast<int*>(vendor + 4) = data[0][3];
    *reinterpret_cast<int*>(vendor + 8) = data[0][2];
    return vendor;
}

typedef LONG(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
std::string PlatformDevice::GetSystemVersion()
{
    RTL_OSVERSIONINFOEXW verInfo = { 0 };
    verInfo.dwOSVersionInfoSize = sizeof(verInfo);
    static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
    if (RtlGetVersion != 0 && RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
    {
        std::ostringstream str;
        str << verInfo.dwMajorVersion << "." << verInfo.dwMinorVersion << " (Build " << (verInfo.dwBuildNumber & 0xFFFF);
        if (verInfo.szCSDVersion[0]) str << ": " << verInfo.szCSDVersion;
        str << ")";
        return str.str();
    }
    else
    {
        //throw SystemException("Cannot get OS version information");
        FKLog("***** Cannot get OS version information.");
    }
    return "";
}

std::string PlatformDevice::GetSDKVersion()
{
    return "";
}

std::string PlatformDevice::GetRendererVersion()
{
    char* szOpenGLVersionString = (char*)glGetString(GL_VERSION);
    return szOpenGLVersionString ? szOpenGLVersionString : "";
}

std::string PlatformDevice::GetCPUArch()
{
    SYSTEM_INFO stInfo;
    GetSystemInfo(&stInfo);
    //GetNativeSystemInfo(&stInfo);
    WORD procArch = stInfo.wProcessorArchitecture;
    if (procArch == PROCESSOR_ARCHITECTURE_INTEL)         return "Intel x86";
    if (procArch == PROCESSOR_ARCHITECTURE_MIPS)          return "mips";
    if (procArch == PROCESSOR_ARCHITECTURE_ALPHA)         return "alpha x86";
    if (procArch == PROCESSOR_ARCHITECTURE_PPC)           return "ppc";
    if (procArch == PROCESSOR_ARCHITECTURE_SHX)           return "shx";
    if (procArch == PROCESSOR_ARCHITECTURE_ARM)           return "arm";
    if (procArch == PROCESSOR_ARCHITECTURE_IA64)          return "Intel x64";
    if (procArch == PROCESSOR_ARCHITECTURE_ALPHA64)       return "alpha x64";
    if (procArch == PROCESSOR_ARCHITECTURE_MSIL)          return "msil";
    if (procArch == PROCESSOR_ARCHITECTURE_AMD64)         return "amd64";
    if (procArch == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64) return "wow64";
    if (procArch == PROCESSOR_ARCHITECTURE_UNKNOWN)       return "Intel x86";
    return "Unknown";
}

int PlatformDevice::GetCPUCoreCount()
{
    SYSTEM_INFO SystemInfo;
    memset(&SystemInfo, 0, sizeof(SystemInfo));
    ::GetSystemInfo(&SystemInfo);
    return SystemInfo.dwNumberOfProcessors;
}

int PlatformDevice::GetCPUFrequency()
{
    return 0;
}

int PlatformDevice::GetNetworkType()
{
    return 0;
}

std::string PlatformDevice::GetIpAddressV4()
{
    std::string ipaddressv4;

    return StringUtils::Trim(ipaddressv4);
}

std::string PlatformDevice::GetIpAddressV6()
{
    std::string ipaddressv6;

    return StringUtils::Trim(ipaddressv6);
}

PlatformDevice::string_list PlatformDevice::GetDNS()
{
    std::vector<std::string> dnslist;

    return dnslist;
}

long long PlatformDevice::GetTotalMemory()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG physical_memory = memInfo.ullTotalPhys;
    return physical_memory;
}

long long PlatformDevice::GetAvailableMemory()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG free_memory = memInfo.ullAvailPhys;
    return free_memory;
}

std::string PlatformDevice::GetGPURenderer()
{
    std::string gpuname = "N/A";
    IDirect3D9* pHandle = Direct3DCreate9(D3D_SDK_VERSION);
    if (NULL == pHandle)
    {
        return gpuname;
    }
    gpuname = "";
    size_t GPUCount = pHandle->GetAdapterCount();
    for (unsigned int i = 0; i < GPUCount; ++i)
    {
        D3DADAPTER_IDENTIFIER9 ident;
        if (SUCCEEDED(pHandle->GetAdapterIdentifier(i, 0, &ident)))
        {
            if (!gpuname.empty())
            {
                gpuname += "|";
            }
            gpuname += ident.Description;
        }
    }
    pHandle->Release();
    return gpuname;
}

static std::string vendor_from_name(const std::string& v)
{
    if (v.find("NVidia") != std::string::npos || v.find("NVIDIA") != std::string::npos)
        return "NVIDIA";
    else if (v.find("AMD") != std::string::npos || v.find("ATi") != std::string::npos || v.find("Advanced Micro Devices") != std::string::npos)
        return "AMD";
    else if (v.find("Intel") != std::string::npos)
        return "Intel";
    else if (v.find("Microsoft") != std::string::npos)
        return "Microsoft";
    else if (v.find("Qualcomm") != std::string::npos)
        return "Qualcomm";
    else
        return "unknown";
}

std::string PlatformDevice::GetGPUVendor()
{
    return vendor_from_name(GetGPURenderer());
}

Rect PlatformDevice::GetScreenResolution()
{
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);
    return Rect(static_cast<float>(0)
        , static_cast<float>(0)
        , static_cast<float>(width)
        , static_cast<float>(height));
}

Rect PlatformDevice::GetScreenNativeResolution()
{
    int dwWidth = GetSystemMetrics(SM_CXSCREEN);
    int dwHeight = GetSystemMetrics(SM_CYSCREEN);
    return Rect(static_cast<float>(0)
        , static_cast<float>(0)
        , static_cast<float>(dwWidth)
        , static_cast<float>(dwHeight));
}

float PlatformDevice::GetScreenDPI()
{
    static float dpi = 0.0f;
    if (dpi < 1.0f)
    {
        HDC hScreenDC = GetDC(nullptr);
        int PixelsX = GetDeviceCaps(hScreenDC, HORZRES);
        int MMX = GetDeviceCaps(hScreenDC, HORZSIZE);
        ReleaseDC(nullptr, hScreenDC);
        dpi = 255.0f*PixelsX / MMX / 10.0f;
    }
    return dpi;
}

float PlatformDevice::GetRefreshRate()
{
    return 60.0f;
}

float PlatformDevice::GetScreenXDPI()
{
    HDC screen = GetDC(0);
    int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(0, screen);
    return static_cast<float>(dpiX);
}

float PlatformDevice::GetScreenYDPI()
{
    HDC screen = GetDC(0);
    int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
    ReleaseDC(0, screen);
    return static_cast<float>(dpiY);
}

float PlatformDevice::GetNativeScale()
{
    return 1.0f;
}

PlatformMemoryConstants& PlatformDevice::GetMemoryConstants()
{
    static PlatformMemoryConstants MemoryConstants;

    // Gather platform memory stats.
    MEMORYSTATUSEX MemoryStatusEx;
    memset(&MemoryStatusEx, 0,sizeof(MemoryStatusEx));
    MemoryStatusEx.dwLength = sizeof(MemoryStatusEx);
    ::GlobalMemoryStatusEx(&MemoryStatusEx);

    PROCESS_MEMORY_COUNTERS ProcessMemoryCounters;
    memset(&ProcessMemoryCounters, 0, sizeof(ProcessMemoryCounters));
    ::GetProcessMemoryInfo(::GetCurrentProcess(), &ProcessMemoryCounters, sizeof(ProcessMemoryCounters));

    SYSTEM_INFO SystemInfo;
    memset(&SystemInfo, 0, sizeof(SystemInfo));
    ::GetSystemInfo(&SystemInfo);

    MemoryConstants.TotalPhysical     = MemoryStatusEx.ullTotalPhys;
    MemoryConstants.TotalVirtual      = MemoryStatusEx.ullTotalVirtual;
    MemoryConstants.PageSize          = SystemInfo.dwAllocationGranularity;	// Use this so we get larger 64KiB pages, instead of 4KiB
    MemoryConstants.AvailablePhysical = MemoryStatusEx.ullAvailPhys;
    MemoryConstants.AvailableVirtual  = MemoryStatusEx.ullAvailVirtual;
    MemoryConstants.UsedPhysical      = ProcessMemoryCounters.WorkingSetSize;
    MemoryConstants.UsedVirtual       = ProcessMemoryCounters.PagefileUsage;
    MemoryConstants.PeakUsedPhysical  = ProcessMemoryCounters.PeakWorkingSetSize;
    MemoryConstants.PeakUsedVirtual   = ProcessMemoryCounters.PeakPagefileUsage;
    return MemoryConstants;
}

NS_FK_END

#endif // OF #if (TARGET_PLATFORM == PLATFORM_WINDOWS)
