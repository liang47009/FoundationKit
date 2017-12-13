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
#include <vector>
#include <stdarg.h>

#include "FoundationKit/forward.hpp"
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/LanguageFeatures.hpp"

#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Crypto/sha1.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Foundation/ITickable.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/MathContent.hpp"
#include "FoundationKit/Foundation/MathUtil.hpp"
#include "FoundationKit/Foundation/Matrix.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Quaternion.hpp"
#include "FoundationKit/Foundation/Rect.hpp"
#include "FoundationKit/Foundation/Runnable.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/Size.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/ThreadTimer.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/Timespan.hpp"
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/Vector2.hpp"
#include "FoundationKit/Foundation/Vector3.hpp"
#include "FoundationKit/Foundation/Vector4.hpp"
#include "FoundationKit/Foundation/Value.hpp"

#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/OpenGL.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"

#include "FoundationKit/Platform/Android/AndroidJNI/AndroidFoundation.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaClass.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"


using namespace FoundationKit;

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
    auto cmdList =  Environment::GetCommandLineArgs();
    ANDROID_LOGE("============== CommandLineArgs:");
    for(std::string val : cmdList)
    {
        ANDROID_LOGE("Command = %s", val.c_str());
    }
    ANDROID_LOGE("============== CommandLineArgs end");
    return JNI_VERSION_1_6;
}


size_t noCache(size_t n)  
{  
    return (n < 2) ? n : noCache(n - 1) + noCache(n - 2);  
}

size_t hasCache(size_t n)  
{  
     return (n < 2) ? n : function_cache(hasCache)(n - 1) + function_cache(hasCache)(n - 2);  
} 

void testFunctionCache()
{
     auto v1 = noCache(45);
     auto v2 = hasCache(45);
}
    
int TestHeapUseAfterFree(int argc)
{
    int *array = new int[100];
    delete [] array;
    return array[argc];  // BOOM
}

int TestHeapBufferOverflow(int argc)
{
    int *array = new int[100];
    array[0] = 0;
    int res = array[argc + 100];  // BOOM
    delete [] array;
    return res;
}

int TestStackBufferOverflow(int argc)
{
    int stack_array[100];
    stack_array[1] = 0;
    return stack_array[argc + 100];  // BOOM
}

int global_array[100] = {-1};
int TestGlobalBufferOverflow(int argc)
 {
    return global_array[argc + 100];  // BOOM
}

JNIEXPORT void JNICALL Java_com_example_test_MainActivity_foundationInit( JNIEnv* env,jobject thiz,jobject context)
{
    ANDROID_LOGE("============== >>>>> foundationInit");
    AndroidJNI::InitializeJavaEnv(g_vm, JNI_VERSION_1_6, context);
    AndroidJavaObject  mainActive(context);
    mainActive.Call("debug_Print", 100,"======", "========");

    AndroidJavaClass  mainActiveClass("com.example.test.MainActivity");

    std::string mainClassName = mainActiveClass.CallStatic<std::string>("getClassName");
    ANDROID_LOGE("========== mainClassName: %s", mainClassName.c_str());
     
    char* leak = new char[1024];
    int ret = TestHeapUseAfterFree(10);
    ret = TestHeapBufferOverflow(10);
    ret = TestStackBufferOverflow(10);
    ret = TestGlobalBufferOverflow(10);
    ANDROID_LOGE(" CPU Core count:%d", PlatformDevice::GetCPUCoreCount());
    ANDROID_LOGE("========== Java_com_example_test_MainActivity_foundationInit End");
    //testFunctionCache();
}

}//extern "C"{








