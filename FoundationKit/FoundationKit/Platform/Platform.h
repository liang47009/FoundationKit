/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
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

	// MemoryUsage
	// KB
	static int getTotalMemory();

	// KB
	static int getUsedMemory();

	// KB
	static int getProcessMemory();
};

NS_FK_END