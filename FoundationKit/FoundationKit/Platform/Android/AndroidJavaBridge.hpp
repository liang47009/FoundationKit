/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVABRIDGE_HPP
#define FOUNDATIONKIT_ANDROIDJAVABRIDGE_HPP

#pragma once
#include <vector>
#include "AndroidJNI/AndroidJavaObject.hpp"

NS_FK_BEGIN

using namespace AndroidNode;

namespace Android
{
    typedef std::vector<AndroidJavaObject> NativeArguments;
} //namespace Android

NS_FK_END

#endif // FOUNDATIONKIT_ANDROIDJAVABRIDGE_HPP
