/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJNIBRIDGE_HPP
#define FOUNDATIONKIT_ANDROIDJNIBRIDGE_HPP
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <vector>
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
NS_FK_BEGIN
namespace Android
{
    typedef std::vector<android::AndroidJavaObject> NativeArguments;
} //namespace Android
NS_FK_END
#endif //#if PLATFORM_ANDROID
#endif // END OF FOUNDATIONKIT_ANDROIDJNIBRIDGE_HPP