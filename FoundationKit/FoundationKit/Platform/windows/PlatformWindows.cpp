/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifdef WIN32
#include <windows.h>
#include <WindowsX.h>
#include <psapi.h>
#include <vector>
#include <memory>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Platform/Platform.h"
#include "FoundationKit/Platform/OpenGL.h"
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Crypto/md5.hpp"
#define TJE_IMPLEMENTATION
#include "FoundationKit/external/TinyJPEG/tiny_jpeg.h"

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

std::string Platform::getDeviceName()
{
    return Environment::GetMachineName();
}

std::string Platform::getOperatingSystemVersion()
{
    return Environment::GetOSVersion();
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

void Platform::systemTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    year = st.wYear;
    month = st.wMonth;
    dayOfWeek = st.wDayOfWeek;
    day = st.wDay;
    hour = st.wHour;
    min = st.wMinute;
    sec = st.wSecond;
    msec = st.wMilliseconds;
}

void Platform::utcTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    SYSTEMTIME st;
    GetSystemTime(&st);

    year = st.wYear;
    month = st.wMonth;
    dayOfWeek = st.wDayOfWeek;
    day = st.wDay;
    hour = st.wHour;
    min = st.wMinute;
    sec = st.wSecond;
    msec = st.wMilliseconds;
}

int64 Platform::getTickCount()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (int64)counter.QuadPart;
}

void Platform::captureScreen(const Rect& rect, const std::string& filename, const std::function<void(bool, const std::string&)>& afterCaptured)
{
    do
    {
        int posx = static_cast<int>(rect.origin.x);
        int posy = static_cast<int>(rect.origin.y);
        int width  = static_cast<int>(rect.size.width);
        int height = static_cast<int>(rect.size.height);
        int buff_width = width - posx;
        int buff_height = height - posy;

        std::shared_ptr<GLubyte> buffer(new GLubyte[buff_width * buff_height * 4], [](GLubyte* p){ SAFE_DELETE_ARRAY(p); });
        BREAK_IF(!buffer);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(posx, posy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());

        std::shared_ptr<GLubyte> flippedBuffer(new GLubyte[buff_width * buff_height * 4], [](GLubyte* p) { SAFE_DELETE_ARRAY(p); });
        BREAK_IF(!flippedBuffer);

        for (int row = 0; row < buff_height; ++row)
        {
            memcpy(flippedBuffer.get() + (buff_height - row - 1) * buff_width * 4, buffer.get() + row * buff_width * 4, buff_width * 4);
        }

        int ret = tje_encode_to_file_at_quality(filename.c_str(), 3, buff_width, buff_height, 4, flippedBuffer.get());

        if (ret)
        {
            if (afterCaptured)
                afterCaptured(true, filename);
        }
        else
        {
            if (afterCaptured)
                afterCaptured(false, filename);
        }
    } while (false);
}

NS_FK_END

#endif









