#include <pthread.h>
#include <android/asset_manager_jni.h>
#include "AndroidJNIHelper.h"

NS_FK_BEGIN

namespace Android
{

    static pthread_key_t SHARED_JNIENV_KEY;

    // For setJavaVM - pthread_key_create
    void _detachCurrentThread(void* a)
    {
        AndroidJNIHelper::getInstance()->getJavaVM()->DetachCurrentThread();
    }

    AndroidJNIHelper::AndroidJNIHelper()
        :_psJavaVM(nullptr)
        , _loadMethod(nullptr)
        , _classloader(nullptr)
        , _activityInstance(nullptr)
    {

    }
    AndroidJNIHelper::~AndroidJNIHelper()
    {
        if (_classloader)
        {
            getEnv()->DeleteGlobalRef(_classloader);
        }
    }



    void AndroidJNIHelper::setJavaVM(JavaVM *javaVM)
    {
        pthread_t thisthread = pthread_self();
        LOGD("JniHelper::setJavaVM(%p), pthread_self() = %ld", javaVM, thisthread);
        _psJavaVM = javaVM;
        pthread_key_create(&SHARED_JNIENV_KEY, _detachCurrentThread);
    }

    bool AndroidJNIHelper::setClassLoaderActivity(jobject activityInstance)
    {
        _activityInstance = activityInstance;
        JniMethodInfo classloaderMethodInfo;
        if (!getDefaultClassLoader(classloaderMethodInfo,
            "android/content/Context",
            "getClassLoader",
            "()Ljava/lang/ClassLoader;"))
        {
            return false;
        }

        jobject obj = getEnv()->CallObjectMethod(activityInstance, classloaderMethodInfo.methodID);

        if (nullptr == obj)
            return false;

        JniMethodInfo loadClassMethodInfo;
        if (!getDefaultClassLoader(loadClassMethodInfo, "java/lang/ClassLoader", "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;"))
        {
            return false;
        }

        _classloader = getEnv()->NewGlobalRef(obj);
        _loadMethod = loadClassMethodInfo.methodID;
        return true;
    }

    JavaVM* AndroidJNIHelper::getJavaVM()
    {
        pthread_t thisthread = pthread_self();
        LOGD("JniHelper::getJavaVM(), pthread_self() = %ld", thisthread);
        return _psJavaVM;
    }

    JNIEnv* AndroidJNIHelper::getEnv()
    {
        JNIEnv* env = (JNIEnv *)pthread_getspecific(SHARED_JNIENV_KEY);
        if (env == nullptr)
            env = cacheJNIEnv(_psJavaVM);
        return env;
    }

    jclass AndroidJNIHelper::getClassID(const char *className)
    {
        if (nullptr == className)
        {
            return nullptr;
        }
        JNIEnv* env = getEnv();
        if (!env)
        {
            LOGE("Failed to get JNIEnv");
            return nullptr;
        }

        jclass clazz = nullptr;

        if (_classloader == nullptr || _loadMethod == nullptr)
        {
            clazz = env->FindClass(className);
            if (!clazz)
            {
                LOGE("Failed to find class %s", className);
                env->ExceptionClear();
            }
        }
        else
        {
            jstring jstrClassName = env->NewStringUTF(className);

            clazz = (jclass)env->CallObjectMethod(_classloader, _loadMethod, jstrClassName);

            if (nullptr == clazz)
            {
                LOGE("Classloader failed to find class of %s", className);
                env->ExceptionClear();
            }
            env->DeleteLocalRef(jstrClassName);
        }
        return clazz;
    }

    JNIEnv* AndroidJNIHelper::cacheJNIEnv(JavaVM* jvm)
    {
        JNIEnv* env = nullptr;
        // get jni environment
        jint ret = jvm->GetEnv((void**)&env, JNI_VERSION_1_4);

        switch (ret) {
        case JNI_OK:
            // Success!
            pthread_setspecific(SHARED_JNIENV_KEY, env);
            break;

        case JNI_EDETACHED:
            // Thread not attached
            if (jvm->AttachCurrentThread(&env, nullptr) < 0)
            {
                LOGE("Failed to get the environment using AttachCurrentThread()");
            }
            else
            {
                // Success : Attached and obtained JNIEnv!
                pthread_setspecific(SHARED_JNIENV_KEY, env);
            }
            break;
        case JNI_EVERSION:
            // Cannot recover from this error
            LOGE("JNI interface version 1.4 not supported");
            break;
        default:
            LOGE("Failed to get the environment using GetEnv()");
            break;
        }
        return env;
    }

