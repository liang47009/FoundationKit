/****************************************************************************
Copyright (c) 2017 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if defined(_MSC_VER)  
# pragma warning(push)  
# pragma warning(disable:4458)
#endif

#if (PLATFORM_WINDOWS)

#include <windows.h>
#include <WindowsX.h>
#include <WinSock2.h>
#include <WS2tcpip.h> 
#include <vector>
#include <memory>
#include <array>
#include <sstream>

#include <psapi.h>
#pragma   comment(lib,"Psapi.lib")

#include <d3d9.h>
#pragma   comment(lib,"d3d9.lib")

#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib") // for GetAdaptersInfo

#include <Powrprof.h>
#pragma comment(lib, "PowrProf.lib") // for CallNtPowerInformation

#include <gdiplus.h>
#pragma  comment(lib, "gdiplus.lib") // gdiplus

#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Base/mutablebuf.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"

NS_FK_BEGIN

namespace detail
{
    enum class AddressType
    {
        IPV4,
        IPV6,
        MAC,
        DNS
    };

    std::vector<std::string> GetAdaptersAddressesByType(AddressType addType)
    {
        std::vector<std::string> AddressList;
        // Set the flags to pass to GetAdaptersAddresses
        ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
        IP_ADAPTER_ADDRESSES  IpAddresses[16];
        static const DWORD bufflen = 64;
        char buff[bufflen] = { 0 };
        ULONG OutBufferLength = sizeof(IP_ADAPTER_ADDRESSES) * 16;
        ULONG family = AF_UNSPEC;

        switch (addType)
        {
        case FoundationKit::detail::AddressType::IPV4:
            family = AF_INET;
            break;
        case FoundationKit::detail::AddressType::IPV6:
            family = AF_INET6;
            break;
        case FoundationKit::detail::AddressType::MAC:
        case FoundationKit::detail::AddressType::DNS:
            family = AF_INET;
            break;
        default:
            break;
        }
        // Read the adapters
        uint32 RetVal = GetAdaptersAddresses(family, flags, NULL, IpAddresses, &OutBufferLength);
        if (RetVal == NO_ERROR)
        {
            PIP_ADAPTER_ADDRESSES IpAddressesPointer = IpAddresses;
            // Walk the set of addresses copying each one
            while (IpAddressesPointer)
            {
                if (addType == AddressType::IPV4)
                {
                    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = IpAddressesPointer->FirstUnicastAddress;
                    while(pUnicast)
                    {
                        sockaddr_in *sa_in = (sockaddr_in *)pUnicast->Address.lpSockaddr;
                        AddressList.push_back(inet_ntop(family, &(sa_in->sin_addr), buff, bufflen));
                        pUnicast = pUnicast->Next;
                    }
                }

                if (addType == AddressType::IPV6)
                {
                    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = IpAddressesPointer->FirstUnicastAddress;
                    while (pUnicast)
                    {
                        sockaddr_in6 *sa_in6 = (sockaddr_in6 *)pUnicast->Address.lpSockaddr;
                        AddressList.push_back(inet_ntop(family, &(sa_in6->sin6_addr), buff, bufflen));
                        pUnicast = pUnicast->Next;
                    }
                }

                if (addType == AddressType::MAC && IpAddressesPointer->PhysicalAddressLength > 0)
                {
                    std::string MacAddress;
                    MacAddress.resize(IpAddressesPointer->PhysicalAddressLength);
                    memcpy(&MacAddress[0], IpAddressesPointer->PhysicalAddress, IpAddressesPointer->PhysicalAddressLength);
                    AddressList.push_back(MacAddress);

                }

                if (addType == AddressType::DNS)
                {
                    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = IpAddressesPointer->FirstDnsServerAddress;
                    if (pDnServer)
                    {
                        sockaddr_in *sa_in = (sockaddr_in *)pDnServer->Address.lpSockaddr;
                        AddressList.push_back(inet_ntop(family, &(sa_in->sin_addr), buff, bufflen));
                    }
                }
                IpAddressesPointer = IpAddressesPointer->Next;
            }
        }
        return AddressList;
    }

    std::string GetMacAddress()
    {
        std::vector<std::string> macVec = GetAdaptersAddressesByType(AddressType::MAC);
        std::string result;
        if (macVec.size() > 0)
        {
            // Copy the data and say we did
            result = StringUtils::Format("%02X%02X%02X%02X%02X%02X"
                , (unsigned char)macVec[0][0]
                , (unsigned char)macVec[0][1]
                , (unsigned char)macVec[0][2]
                , (unsigned char)macVec[0][3]
                , (unsigned char)macVec[0][4]
                , (unsigned char)macVec[0][5]);
        }
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
#ifdef _M_ARM
	return "Windows";
#else
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
#endif
}

std::string PlatformDevice::GetModel()
{
    return "";
}

std::string PlatformDevice::GetManufacturer()
{
#ifdef _M_ARM
	return "Windows";
#else
	// https://msdn.microsoft.com/en-us/library/hskdteyh.aspx
	// https://en.wikipedia.org/wiki/CPUID
	std::array<int, 4> cpui;
	__cpuidex(cpui.data(), 0, 0);
	int nIds = cpui[0];
	std::vector<std::array<int, 4>> data;
	for (int i = 0; i <= nIds; ++i)
	{
		__cpuidex(cpui.data(), i, 0);
		data.push_back(cpui);
	}
	char vendor[32] = { 0 };
	*reinterpret_cast<int*>(vendor) = data[0][1];
	*reinterpret_cast<int*>(vendor + 4) = data[0][3];
	*reinterpret_cast<int*>(vendor + 8) = data[0][2];
	return vendor;
#endif // ARM

}

typedef LONG(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
std::string PlatformDevice::GetSystemVersion()
{
    RTL_OSVERSIONINFOEXW verInfo = { 0 };
    verInfo.dwOSVersionInfoSize = sizeof(verInfo);
    static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
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
	auto status = ::CallNtPowerInformation(ProcessorInformation, NULL, 0, buffer.data(), size);
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
    std::vector<std::string> AddressList = detail::GetAdaptersAddressesByType(detail::AddressType::IPV4);
    return AddressList.size() > 0 ? AddressList[0] : "0.0.0.0";
}

std::string PlatformDevice::GetIpAddressV6()
{
    std::vector<std::string> AddressList = detail::GetAdaptersAddressesByType(detail::AddressType::IPV6);
    return AddressList.size() > 0 ? AddressList[0]:"0.0.0.0";
}

PlatformDevice::string_list PlatformDevice::GetDNS()
{
    std::vector<std::string> AddressList = detail::GetAdaptersAddressesByType(detail::AddressType::DNS);
    return AddressList;
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
    auto bb = detail::GetMacAddress();
    std::ostringstream ss;
    ss << "GetDeviceId:" << GetDeviceId() << "\n";
    ss << "GetProduct:" << GetProduct() << "\n";
    ss << "GetHardware:" << GetHardware() << "\n";
    ss << "GetDevice:" << GetDevice() << "\n";
    ss << "GetModel:" << GetModel() << "\n";
    ss << "GetManufacturer:" << GetManufacturer() << "\n";
    ss << "GetSystemVersion:" << GetSystemVersion() << "\n";
    ss << "GetSDKVersion:" << GetSDKVersion() << "\n";
    ss << "GetCPUArch:" << GetCPUArch() << "\n";
    ss << "GetCPUCoreCount:" << GetCPUCoreCount() << "\n";
    ss << "GetCPUFrequency:" << GetCPUFrequency() << "\n";
    ss << "GetNetworkType:" << GetNetworkType() << " 1 WIFI,2 2G,3 3G,4 4G,0 other. \n";
    ss << "GetMacAddress:" << detail::GetMacAddress() << "\n";
    ss << "GetIpAddressV4:" << GetIpAddressV4() << "\n";
    ss << "GetIpAddressV6:" << GetIpAddressV6() << "\n";
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


class ScopeGdiplus
{
    ULONG_PTR gdiToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
public:
    ScopeGdiplus()
    {
        // https://msdn.microsoft.com/en-us/library/ms534077
        Gdiplus::GdiplusStartup(&gdiToken, &gdiplusStartupInput, NULL);
    }
    ~ScopeGdiplus()
    {
        Gdiplus::GdiplusShutdown(gdiToken);
    }
};

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
bool PlatformDevice::ScreenCapture(std::string& outSavePath)
{
    ScopeGdiplus sg;
    bool ShotResult = false;
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    HWND hDesktopWnd = GetDesktopWindow();
    HDC hDesktopDC = GetDC(hDesktopWnd);
    HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
    HGDIOBJ OldGdiObj = SelectObject(hCaptureDC, hCaptureBitmap);
    DWORD bltFlags = SRCCOPY;
    bltFlags |= CAPTUREBLT;
    BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, bltFlags);
    SelectObject(hCaptureDC, OldGdiObj);
    outSavePath = Path::GetDocumentsPath();
    outSavePath += "ScreenShot_";
    DateTime  dateNow = DateTime::Now();
    outSavePath += dateNow.ToString("%Y%m%d%H%M%S");
    outSavePath += ".jpg";
    CLSID ImageClsid;
    GetEncoderClsid(L"image/jpeg", &ImageClsid);
    Gdiplus::Bitmap bitmap(hCaptureBitmap, NULL);
    std::wstring wstrPath = StringUtils::string2UTF8wstring(outSavePath);
    //int quality = 100;
    //Gdiplus::EncoderParameters encoderParameters;
    //encoderParameters.Count = 1;
    //encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
    //encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    //encoderParameters.Parameter[0].NumberOfValues = 1;
    //encoderParameters.Parameter[0].Value = &quality;
    //bitmap.Save(wstrPath.c_str(), &ImageClsid, &encoderParameters);
    bitmap.Save(wstrPath.c_str(), &ImageClsid, NULL);
    ReleaseDC(hDesktopWnd, hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap);
    return ShotResult;
}

/**
 * The function calls GetImageEncoders to get an array of ImageCodecInfo objects.
 * If one of the ImageCodecInfo objects in that array represents the requested encoder, 
 * the function returns the index of the ImageCodecInfo object and copies the CLSID into 
 * the variable pointed to by pClsid.If the function fails, it returns ¨C1.
 * //https://msdn.microsoft.com/en-us/library/windows/desktop/ms533843(v=vs.85).aspx
 * @param  format Image format.e.g:
 *                image/bmp
 *                image/jpeg
 *                image/gif
 *                image/tiff
 *                image/png
 * @param pClsid   class identifier (CLSID) of that encoder. 
 */
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }
    free(pImageCodecInfo);
    return -1;  // Failure
}

NS_FK_END
#if defined(_MSC_VER)  
# pragma warning(pop)  
#endif 
#endif // OF #if (PLATFORM_WINDOWS)
