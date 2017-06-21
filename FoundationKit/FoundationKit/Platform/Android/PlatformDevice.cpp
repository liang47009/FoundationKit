
#include <sys/system_properties.h>//for __system_property_get
#include <jni.h>
#include <cpu-features.h> //fou cpu count
#include <sys/sysinfo.h>

#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaClass.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"


NS_FK_BEGIN


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
            return value;
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
        }
        else
        {
            LOG_ERROR("Cannot open %s file!%s [%s:%d]", filePath, strerror(errno), __FILE__, __LINE__);
        }
        return value;
    }

    static int ReadCPUsFreq(const char* freq, int cpuIndex, bool bFloor = false)
    {
        int result = 0;
        if (cpuIndex < 0)
        {
            int cpuCount = PlatformDevice::GetCPUCoreCount();
            for (int i = 0; i < cpuCount; ++i)
            {
                std::string cpufreqpath = StringUtils::Format("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_%s_freq", i, freq);
                int freqValue = ReadIntWithFile(cpufreqpath.c_str());
                if (freqValue == 0)
                {
                    cpufreqpath = StringUtils::Format("/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_%s_freq", i, freq);
                    freqValue = ReadIntWithFile(cpufreqpath.c_str());
                }
                if (result != 0)
                {
                    if (bFloor)
                        result = Math::Min(result, freqValue);
                    else
                        result = Math::Max(result, freqValue);
                }
                else
                {
                    result = freqValue;
                }
            }
        }
        else
        {
            std::string cpufreqpath = StringUtils::Format("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_%s_freq", cpuIndex, freq);
            result = ReadIntWithFile(cpufreqpath.c_str());
            if (result == 0)
            {
                cpufreqpath = StringUtils::Format("/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_%s_freq", cpuIndex, freq);
                result = ReadIntWithFile(cpufreqpath.c_str());
            }
        }
        return result;
    }


    enum NetworkState
    {
        CONNECTING,
        CONNECTED,
        SUSPENDED,
        DISCONNECTING,
        DISCONNECTED,
        UNKNOWN
    };

    enum NetworkType
    {
        NONE,
        NWIFI,
        N2G,
        N3G,
        N4G
    };

    struct TelephonyManager
    {
        /** Network type is unknown */
        static const int NETWORK_TYPE_UNKNOWN = 0;
        /** Current network is GPRS */
        static const int NETWORK_TYPE_GPRS = 1;
        /** Current network is EDGE */
        static const int NETWORK_TYPE_EDGE = 2;
        /** Current network is UMTS */
        static const int NETWORK_TYPE_UMTS = 3;
        /** Current network is CDMA: Either IS95A or IS95B*/
        static const int NETWORK_TYPE_CDMA = 4;
        /** Current network is EVDO revision 0*/
        static const int NETWORK_TYPE_EVDO_0 = 5;
        /** Current network is EVDO revision A*/
        static const int NETWORK_TYPE_EVDO_A = 6;
        /** Current network is 1xRTT*/
        static const int NETWORK_TYPE_1xRTT = 7;
        /** Current network is HSDPA */
        static const int NETWORK_TYPE_HSDPA = 8;
        /** Current network is HSUPA */
        static const int NETWORK_TYPE_HSUPA = 9;
        /** Current network is HSPA */
        static const int NETWORK_TYPE_HSPA = 10;
        /** Current network is iDen */
        static const int NETWORK_TYPE_IDEN = 11;
        /** Current network is EVDO revision B*/
        static const int NETWORK_TYPE_EVDO_B = 12;
        /** Current network is LTE */
        static const int NETWORK_TYPE_LTE = 13;
        /** Current network is eHRPD */
        static const int NETWORK_TYPE_EHRPD = 14;
        /** Current network is HSPA+ */
        static const int NETWORK_TYPE_HSPAP = 15;
        /** Current network is GSM */
        static const int NETWORK_TYPE_GSM = 16;
        /** Current network is TD_SCDMA */
        static const int NETWORK_TYPE_TD_SCDMA = 17;
        /** Current network is IWLAN */
        static const int NETWORK_TYPE_IWLAN = 18;
        /** Current network is LTE_CA {@hide} */
        static const int NETWORK_TYPE_LTE_CA = 19;
    };

}//namespace


