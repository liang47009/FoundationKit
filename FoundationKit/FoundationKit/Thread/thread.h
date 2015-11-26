#ifndef FoundationKit_thread_H
#define FoundationKit_thread_H

#include "Impl/thread.h"

#if defined(WIN32) && defined(_WINDOWS)
#include "Impl/thread_win32.h"
#elif defined(ANDROID) || defined(IOS)
#include "Impl/thread_posix.h"
#elif defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#include "Impl/thread_wp.h"
#else
#error "Unsupported platform."
#endif

#endif // FoundationKit_thread_H