#ifdef ANDROID

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include <sys/system_properties.h>//for __system_property_get
#include <sys/time.h>
#include <sched.h>  // maybe the BSD time and sched.h need to be independent, but they are normally available together
#include <string>
#include <unordered_map>
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Platform/Platform.h"
#include "FoundationKit/Foundation/Logger.h"

NS_FK_BEGIN

typedef std::unordered_map<std::string, unsigned long long> MemInfo;
static const int LINE_BUF_SIZE = 256;

class PlatformHelper
{
public:
    static unsigned long long getMemoryForKey(const char* key)
    {
        const int MEMKEY_BUF_SIZE = 32;
        FILE *fp = nullptr;
        if ((fp = fopen("/proc/meminfo", "r")) == NULL)
        {
            LOG_ERROR("Cannot open /proc/meminfo file!");
            return 0;
        }

        char *line_ptr = nullptr;
        char line_buf[LINE_BUF_SIZE] = { 0 };
        char key_buf[MEMKEY_BUF_SIZE] = { 0 };
        unsigned long long value = 0;
        unsigned long long resultValue = 0;
        while ((line_ptr = fgets(line_buf, sizeof(line_buf), fp)) != NULL)
        {
            if (sscanf(line_buf, "%[^:]%*s %llu kB", key_buf, &value) != 2)
                continue;

            if (strcmp(key, "MemFree") == 0)
            {
                //    debian之类的系统认为：
                //    可用内存=MemFree的内存+Buffers的内存+Cached的内存
                //    而SUSE之类的系统则认为：
                //    可用内存=MemFree的内存+Buffers的内存+Cached的内存+SReclaimable的内存
                if (strcmp(key_buf, "MemFree") == 0 ||
                    strcmp(key_buf, "Buffers") == 0 ||
                    strcmp(key_buf, "Cached") == 0)
                {
                    resultValue += value;
                }

            }
            else if (strcmp(key_buf, key) == 0)
            {
                resultValue = value;
                break;
            }
        }
        fclose(fp);

        return resultValue;

    }


    static std::string getSystemProperty(const std::string& propName)
    {
        char propValue[PROP_VALUE_MAX + 1] = { 0 };
        __system_property_get(propName.c_str(), propValue);
        return propValue;
    }
};

float Platform::getTotalMemory()
{
    long megabyte = 1024 * 1024;
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    long long physical_memory = memInfo.totalram;
    physical_memory *= memInfo.mem_unit;
    return (physical_memory / megabyte);
}

float Platform::getFreeMemory()
{
    unsigned long long freeMem = PlatformHelper::getMemoryForKey("MemFree");
    return freeMem / 1024;
}


float Platform::getProcessMemory()
{
    FILE *fp = nullptr;
    if ((fp = fopen("/proc/self/status", "r")) == NULL)
    {
        LOG_ERROR("Cannot open /proc/self/status file!");
        return 0.0f;
    }

    auto parseNumber = [](char* line)->int
    {
        int i = strlen(line);
        while (*line < '0' || *line > '9') line++;
        line[i - 3] = '\0';
        i = atoi(line);
        return i;
    };

	int result = 0;
    char line[LINE_BUF_SIZE] = { 0 };
    while (fgets(line, sizeof(line), fp) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0)
        {
            result = parseNumber(line);
            break;
        }
	}
    fclose(fp);
	return result/1024;
}


std::string Platform::getMacAddress()
{
    char line_buf[LINE_BUF_SIZE] = { 0 };

    char mac_addr[16] = { 0 };

    FILE *fp = nullptr;
    if ((fp = fopen("/sys/class/net/wlan0/address", "r")) == NULL)
    {
        LOG_ERROR("/sys/class/net/wlan0/address !");
    }
    else
    {
        while (fgets(line_buf, sizeof(line_buf), fp) != NULL)
        {
            char mac0[3] = { 0 };
            char mac1[3] = { 0 };
            char mac2[3] = { 0 };
            char mac3[3] = { 0 };
            char mac4[3] = { 0 };
            char mac5[3] = { 0 };
            if (sscanf(line_buf, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]", mac0, mac1, mac2, mac3, mac4, mac5) != 6)
                continue;

            sprintf(mac_addr, "%02X%02X%02X%02X%02X%02X", mac0, mac1, mac2, mac3, mac4, mac5);
        }
        fclose(fp);
    }
    return mac_addr;
}



std::string Platform::getDeviceId()
{
    std::string strDeviceId;
    strDeviceId = PlatformHelper::getSystemProperty("gsm.sim.imei");
    if (strDeviceId.empty())
        strDeviceId = PlatformHelper::getSystemProperty("ro.serialno");
    if (strDeviceId.empty())
        strDeviceId = MD5::md5_hash_hex(getMacAddress().c_str());
    return strDeviceId;
}


std::string Platform::getDeviceName()
{
    return PlatformHelper::getSystemProperty("ro.product.model");
}

std::string Platform::getOperatingSystemVersion()
{
    return PlatformHelper::getSystemProperty("ro.build.version.release");
}

std::string Platform::getCPUArchitecture()
{
    return PlatformHelper::getSystemProperty("ro.product.cpu.abi");
}

void Platform::systemTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    // convert it to local time
    struct tm localTime;
    localtime_r(&tmVal.tv_sec, &localTime);

    // pull out data/time
    year = localTime.tm_year + 1900;
    month = localTime.tm_mon + 1;
    dayOfWeek = localTime.tm_wday;
    day = localTime.tm_mday;
    hour = localTime.tm_hour;
    min = localTime.tm_min;
    sec = localTime.tm_sec;
    msec = tmVal.tv_usec / 1000;
}

void Platform::utcTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    // convert it to UTC
    struct tm localTime;
    gmtime_r(&tmVal.tv_sec, &localTime);

    // pull out data/time
    year = localTime.tm_year + 1900;
    month = localTime.tm_mon + 1;
    dayOfWeek = localTime.tm_wday;
    day = localTime.tm_mday;
    hour = localTime.tm_hour;
    min = localTime.tm_min;
    sec = localTime.tm_sec;
    msec = tmVal.tv_usec / 1000;
}

NS_FK_END

#endif