#include "AndroidSystemInfo.h"
#include "FoundationKit/Foundation/Logger.h"
#include <sys/system_properties.h>//for __system_property_get
NS_FK_BEGIN

char AndroidSystemInfo::processStartTimeString[16]={0};

AndroidSystemInfo::AndroidSystemInfo()
{
}

AndroidSystemInfo::~AndroidSystemInfo()
{
}

void AndroidSystemInfo::applicationStart()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    sprintf(processStartTimeString, "%ld", 1000000*tv.tv_sec + tv.tv_usec);
}

unsigned long long AndroidSystemInfo::getApplicationRunTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t processStartTime = strtol(processStartTimeString,NULL, 16);
    time_t processUptime = (1000000*tv.tv_sec+tv.tv_usec) - processStartTime;
    // Store the uptime in milliseconds.
    return (static_cast<unsigned long long>(processUptime) / 1000); //ms
}

AndroidSystemInfo::MemInfo& AndroidSystemInfo::getMemInfo()
{
    const int LINE_BUF_SIZE = 256;
    const int MEMKEY_BUF_SIZE=32;
    char *line_ptr=nullptr;
    char line_buf[LINE_BUF_SIZE]={0};
    char key_buf[MEMKEY_BUF_SIZE]={0};
    unsigned long long value = 0;
    _meminfo.clear();
    FILE *fp = nullptr;
    if ((fp = fopen("/proc/meminfo","r")) == NULL)
    {
        LOG_ERROR("Cannot open /proc/meminfo file!");
        return _meminfo;
    }
    while ((line_ptr = fgets(line_buf,sizeof(line_buf), fp)) != NULL)
    {
        if ( sscanf(line_buf,"%[^:]%*s %llu kB", key_buf, &value) != 2)
            continue;
        _meminfo.insert(std::make_pair(key_buf,value));
    }
    fclose(fp);

    //    debian之类的系统认为：
    //    可用内存=free的内存+cached的内存+buffers的内存
    //    而SUSE之类的系统则认为：
    //    可用内存=free的内存+cached的内存+buffers的内存+SReclaimable的内存
    _meminfo["MemFree"] = _meminfo["MemFree"] + _meminfo["Buffers"] + _meminfo["Cached"];
    return _meminfo;
}

std::string AndroidSystemInfo::getDeviceId()
{
    char szDeviceId[PROP_VALUE_MAX+1] = {0};
    __system_property_get("gsm.sim.imei",szDeviceId);
    if(strcmp(szDeviceId,"") ==0)
        __system_property_get("ro.serialno",szDeviceId);
    return szDeviceId;
}

std::string AndroidSystemInfo::getMacAddress()
{
    const int LINE_BUF_SIZE = 256;
    char line_buf[LINE_BUF_SIZE]={0};
    
    char mac_addr[16]={0};
    
    FILE *fp = nullptr;
    if ((fp = fopen("/sys/class/net/wlan0/address","r")) == NULL)
    {
        LOG_ERROR("/sys/class/net/wlan0/address !");
    }
    else
    {
        while (fgets(line_buf,sizeof(line_buf), fp) != NULL)
        {
            char mac0[3] = { 0 };
            char mac1[3] = { 0 };
            char mac2[3] = { 0 };
            char mac3[3] = { 0 };
            char mac4[3] = { 0 };
            char mac5[3] = { 0 };
            if ( sscanf(line_buf, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]", mac0, mac1, mac2, mac3, mac4, mac5) != 6)
                continue;
            
            sprintf(mac_addr, "%s%s%s%s%s%s",mac0, mac1, mac2, mac3, mac4, mac5);
        }
        fclose(fp);
    }
    return mac_addr;
}



NS_FK_END