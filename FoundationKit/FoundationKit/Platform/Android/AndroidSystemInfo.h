#ifndef __ANDROIDSYSTEMINFO_H__
#define __ANDROIDSYSTEMINFO_H__
#include <unordered_map>
#include <string>
#include "FoundationKit/Foundation/Singleton.h"
NS_FK_BEGIN
class AndroidSystemInfo: public Singleton<AndroidSystemInfo>
{
public:
    friend class Singleton < AndroidSystemInfo >;
    typedef std::unordered_map<std::string, unsigned long long> MemInfo;
    
    void applicationStart();
    unsigned long long getApplicationRunTime();
    
    /**
     * See android meminfo command:$adb shell cat /proc/meminfo
     * The value Unit is kB.
     */
    MemInfo& getMemInfo();
    
    std::string getDeviceId();
    
    std::string getMacAddress();
    
    
protected:
    AndroidSystemInfo();
    ~AndroidSystemInfo();

private:
    MemInfo  _meminfo;
    static char processStartTimeString[16];
};
NS_FK_END

#endif //__ANDROIDSYSTEMINFO_H__


