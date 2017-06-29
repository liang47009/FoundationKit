/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORMDEVICE_HPP
#define FOUNDATIONKIT_PLATFORMDEVICE_HPP

#include <string>
#include <vector>
#include "FoundationKit/Foundation/Rect.hpp"
#include "FoundationKit/Base/types.hpp"
NS_FK_BEGIN

struct PlatformMemoryConstants
{
    /** The size of a page, in bytes. */
    size_t PageSize;

    /** The amount of actual physical memory, in bytes. */
    uint64 TotalPhysical;

    /** The amount of virtual memory, in bytes. */
    uint64 TotalVirtual;

    // AddressLimit - Second parameter is estimate of the range of addresses expected to be returns by BinnedAllocFromOS(). Binned
    // Malloc will adjust its internal structures to make lookups for memory allocations O(1) for this range. 
    // It is ok to go outside this range, lookups will just be a little slower
    uint64 AddressLimit;

    /** The amount of physical memory currently available, in bytes. */
    uint64 AvailablePhysical;

    /** The amount of virtual memory currently available, in bytes. */
    uint64 AvailableVirtual;

    /** The amount of physical memory used by the process, in bytes. */
    uint64 UsedPhysical;

    /** The peak amount of physical memory used by the process, in bytes. */
    uint64 PeakUsedPhysical;

    /** Total amount of virtual memory used by the process. */
    uint64 UsedVirtual;

    /** The peak amount of virtual memory used by the process. */
    uint64 PeakUsedVirtual;

    /** Default constructor, clears all variables. */
    PlatformMemoryConstants()
        : PageSize(0)
        , TotalPhysical(0)
        , TotalVirtual(0)
        , AddressLimit((uint64)0xffffffff + 1)
        , AvailablePhysical(0)
        , AvailableVirtual(0)
        , UsedPhysical(0)
        , PeakUsedPhysical(0)
        , UsedVirtual(0)
        , PeakUsedVirtual(0)
    {}

    /** Copy constructor, used by the generic platform memory stats. */
    PlatformMemoryConstants(const PlatformMemoryConstants& Other)
        : PageSize(Other.PageSize)
        , TotalPhysical(Other.TotalPhysical)
        , TotalVirtual(Other.TotalVirtual)
        , AddressLimit(Other.AddressLimit)
        , AvailablePhysical(Other.AvailablePhysical)
        , AvailableVirtual(Other.AvailableVirtual)
        , UsedPhysical(Other.UsedPhysical)
        , PeakUsedPhysical(Other.PeakUsedPhysical)
        , UsedVirtual(Other.UsedVirtual)
        , PeakUsedVirtual(Other.PeakUsedVirtual)
    {}
};

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
     * @param cpuIndex The CPU core index
     * @return Unit: KHz
     */
    static int         GetCPUMaxFreq(int cpuIndex = -1);

    /**
     * If cpuIndex is equal to -1, then the function will return
     * the maximum frequency of all the CPU's current frequency.
     * @param cpuIndex  The CPU core index
     * @return Unit: KHz
     */
    static int         GetCPUCurFreq(int cpuIndex = -1);

    /**
     * If cpuIndex is equal to -1, then the function will return
     * the smallest of the minimum frequency of all CPUs.
     * @param cpuIndex  The CPU core index
     * @return Unit: KHz
     */
    static int         GetCPUMinFreq(int cpuIndex = -1);

    //1 WIFI,2 2G,3 3G,4 4G,0 other.
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
    static Rect GetScreenResolution();
    /// Get the screen resolution, including the virtual button area
    static Rect GetScreenNativeResolution();

    //px = dp * (dpi / 160)
    //dp = px/(dpi/160)
    //dpi = px/dp*160
    static float GetScreenDPI();
    static float GetScreenFPS();
    static float GetScreenXDPI();
    static float GetScreenYDPI();
    static float GetNativeScale();
    static PlatformMemoryConstants& GetMemoryConstants();
};
NS_FK_END

#endif // FOUNDATIONKIT_PLATFORMDEVICE_HPP


