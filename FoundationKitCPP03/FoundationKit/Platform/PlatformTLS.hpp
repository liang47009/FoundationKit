/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORMTLS_HPP
#define FOUNDATIONKIT_PLATFORMTLS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"

#if (TARGET_PLATFORM == PLATFORM_ANDROID)
#include "FoundationKit/Platform/Android/AndroidPlatformTLS.hpp"
#elif (TARGET_PLATFORM == PLATFORM_IOS) || (TARGET_PLATFORM == PLATFORM_MAC)
#include "FoundationKit/Platform/Apple/ApplePlatformTLS.hpp"
#elif (TARGET_PLATFORM == PLATFORM_WINDOWS)
#include "FoundationKit/Platform/windows/WindowsPlatformTLS.hpp"
#endif

#endif // FOUNDATIONKIT_PLATFORMTLS_HPP



