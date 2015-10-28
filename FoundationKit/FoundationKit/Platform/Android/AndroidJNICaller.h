#pragma once

#include "AndroidJNIConverter.h"

NS_FK_BEGIN

namespace Android
{

    //default implementation (for jobject types)

    template <typename T, typename... Args>
    struct JNICaller {
        static T call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            auto obj = env->CallObjectMethod(instance, method, v...);
            T result = JNIToCPPConverter<decltype(JobjectToArrayConverter<T>::convert(obj))>::convert(JobjectToArrayConverter<T>::convert(obj));
            if (obj)
                env->DeleteLocalRef(obj);
            return result;
        }

        static T callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            auto obj = env->CallStaticObjectMethod(clazz, method, v...);
            T result = JNIToCPPConverter<decltype(JobjectToArrayConverter<T>::convert(obj))>::convert(JobjectToArrayConverter<T>::convert(obj));
            if (obj)
                env->DeleteLocalRef(obj);
            return result;
        }


        static T getField(JNIEnv * env, jobject instance, jfieldID fid) 
        {
            auto obj = env->GetObjectField(instance, fid);
            T result = JNIToCPPConverter<decltype(JobjectToArrayConverter<T>::convert(obj))>::convert(JobjectToArrayConverter<T>::convert(obj));
            if (obj)
                env->DeleteLocalRef(obj);
            return result;
        }

        static T getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            auto obj = env->GetStaticObjectField(clazz, fid);
            T result = JNIToCPPConverter<decltype(JobjectToArrayConverter<T>::convert(obj))>::convert(JobjectToArrayConverter<T>::convert(obj));
            if (obj)
                env->DeleteLocalRef(obj);
            return result;
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, T fieldValue)
        {
            env->SetObjectField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jobject instance, jfieldID fid, T fieldValue)
        {
            env->SetStaticObjectField(instance, fid, fieldValue);
        }
    };


    //generic pointer implementation (using jlong types)
 
    template <typename T, typename... Args>
    struct JNICaller < T*, Args... >
    {
        static T* call(JNIEnv *env, jobject instance, jmethodID method, Args... v) 
        {
            return (T*)env->CallLongMethod(instance, method, v...);
        }

        static T* callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v) 
        {
            return (T*)env->CallStaticLongMethod(clazz, method, v...);
        }

        static T* getField(JNIEnv * env, jobject instance, jfieldID fid) 
        {
            return (T*)env->GetLongField(instance, fid);
        }

        static T* getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return (T*)env->GetStaticLongField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jlong fieldValue)
        {
            env->SetLongField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jlong fieldValue)
        {
            env->SetStaticLongField(clazz, fid, fieldValue);
        }
    };


    //void implementation
    template <typename... Args>
    struct JNICaller < void, Args... >
    {
        static void call(JNIEnv *env, jobject instance, jmethodID method, Args... v) 
        {
            env->CallVoidMethod(instance, method, v...);
        }
        static void callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            env->CallStaticVoidMethod(clazz, method, v...);
        }
    };

    //primitive types implementations
    template <typename... Args>
    struct JNICaller < bool, Args... > 
    {
        static bool call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallBooleanMethod(instance, method, v...);
        }

        static bool callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticBooleanMethod(clazz, method, v...);
        }

        static bool getField(JNIEnv * env, jobject instance, jfieldID fid) 
        {
            return env->GetBooleanField(instance, fid);
        }

        static bool getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticBooleanField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jboolean fieldValue)
        {
            env->SetBooleanField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jboolean fieldValue)
        {
            env->SetStaticBooleanField(clazz, fid, fieldValue);
        }
    };

    template <typename... Args>
    struct JNICaller < unsigned char, Args... > 
    {

        static unsigned char call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallByteMethod(instance, method, v...);
        }

        static unsigned char callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticByteMethod(clazz, method, v...);
        }

        static unsigned char getField(JNIEnv * env, jobject instance, jfieldID fid)
        {
            return env->GetByteField(instance, fid);
        }

        static unsigned char getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticByteField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jbyte fieldValue)
        {
            env->SetByteField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jbyte fieldValue)
        {
            env->SetStaticByteField(clazz, fid, fieldValue);
        }
    };

    template <typename... Args>
    struct JNICaller < char, Args... > 
    {
        static char call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallCharMethod(instance, method, v...);
        }

        static char callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticCharMethod(clazz, method, v...);
        }

        static char getField(JNIEnv * env, jobject instance, jfieldID fid)
        {
            return env->GetCharField(instance, fid);
        }

        static char getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticCharField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jchar fieldValue)
        {
            env->SetCharField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jchar fieldValue)
        {
            env->SetStaticCharField(clazz, fid, fieldValue);
        }
    };

    template <typename... Args>
    struct JNICaller < short, Args... > 
    {
        static short call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallShortMethod(instance, method, v...);
        }

        static short callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticShortMethod(clazz, method, v...);
        }

        static short getField(JNIEnv * env, jobject instance, jfieldID fid)
        {
            return env->GetShortField(instance, fid);
        }

        static short getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticShortField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jshort fieldValue)
        {
            env->SetShortField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jshort fieldValue)
        {
            env->SetStaticShortField(clazz, fid, fieldValue);
        }
    };

    template <typename... Args>
    struct JNICaller < int, Args... > 
    {

        static int call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallIntMethod(instance, method, v...);
        }

        static int callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticIntMethod(clazz, method, v...);
        }

        static int getField(JNIEnv * env, jobject instance, jfieldID fid)
        {
            return env->GetIntField(instance, fid);
        }

        static int getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticIntField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jint fieldValue)
        {
            env->SetIntField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jint fieldValue)
        {
            env->SetStaticIntField(clazz, fid, fieldValue);
        }
    };

    template <typename... Args>
    struct JNICaller < long, Args... > 
    {
        static long call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallLongMethod(instance, method, v...);
        }

        static long callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticLongMethod(clazz, method, v...);
        }

        static long getField(JNIEnv * env, jobject instance, jfieldID fid)
        {
            return env->GetLongField(instance, fid);
        }

        static long getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticLongField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jlong fieldValue)
        {
            env->SetLongField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jlong fieldValue)
        {
            env->SetStaticLongField(clazz, fid, fieldValue);
        }
    };

    template <typename... Args>
    struct JNICaller < float, Args... > 
    {
        static float call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallFloatMethod(instance, method, v...);
        }

        static float callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticFloatMethod(clazz, method, v...);
        }

        static float getField(JNIEnv * env, jobject instance, jfieldID fid) 
        {
            return env->GetFloatField(instance, fid);
        }

        static float getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticFloatField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jfloat fieldValue)
        {
            env->SetFloatField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jfloat fieldValue)
        {
            env->SetStaticFloatField(clazz, fid, fieldValue);
        }
    };

    template <typename... Args>
    struct JNICaller < double, Args... > 
    {
        static double call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
        {
            return env->CallDoubleMethod(instance, method, v...);
        }

        static double callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
        {
            return env->CallStaticDoubleMethod(clazz, method, v...);
        }

        static double getField(JNIEnv * env, jobject instance, jfieldID fid)
        {
            return env->GetDoubleField(instance, fid);
        }

        static double getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
        {
            return env->GetStaticDoubleField(clazz, fid);
        }

        static void setField(JNIEnv* env, jobject instance, jfieldID fid, jdouble fieldValue)
        {
            env->SetDoubleField(instance, fid, fieldValue);
        }

        static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jdouble fieldValue)
        {
            env->SetStaticDoubleField(clazz, fid, fieldValue);
        }
    };




    //generic call to instance method
    template<typename T = void, typename... Args>
    T call(jobject instance, const std::string & className, const std::string & methodName, Args... v)
    {
        JNIEnv* jniEnv = AndroidJNIHelper::getInstance()->getEnv();
        JniMethodInfo methodInfo;
        AndroidJNIHelper::getInstance()->getMethodInfo(methodInfo, className, methodName, getJNISignature<T, Args...>(v...));
        return JNICaller<T, decltype(CPPToJNIConverter<Args>::convert(v))...>::call(jniEnv, instance, methodInfo.methodID, CPPToJNIConverter<Args>::convert(v)...);
    }

    //generic call to static method
    template<typename T = void, typename... Args> 
    T callStatic(const std::string & className, const std::string & methodName, Args... v)
    {
        JNIEnv* jniEnv = AndroidJNIHelper::getInstance()->getEnv();
        JniMethodInfo methodInfo;
        AndroidJNIHelper::getInstance()->getMethodInfo(methodInfo, className, methodName, getJNISignature<T, Args...>(v...));
        return JNICaller<T, decltype(CPPToJNIConverter<Args>::convert(v))...>::callStatic(jniEnv, methodInfo.classID, methodInfo.methodID, CPPToJNIConverter<Args>::convert(v)...);
    }

    template<typename T> 
    T getField(jobject instance, const std::string & propertyName)
    {
        JNIEnv* jniEnv = AndroidJNIHelper::getInstance()->getEnv();
        jclass clazz = jniEnv->GetObjectClass(instance);
        const char * signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > > ::Result::value();
        jfieldID fid = jniEnv->GetFieldID(clazz, propertyName.c_str(), signature);
        return JNICaller<T>::getField(jniEnv, instance, fid);
    }

    template<typename T> 
    T getFieldStatic(jobject instance, const std::string & propertyName)
    {
        JNIEnv* jniEnv = AndroidJNIHelper::getInstance()->getEnv();
        jclass clazz = jniEnv->GetObjectClass(instance);
        const char * signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > > ::Result::value();
        jfieldID fid = jniEnv->GetStaticFieldID(clazz, propertyName.c_str(), signature);
        return JNICaller<T>::getFieldStatic(jniEnv, clazz, fid);
    }

    template<typename T>
    void setField(jobject instance, const std::string& propertyName, T fieldValue)
    {
        JNIEnv* jniEnv = AndroidJNIHelper::getInstance()->getEnv();
        jclass clazz = jniEnv->GetObjectClass(instance);
        const char * signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > >::Result::value();
        jfieldID fid = jniEnv->GetFieldID(clazz, propertyName.c_str(), signature);
        JNICaller<T>::setField(jniEnv, instance, fid, CPPToJNIConverter<T>::convert(fieldValue));
    }

    template<typename T>
    void setFieldStatic(jobject instance, const std::string& propertyName, T fieldValue)
    {
        JNIEnv* jniEnv = AndroidJNIHelper::getInstance()->getEnv();
        jclass clazz = jniEnv->GetObjectClass(instance);
        const char * signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > >::Result::value();
        jfieldID fid = jniEnv->GetStaticFieldID(clazz, propertyName.c_str(), signature);
        JNICaller<T>::setFieldStatic(jniEnv, instance, fid, CPPToJNIConverter<T>::convert(fieldValue));
    }



}//namespace Andorid

NS_FK_END




