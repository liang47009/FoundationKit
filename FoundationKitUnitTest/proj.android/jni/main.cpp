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
#include "FoundationKit/Platform/Android/AndroidJNIHelper.h"
#include "FoundationKit/Platform/Android/AndroidSystemInfo.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/Platform/Android/AndroidTest.h"
#include "FoundationKit/Platform/Android/AndroidJavaBridge.h"
#include "FoundationKit/stdextensions/utility.hpp"
#include "FoundationKit/Foundation/Timer.h"
#include <vector>
#include <stdarg.h>


using namespace FoundationKit;
using namespace FoundationKit::Android;


template<typename... Args>
static void doTest(Args&&... args)
{

}


/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
extern "C"{

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	LOGD("============== >>>>> JNI_OnLoad");
	AndroidJNIHelper::getInstance()->setJavaVM(vm);
	//initAndroidCrashDump();
    return JNI_VERSION_1_4;
}

void getTestValue(NativeArguments args)
{
    LOGD("====== getTestValue by call.");

    int v0 = args[0].convertWith<jint>();
    int v1 = args[1].convertWith<jint>();
    std::string v2 = args[2].convertWith<jstring>();
    LOGD("====== getTestValue v0:%d", v0);
    LOGD("====== getTestValue v1:%d", v1);
    LOGD("====== getTestValue v1:%s", v2.c_str());
    std::vector<unsigned char> arr = args[3].convertWith<jcharArray>();
    for(auto c : arr)
    {
        LOGD("======= c: %c", c);
    }
}

size_t noCache(size_t n)  
{  
    return (n < 2) ? n : noCache(n - 1) + noCache(n - 2);  
}

size_t hasCache(size_t n)  
{  
     return (n < 2) ? n : sugar(hasCache)(n - 1) + sugar(hasCache)(n - 2);  
} 

void testFunctionCache()
{
    Timer t1;
     auto v1 = noCache(45);
     LOGD("=========== noCache value:%d run time:%lld", v1, t1.elapsed_seconds());
     Timer t2;
     auto v2 = hasCache(45);
     LOGD("=========== hasCache value:%d run time:%lld", v2, t2.elapsed_seconds());
}
    

JNIEXPORT void JNICALL Java_com_example_foundationkitunittest_MainActivity_foundationInit( JNIEnv* env,jobject thiz,jobject context)
{
    LOG_TRACE("============== >>>>> foundationInit");
    std::string macaddress = AndroidSystemInfo::getInstance()->getMacAddress();
    LOG_TRACE("============== >>>>> macaddress = %s",macaddress.c_str());
    std::string deviceid = AndroidSystemInfo::getInstance()->getDeviceId();
    LOG_TRACE("============== >>>>> deviceid = %s",deviceid.c_str());

    testJavaString();

    AndroidJavaBridge::getInstance()->registerMethod<decltype(getTestValue)>("getTestValue",getTestValue);
    testFunctionCache();


}

}//extern "C"{








