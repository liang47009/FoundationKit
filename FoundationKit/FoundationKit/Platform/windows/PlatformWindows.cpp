/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifdef WIN32
#include <windows.h>
#include <WindowsX.h>
#include <psapi.h>
#include <cassert>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Platform/Platform.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"
#pragma   comment(lib,"Psapi.lib")

// for getMacAddressUseGetAdaptersInfo
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


NS_FK_BEGIN


class PlatformHelper
{
public:

};



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



std::string Platform::getMacAddress()
{
    // http://www.codeguru.com/cpp/i-n/network/networkinformation/article.php/c5451/Three-ways-to-get-your-MAC-address.htm
    // http://www.codeguru.com/cpp/i-n/network/networkinformation/article.php/c5437/A-New-Method-to-Get-MACNIC-Statistical-Information.htm

    auto getMacAddressUseGetAdaptersInfo = []()
    {
        IP_ADAPTER_INFO AdapterInfo[16] = {0};       // Allocate information 
        // for up to 16 NICs
        DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

        DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
            AdapterInfo,                 // [out] buffer to receive data
            &dwBufLen);                  // [in] size of receive data buffer
        assert(dwStatus == ERROR_SUCCESS);  // Verify return value is 
        // valid, no buffer overflow

        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
        std::string macAddr;
        do {
            macAddr = StringUtils::format("%02X-%02X-%02X-%02X-%02X-%02X"
                , pAdapterInfo->Address[0]
                , pAdapterInfo->Address[1]
                , pAdapterInfo->Address[2]
                , pAdapterInfo->Address[3]
                , pAdapterInfo->Address[4]
                , pAdapterInfo->Address[5]);
            pAdapterInfo = pAdapterInfo->Next;    // Progress through 
            break; // we just need one
            // linked list
        } while (false);                        // Terminate if last adapter
        //} while (pAdapterInfo);                    // Terminate if last adapter

        return macAddr;
    };

    return getMacAddressUseGetAdaptersInfo();
}

std::string Platform::getDeviceId()
{
    char value[255] = {0};
    DWORD BufferSize = 255;
    HKEY key = NULL;
    auto result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);
    if (NO_ERROR != result)
    {
        result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &key);
    }
    if (NO_ERROR != result)
    {
        LOG_ERROR("===== Cannot open regedit");
        return "";
    }
    result = RegGetValueA(key, "", "ProductId", RRF_RT_ANY, NULL, (PVOID)&value, &BufferSize);
    if (NO_ERROR != result)
    {
        LOG_ERROR("===== Cannot get regedit value for key:ProductId");
    }
    return value;
}

std::string Platform::getDeviceName()
{
    char value[255] = { 0 };
    DWORD BufferSize = 255;
    HKEY key = NULL;
    auto result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);
    if (NO_ERROR != result)
    {
        result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &key);
    }
    if (NO_ERROR != result)
    {
        LOG_ERROR("===== Cannot open regedit");
        return "";
    }
    result = RegGetValueA(key, "", "ProductName", RRF_RT_ANY, NULL, (PVOID)&value, &BufferSize);
    if (NO_ERROR != result)
    {
        LOG_ERROR("===== Cannot get regedit value for key:ProductName");
    }
    return value;
}

std::string Platform::getOperatingSystemVersion()
{
    char value[255] = { 0 };
    DWORD BufferSize = 255;
    HKEY key = NULL;
    auto result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);
    if (NO_ERROR != result)
    {
        result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &key);
    }
    if (NO_ERROR != result)
    {
        LOG_ERROR("===== Cannot open regedit");
        return "";
    }
    result = RegGetValueA(key, "", "CurrentVersion", RRF_RT_ANY, NULL, (PVOID)&value, &BufferSize);
    if (NO_ERROR != result)
    {
        LOG_ERROR("===== Cannot get regedit value for key:CurrentVersion");
    }
    return value;
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

NS_FK_END

#endif









