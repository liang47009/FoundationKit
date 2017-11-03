/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORMTLS_HPP
#define FOUNDATIONKIT_PLATFORMTLS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"

#if (PLATFORM_ANDROID)
#include "FoundationKit/Platform/Android/AndroidPlatformTLS.hpp"
#elif (PLATFORM_IOS) || (PLATFORM_MAC)
#include "FoundationKit/Platform/Apple/ApplePlatformTLS.hpp"
#elif (PLATFORM_WINDOWS)
#include "FoundationKit/Platform/windows/WindowsPlatformTLS.hpp"
#endif

#endif // FOUNDATIONKIT_PLATFORMTLS_HPP



