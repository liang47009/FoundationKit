/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORMTLS_HPP
#define FOUNDATIONKIT_PLATFORMTLS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"

#if (PLATFORM_ANDROID)
#include "FoundationKit/Platform/Android/AndroidPlatformTLS.hpp"
#elif (PLATFORM_APPLE)
#include "FoundationKit/Platform/Apple/ApplePlatformTLS.hpp"
#elif (PLATFORM_WINDOWS)
#include "FoundationKit/Platform/Windows/WindowsPlatformTLS.hpp"
#elif (PLATFORM_LINUX)
#include "FoundationKit/Platform/Linux/LinuxPlatformTLS.hpp"
#elif (PLATFORM_SWITCH)
#include "FoundationKit/Platform/Switch/SwitchPlatformTLS.hpp"
#else
#error Unknown platform
#endif

#endif // FOUNDATIONKIT_PLATFORMTLS_HPP



