#ifdef ANDROID
#include "FoundationKit/Platform/Platform.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

NS_FK_BEGIN

int Platform::getTotalMemory()
{
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long physical_memory = memInfo.totalram;
	physical_memory *= memInfo.mem_unit;
	return (physical_memory / 1048576)*1024;
}

int Platform::getUsedMemory()
{
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long used_memory = memInfo.totalram;
	used_memory *= memInfo.mem_unit;
	return (used_memory / 1048576)*1024;
}

static int parseLine(char* line)
{ 
    int i = strlen(line); 
    while (*line < '0' || *line > '9') line++; 
    line[i-3] = '\0'; 
    i = atoi(line); 
    return i; 
}


int Platform::getProcessMemory()
{
	FILE* file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];
	while (fgets(line, 128, file) != NULL){
		if (strncmp(line, "VmRSS:", 6) == 0) result = parseLine(line);
		break;
	}
	fclose(file);
	return result;
}

NS_FK_END

#endif