std::string PlatformDevice::GetDeviceId()
{
    std::string strDeviceId = detail::GetSystemProperty("ro.serialno");
    do
    {
        BREAK_IF(!strDeviceId.empty());
        strDeviceId = detail::GetSystemProperty("ro.boot.serialno");
        BREAK_IF(!strDeviceId.empty());
        strDeviceId = detail::GetSystemProperty("gsm.sim.imei");
    } while (false);

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

std::string PlatformDevice::GetRendererVersion()
{
    //return detail::GetSystemProperty("ro.opengles.version");
    char* szOpenGLVersionString = (char*)glGetString(GL_VERSION);
    return szOpenGLVersionString ? szOpenGLVersionString : "";
}

std::string PlatformDevice::GetCPUArch()
{
    return detail::GetSystemProperty("ro.product.cpu.abi");
}

int PlatformDevice::GetCPUCoreCount()
{
    return android_getCpuCount();
}

int PlatformDevice::GetCPUMaxFreq(int cpuIndex/* = -1*/)
{
    return detail::ReadCPUsFreq("max", cpuIndex);
}

int PlatformDevice::GetCPUCurFreq(int cpuIndex/* = -1*/)
{
    return detail::ReadCPUsFreq("cur", cpuIndex);
}

int PlatformDevice::GetCPUMinFreq(int cpuIndex/* = -1*/)
{
    return detail::ReadCPUsFreq("min", cpuIndex, true);
}

int PlatformDevice::GetNetworkType()
{
    static const std::string CONNECTIVITY_SERVICE = "connectivity";
    static const int TYPE_MOBILE = 0;
    static const int TYPE_WIFI = 1;

    JNIEnv* env = AndroidNode::AndroidJNI::GetJavaEnv();
    jobject mainActivity = AndroidNode::AndroidJNI::GetMainActivity();
    AndroidNode::AndroidJavaObject  ajoMainActivity(mainActivity);
    jobject ConnectivityManager = ajoMainActivity.Call<jobject>("getSystemService", CONNECTIVITY_SERVICE);
    AndroidNode::AndroidJavaObject ajoConnectivityManager(ConnectivityManager);
    jobject NetworkInfo = ajoConnectivityManager.CallWithSig<jobject>("getNetworkInfo", "(I)Landroid/net/NetworkInfo;", TYPE_WIFI);
    if (NetworkInfo != nullptr)
    {
        AndroidNode::AndroidJavaObject ajoNetworkInfo(NetworkInfo);
        jobject State = ajoNetworkInfo.CallWithSig<jobject>("getState", "()Landroid.net.NetworkInfo$State;");
        int stateValue = AndroidNode::Call<int>(State, "ordinal");
        env->DeleteLocalRef(State);
        if (stateValue == detail::NetworkState::CONNECTED)
        {
            env->DeleteLocalRef(NetworkInfo);
            env->DeleteLocalRef(ConnectivityManager);
            return detail::NetworkType::NWIFI;
        }
        env->DeleteLocalRef(NetworkInfo);
    }
    NetworkInfo = ajoConnectivityManager.CallWithSig<jobject>("getNetworkInfo", "(I)Landroid/net/NetworkInfo;", TYPE_MOBILE);
    detail::NetworkType netType = detail::NetworkType::NONE;
    if (NetworkInfo != nullptr)
    {
        AndroidNode::AndroidJavaObject ajoNetworkInfo(NetworkInfo);
        int subType = ajoNetworkInfo.Call<int>("getSubtype");
        switch (subType)
        {
        case detail::TelephonyManager::NETWORK_TYPE_GPRS:
        case detail::TelephonyManager::NETWORK_TYPE_EDGE:
        case detail::TelephonyManager::NETWORK_TYPE_CDMA:
        case detail::TelephonyManager::NETWORK_TYPE_1xRTT:
        case detail::TelephonyManager::NETWORK_TYPE_IDEN:
            netType = detail::NetworkType::N2G;
            break;

        case detail::TelephonyManager::NETWORK_TYPE_UMTS:
        case detail::TelephonyManager::NETWORK_TYPE_EVDO_0:
        case detail::TelephonyManager::NETWORK_TYPE_EVDO_A:
        case detail::TelephonyManager::NETWORK_TYPE_HSDPA:
        case detail::TelephonyManager::NETWORK_TYPE_HSUPA:
        case detail::TelephonyManager::NETWORK_TYPE_HSPA:
        case detail::TelephonyManager::NETWORK_TYPE_EVDO_B:
        case detail::TelephonyManager::NETWORK_TYPE_EHRPD:
        case detail::TelephonyManager::NETWORK_TYPE_HSPAP:
            netType = detail::NetworkType::N3G;
            break;
        case detail::TelephonyManager::NETWORK_TYPE_LTE:
            netType = detail::NetworkType::N4G;
            break;
        case detail::TelephonyManager::NETWORK_TYPE_UNKNOWN:
            netType = detail::NetworkType::NONE;
            break;
        default:
            netType = detail::NetworkType::N3G;

        }
        env->DeleteLocalRef(NetworkInfo);
    }
    env->DeleteLocalRef(ConnectivityManager);
    return netType;
}

std::string PlatformDevice::GetIpAddressV4()
{
    std::string ipaddressv4;
    std::string result = Platform::ExecuteSystemCommand("ip addr show wlan0 |grep \"inet \"");
    size_t inetPos = result.find("inet");
    if (inetPos != std::string::npos)
    {
        std::size_t chPos = result.find('/', inetPos);
        if (chPos != std::string::npos)
        {
            ipaddressv4 = result.substr(inetPos + strlen("inet"), chPos - inetPos - strlen("inet"));
        }
    }
    return StringUtils::Trim(ipaddressv4);
}

std::string PlatformDevice::GetIpAddressV6()
{
    std::string ipaddressv6;
    std::string result = Platform::ExecuteSystemCommand("ip addr show wlan0 |grep \"inet6 \"");
    size_t inet6Pos = result.find("inet6");
    if (inet6Pos != std::string::npos)
    {
        std::size_t chPos = result.find('/', inet6Pos);
        if (chPos != std::string::npos)
        {
            ipaddressv6 = result.substr(inet6Pos + strlen("inet6"), chPos - inet6Pos - strlen("inet6"));
        }
    }
    return StringUtils::Trim(ipaddressv6);
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

std::string PlatformDevice::GetGPURenderer()
{
    char* szRenderer = (char*)glGetString(GL_RENDERER);
    return szRenderer ? szRenderer : "";
}

std::string PlatformDevice::GetGPUVendor()
{
    char* szVendor = (char*)glGetString(GL_VENDOR);
    return szVendor ? szVendor : "";
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
        jobject mainActivity = AndroidNode::AndroidJNI::GetMainActivity();
        AndroidNode::AndroidJavaObject  ajoMainActivity(mainActivity);
        jobject WindowManager = ajoMainActivity.CallWithSig<jobject>("getWindowManager", "()Landroid/view/WindowManager;");
        AndroidNode::AndroidJavaObject ajoWindowManager(WindowManager);
        jobject Display = ajoWindowManager.CallWithSig<jobject>("getDefaultDisplay", "()Landroid/view/Display;");
        AndroidNode::AndroidJavaObject ajoDisplay(Display);
        AndroidNode::AndroidJavaObject ajoPoint("android.graphics.Point");
        AndroidNode::AndroidJavaObject ajoPointReal("android.graphics.Point");
        ajoDisplay.CallWithSig("getSize", "(Landroid/graphics/Point;)V", ajoPoint.GetRawObject());
        ajoDisplay.CallWithSig("getRealSize", "(Landroid/graphics/Point;)V", ajoPointReal.GetRawObject());
        GLOBAL_DisplayInfo.appSize = Size(ajoPoint.Get<int>("x"), ajoPoint.Get<int>("y") );
        GLOBAL_DisplayInfo.realSize = Size(ajoPointReal.Get<int>("x"), ajoPointReal.Get<int>("y"));
        AndroidNode::AndroidJavaObject ajoDisplayMetrics("android.util.DisplayMetrics");
        ajoDisplay.CallWithSig("getRealMetrics", "(Landroid.util.DisplayMetrics;)V", ajoDisplayMetrics.GetRawObject());
        GLOBAL_DisplayInfo.fps = ajoDisplay.Call<float>("getRefreshRate");
        GLOBAL_DisplayInfo.densityDpi = ajoDisplayMetrics.Get<int>("densityDpi");
        GLOBAL_DisplayInfo.nativeScale = ajoDisplayMetrics.Get<float>("density");
        GLOBAL_DisplayInfo.xdpi = ajoDisplayMetrics.Get<float>("xdpi");
        GLOBAL_DisplayInfo.ydpi = ajoDisplayMetrics.Get<float>("ydpi");
        JNIEnv* env = AndroidNode::AndroidJNI::GetJavaEnv();
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
        , static_cast<float>(GLOBAL_DisplayInfo.appSize.width)
        , static_cast<float>(GLOBAL_DisplayInfo.appSize.height));
}

Rect PlatformDevice::GetScreenNativeResolution()
{
    lazyGetScreenResolution();
     return Rect(static_cast<float>(0)
        , static_cast<float>(0)
        , static_cast<float>(GLOBAL_DisplayInfo.realSize.width)
        , static_cast<float>(GLOBAL_DisplayInfo.realSize.height));
}

float PlatformDevice::GetScreenDPI()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.densityDpi;
}

float PlatformDevice::GetScreenFPS()
{
    lazyGetScreenResolution();
    return GLOBAL_DisplayInfo.fps;
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

float PlatformDevice::GetNativeScale()
{
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

NS_FK_END
