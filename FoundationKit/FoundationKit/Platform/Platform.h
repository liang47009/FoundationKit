/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_PLATFORM_H
#define LOSEMYMIND_PLATFORM_H

#pragma once
#include <string>
#include <functional>
#include <vector>
#include "FoundationKit/Base/Rect.h"
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"

NS_FK_BEGIN

enum class UnitOfMemory : int64
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
#endif // LOSEMYMIND_PLATFORM_H


