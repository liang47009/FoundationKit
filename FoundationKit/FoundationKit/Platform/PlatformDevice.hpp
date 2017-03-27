/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORMDEVICE_HPP
#define FOUNDATIONKIT_PLATFORMDEVICE_HPP

#include <string>
#include <vector>
#include "FoundationKit/Foundation/Size.hpp"
NS_FK_BEGIN

class PlatformDevice
{
public:
    typedef std::vector<std::string> string_list;
    static std::string GetDeviceId();
    static std::string GetProduct();
    static std::string GetHardware();
    static std::string GetDevice();
    static std::string GetBrandName();
    static std::string GetModel();
    static std::string GetManufacturer();
    static std::string GetSystemVersion();
    static std::string GetSDKVersion();
    static std::string GetRendererVersion();
    static std::string GetCPUArch();
    static int         GetCPUCoreCount();

    /** 
     * If cpuIndex is equal to - 1, then the function will return 
     * the largest of the maximum frequency of all CPUs.
     * @param cpuIndex
     * @return Unit: KHz
     */
    static int         GetCPUMaxFreq(int cpuIndex = -1);

    /**
     * If cpuIndex is equal to -1, then the function will return
     * the maximum frequency of all the CPU's current frequency.
     * @param cpuIndex
     * @return Unit: KHz
     */
    static int         GetCPUCurFreq(int cpuIndex = -1);

    /**
     * If cpuIndex is equal to -1, then the function will return
     * the smallest of the minimum frequency of all CPUs.
     * @param cpuIndex
     * @return Unit: KHz
     */
    static int         GetCPUMinFreq(int cpuIndex = -1);

    //1 WIFI£¬2 2G£¬3 3G£¬4 4G£¬0 other.
    static int         GetNetworkType();
    static std::string GetIpAddressV4();
    static std::string GetIpAddressV6();
    static string_list GetDNS();
    // byte
    static long long   GetTotalMemory();
    // byte
    static long long   GetAvailableMemory();
    static std::string GetGPURenderer();
    static std::string GetGPUVendor();
    /// Get the screen resolution, not including the virtual button area
    static Size GetScreenResolution();
    /// Get the screen resolution, including the virtual button area
    static Size GetScreenNativeResolution();

    //px = dp * (dpi / 160)
    //dp = px/(dpi/160)
    //dpi = px/dp*160
    static int   GetScreenDPI();
    static float GetScreenFPS();
    static float GetScreenXDPI();
    static float GetScreenYDPI();
    static float GetNativeScale();

    static std::string ExecuteSystemCommand(const std::string& command);
};
NS_FK_END

#endif // FOUNDATIONKIT_PLATFORMDEVICE_HPP


