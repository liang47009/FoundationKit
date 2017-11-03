#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_WINDOWS)
#include <windows.h>
#include <WindowsX.h>
#include <vector>
#include <memory>
#include <sstream>

#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Base/mutablebuf.hpp"
#include "FoundationKit/Crypto/md5.hpp"

#include <psapi.h>
#pragma   comment(lib,"Psapi.lib")

#include <d3d9.h>
#pragma   comment(lib,"d3d9.lib")

#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib") // for GetAdaptersInfo

#include <Powrprof.h>
#pragma comment(lib, "PowrProf.lib") // for CallNtPowerInformation

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
                    std::memcpy(&result[0], AdapterList->Address, result.size());
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
    static fnRtlGetVersion RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
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
	return "Windows";
}

std::string PlatformDevice::GetModel()
{
    return "";
}

std::string PlatformDevice::GetManufacturer()
{
	return "Windows";
}

typedef LONG(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
std::string PlatformDevice::GetSystemVersion()
{
    RTL_OSVERSIONINFOEXW verInfo = { 0 };
    verInfo.dwOSVersionInfoSize = sizeof(verInfo);
    static fnRtlGetVersion RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
    if (RtlGetVersion != 0 && RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
    {
        //std::ostringstream str;
        //str << verInfo.dwMajorVersion << "." << verInfo.dwMinorVersion << " (Build " << (verInfo.dwBuildNumber & 0xFFFF);
        //if (verInfo.szCSDVersion[0]) str << ": " << verInfo.szCSDVersion;
        //str << ")";
		Version version(verInfo.dwMajorVersion, verInfo.dwMinorVersion, (verInfo.dwBuildNumber & 0xFFFF));
        return version.ToString();
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
//
//std::string PlatformDevice::GetRendererVersion()
//{
//    char* szOpenGLVersionString = (char*)glGetString(GL_VERSION);
//    return szOpenGLVersionString ? szOpenGLVersionString : "";
//}

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

typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

int PlatformDevice::GetCPUFrequency()
{
	DWORD ProcessorIndex = 0;
	int   ProcessorFrequencyKHz = 0;
	// get the number or processors 
	SYSTEM_INFO si = { 0 };
	::GetSystemInfo(&si);
	// allocate buffer to get info for each processor
	const int size = si.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);
	mutablebuf buffer(size);
	LONG status = ::CallNtPowerInformation(ProcessorInformation, NULL, 0, buffer.data(), size);
	if (0 == status)
	{
		// list each processor frequency 
		PPROCESSOR_POWER_INFORMATION ppi = (PPROCESSOR_POWER_INFORMATION)buffer.data();
		for (DWORD nIndex = 0; nIndex < si.dwNumberOfProcessors; nIndex++)
		{
			ProcessorFrequencyKHz = ppi->CurrentMhz * 1024;
			if (nIndex == ProcessorIndex)
				break;
			ppi++;
		}
	}
    return ProcessorFrequencyKHz;
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

/**

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
*/

Rect PlatformDevice::GetScreenResolution()
{
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);
    return Rect(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
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

void PlatformDevice::DumpDeviceInfo()
{
    FKLog("============ Device Info===============");
    std::ostringstream ss;
    ss << "GetDeviceId:" << GetDeviceId() << "\n";
    ss << "GetDeviceId:" << GetProduct() << "\n";
    ss << "GetHardware:" << GetHardware() << "\n";
    ss << "GetDevice:" << GetDevice() << "\n";
    ss << "GetModel:" << GetModel() << "\n";
    ss << "GetManufacturer:" << GetManufacturer() << "\n";
    ss << "GetSystemVersion:" << GetSystemVersion() << "\n";
    ss << "GetSDKVersion:" << GetSDKVersion() << "\n";
    ss << "GetCPUCoreCount:" << GetCPUCoreCount() << "\n";
    ss << "GetCPUFrequency:" << GetCPUFrequency() << "\n";
    ss << "GetNetworkType:" << GetNetworkType() << " 1 WIFI,2 2G,3 3G,4 4G,0 other. \n";
    ss << "GetIpAddressV4:" << GetIpAddressV4() << "\n";
    ss << "GetIpAddressV6:" << GetIpAddressV6() << "\n";
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
    return ::IsDebuggerPresent() == TRUE;
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
    CloseHandle(hWrite);
    if (ret)
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    else
    {
        return "CreateProcessA Falied.";
    }
    const size_t buffer_size = 256;
    char buffer[buffer_size] = { 0 };
    std::string result = "";
    DWORD byteRead = 0;
    while (ReadFile(hRead, buffer, buffer_size, &byteRead, NULL))
    {
        result.append(buffer, byteRead);
    }
    CloseHandle(hRead);
    return result;
#endif
}

bool PlatformDevice::ScreenShot(std::string& outSavePath)
{
    return false;
}

NS_FK_END

#endif // OF #if (PLATFORM_WINDOWS)
