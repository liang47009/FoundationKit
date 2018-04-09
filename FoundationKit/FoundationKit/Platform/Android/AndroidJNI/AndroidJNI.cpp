/****************************************************************************
  Copyright (c) 2016 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <algorithm>
#include <android/asset_manager_jni.h>
#include "AndroidJNI.hpp"

NS_FK_BEGIN

std::string GExternalFilePath;
std::string GFilePathBase;

jint          AndroidJNI::CurrentJavaVersion = JNI_VERSION_1_6;
JavaVM*       AndroidJNI::CurrentJavaVM      = nullptr;
jobject       AndroidJNI::MainActivityRef    = nullptr;
jobject       AndroidJNI::ClassLoader        = nullptr;
jmethodID     AndroidJNI::FindClassMethod    = nullptr;
pthread_key_t AndroidJNI::TlsSlot;

/************************************************************************
Memory Manage:
NewGlobalRef           -> DeleteGlobalRef
FindClass              -> DeleteLocalRef
GetObjectField         -> DeleteLocalRef
GetObjectClass         -> DeleteLocalRef
GetObjectArrayElement  -> DeleteLocalRef
NewString              -> DeleteLocalRef
NewStringUTF           -> DeleteLocalRef
NewObject              -> DeleteLocalRef
NewByteArray           -> DeleteLocalRef
GetStringUTFChars      -> ReleaseStringUTFChars
GetStringChars         -> ReleaseStringChars
GetByteArrayElements   -> ReleaseByteArrayElements
************************************************************************/

AndroidJNI::AndroidJNI()
{

}

AndroidJNI::~AndroidJNI()
{
    JNIEnv* env = AndroidJNI::GetJavaEnv();
    if (MainActivityRef)
    {
        env->DeleteGlobalRef(MainActivityRef);
    }

    if (ClassLoader)
    {
        env->DeleteGlobalRef(ClassLoader);
    }
}

static void JavaEnvDestructor(void*)
{
    ANDROID_LOGI("*** JavaEnvDestructor: %ld", pthread_self());
    AndroidJNI::DetachJavaEnv();
}

