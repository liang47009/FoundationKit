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

#include "FoundationKit/Crypto/aes.hpp"
#include "FoundationKit/Crypto/Base58.hpp"
#include "FoundationKit/Crypto/Base64.hpp"
#include "FoundationKit/Crypto/crc.hpp"
#include "FoundationKit/Crypto/des.hpp"
#include "FoundationKit/Crypto/md5.hpp"
#include "FoundationKit/Crypto/sha1.hpp"
#include "FoundationKit/Crypto/urlencode.hpp"

#include "FoundationKit/Foundation/BitStream.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/Dictionary.hpp"
#include "FoundationKit/Foundation/Directory.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Exception.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Foundation/FunctionBox.hpp"
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/Singleton.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/Version.hpp"

#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Environment.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/PlatformDevice.hpp"
#include "FoundationKit/Platform/PlatformMemory.hpp"
#include "FoundationKit/Platform/PlatformMisc.hpp"
#include "FoundationKit/Platform/PlatformTLS.hpp"

#include "FoundationKit/Math/Math.hpp"
#include "FoundationKit/Math/MathContent.hpp"
#include "FoundationKit/Math/MathUtil.hpp"
#include "FoundationKit/Math/Matrix.hpp"
#include "FoundationKit/Math/Quaternion.hpp"
#include "FoundationKit/Math/Rect.hpp"
#include "FoundationKit/Math/Size.hpp"
#include "FoundationKit/Math/Vector2.hpp"
#include "FoundationKit/Math/Vector3.hpp"
#include "FoundationKit/Math/Vector4.hpp"

#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaProxy.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidLexicalCast.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNIOperation.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaCore.hpp"
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
    auto cmdList =  PlatformMisc::GetCommandLineArgs();
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

char *chararray =nullptr;
void Teststackprotector()
{
    chararray = new char[2];
    memcpy(chararray, "ssss", 4);
    char array[2];
    memcpy(array, "ssss", 4);
}


void JNIBridgeTest(char B
    , short S
    , int I
    , long J
    , float F
    , double D
    , char C
    , bool Z)
{

}

class AndroidJNIProxyTest : public android::AndroidJavaProxy
{
public:
    AndroidJNIProxyTest() : android::AndroidJavaProxy("com.losemymind.foundationkit.AndroidJNIProxyTest")
    {
        REG_PROXY_METHOD(AndroidJNIProxyTest, OnMethodInvoke);
        REG_PROXY_METHOD(AndroidJNIProxyTest, OnMethodInvoke1);
    }

    virtual void OnMethodInvoke(std::string MethodName, int a, double b)
    {
        ANDROID_LOGE("============== OnMethodInvoke MethodName=%s, a=%d, b=%f", MethodName.c_str(), a, b);
    }

    virtual void OnMethodInvoke1(std::string MethodName, wchar_t a, bool b)
    {
        ANDROID_LOGE("============== OnMethodInvoke1 MethodName=%s, a=%c, b=%d", MethodName.c_str(), a,b=true?1:0);
    }
};


void OnJavaCall(std::string value, int i, float f, const char* c)
{
    ANDROID_LOGE("============== OnJavaCall value=%s, i=%d, f=%f, c=%s", value.c_str(), i, f, c);
}

static AndroidJNIProxyTest* AndroidJNIProxyTestInstance = nullptr;
JNIEXPORT void JNICALL Java_com_example_test_MainActivity_foundationInit( JNIEnv* env,jobject thiz,jobject context)
{
    ANDROID_LOGE("============== >>>>> foundationInit");
    AndroidJNI::InitializeJavaEnv(g_vm, JNI_VERSION_1_6, context);
    PlatformDevice::DumpDeviceInfo();

    FoundationKit::NotificationCenter::DefaultCenter.AddObserver("OnJavaCall", BindFunctionBox(OnJavaCall));

    android::AndroidJavaObject  mainActive(context);
    mainActive.Call("debug_Print", 100,"======", "========");
    android::AndroidJavaClass  mainActiveClass("com.example.test.MainActivity");
    std::string mainClassName = mainActiveClass.CallStatic<std::string>("getClassName");
    ANDROID_LOGE("========== mainClassName: %s", mainClassName.c_str());
    AndroidJNIProxyTestInstance = new AndroidJNIProxyTest();
    mainActive.Call("setProxy", (jobject)(*AndroidJNIProxyTestInstance));
     
    //char* leak = new char[1024];
    //int ret = TestHeapUseAfterFree(10);
    //ret = TestHeapBufferOverflow(10);
    //ret = TestStackBufferOverflow(10);
    //ret = TestGlobalBufferOverflow(10);
    //Teststackprotector();
    ANDROID_LOGE("========== Java_com_example_test_MainActivity_foundationInit End");
    //testFunctionCache();
}

}//extern "C"{








