/****************************************************************************
Copyright (c) 2015 libo.

losemymind.libo@gmail.com

****************************************************************************/
#ifdef WIN32
#include <windows.h>
#include <WindowsX.h>
#include <psapi.h>
#include "FoundationKit/FoundationKitDefines.h"
#include "FoundationKit/Platform/Platform.h"

USING_NS_FK;


int Platform::getTotalMemory()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	//DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
	DWORDLONG physical_memory = memInfo.ullTotalPhys;
	return static_cast<int>((physical_memory / 1048576)*1024);
}

int Platform::getUsedMemory()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG used_memory = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	return static_cast<int>((used_memory / 1048576) * 1024);
}

int Platform::getProcessMemory()
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	//SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	SIZE_T used_memory = pmc.WorkingSetSize;
	return (used_memory / 1048576)*1024;
}

#endif