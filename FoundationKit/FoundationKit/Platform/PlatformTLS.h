#ifndef LOSEMYMIND_PLATFORMTLS_H
#define LOSEMYMIND_PLATFORMTLS_H

#include "FoundationKit/GenericPlatformMacros.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "FoundationKit/Platform/Android/AndroidPlatformTLS.h"
#elif TARGET_PLATFORM == PLATFORM_IOS
#include "FoundationKit/Platform/ios/ApplePlatformTLS.h"
#elif TARGET_PLATFORM == PLATFORM_WINDOWS
#include "FoundationKit/Platform/windows/WindowsPlatformTLS.h"
#endif

#endif // LOSEMYMIND_PLATFORMTLS_H


