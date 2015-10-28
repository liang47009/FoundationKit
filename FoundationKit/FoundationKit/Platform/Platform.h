/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include "FoundationKit/FoundationKitDefines.h"
#include "FoundationKit/Events/EventKeyboard.h"
NS_FK_BEGIN

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