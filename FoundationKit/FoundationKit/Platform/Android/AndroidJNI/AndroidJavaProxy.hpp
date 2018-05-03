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
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidLexicalCast.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaObject.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaClass.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaProxyHelper.hpp"
NS_FK_BEGIN
namespace android 
{
    //template<typename TX>
    class AndroidJavaProxy
    {
    public:
        AndroidJavaProxy(const std::string& InInterfaceClass)
        {
            std::vector<jobject> Interfaces;
            Interfaces.push_back(AndroidJNI::FindJavaClass(InInterfaceClass.c_str()));
            ProxyObject = NewInstance(this, Interfaces);
            ClassName = InInterfaceClass;
            Clazz = AndroidJNI::FindJavaClass(InInterfaceClass.c_str());
        }

        virtual ~AndroidJavaProxy()
        {
            DisableInstance(GetProxyObject());
        }

        jobject GetProxyObject()
        {
            return ProxyObject.Get();
        }

        inline operator jobject()
        {
            return ProxyObject.Get();
        }

        virtual jobject __OnInvoke(jclass InClass, jmethodID InMethod, jobjectArray InArgs)
        {
            static android::AndroidJavaClass ObjectClass("java/lang/Object");
            JNIEnv* env = AndroidJNI::GetJNIEnv();
            if (!env->IsSameObject(InClass, Clazz.Get()))
                return nullptr;

            jobject result = nullptr;
            for (auto mapIter : MethodsMap)
            {
                if (mapIter.first->GetMethodID((jclass)(Clazz.Get())) == InMethod)
                {
                    result = mapIter.second->Invoke(InArgs);
                    return result;
                }
            }

            static jmethodID methodIDs[] =
            {
                env->GetMethodID(ObjectClass, "hashCode", "()I"),
                env->GetMethodID(ObjectClass, "equals", "(Ljava/lang/Object;)Z"),
                env->GetMethodID(ObjectClass, "toString", "()Ljava/lang/String;")
            };
            if (methodIDs[0] == InMethod) { result = env->NewLocalRef(android::lexical_cast<jobject>(__HashCode())); }
            if (methodIDs[1] == InMethod) { result = env->NewLocalRef(android::lexical_cast<jobject>(__Equals(env->GetObjectArrayElement(InArgs, 0)))); }
            if (methodIDs[2] == InMethod) { result = env->NewLocalRef(__ToString()); }
            return result;
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
protected:
        std::unordered_map<LazyJNIMethodPointer, JNIInvokeHandlerPointer> MethodsMap;
        android::AndroidJavaObject  ProxyObject;
        std::string                 ClassName;
        JNIGlobalReference          Clazz;
    };

#define REG_PROXY_METHOD(CLS, FUN) \
MethodsMap.insert(std::make_pair(android::LazyJNIMethodPointer(new android::LazyJNIMethod<decltype(&CLS::FUN)>(#FUN)), android::BindJNIInvokeHandler(&CLS::FUN, this)))


} // namespace android 

NS_FK_END
#endif //#if PLATFORM_ANDROID
#endif // END OF FOUNDATIONKIT_ANDROIDJAVAPROXY_HPP