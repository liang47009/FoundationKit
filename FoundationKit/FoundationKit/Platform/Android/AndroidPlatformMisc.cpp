/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_ANDROID)
#include <stdlib.h>
#include "FoundationKit/Platform/PlatformMisc.hpp"
NS_FK_BEGIN

std::vector<std::string> PlatformMisc::GetCommandLineArgs()
{
    std::vector<std::string> commandArgs;// = StringUtils::Split(GSavedCommandLine, ' ');
    FILE *fp = nullptr;
    if ((fp = fopen("/proc/self/cmdline", "r")) == NULL)
    {
        FKLog("Cannot open /proc/self/cmdline file!");
        return commandArgs;
    }
    char line_buf[256] = { 0 };
    while (fgets(line_buf, sizeof(line_buf), fp) != NULL)
    {
        commandArgs.push_back(line_buf);
    }
    fclose(fp);
    return commandArgs;
}

NS_FK_END

#endif // #if (PLATFORM_ANDROID)

