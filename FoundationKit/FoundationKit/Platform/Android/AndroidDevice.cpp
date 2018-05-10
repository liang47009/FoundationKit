/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <sys/system_properties.h>//for __system_property_get
#include <jni.h>
#include <cpu-features.h> //for cpu count
#include <sys/sysinfo.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sstream>
#include "FoundationKit/Base/mutablebuf.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Math/Math.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaClass.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Platform/Android/AndroidInternal/ConnectivityManager.hpp"
#include "FoundationKit/Platform/Android/AndroidInternal/TelephonyManager.hpp"


NS_FK_BEGIN

using namespace android::net;
using namespace android::telephony;
namespace detail
{
    const int MEMKEY_BUF_SIZE = 32;
   /**
    * @brief Returns value in bytes from a status line
    * @param Line in format "Blah:  10000 kB" - needs to be writable as it will modify it
    * @return value in bytes (10240000, i.e. 10000 * 1024 for the above example)
    */
    static uint64 GetBytesFromStatusLine(char * Line)
    {
        if (!Line) return 0;
        char key_buf[MEMKEY_BUF_SIZE] = { 0 };
        uint64  value = 0;
        if (sscanf(Line, "%[^:]%*s %llu kB", key_buf, &value) == 2)
        {
            return value * 1024;
        }
        return 0;
    }

    static std::string GetSystemProperty(const std::string& propName)
    {
        char propValue[PROP_VALUE_MAX + 1] = { 0 };
        __system_property_get(propName.c_str(), propValue);
        return propValue;
    }

