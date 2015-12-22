#ifndef FoundationKit_AndroidJavaBridge_H
#define FoundationKit_AndroidJavaBridge_H

#pragma once
#include <vector>
#include "AndroidJavaObject.h"

NS_FK_BEGIN

namespace Android
{
    typedef std::vector<AndroidJavaObject> NativeArguments;
} //namespace Android

NS_FK_END
#endif // FoundationKit_AndroidJavaBridge_H