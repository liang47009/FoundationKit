/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJNIBRIDGE_HPP
#define FOUNDATIONKIT_ANDROIDJNIBRIDGE_HPP
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <jni.h>
extern "C"
{
    JNIEXPORT jobject JNICALL Java_com_losemymind_foundationkit_AndroidJNIBridge_InvokeProxy(JNIEnv* env, jobject thiz, jlong ptr, jclass clazz, jobject method, jobjectArray args);
    JNIEXPORT void    JNICALL Java_com_losemymind_foundationkit_AndroidJNIBridge_DeleteProxy(JNIEnv* env, jobject thiz, jlong ptr);
    JNIEXPORT void    JNICALL Java_com_losemymind_foundationkit_AndroidJNIBridge_NativeSendMeeage(JNIEnv* env,jobject obj, jstring InMethodName, jstring InSignature, jobjectArray InArgs);
}//extern "C"


#endif //#if PLATFORM_ANDROID
#endif // END OF FOUNDATIONKIT_ANDROIDJNIBRIDGE_HPP