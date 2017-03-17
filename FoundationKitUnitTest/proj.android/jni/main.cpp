/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
#include "FoundationKit/Platform/Platform.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaClass.hpp"
#include "FoundationKit/std/function_cache.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"
#include <vector>
#include <stdarg.h>


using namespace FoundationKit;
using namespace AndroidNode;

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
extern "C"{

static JavaVM* g_vm = nullptr;

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	ANDROID_LOGE("============== >>>>> JNI_OnLoad");
    g_vm = vm;
    return JNI_VERSION_1_6;
}


size_t noCache(size_t n)  
{  
    return (n < 2) ? n : noCache(n - 1) + noCache(n - 2);  
}

size_t hasCache(size_t n)  
{  
     return (n < 2) ? n : std::function_cache(hasCache)(n - 1) + std::function_cache(hasCache)(n - 2);  
} 

void testFunctionCache()
{
     auto v1 = noCache(45);
     auto v2 = hasCache(45);
}
    

JNIEXPORT void JNICALL Java_com_example_foundationkitunittest_MainActivity_foundationInit( JNIEnv* env,jobject thiz,jobject context)
{
    ANDROID_LOGE("============== >>>>> foundationInit");
     AndroidJNI::initializeJavaEnv(g_vm, JNI_VERSION_1_6, context);
     AndroidJavaObject  mainActive(context);
     mainActive.call("debug_Print", 100,"======", "========");
    ANDROID_LOGE("========== MAC ADDRESS: %s", Platform::getMacAddress().c_str());

    AndroidJavaClass  mainActiveClass("com.example.foundationkitunittest.MainActivity");

    std::string mainClassName = mainActiveClass.callStatic<std::string>("getClassName");

    ANDROID_LOGE("========== mainClassName: %s", mainClassName.c_str());
     



    //testFunctionCache();
}

}//extern "C"{








