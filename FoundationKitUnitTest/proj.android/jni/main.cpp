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

void A3A8(unsigned char key[16], unsigned char randout[16])
{
    unsigned char* pKey = key;
    unsigned char* pRandout = randout;
    LOG_TRACE("============== A3A8 M %s, %s", pKey, pRandout);

    pKey[0]='A';
    pKey[1]='B';
    pKey[2]='C';
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
  
    

JNIEXPORT void JNICALL Java_com_example_foundationkitunittest_MainActivity_foundationInit( JNIEnv* env,jobject thiz,jobject context)
{
    LOG_TRACE("============== >>>>> foundationInit");
    std::string macaddress = AndroidSystemInfo::getInstance()->getMacAddress();
    LOG_TRACE("============== >>>>> macaddress = %s",macaddress.c_str());
    std::string deviceid = AndroidSystemInfo::getInstance()->getDeviceId();
    LOG_TRACE("============== >>>>> deviceid = %s",deviceid.c_str());

    testJavaString();
}


JNIEXPORT void JNICALL Java_com_example_foundationkitunittest_MainActivity_A3A8( JNIEnv* env,jobject thiz, jbyteArray key, jbyteArray randout)
{
    jbyte * recivedkey =env->GetByteArrayElements(key,0);
    jbyte * recivedrandout =env->GetByteArrayElements(randout,0);

    LOG_TRACE("============== A3A8 S %s, %s", recivedkey, recivedrandout);
    A3A8((unsigned char*)recivedkey, (unsigned char*)recivedrandout);
    LOG_TRACE("============== A3A8 E %s, %s", recivedkey, recivedrandout);

    
    for (int index = 0;recivedkey; ++index, ++recivedkey)
    {
        recivedkey[index]= (jbyte)*recivedkey;
    }
    
    env->ReleaseByteArrayElements(key,recivedkey,0);
    env->ReleaseByteArrayElements(randout,recivedrandout,0);

}


}//extern "C"{








