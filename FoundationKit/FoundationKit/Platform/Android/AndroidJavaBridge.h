#ifndef LOSEMYMIND_ANDROIDJAVABRIDGE_H
#define LOSEMYMIND_ANDROIDJAVABRIDGE_H


#pragma once
#include <vector>
#include "AndroidJNI/AndroidJavaObject.h"

NS_FK_BEGIN

using namespace AndroidNode;

namespace Android
{
    typedef std::vector<AndroidJavaObject> NativeArguments;
} //namespace Android

NS_FK_END
#endif // LOSEMYMIND_ANDROIDJAVABRIDGE_H
