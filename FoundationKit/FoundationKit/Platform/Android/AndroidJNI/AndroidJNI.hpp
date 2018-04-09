/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJNI_HPP
#define FOUNDATIONKIT_ANDROIDJNI_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <jni.h>
#include <pthread.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <string>
#include <memory>

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
    ~AndroidJNI();
private:
    AndroidJNI();
    static jint          CurrentJavaVersion;
    static JavaVM*       CurrentJavaVM;
    static jobject       MainActivityRef;
    static jobject       ClassLoader;
    static jmethodID     FindClassMethod;
    static pthread_key_t TlsSlot;
};


class ScopeJavaObjectRef
{
    jobject  jobject_ref;
public:
    
    ScopeJavaObjectRef(jobject obj) :jobject_ref(obj) {}
    ScopeJavaObjectRef& operator= (jobject obj) { jobject_ref = obj; return (*this); };
    ~ScopeJavaObjectRef() { AndroidJNI::GetJavaEnv()->DeleteLocalRef(jobject_ref); }
    explicit operator bool() const// _NOEXCEPT
    {
        return (!!jobject_ref);
    }

    jobject Get()const
    {
        return jobject_ref;
    }
};

class GlobalJavaObjectRef
{
public:
    GlobalJavaObjectRef():jobject_pointer(nullptr)
    {

    }
    GlobalJavaObjectRef(jobject InJobject)
    {
        JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
        if (InJobject)
        {
            jobject_pointer = std::shared_ptr<_jobject>(jniEnv->NewGlobalRef(InJobject), jobjectDeleter);
        }
        else
        {
            jobject_pointer = nullptr;
        }
    }

    ~GlobalJavaObjectRef()
    {
       
    }

    GlobalJavaObjectRef& operator= (jobject InJobject)
    {
        JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
        if (InJobject)
        {
            jobject_pointer = std::shared_ptr<_jobject>(jniEnv->NewGlobalRef(InJobject), jobjectDeleter);
        }
        else
        {
            jobject_pointer = nullptr;
        }
        return *(this);
    }

    GlobalJavaObjectRef(const GlobalJavaObjectRef& Other)
    {
        jobject_pointer = Other.jobject_pointer;
    }

    GlobalJavaObjectRef(GlobalJavaObjectRef&& Other)
    {
        jobject_pointer = std::move(Other.jobject_pointer);
    }

    GlobalJavaObjectRef& operator= (const GlobalJavaObjectRef& Other)
    {
        jobject_pointer = Other.jobject_pointer;
        return *(this);
    }

    GlobalJavaObjectRef& operator= (GlobalJavaObjectRef&& Other)
    {
        jobject_pointer = std::move(Other.jobject_pointer);
        return *(this);
    }

    jobject Get()const
    {
        return jobject_pointer.get();
    }

    operator jobject()const
    {
        return jobject_pointer.get();
    }

    operator bool() const
    {
        return (jobject_pointer != nullptr);
    }

    static void jobjectDeleter(jobject jobj)
    {
        if (jobj)
        {
            JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
            jniEnv->DeleteGlobalRef(jobj);
        }
    }
protected:
    std::shared_ptr<_jobject> jobject_pointer;
};

NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJNI_HPP
