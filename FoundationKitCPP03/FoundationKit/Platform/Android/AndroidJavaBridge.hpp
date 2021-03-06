/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVABRIDGE_HPP
#define FOUNDATIONKIT_ANDROIDJAVABRIDGE_HPP
#pragma once

#include "FoundationKit/GenericPlatformMacros.hpp"
#if TARGET_PLATFORM == PLATFORM_ANDROID

#include <vector>
#include "AndroidJNI/AndroidJavaObject.hpp"

NS_FK_BEGIN

using namespace AndroidNode;

namespace Android
{
    typedef std::vector<AndroidJavaObject> NativeArguments;
} //namespace Android

NS_FK_END

#endif //#if TARGET_PLATFORM == PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJAVABRIDGE_HPP
