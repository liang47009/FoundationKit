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
#pragma   comment(lib,"Psapi.lib")

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
    return "";
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
    return "";
}

NS_FK_END

#endif









