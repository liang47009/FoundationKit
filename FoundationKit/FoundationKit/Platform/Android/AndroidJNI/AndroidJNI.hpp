/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJNI_HPP
#define FOUNDATIONKIT_ANDROIDJNI_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if TARGET_PLATFORM == PLATFORM_ANDROID

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

namespace AndroidNode{

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

    /*
     * Register JNI native methods
     * @code
     * Defined the method.
     * JNIEXPORT jstring JNICALL native_hello(JNIEnv *env, jclass clazz)
     *  {
     *      printf("hello in c native code./n");
     *      return (*env)->NewStringUTF(env, "hello world returned.");
     *  }
     *  
     *  Defined JNINativeMethod array
     * static JNINativeMethod gMethods[] = 
     * {
     *     { "hello", "()Ljava/lang/String;", (void*)native_hello},
     * };
     * bool ret = AndroidJNI::registerNativeMethods("com/example/MainActivity", gMethods);
     * @endcode
     */
    static bool            RegisterNativeMethods(const char* className, JNINativeMethod* nativeMethods);
    ~AndroidJNI();
private:
    AndroidJNI();
};

} // namespace AndroidNode

#endif //#if TARGET_PLATFORM == PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJNI_HPP
