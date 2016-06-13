#ifdef ANDROID

#include <unistd.h>   //close
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <netinet/in.h>
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
#include "FoundationKit/Platform/Environment.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Foundation/StringUtils.h"

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
        
        char line_buf[LINE_BUF_SIZE] = { 0 };
        char key_buf[MEMKEY_BUF_SIZE] = { 0 };
        unsigned long long value = 0;
        unsigned long long resultValue = 0;
        while (fgets(line_buf, sizeof(line_buf), fp) != NULL)
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

std::vector<uint8> Platform::getMacAddressRaw()
{
    std::vector<uint8> result;
    result.resize(6);
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024] = {0};
    do 
    {
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock == -1)
        {
            LOG_ERROR("====== sock == -1");
            break;
        }

        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        if (ioctl(sock, SIOCGIFCONF, &ifc) == -1)
        {
            LOG_ERROR("====== ioctl falied");
            break;
        }
 
        struct ifreq* it = ifc.ifc_req;
        const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
        for (; it != end; ++it) 
        {
            strcpy(ifr.ifr_name, it->ifr_name);
            if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
            {
                if (!(ifr.ifr_flags & IFF_LOOPBACK)) 
                { 
                    // don't count loopback
                    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) 
                    {
                        std::memcpy(result.data(), ifr.ifr_hwaddr.sa_data, result.size());
                        break;
                    }
                    else
                    {
                        LOG_ERROR("===== ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                    }
                }
            }
        }

        if (sock != -1)
        {
            close(sock);
        }
    } while (false);

    
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
    std::string strDeviceId = strDeviceId = PlatformHelper::getSystemProperty("ro.serialno");
    do 
    {
        if (!strDeviceId.empty()) break;
        strDeviceId = PlatformHelper::getSystemProperty("ro.boot.serialno");
        if (!strDeviceId.empty()) break;
        strDeviceId = PlatformHelper::getSystemProperty("gsm.sim.imei");
        if (!strDeviceId.empty()) break;
        strDeviceId = MD5::md5_hash_hex(getMacAddress().c_str());
    } while (false);

    return strDeviceId;
}


std::string Platform::getDeviceName()
{
    //return PlatformHelper::getSystemProperty("ro.product.model");
    return Environment::GetMachineName();
}

std::string Platform::getOperatingSystemVersion()
{
    //return PlatformHelper::getSystemProperty("ro.build.version.release");
    return Environment::GetOSVersion();
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

int64 Platform::getTickCount()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (int64)tp.tv_sec * 1000000000 + tp.tv_nsec;
}

NS_FK_END

#endif