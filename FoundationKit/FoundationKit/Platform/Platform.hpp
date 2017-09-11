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

enum class TargetPlatform
{
    // Unknown target platform
    Unknown,

    // 32-bit Windows
    Win32,

    // 64-bit Windows
    Win64,

    // Mac
    Mac,

    // XboxOne
    XboxOne,

    // Playstation 4
    PS4,

    // iOS
    iOS,

    // Android
    Android,

    // HTML5
    HTML5,

    // Linux
    Linux,

    // All desktop platforms
    AllDesktop,

    // TVOS
    TVOS,

    // Nintendo Switch
    Switch,
};


class Platform
{
public:
	Platform();
	~Platform();
    static bool IsDebuggerPresent();
    static size_t MallocUsableSize(void* ptr);
    static std::string ExecuteSystemCommand(const std::string& command);
    static bool ScreenShot(std::string& outSavePath);
};

NS_FK_END


#endif // FOUNDATIONKIT_PLATFORM_HPP
