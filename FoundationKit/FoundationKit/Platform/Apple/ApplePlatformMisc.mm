/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_APPLE)
#include <stdlib.h>
#include "FoundationKit/Platform/PlatformMisc.hpp"
NS_FK_BEGIN

std::vector<std::string> PlatformMisc::GetCommandLineArgs()
{
    std::vector<std::string> commandArgs;
    NSArray* arguments = [[NSProcessInfo processInfo] arguments];
    for (NSString *item in arguments)
    {
        commandArgs.push_back([item UTF8String]);
    }
    return commandArgs;
}

NS_FK_END

#endif // #if (PLATFORM_APPLE)

