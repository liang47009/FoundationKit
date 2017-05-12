
#include <windows.h>
#include <WindowsX.h>
#include <psapi.h>
#include <vector>
#include <memory>

#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Crypto/md5.hpp"

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
        result = StringUtils::format("%02X%02X%02X%02X%02X%02X"
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
    return "";
}


std::string PlatformDevice::GetDevice()
{
    return GetProduct();
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
        LOG_ERROR("***** Cannot get OS version information.");
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
    return 0;
}

int PlatformDevice::GetCPUMaxFreq(int cpuIndex/* = -1*/)
{
    return 0;
}

int PlatformDevice::GetCPUCurFreq(int cpuIndex/* = -1*/)
{
    return 0;
}

int PlatformDevice::GetCPUMinFreq(int cpuIndex/* = -1*/)
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

    return StringUtils::trim(ipaddressv4);
}

std::string PlatformDevice::GetIpAddressV6()
{
    std::string ipaddressv6;

    return StringUtils::trim(ipaddressv6);
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
    char* szRenderer = (char*)glGetString(GL_RENDERER);
    return szRenderer ? szRenderer : "";
}

std::string PlatformDevice::GetGPUVendor()
{
    char* szVendor = (char*)glGetString(GL_VENDOR);
    return szVendor ? szVendor : "";
}

Size PlatformDevice::GetScreenResolution()
{
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);
    return Size(static_cast<float>(width), static_cast<float>(height));
}

Size PlatformDevice::GetScreenNativeResolution()
{
    int dwWidth = GetSystemMetrics(SM_CXSCREEN);
    int dwHeight = GetSystemMetrics(SM_CYSCREEN);
    return Size(static_cast<float>(dwWidth), static_cast<float>(dwHeight));
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
        dpi = 255*PixelsX / MMX / 10;
    }
    return dpi;
}

float PlatformDevice::GetScreenFPS()
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

#define USER_POPEN 0
std::string PlatformDevice::ExecuteSystemCommand(const std::string& command)
{
#if USER_POPEN
    char buffer[128] = { 0 };
    std::string result = "";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe))
        {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    }
    catch (...)
    {
        _pclose(pipe);
        throw;
    }
    _pclose(pipe);
    return result;
#else
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead, hWrite;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return "CreatePipe falied.";
    }
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFOA);

    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    std::string finalCommand = "cmd.exe /C ";
    finalCommand += command;

    BOOL ret = ::CreateProcessA(NULL, const_cast<char*>(finalCommand.c_str()), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
    if (ret)
    {
        CloseHandle(hWrite);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

    }
    else
    {
        return "CreateProcessA Falied.";
    }

    const size_t buffer_size = 128;
    char buffer[buffer_size] = { 0 };
    std::string result = "";
    DWORD byteRead = 0;
    while (true)
    {
        if (ReadFile(hRead, buffer, buffer_size, &byteRead, NULL) == NULL)
        {
            break;
        }
        result.append(buffer, byteRead);
    }

    CloseHandle(hRead);
    return result;
#endif
}

NS_FK_END
