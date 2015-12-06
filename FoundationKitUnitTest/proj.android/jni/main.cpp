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
#include <vector>


using namespace FoundationKit;
using namespace FoundationKit::Android;

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
  
    

JNIEXPORT void JNICALL Java_com_example_foundationkitunittest_MainActivity_foundationInit( JNIEnv* env,jobject thiz,jobject context)
{
    LOG_TRACE("============== >>>>> foundationInit");
    std::string macaddress = AndroidSystemInfo::getInstance()->getMacAddress();
    LOG_TRACE("============== >>>>> macaddress = %s",macaddress.c_str());
    std::string deviceid = AndroidSystemInfo::getInstance()->getDeviceId();
    LOG_TRACE("============== >>>>> deviceid = %s",deviceid.c_str());

    testJavaString();
}

}//extern "C"{








