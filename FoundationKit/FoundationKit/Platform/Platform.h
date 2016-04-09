/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <string>
#include "FoundationKit/FoundationKitDefines.h"
NS_FK_BEGIN

enum class CPU_TYPE
{
    //https://sourceforge.net/p/predef/wiki/Home/
    // https://sourceforge.net/p/predef/wiki/Architectures/
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


    static std::string getMacAddress();

    static std::string getDeviceId();

    static std::string getDeviceName();

    static std::string getOperatingSystemVersion();

    static std::string getCPUArchitecture();

};

NS_FK_END