    static const int LINE_BUF_SIZE = 256;
    static long long getMemoryForKey(const char* key)
    {
        FILE *fp = nullptr;
        if ((fp = fopen("/proc/meminfo", "r")) == NULL)
        {
            FKLog("Cannot open /proc/meminfo file!");
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
                if (strcmp(key_buf, "MemFree") == 0 || strcmp(key_buf, "Buffers") == 0 ||strcmp(key_buf, "Cached") == 0)
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

    static int ReadIntWithFile(const char* filePath)
    {
        FILE* fp = fopen(filePath, "r");
        int value = 0;
        if (fp)
        {
            char buffer[32] = { 0 };
            while (fgets(buffer, 32, fp))
            {
                sscanf(buffer, "%d", &value);
            }
            fclose(fp);
        }
        else
        {
            FKLog("Cannot open %s file!%s [%s:%d]", filePath, strerror(errno), __FILE__, __LINE__);
        }
        return value;
    }

    static int ReadCPUsFreq(const char* freq, int cpuIndex)
    {
        int result = 0;
		std::string cpufreqpath = StringUtils::Format("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_%s_freq", cpuIndex, freq);
		result = ReadIntWithFile(cpufreqpath.c_str());
		if (result == 0)
		{
			cpufreqpath = StringUtils::Format("/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_%s_freq", cpuIndex, freq);
			result = ReadIntWithFile(cpufreqpath.c_str());
		}
        return result;
    }

    /* Extract the content of a the first occurence of a given field in
    * the content of /proc/cpuinfo and return it as a heap-allocated
    * string that must be freed by the caller.
    *
    * Return NULL if not found
    */
    static char* extract_cpuinfo_field(const char* buffer, int buflen, const char* field)
    {
        int  fieldlen = strlen(field);
        const char* bufend = buffer + buflen;
        char* result = NULL;
        int len;
        const char *p, *q;

        /* Look for first field occurence, and ensures it starts the line. */
        p = buffer;
        for (;;) {
            p = (const char*)memmem(p, bufend - p, field, fieldlen);
            if (p == NULL)
                goto EXIT;

            if (p == buffer || p[-1] == '\n')
                break;

            p += fieldlen;
        }

        /* Skip to the first column followed by a space */
        p += fieldlen;
        p = (const char*)memchr(p, ':', bufend - p);
        if (p == NULL || p[1] != ' ')
            goto EXIT;

        /* Find the end of the line */
        p += 2;
        q = (const char*)memchr(p, '\n', bufend - p);
        if (q == NULL)
            q = bufend;

        /* Copy the line into a heap-allocated buffer */
        len = q - p;
        result = (char*)malloc(len + 1);
        if (result == NULL)
            goto EXIT;

        memcpy(result, p, len);
        result[len] = '\0';

    EXIT: 
        return result;
    }

    enum NetworkType
    {
        NONE,
        NWIFI,
        N2G,
        N3G,
        N4G
    };

}//namespace


std::string PlatformDevice::GetDeviceId()
{
    std::string strDeviceId;
    Version SystemVersion(GetSystemVersion());
    if (SystemVersion < Version("8.0.0"))
    {
        strDeviceId = detail::GetSystemProperty("ro.serialno");
        do {
            BREAK_IF(!strDeviceId.empty());
            FKLog("Try get system property [ro.boot.serialno]");
            strDeviceId = detail::GetSystemProperty("ro.boot.serialno");
            BREAK_IF(!strDeviceId.empty());
            FKLog("Try get system property [gsm.sim.imei]");
            strDeviceId = detail::GetSystemProperty("gsm.sim.imei");
            BREAK_IF(!strDeviceId.empty());
            FKLog("Try use GetMacAddress");
            strDeviceId = GetMacAddress();
        } while (false);
    }
    else
    {
        android::AndroidJavaClass android_os_build("android.os.Build");
        strDeviceId = android_os_build.CallStatic<std::string>("getSerial");
    }
    return strDeviceId;
}

std::string PlatformDevice::GetProduct()
{
    return detail::GetSystemProperty("ro.build.product");
}

std::string PlatformDevice::GetHardware()
{
    return detail::GetSystemProperty("ro.hardware");
}

std::string PlatformDevice::GetDevice()
{
    return detail::GetSystemProperty("ro.product.device");
}

std::string PlatformDevice::GetBrandName()
{
    return detail::GetSystemProperty("ro.product.brand");
}

std::string PlatformDevice::GetModel()
{
    return detail::GetSystemProperty("ro.product.model");
}

std::string PlatformDevice::GetManufacturer()
{
    return detail::GetSystemProperty("ro.product.manufacturer");
}

std::string PlatformDevice::GetSystemVersion()
{
    return detail::GetSystemProperty("ro.build.version.release");
}

std::string PlatformDevice::GetSDKVersion()
{
    return detail::GetSystemProperty("ro.build.version.sdk");
}

std::string PlatformDevice::GetCPUBrand()
{
    static std::string CPUHardwareStr;
    if (CPUHardwareStr.empty())
    {
        std::vector<uint8> CpuInfo = File::ReadAllBytes("/proc/cpuinfo");
        char* CPUHardware = detail::extract_cpuinfo_field((const char*)CpuInfo.data(), CpuInfo.size(), "Hardware");
        if (CPUHardware)
        {
            CPUHardwareStr = CPUHardware;
            free(CPUHardware);
        }
        else
        {
            CPUHardwareStr = detail::GetSystemProperty("ro.board.platform");
        }
    }
    return CPUHardwareStr;
}

std::string PlatformDevice::GetCPUVendor()
{
    return "";
}

std::string PlatformDevice::GetCPUArch()
{
    return detail::GetSystemProperty("ro.product.cpu.abi");
}

int PlatformDevice::GetCPUCoreCount()
{
    return android_getCpuCount();
}

int PlatformDevice::GetCPUFrequency()
{
    return detail::ReadCPUsFreq("max", 0);
}

std::string PlatformDevice::GetGPUBrand()
{
    return "";
}

int PlatformDevice::GetNetworkType()
{
    ConnectivityManager  cm;
    NetworkInfo networkInfo = cm.getNetworkInfo(ConnectivityManager::TYPE_WIFI);
    if (networkInfo)
    {
        if (networkInfo.getState() == NetworkInfo::State::CONNECTED)
        {
            return detail::NetworkType::NWIFI;
        }
    }
    networkInfo = cm.getNetworkInfo(ConnectivityManager::TYPE_MOBILE);
    detail::NetworkType netType = detail::NetworkType::NONE;
    if (networkInfo)
    {
        int subType = networkInfo.getSubtype();
        switch (subType)
        {
        case TelephonyManager::NETWORK_TYPE_GPRS:
        case TelephonyManager::NETWORK_TYPE_EDGE:
        case TelephonyManager::NETWORK_TYPE_CDMA:
        case TelephonyManager::NETWORK_TYPE_1xRTT:
        case TelephonyManager::NETWORK_TYPE_IDEN:
            netType = detail::NetworkType::N2G;
            break;

        case TelephonyManager::NETWORK_TYPE_UMTS:
        case TelephonyManager::NETWORK_TYPE_EVDO_0:
        case TelephonyManager::NETWORK_TYPE_EVDO_A:
        case TelephonyManager::NETWORK_TYPE_HSDPA:
        case TelephonyManager::NETWORK_TYPE_HSUPA:
        case TelephonyManager::NETWORK_TYPE_HSPA:
        case TelephonyManager::NETWORK_TYPE_EVDO_B:
        case TelephonyManager::NETWORK_TYPE_EHRPD:
        case TelephonyManager::NETWORK_TYPE_HSPAP:
            netType = detail::NetworkType::N3G;
            break;
        case TelephonyManager::NETWORK_TYPE_LTE:
            netType = detail::NetworkType::N4G;
            break;
        case TelephonyManager::NETWORK_TYPE_UNKNOWN:
            netType = detail::NetworkType::NONE;
            break;
        default:
            netType = detail::NetworkType::N3G;
        }
    }
    return netType;
}

std::string PlatformDevice::GetIpAddressV4()
{
    std::string ipaddressv4;
    std::string result = ExecuteSystemCommand("ip addr show wlan0 |grep \"inet \"");
    if (result.empty()) // WIFI is not connected.
    {
        result = ExecuteSystemCommand("ip addr show rmnet_data0 |grep \"inet \"");
    }
    size_t inetPosBegin = result.find("inet ");
    if (inetPosBegin != std::string::npos)
    {
        size_t inetPosEnd = inetPosBegin + strlen("inet ");
        std::size_t chPos = result.find('/', inetPosEnd);
        if (chPos != std::string::npos)
        {
            ipaddressv4 = result.substr(inetPosEnd, chPos - inetPosEnd);
        }
    }
    return StringUtils::Trim(ipaddressv4);
}

std::string PlatformDevice::GetIpAddressV6()
{
    std::string ipaddressv6;
    std::string result = ExecuteSystemCommand("ip addr show wlan0 |grep \"inet6 \"");
    if (result.empty()) // WIFI is not connected.
    {
        result = ExecuteSystemCommand("ip addr show rmnet_data0 |grep \"inet6 \"");
    }
    size_t inet6PosBegin = result.find("inet6 ");
    if (inet6PosBegin != std::string::npos)
    {
        size_t inet6PosEnd = inet6PosBegin + strlen("inet6 ");
        std::size_t chPos = result.find('/', inet6PosEnd);
        if (chPos != std::string::npos)
        {
            ipaddressv6 = result.substr(inet6PosEnd, chPos - inet6PosEnd);
        }
    }
    return StringUtils::Trim(ipaddressv6);
}

std::string PlatformDevice::GetMacAddress()
{
    std::string strMacAddress = "";
    std::string result = ExecuteSystemCommand("ip addr show wlan0 |grep \"link/ether \"");
    size_t etherFlagBegin = result.find("link/ether ");
    if (etherFlagBegin != std::string::npos)
    {
        size_t etherFlagEnd = etherFlagBegin + strlen("link/ether ");
        std::size_t chPos = result.find(' ', etherFlagEnd);
        if (chPos != std::string::npos)
        {
            strMacAddress = result.substr(etherFlagEnd, chPos - etherFlagEnd);
        }
    }
    std::string MacAddressResult;
    for (auto ch : strMacAddress)
    {
        if (ch != ':')
        {
            MacAddressResult += ch;
        }
    }
    MacAddressResult = StringUtils::Toupper(MacAddressResult);
    return MacAddressResult;
}

PlatformDevice::string_list PlatformDevice::GetDNS()
{
    std::vector<std::string> dnslist;
    for (int i = 1; i <= 4; ++i)
    {
        std::string propName = "net.dns";
        propName += StringUtils::Tostring(i);
        std::string propValue = detail::GetSystemProperty(propName.c_str());
        if (propValue.size() > 0)
        {
            dnslist.push_back(propValue);
        }
    }
    return dnslist;
}

long long PlatformDevice::GetTotalMemory()
{
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    long long physical_memory = memInfo.totalram;
    physical_memory *= memInfo.mem_unit;
    return physical_memory;
}

long long PlatformDevice::GetAvailableMemory()
{
    long long physical_memory = detail::getMemoryForKey("MemFree");
    return physical_memory * 1024;
}

struct DisplayInfo
{
    bool bInit = false;
    Size appSize;
    Size realSize;
    int densityDpi;
    float fps;
    float nativeScale;
    float xdpi;
    float ydpi;
};
static DisplayInfo  GLOBAL_DisplayInfo;
static void lazyGetScreenResolution()
{
    if (!GLOBAL_DisplayInfo.bInit)
    {
        jobject mainActivity = AndroidJNI::GetMainActivity();
        android::AndroidJavaObject  ajoMainActivity(mainActivity);
        jobject WindowManager = ajoMainActivity.CallWithSig<jobject>("getWindowManager", "()Landroid/view/WindowManager;");
        android::AndroidJavaObject ajoWindowManager(WindowManager);
        jobject Display = ajoWindowManager.CallWithSig<jobject>("getDefaultDisplay", "()Landroid/view/Display;");
        android::AndroidJavaObject ajoDisplay(Display);
        android::AndroidJavaObject ajoPoint("android.graphics.Point");
        android::AndroidJavaObject ajoPointReal("android.graphics.Point");
        ajoDisplay.CallWithSig("getSize", "(Landroid/graphics/Point;)V", ajoPoint.Get());
        ajoDisplay.CallWithSig("getRealSize", "(Landroid/graphics/Point;)V", ajoPointReal.Get());
        GLOBAL_DisplayInfo.appSize = Size(ajoPoint.Get<int>("x"), ajoPoint.Get<int>("y") );
        GLOBAL_DisplayInfo.realSize = Size(ajoPointReal.Get<int>("x"), ajoPointReal.Get<int>("y"));
        android::AndroidJavaObject ajoDisplayMetrics("android.util.DisplayMetrics");
        ajoDisplay.CallWithSig("getRealMetrics", "(Landroid.util.DisplayMetrics;)V", ajoDisplayMetrics.Get());
        GLOBAL_DisplayInfo.fps = ajoDisplay.Call<float>("getRefreshRate");
        GLOBAL_DisplayInfo.densityDpi = ajoDisplayMetrics.Get<int>("densityDpi");
        GLOBAL_DisplayInfo.nativeScale = ajoDisplayMetrics.Get<float>("density");
        GLOBAL_DisplayInfo.xdpi = ajoDisplayMetrics.Get<float>("xdpi");
        GLOBAL_DisplayInfo.ydpi = ajoDisplayMetrics.Get<float>("ydpi");
        JNIEnv* env = AndroidJNI::GetJNIEnv();
        env->DeleteLocalRef(Display);
        env->DeleteLocalRef(WindowManager);
        GLOBAL_DisplayInfo.bInit = true;
    }
}

Rect PlatformDevice::GetScreenResolution()
{
    lazyGetScreenResolution();
    return Rect(static_cast<float>(0)
        , static_cast<float>(0)
        , static_cast<float>(GLOBAL_DisplayInfo.appSize.Width)
        , static_cast<float>(GLOBAL_DisplayInfo.appSize.Height));
}

Rect PlatformDevice::GetScreenNativeResolution()
{
    lazyGetScreenResolution();
     return Rect(static_cast<float>(0)
        , static_cast<float>(0)
        , static_cast<float>(GLOBAL_DisplayInfo.realSize.Width)
        , static_cast<float>(GLOBAL_DisplayInfo.realSize.Height));
}

float PlatformDevice::GetScreenDPI()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.densityDpi;
}

float PlatformDevice::GetScreenXDPI()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.xdpi;
}

