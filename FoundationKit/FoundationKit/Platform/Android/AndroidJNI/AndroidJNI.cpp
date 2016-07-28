/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#include <pthread.h>
#include <string>
#include <algorithm>
#include <android/asset_manager_jni.h>
#include "AndroidJNI.h"

namespace AndroidNode{

static jint      CurrentJavaVersion = JNI_VERSION_1_4;
static JavaVM*   CurrentJavaVM      = nullptr;
static jobject   MainActivityRef    = nullptr;
static jobject   ClassLoader        = nullptr;
static jmethodID FindClassMethod    = nullptr;
static pthread_key_t TlsSlot;

AndroidJNI::AndroidJNI()
{

}

AndroidJNI::~AndroidJNI()
{

}

static void JavaEnvDestructor(void*)
{
    ANDROID_LOGI("*** JavaEnvDestructor: %ld", pthread_self());
    AndroidJNI::detachJavaEnv();
}

void AndroidJNI::initializeJavaEnv(JavaVM* vm, jint version, jobject activityInstance/* = nullptr*/)
{
    if (CurrentJavaVM == nullptr)
    {
        CurrentJavaVM = vm;
        CurrentJavaVersion = version;
        MainActivityRef = activityInstance;
        pthread_key_create(&TlsSlot, &JavaEnvDestructor);
        JNIEnv* env = AndroidJNI::getJavaEnv();
        jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");

        if (activityInstance)
        {
            jclass classClass = env->FindClass("java/lang/Class");
            jmethodID classLoaderMethod = env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
            jclass MainClass = env->GetObjectClass(activityInstance);
            jobject classLoader = env->CallObjectMethod(MainClass, classLoaderMethod);
            ClassLoader = env->NewGlobalRef(classLoader);
            // The old version of the DexClassLoader SDK has the findClass method, 
            // the new version of SDK is the loadClass method
            FindClassMethod = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
            if (FindClassMethod == nullptr)
            {
                FindClassMethod = env->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
            }
        }
    }
}

jobject AndroidJNI::getMainActivity()
{
    return MainActivityRef;
}

JNIEnv* AndroidJNI::getJavaEnv()
{
    // register a destructor to detach this thread
    JNIEnv* Env = (JNIEnv *)pthread_getspecific(TlsSlot);
    if (Env == nullptr)
    {
        jint GetEnvResult = CurrentJavaVM->GetEnv((void **)&Env, CurrentJavaVersion);
        if (GetEnvResult == JNI_EDETACHED)
        {
            // attach to this thread
            jint AttachResult = CurrentJavaVM->AttachCurrentThread(&Env, NULL);
            if (AttachResult == JNI_ERR)
            {
                ANDROID_CHECKF(false, "*** UNIT TEST -- Failed to attach thread to get the JNI environment!");
                return nullptr;
            }
            pthread_setspecific(TlsSlot, (void*)Env);
        }
        else if (GetEnvResult != JNI_OK)
        {
            ANDROID_CHECKF(false, "*** UNIT TEST -- Failed to get the JNI environment! Result = %d", GetEnvResult);
            return nullptr;
        }
    }
    return Env;
}

jclass AndroidJNI::findJavaClass(const char* name)
{
    JNIEnv* env = AndroidJNI::getJavaEnv();
    if (!env || !name)
    {
        ANDROID_CHECKF(false, "*** Unable to find Java class %s, Env:%p", name, env);
        return nullptr;
    }
    ANDROID_CHECKF(ClassLoader, "*** ClassLoader:%p", ClassLoader);
    ANDROID_CHECKF(FindClassMethod, "*** FindClassMethod:%p", FindClassMethod);
    std::string className(name);
    std::replace(className.begin(), className.end(), '.', '/');
    jclass FoundClass = nullptr;
    if (ClassLoader != nullptr && FindClassMethod != nullptr)
    {
        jstring jclassName = env->NewStringUTF(className.c_str());
        FoundClass = static_cast<jclass>(env->CallObjectMethod(ClassLoader, FindClassMethod, jclassName));
        env->DeleteLocalRef(jclassName);
    }
    else
    {
        FoundClass = env->FindClass(className.c_str());
    }
    ANDROID_CHECKF(FoundClass, "*** Failed to find java class:%s", name);
    AndroidJNI::checkJavaException();
    return FoundClass;
} 

JavaClassMethod AndroidJNI::getClassMethod(jclass clazz, const char* methodName, const char* funcSig, bool isStatic/* = false*/)
{
    JNIEnv*	env = AndroidJNI::getJavaEnv();
    JavaClassMethod javaMethod;
    if (isStatic)
        javaMethod.method = env->GetStaticMethodID(clazz, methodName, funcSig);
    else
        javaMethod.method = env->GetMethodID(clazz, methodName, funcSig);
    javaMethod.clazz = clazz;
    javaMethod.env = env;
    // Is method valid?
    ANDROID_CHECKF(javaMethod.method, "*** Unable to find Java Method %s with Signature %s", methodName, funcSig);
    return javaMethod;
}

JavaClassMethod AndroidJNI::getClassMethod(const char* className, const char* methodName, const char* funcSig, bool isStatic/* = false*/)
{
    jclass FoundClass = AndroidJNI::findJavaClass(className);
    return AndroidJNI::getClassMethod(FoundClass, methodName, funcSig, isStatic);
}

bool AndroidJNI::checkJavaException()
{
    JNIEnv* env = AndroidJNI::getJavaEnv();
    if (!env)
    {
        return true;
    }
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        ANDROID_CHECKF(false,"*** Java JNI call failed with an exception.");
        //jthrowable jthrowable = env->ExceptionOccurred();
        //JavaClassMethod method = AndroidJNI::getClassMethod("java/lang/Throwable", "getMessage", "()Ljava/lang/String;");
        //jstring exceptionMessage = static_cast<jstring>(env->CallObjectMethod(jthrowable, method.method) );
        //const char * UTFString = env->GetStringUTFChars(exceptionMessage, nullptr);
        //ANDROID_LOGE("*** Exception:%s",UTFString);
        //env->ReleaseStringUTFChars(exceptionMessage, UTFString);
        return true;
    }
    return false;
}

std::string AndroidJNI::jstring2string(jstring jstr)
{
    JNIEnv *env = AndroidJNI::getJavaEnv();
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    std::string ret(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return ret;
}


jstring AndroidJNI::string2jstring(const std::string& str)
{
    JNIEnv*	env = AndroidJNI::getJavaEnv();
    jstring local = env->NewStringUTF(str.c_str());
    jstring result = (jstring)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);
    return result;
}

AAssetManager* AndroidJNI::getAAssetManager()
{
    JNIEnv *env = AndroidJNI::getJavaEnv();
    if (!env || !MainActivityRef)
    {
        return nullptr;
    }
    jclass activity_class = env->GetObjectClass(MainActivityRef);
    jmethodID getAssets = env->GetMethodID(activity_class, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assetManager = env->CallObjectMethod(MainActivityRef, getAssets);
    return AAssetManager_fromJava(env, assetManager);
}

void AndroidJNI::detachJavaEnv()
{
    CurrentJavaVM->DetachCurrentThread();
}


} // namespace AndroidNode