void AndroidJNI::InitializeJavaEnv(JavaVM* vm, jint version, jobject activityInstance/* = nullptr*/)
{
    ANDROID_LOGI("InitializeJavaEnv CurrentJavaVM:%p, vm:%p, version:%d, activityInstance:%p", CurrentJavaVM, vm, version, activityInstance);
    if (CurrentJavaVM == nullptr)
    {
        CurrentJavaVM = vm;
        CurrentJavaVersion = version;
        pthread_key_create(&TlsSlot, &JavaEnvDestructor);
        JNIEnv* env = AndroidJNI::GetJavaEnv();
        if (activityInstance)
        {
            MainActivityRef = env->NewGlobalRef(activityInstance);
            jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
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

            // Cache path to external files directory
            jclass contextClass = env->FindClass("android/content/Context");
            jmethodID getExternalFilesDir = env->GetMethodID(contextClass, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
            jobject externalFilesDirPath = env->CallObjectMethod(activityInstance, getExternalFilesDir, nullptr);
            jmethodID getFilePath = env->GetMethodID(env->FindClass("java/io/File"), "getPath", "()Ljava/lang/String;");
            jstring externalFilesPathString = (jstring)env->CallObjectMethod(externalFilesDirPath, getFilePath, nullptr);
            const char *nativeExternalFilesPathString = env->GetStringUTFChars(externalFilesPathString, 0);
            // Copy that somewhere safe 
            GExternalFilePath = nativeExternalFilesPathString;

            // then release...
            env->ReleaseStringUTFChars(externalFilesPathString, nativeExternalFilesPathString);
            env->DeleteLocalRef(externalFilesPathString);
            env->DeleteLocalRef(externalFilesDirPath);
            env->DeleteLocalRef(contextClass);
            env->DeleteLocalRef(MainClass);
            env->DeleteLocalRef(classLoader);
            env->DeleteLocalRef(classClass);
            env->DeleteLocalRef(classLoaderClass);
            ANDROID_LOGI("ExternalFilePath found as '%s'\n", GExternalFilePath.c_str());
        }

        // Cache path to external storage
        jclass envClass = env->FindClass("android/os/Environment");
        jmethodID getExternalStorageDir = env->GetStaticMethodID(envClass, "getExternalStorageDirectory", "()Ljava/io/File;");
        jobject externalStoragePath = env->CallStaticObjectMethod(envClass, getExternalStorageDir, nullptr);
        jmethodID getFilePath = env->GetMethodID(env->FindClass("java/io/File"), "getPath", "()Ljava/lang/String;");
        jstring pathString = (jstring)env->CallObjectMethod(externalStoragePath, getFilePath, nullptr);
        const char *nativePathString = env->GetStringUTFChars(pathString, 0);
        // Copy that somewhere safe 
        GFilePathBase = nativePathString;

        // then release...
        env->ReleaseStringUTFChars(pathString, nativePathString);
        env->DeleteLocalRef(pathString);
        env->DeleteLocalRef(externalStoragePath);
        env->DeleteLocalRef(envClass);
        ANDROID_LOGI("Path found as '%s'\n", GFilePathBase.c_str());
    }
}

jobject AndroidJNI::GetMainActivity()
{
    return MainActivityRef;
}

JNIEnv* AndroidJNI::GetJavaEnv()
{
    // register a destructor to detach this thread
    JNIEnv* Env = (JNIEnv*)pthread_getspecific(TlsSlot);
    if (Env == nullptr)
    {
        jint GetEnvResult = CurrentJavaVM->GetEnv((void**)&Env, CurrentJavaVersion);
        switch (GetEnvResult)
        {
        case JNI_OK:
            pthread_setspecific(TlsSlot, (void*)Env);
            break;
        case JNI_EDETACHED:
            {
                // attach to this thread
                jint AttachResult = CurrentJavaVM->AttachCurrentThread(&Env, NULL);
                if (AttachResult == JNI_ERR)
                {
                    ANDROID_CHECKF(false, "*** Failed to attach thread to get the JNI environment!");
                    return nullptr;
                }
                pthread_setspecific(TlsSlot, (void*)Env);
            }
            break;
        case JNI_EVERSION:
            // Cannot recover from this error
            ANDROID_CHECKF(false, "*** JNI interface version %d not supported", CurrentJavaVersion);
        default:
            ANDROID_CHECKF(false, "*** Failed to get the JNI environment! Result = %d", GetEnvResult);
            return nullptr;
        }
    }
    return Env;
}

jclass AndroidJNI::FindJavaClass(const char* name)
{
    JNIEnv* env = AndroidJNI::GetJavaEnv();
    if (!env || !name)
    {
        ANDROID_CHECKF(false, "*** Unable to find Java class %s, Env:%p", name, env);
        return nullptr;
    }
    ANDROID_CHECKF(ClassLoader, "*** ClassLoader:%p", ClassLoader);
    ANDROID_CHECKF(FindClassMethod, "*** FindClassMethod:%p", FindClassMethod);
    std::string className(name);
    std::replace(className.begin(), className.end(), '.', '/');
    jclass foundClass = nullptr;
    if (ClassLoader != nullptr && FindClassMethod != nullptr)
    {
        jstring jclassName = env->NewStringUTF(className.c_str());
        foundClass = static_cast<jclass>(env->CallObjectMethod(ClassLoader, FindClassMethod, jclassName));
        env->DeleteLocalRef(jclassName);
    }
    else
    {
        foundClass = env->FindClass(className.c_str());
    }
    ANDROID_CHECKF(foundClass, "*** Failed to find java class:%s", name);
    AndroidJNI::CheckJavaException();
    return foundClass;
} 

JavaClassMethod AndroidJNI::GetClassMethod(jclass clazz, const char* methodName, const char* funcSig, bool isStatic/* = false*/)
{
    JNIEnv*	env = AndroidJNI::GetJavaEnv();
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

JavaClassMethod AndroidJNI::GetClassMethod(const char* className, const char* methodName, const char* funcSig, bool isStatic/* = false*/)
{
    jclass foundClass = AndroidJNI::FindJavaClass(className);
    return AndroidJNI::GetClassMethod(foundClass, methodName, funcSig, isStatic);
}

bool AndroidJNI::CheckJavaException()
{
    JNIEnv* env = AndroidJNI::GetJavaEnv();
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
    JNIEnv *env = AndroidJNI::GetJavaEnv();
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    std::string ret(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return ret;
}


jstring AndroidJNI::string2jstring(const std::string& str)
{
    JNIEnv*	env = AndroidJNI::GetJavaEnv();
    jstring local = env->NewStringUTF(str.c_str());
    jstring result = (jstring)env->NewGlobalRef(local);
    env->DeleteLocalRef(local);
    return result;
}

AAssetManager* AndroidJNI::GetAAssetManager()
{
    JNIEnv *env = AndroidJNI::GetJavaEnv();
    if (!env || !MainActivityRef)
    {
        return nullptr;
    }
    jclass activity_class = env->GetObjectClass(MainActivityRef);
    jmethodID getAssets = env->GetMethodID(activity_class, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assetManager = env->CallObjectMethod(MainActivityRef, getAssets);
    env->DeleteLocalRef(activity_class);
    return AAssetManager_fromJava(env, assetManager);
}

void AndroidJNI::DetachJavaEnv()
{
    CurrentJavaVM->DetachCurrentThread();
}

NS_FK_END

#endif //#if PLATFORM_ANDROID











