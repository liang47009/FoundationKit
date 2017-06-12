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

class Platform
{
public:
	Platform();
	~Platform();
    static bool IsDebuggerPresent();
    static size_t MallocUsableSize(void* ptr);
    static std::string ExecuteSystemCommand(const std::string& command);
    static std::string ScreenShot();
};

NS_FK_END


#endif // FOUNDATIONKIT_PLATFORM_HPP