float PlatformDevice::GetScreenYDPI()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.ydpi;
}

float PlatformDevice::GetRefreshRate()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.fps;
}

float PlatformDevice::GetNativeScale()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.nativeScale;
}

PlatformMemoryConstants& PlatformDevice::GetMemoryConstants()
{
    static PlatformMemoryConstants MemoryConstants;

    // Gather platform memory stats.
    struct sysinfo SysInfo;
    unsigned long long MaxPhysicalRAMBytes = 0;
    unsigned long long MaxVirtualRAMBytes = 0;

    if (0 == sysinfo(&SysInfo))
    {
        MaxPhysicalRAMBytes = static_cast<unsigned long long>(SysInfo.mem_unit) * static_cast<unsigned long long>(SysInfo.totalram);
        MaxVirtualRAMBytes = static_cast<unsigned long long>(SysInfo.mem_unit) * static_cast<unsigned long long>(SysInfo.totalswap);
    }

    MemoryConstants.TotalPhysical = MaxPhysicalRAMBytes;
    MemoryConstants.TotalVirtual = MaxVirtualRAMBytes;
    MemoryConstants.PageSize = sysconf(_SC_PAGESIZE);

    if (FILE* FileGlobalMemStats = fopen("/proc/meminfo", "r"))
    {
        int FieldsSetSuccessfully = 0;
        size_t MemFree = 0, Cached = 0, Buffers = 0;
        do
        {
            char LineBuffer[256] = { 0 };
            char *Line = fgets(LineBuffer, sizeof(LineBuffer), FileGlobalMemStats);
            if (Line == nullptr)
            {
                break;	// eof or an error
            }

            // if we have MemAvailable, favor that (see http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=34e431b0ae398fc54ea69ff85ec700722c9da773)
            if (strstr(Line, "MemAvailable:") == Line)
            {
                MemoryConstants.AvailablePhysical = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
            else if (strstr(Line, "SwapFree:") == Line)
            {
                MemoryConstants.AvailableVirtual = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
            else if (strstr(Line, "MemFree:") == Line)
            {
                MemFree = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
            else if (strstr(Line, "Cached:") == Line)
            {
                Cached = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
            else if (strstr(Line, "Buffers:") == Line)
            {
                Buffers = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
        } while (FieldsSetSuccessfully < 5);

        // if we didn't have MemAvailable (kernels < 3.14 or CentOS 6.x), use free + cached as a (bad) approximation
        if (MemoryConstants.AvailablePhysical == 0)
        {
            MemoryConstants.AvailablePhysical = Math::Min((uint64)(MemFree + Cached + Buffers), MemoryConstants.TotalPhysical);
        }

        fclose(FileGlobalMemStats);
    }

    // again /proc "API" :/
    if (FILE* ProcMemStats = fopen("/proc/self/status", "r"))
    {
        int FieldsSetSuccessfully = 0;
        do
        {
            char LineBuffer[256] = { 0 };
            char *Line = fgets(LineBuffer, sizeof(LineBuffer), ProcMemStats);
            if (Line == nullptr)
            {
                break;	// eof or an error
            }

            if (strstr(Line, "VmPeak:") == Line)
            {
                MemoryConstants.PeakUsedVirtual = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
            else if (strstr(Line, "VmSize:") == Line)
            {
                MemoryConstants.UsedVirtual = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
            else if (strstr(Line, "VmHWM:") == Line)
            {
                MemoryConstants.PeakUsedPhysical = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
            else if (strstr(Line, "VmRSS:") == Line)
            {
                MemoryConstants.UsedPhysical = detail::GetBytesFromStatusLine(Line);
                ++FieldsSetSuccessfully;
            }
        } while (FieldsSetSuccessfully < 4);

        fclose(ProcMemStats);
    }

    // sanitize stats as sometimes peak < used for some reason
    MemoryConstants.PeakUsedVirtual = Math::Max(MemoryConstants.PeakUsedVirtual, MemoryConstants.UsedVirtual);
    MemoryConstants.PeakUsedPhysical = Math::Max(MemoryConstants.PeakUsedPhysical, MemoryConstants.UsedPhysical);
    return MemoryConstants;
}

void PlatformDevice::DumpDeviceInfo()
{
    FKLog("============ Device Info===============");
    std::ostringstream ss;
    ss << "GetDeviceId:" << GetDeviceId() << "\n";
    //FKLog("============ GetDeviceId===============");
    ss << "GetProduct:" << GetProduct() << "\n";
    //FKLog("============ GetProduct===============");
    ss << "GetHardware:" << GetHardware() << "\n";
    //FKLog("============ GetHardware===============");
    ss << "GetDevice:" << GetDevice() << "\n";
    //FKLog("============ GetDevice===============");
    ss << "GetModel:" << GetModel() << "\n";
    //FKLog("============ GetModel===============");
    ss << "GetManufacturer:" << GetManufacturer() << "\n";
    //FKLog("============ GetManufacturer===============");
    ss << "GetSystemVersion:" << GetSystemVersion() << "\n";
    //FKLog("============ GetSystemVersion===============");
    ss << "GetSDKVersion:" << GetSDKVersion() << "\n";
    //FKLog("============ GetSDKVersion===============");
    ss << "GetCPUBrand:" << GetCPUBrand() << "\n";
    //FKLog("============ GetCPUBrand===============");
    ss << "GetCPUVendor:" << GetCPUVendor() << "\n";
    //FKLog("============ GetCPUVendor===============");
    ss << "GetCPUCoreCount:" << GetCPUCoreCount() << "\n";
    //FKLog("============ GetCPUCoreCount===============");
    ss << "GetCPUFrequency:" << GetCPUFrequency() << "\n";
    //FKLog("============ GetCPUFrequency===============");
    ss << "GetNetworkType:" << GetNetworkType() << " 1 WIFI,2 2G,3 3G,4 4G,0 other. \n";
    //FKLog("============ GetNetworkType===============");
    ss << "GetIpAddressV4:" << GetIpAddressV4() << "\n";
    //FKLog("============ GetIpAddressV4===============");
    ss << "GetIpAddressV6:" << GetIpAddressV6() << "\n";
    //FKLog("============ GetIpAddressV6===============");
    ss << "GetMacAddress:" << GetMacAddress() << "\n";
    //FKLog("============ GetMacAddress===============");
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
    return false;
}

std::string PlatformDevice::ExecuteSystemCommand(const std::string& command)
{
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        char buffer[256] = { 0 };
        //while (!feof(pipe))
        //{
        //    if (fgets(buffer, 256, pipe) != NULL)
        //        result += buffer;
        //}
        for(;;)
        {
            size_t NumRead = fread(buffer, sizeof(char), sizeof(buffer), pipe);
            if (NumRead == 0)
            {
                if (errno == EINTR)
                    continue;
                if (feof(pipe) == 0) // read file error.
                {
                    FKLog("Error while execute command %s: %s\n", command.c_str(), strerror(errno));
                }
                break;
            }
            result += buffer;
        }
    }
    catch (...)
    {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

bool PlatformDevice::ScreenCapture(std::string& outSavePath)
{
    return false;
}
NS_FK_END

#endif //#if PLATFORM_ANDROID

