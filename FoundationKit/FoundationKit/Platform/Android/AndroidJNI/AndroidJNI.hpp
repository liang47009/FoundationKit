/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJNI_HPP
#define FOUNDATIONKIT_ANDROIDJNI_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <string>

#if defined(_DEBUG) || defined(DEBUG)
#define ANDROID_LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,"AndroidJNI", __VA_ARGS__))
#else
#define ANDROID_LOGD(...) 
#endif

#define ANDROID_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidJNI", __VA_ARGS__))
#define ANDROID_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidJNI", __VA_ARGS__))
#define ANDROID_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"AndroidJNI", __VA_ARGS__))
#define ANDROID_CHECKF(expr, format,  ...) { if(!(expr)) {ANDROID_LOGE(format, ##__VA_ARGS__);}}

NS_FK_BEGIN


struct JavaClassMethod
{
    JNIEnv *    env;
    jclass      clazz;
    jmethodID	method;
};

class AndroidJNI
{
public:
    static void            InitializeJavaEnv(JavaVM* vm, jint version, jobject activityInstance = nullptr);
    static jobject         GetMainActivity();
    // Returns the java environment
    static JNIEnv*         GetJavaEnv();
    static jclass          FindJavaClass(const char* name);
    static JavaClassMethod GetClassMethod(jclass clazz, const char* methodName, const char* funcSig, bool isStatic = false);
    static JavaClassMethod GetClassMethod(const char* className, const char* methodName, const char* funcSig, bool isStatic = false);
    static bool            CheckJavaException();
    static std::string     jstring2string(jstring jstr);
    static jstring         string2jstring(const std::string& str);
    static AAssetManager*  GetAAssetManager();
    static void            DetachJavaEnv();
    static bool            RegisterNativeMethods(const char* className, JNINativeMethod* nativeMethods);
    ~AndroidJNI();
private:
    AndroidJNI();
};


class Scope_JObject
{
public:
    jobject  jobject_ref;
    Scope_JObject(jobject obj) :jobject_ref(obj) {}
    Scope_JObject& operator= (jobject obj) { jobject_ref = obj; return (*this); };
    ~Scope_JObject() { AndroidJNI::GetJavaEnv()->DeleteLocalRef(jobject_ref); }
    explicit operator bool() const// _NOEXCEPT
    {
        return (!!jobject_ref);
    }
};


NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJNI_HPP
