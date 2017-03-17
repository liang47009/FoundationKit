/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORM_HPP
#define FOUNDATIONKIT_PLATFORM_HPP

#pragma once
#include <string>
#include <functional>
#include <vector>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/Rect.hpp"
#include "FoundationKit/Base/types.hpp"

NS_FK_BEGIN

enum UnitOfMemory : int64
{
    Kb = 1 * 1024,
    Mb = Kb * 1024,
    Gb = Mb * 1024,
    Tb = Gb * 1024,
    Pb = Tb * 1024,
    Eb = Pb * 1024
};

class Platform
{
public:
	Platform();
	~Platform();

	// MB
	static float getTotalMemory();

	// MB
    static float getFreeMemory();

	// MB
    static float getProcessMemory();

    static std::vector<uint8> getMacAddressRaw();
    static std::string getMacAddress();

    static std::string getDeviceId();

    static std::string getDeviceName();

    static std::string getOperatingSystemVersion();

    static std::string getCPUArchitecture();

    static int64 getTickCount();

    static std::string executeSystemCommand(const std::string& command);
};

NS_FK_END


#endif // FOUNDATIONKIT_PLATFORM_HPP