    bool AndroidJNIHelper::getDefaultClassLoader(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *signature)
    {
        if ((nullptr == className) ||
            (nullptr == methodName) ||
            (nullptr == signature))
        {
            return false;
        }

        JNIEnv *env = getEnv();
        if (!env)
        {
            return false;
        }

        jclass classID = env->FindClass(className);
        if (!classID)
        {
            LOGE("Failed to find class %s", className);
            env->ExceptionClear();
            return false;
        }

        jmethodID methodID = env->GetMethodID(classID, methodName, signature);
        if (!methodID)
        {
            LOGE("Failed to find method id of %s", methodName);
            env->ExceptionClear();
            return false;
        }

        methodinfo.classID = classID;
        methodinfo.env = env;
        methodinfo.methodID = methodID;
        return true;
    }

    std::string AndroidJNIHelper::jstring2string(jstring jstr)
    {
        if (jstr == nullptr)
            return "";

        JNIEnv *env = getEnv();
        if (!env)
            return nullptr;

        const char* chars = env->GetStringUTFChars(jstr, nullptr);
        std::string ret(chars);
        env->ReleaseStringUTFChars(jstr, chars);
        return ret;
    }

    jstring AndroidJNIHelper::string2jstring(std::string str)
    {
        JNIEnv *env = getEnv();
        if (!env)
        {
            return nullptr;
        }
        return env->NewStringUTF(str.c_str());
    }

    bool AndroidJNIHelper::getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *signature)
    {
        if ((nullptr == className) ||
            (nullptr == methodName) ||
            (nullptr == signature))
        {
            return false;
        }

        JNIEnv *env = getEnv();
        if (!env)
        {
            LOGE("Failed to get JNIEnv");
            return false;
        }

        jclass classID = getClassID(className);
        if (!classID)
        {
            LOGE("Failed to find class %s", className);
            env->ExceptionClear();
            return false;
        }

        return getStaticMethodInfo(methodinfo, classID, methodName, signature);

    }

    bool AndroidJNIHelper::getStaticMethodInfo(JniMethodInfo &methodinfo, jclass classID, const char *methodName, const char *signature)
    {
        JNIEnv *env = getEnv();
        if (!env)
        {
            LOGE("Failed to get JNIEnv");
            return false;
        }

        jmethodID methodID = env->GetStaticMethodID(classID, methodName, signature);
        if (!methodID)
        {
            LOGE("Failed to find static method id of %s", methodName);
            env->ExceptionClear();
            return false;
        }

        methodinfo.classID = classID;
        methodinfo.env = env;
        methodinfo.methodID = methodID;
        return true;
    }

    bool AndroidJNIHelper::getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *signature)
    {
        if ((nullptr == className) ||
            (nullptr == methodName) ||
            (nullptr == signature))
        {
            return false;
        }

        JNIEnv *env = getEnv();
        if (!env)
        {
            return false;
        }

        jclass classID = getClassID(className);
        if (!classID)
        {
            LOGE("Failed to find class %s", className);
            env->ExceptionClear();
            return false;
        }
        return getMethodInfo(methodinfo, classID, methodName, signature);
    }

    bool AndroidJNIHelper::getMethodInfo(JniMethodInfo &methodinfo, jclass classID, const char *methodName, const char *signature)
    {
        JNIEnv *env = getEnv();
        if (!env)
        {
            return false;
        }

        jmethodID methodID = env->GetMethodID(classID, methodName, signature);
        if (!methodID)
        {
            LOGE("Failed to find method id of %s", methodName);
            env->ExceptionClear();
            return false;
        }

        methodinfo.classID = classID;
        methodinfo.env = env;
        methodinfo.methodID = methodID;
        return true;
    }


    void AndroidJNIHelper::checkException()
    {
        JNIEnv *env = getEnv();
        if (!env)
        {
            return;
        }
        if (env->ExceptionCheck())
        {
            jthrowable jthrowable = env->ExceptionOccurred();
            env->ExceptionDescribe();
            env->ExceptionClear();
            JniMethodInfo methodinfo;
            getMethodInfo(methodinfo, "java/lang/Throwable", "getMessage", "()Ljava/lang/String;");
            std::string exceptionMessage = jstring2string(reinterpret_cast<jstring>(env->CallObjectMethod(jthrowable, methodinfo.methodID)));
            throw std::runtime_error(exceptionMessage.c_str());
        }
    }

    AAssetManager* AndroidJNIHelper::getAAssetManager()
    {
        JNIEnv *env = getEnv();
        if (!env || !_activityInstance)
        {
            return nullptr;
        }
        jclass activity_class = env->GetObjectClass(_activityInstance);
        jmethodID getAssets = env->GetMethodID(activity_class, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject assetManager = env->CallObjectMethod(_activityInstance, getAssets);
        return AAssetManager_fromJava(env, assetManager);
    }

    jobject AndroidJNIHelper::getActivityInstance()
    {
        return _activityInstance;
    }

} // namespace Android

NS_FK_END


