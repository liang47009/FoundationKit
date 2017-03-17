/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/Platform/Platform.hpp"
#if (TARGET_PLATFORM == PLATFORM_WINDOWS)

#include <windows.h>
#include <WindowsX.h>
#include <psapi.h>
#include <vector>
#include <memory>
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Crypto/md5.hpp"

#pragma   comment(lib,"Psapi.lib")

// for getMacAddressUseGetAdaptersInfo
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


NS_FK_BEGIN

float Platform::getTotalMemory()
{
    long megabyte = 1024 * 1024;
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG physical_memory = memInfo.ullTotalPhys;
    return static_cast<float>((physical_memory / megabyte));
}

float Platform::getFreeMemory()
{
    long megabyte = 1024 * 1024;
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG free_memory = memInfo.ullAvailPhys;
    return static_cast<float>(free_memory / megabyte);
}

float Platform::getProcessMemory()
{
    long megabyte = 1024 * 1024;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T used_memory = pmc.WorkingSetSize;
    return static_cast<float>(used_memory / megabyte);
}

std::vector<uint8> Platform::getMacAddressRaw()
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

std::string Platform::getMacAddress()
{
    std::vector<uint8> macVec = getMacAddressRaw();
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

std::string Platform::getDeviceId()
{
    return MD5::md5_hash_hex(getMacAddress().c_str());
}

typedef LONG(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
std::string Platform::getDeviceName()
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
    return "";
}

std::string Platform::getOperatingSystemVersion()
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

std::string Platform::getCPUArchitecture()
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

int64 Platform::getTickCount()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (int64)counter.QuadPart;
}

#define USER_POPEN 0

std::string Platform::executeSystemCommand(const std::string& command)
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

#endif









