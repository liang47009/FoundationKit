/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifdef WIN32
#include <windows.h>
#include <WindowsX.h>
#include <psapi.h>
#include "FoundationKit/FoundationKitDefines.h"
#include "FoundationKit/Platform/Platform.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"
#pragma   comment(lib,"Psapi.lib")

// for getMacAddressUseUuidCreate
#include <rpc.h>
#include <rpcdce.h>
#pragma comment(lib, "rpcrt4.lib")

// for getMacAddressUseGetAdaptersInfo
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

// for getMacAddressUseNetBIOS
#include <lm.h>
#pragma comment(lib, "Netapi32.lib")

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
    return static_cast<int>((physical_memory / megabyte));
}

float Platform::getFreeMemory()
{
    long megabyte = 1024 * 1024;
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG free_memory = memInfo.ullAvailPhys;
    return static_cast<int>((free_memory / megabyte));
}

float Platform::getProcessMemory()
{
    long megabyte = 1024 * 1024;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T used_memory = pmc.WorkingSetSize;
    return (used_memory / megabyte);
}



std::string Platform::getMacAddress()
{
    auto getMacAddressUseUuidCreate = []()
    {
        unsigned char MACData[6] = { 0 };
        UUID uuid;
        UuidCreateSequential(&uuid);    // Ask OS to create UUID
        for (int i = 2; i < 8; i++)  // Bytes 2 through 7 inclusive  are MAC address
            MACData[i - 2] = uuid.Data4[i];

        return StringUtils::format("%02X-%02X-%02X-%02X-%02X-%02X", 
            MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
    };

    auto getMacAddressUseNetBIOS = []()\
    {
        unsigned char MACData[6] = {0};      // Allocate data structure 
        // for MAC (6 bytes needed)

        WKSTA_TRANSPORT_INFO_0 *pwkti; // Allocate data structure 
        // for NetBIOS
        DWORD dwEntriesRead;
        DWORD dwTotalEntries;
        BYTE *pbBuffer;

        // Get MAC address via NetBIOS's enumerate function
        NET_API_STATUS dwStatus = NetWkstaTransportEnum(
            NULL,                 // [in]  server name
            0,                    // [in]  data structure to return
            &pbBuffer,            // [out] pointer to buffer
            MAX_PREFERRED_LENGTH, // [in]  maximum length
            &dwEntriesRead,       // [out] counter of elements 
            //       actually enumerated
            &dwTotalEntries,      // [out] total number of elements 
            //       that could be enumerated
            NULL);                // [in/out] resume handle
        assert(dwStatus == NERR_Success);

        pwkti = (WKSTA_TRANSPORT_INFO_0 *)pbBuffer; // type cast the buffer

        std::string macAddr;
        for (DWORD i = 0; i < dwEntriesRead; i++)  // first address is
            // 00000000, skip it
        {                                         // enumerate MACs & print
            swscanf((wchar_t *)pwkti[i].wkti0_transport_address,
                L"%2hx%2hx%2hx%2hx%2hx%2hx",
                &MACData[0],
                &MACData[1],
                &MACData[2],
                &MACData[3],
                &MACData[4],
                &MACData[5]);

            macAddr = StringUtils::format("%02X-%02X-%02X-%02X-%02X-%02X",
                MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
            break; // we just need one
        }

        // Release pbBuffer allocated by above function
        dwStatus = NetApiBufferFree(pbBuffer);
        assert(dwStatus == NERR_Success);
        return macAddr;
    };

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
        } while (pAdapterInfo);                    // Terminate if last adapter

        return macAddr;
    };


    //std::string ma0 = getMacAddressUseUuidCreate();
    //std::string ma1 = getMacAddressUseNetBIOS();
    //std::string ma2 = getMacAddressUseGetAdaptersInfo();

    return getMacAddressUseUuidCreate();
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









