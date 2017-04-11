/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJNI_HPP
#define FOUNDATIONKIT_ANDROIDJNI_HPP

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
    static void            initializeJavaEnv(JavaVM* vm, jint version, jobject activityInstance = nullptr);
    static jobject         getMainActivity();
    // Returns the java environment
    static JNIEnv*         getJavaEnv();
    static jclass          findJavaClass(const char* name);
    static JavaClassMethod getClassMethod(jclass clazz, const char* methodName, const char* funcSig, bool isStatic = false);
    static JavaClassMethod getClassMethod(const char* className, const char* methodName, const char* funcSig, bool isStatic = false);
    static bool            checkJavaException();
    static std::string     jstring2string(jstring jstr);
    static jstring         string2jstring(const std::string& str);
    static AAssetManager*  getAAssetManager();
    static void            detachJavaEnv();

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
    static bool            registerNativeMethods(const char* className, JNINativeMethod* nativeMethods);
    ~AndroidJNI();
private:
    AndroidJNI();
};

} // namespace AndroidNode

#endif // FOUNDATIONKIT_ANDROIDJNI_HPP