/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_SWITCH)
#include <stdlib.h>
#include "FoundationKit/Platform/PlatformMisc.hpp"
NS_FK_BEGIN

std::vector<std::string> PlatformMisc::GetCommandLineArgs()
{
    std::vector<std::string> commandArgs;// = StringUtils::Split(GSavedCommandLine, ' ');

    return commandArgs;
}

NS_FK_END

#endif // #if (PLATFORM_SWITCH)

