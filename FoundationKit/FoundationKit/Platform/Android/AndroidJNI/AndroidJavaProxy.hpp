/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVAPROXY_HPP
#define FOUNDATIONKIT_ANDROIDJAVAPROXY_HPP
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <unordered_map>
#include <string>
#include <jni.h>
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaClass.hpp"
NS_FK_BEGIN
namespace android 
{
    class AndroidJavaProxy
    {
    public:
        AndroidJavaProxy(const std::string& InInterfaceClass)
        {
            std::vector<jobject> Interfaces;
            Interfaces.push_back(AndroidJNI::FindJavaClass(InInterfaceClass.c_str()));
            ProxyObject = NewInstance(this, Interfaces);
            ClassName = InInterfaceClass;
    }

        virtual ~AndroidJavaProxy()
        {
            DisableInstance(GetProxyObject());
        }

        jobject GetProxyObject()
        {
            return ProxyObject.Get();
        }

        jobject __OnInvoke(jclass InClass, jmethodID InMethod, jobjectArray InArgs)
        {
            jobject result = nullptr;
            bool success = false;
            __OnTryInvoke(InClass, InMethod, InArgs, &success, &result);
            return result;
        }

        virtual bool __OnTryInvoke(jclass clazz, jmethodID methodID, jobjectArray args, bool* success, jobject* result)
        {
            static android::AndroidJavaClass ObjectClass("java/lang/Object");
            JNIEnv* env = AndroidJNI::GetJNIEnv();
            if (!env->IsSameObject(clazz, ObjectClass))
                return false;

            static jmethodID methodIDs[] =
            {
                env->GetMethodID(ObjectClass, "hashCode", "()I"),
                env->GetMethodID(ObjectClass, "equals", "(Ljava/lang/Object;)Z"),
                env->GetMethodID(ObjectClass, "toString", "()Ljava/lang/String;")
            };

            if (methodIDs[0] == methodID) { *result = env->NewLocalRef(android::lexical_cast<jobject>(__HashCode())); *success = true; return true; }
            if (methodIDs[1] == methodID) { *result = env->NewLocalRef(android::lexical_cast<jobject>(__Equals(env->GetObjectArrayElement(args, 0)))); *success = true; return true; }
            if (methodIDs[2] == methodID) { *result = env->NewLocalRef(__ToString()); *success = true;	return true; }

            return false;
        }



    private:
        int __HashCode()
        {
            return android::CallStaticWithSig<int>("java/lang/System", "identityHashCode", "(Ljava/lang/Object;)I", GetProxyObject());
        }

        bool __Equals(jobject InOther)
        {
            JNIEnv* env = AndroidJNI::GetJNIEnv();
            return env->IsSameObject(GetProxyObject(), InOther);
        }

        jstring __ToString()
        {
            return AndroidJNI::string2jstring(ClassName);
        }

    protected:
        static jobject NewInstance(void* NativePtr, const std::vector<jobject>& Interfaces)
        {
            jobjectArray objarray = android::lexical_cast<jobjectArray>(Interfaces);
            android::AndroidJavaObject objectref((jobject)objarray);
            return android::CallStaticWithSig<jobject>("com.losemymind.foundationkit.AndroidJNIBridge", "NewProxyInterface", "(J[Ljava/lang/Class;)Ljava/lang/Object;", (int64)NativePtr, (jobjectArray)objectref.Get());
        }

        static void DisableInstance(jobject InProxy)
        {
            android::CallStaticWithSig<jobject>("com.losemymind.foundationkit.AndroidJNIBridge", "DisableProxyInterface", "(Ljava/lang/Object;)V", InProxy);

        }
    private:
        std::unordered_map<std::string, FunctionHandlerPointer> MethodsMap;
        android::AndroidJavaObject  ProxyObject;
        std::string                 ClassName;
    };

} // namespace android 

NS_FK_END
#endif //#if PLATFORM_ANDROID
#endif // END OF FOUNDATIONKIT_ANDROIDJAVAPROXY_HPP