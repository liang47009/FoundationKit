/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVACLASS_HPP
#define FOUNDATIONKIT_ANDROIDJAVACLASS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <memory>
#include "AndroidJNI.hpp"
#include "AndroidJavaCore.hpp"

NS_FK_BEGIN

namespace android
{
    class AndroidJavaClass
    {
    public:
        AndroidJavaClass()
            :ClassHolder(nullptr)
        {

        }

        AndroidJavaClass(const AndroidJavaClass& InOther)
            :ClassHolder(InOther.ClassHolder)
        {

        }

        AndroidJavaClass(AndroidJavaClass&& InOther)
        {
            ClassHolder = std::move(InOther.ClassHolder);
        }

        AndroidJavaClass(jclass InClass)
        {
            ClassHolder = NewGlobalReference(InClass);
        }

        AndroidJavaClass(const std::string& InClassName)
        {
            jclass ObjectClass = AndroidJNI::FindJavaClass(InClassName.c_str());
            ClassHolder = NewGlobalReference(ObjectClass);
            JNIEnv* Env = AndroidJNI::GetJNIEnv();
            Env->DeleteLocalRef(ObjectClass);
        }

        AndroidJavaClass& operator= (const AndroidJavaClass& InOther)
        {
            ClassHolder = InOther.ClassHolder;
            return *(this);
        }

        AndroidJavaClass& operator= (AndroidJavaClass&& InOther)
        {
            ClassHolder = std::move(InOther.ClassHolder);
            return *(this);
        }

        AndroidJavaClass& operator= (jclass InClass)
        {
            ClassHolder = NewGlobalReference(InClass);
            return *(this);
        }

        bool operator== (const AndroidJavaClass& InOther)
        {
            // CALL JNI IS SAME OBJECT
            return ClassHolder.get() == InOther.ClassHolder.get();
        }

        bool operator== (const AndroidJavaClass& InOther) const
        {
            // CALL JNI IS SAME OBJECT
            return ClassHolder.get() == InOther.ClassHolder.get();
        }

        bool operator!= (const AndroidJavaClass& InOther)
        {
            return !(*this == InOther);
        }

        bool operator!= (const AndroidJavaClass& InOther) const
        {
            return !(*this == InOther);
        }

        inline operator jclass()
        {
            return ClassHolder.get();
        }

        explicit operator bool() const// _NOEXCEPT
        {
            return (ClassHolder != nullptr);
        }

        jclass Get()
        {
            return ClassHolder.get();
        }

        template<typename T = void, typename... Args>
        T CallStaticWithSig(const std::string& methodName, const std::string& sig, Args&&... args)
        {
            return android::CallStaticWithSig<T>(*this, methodName, sig, std::forward<Args>(args)...);
        }

        template<typename T = void, typename... Args>
        T CallStatic(std::string methodName, Args&&... args)
        {
            return android::CallStatic<T>(*this, methodName, std::forward<Args>(args)...);
        }

        template<typename T>
        void SetStatic(std::string fieldName, T fieldValue, std::string sig = "")
        {
            android::SetStaticField<T>(*this, fieldName, fieldValue, sig);
        }

        template<typename T>
        T GetStatic(std::string fieldName, std::string sig = "")
        {
            return android::GetStaticField<T>(*this, fieldName, sig);
        }
    private:
        static std::shared_ptr<_jclass> NewGlobalReference(jclass InClass)
        {
            if (InClass)
            {
                JNIEnv* Env = AndroidJNI::GetJNIEnv();
                return std::shared_ptr<_jclass>((_jclass*)Env->NewGlobalRef(InClass), DeleteGlobalReference);
            }
            return std::shared_ptr<_jclass>();
        }

        static void DeleteGlobalReference(jclass InClass)
        {
            if (InClass)
            {
                JNIEnv* Env = AndroidJNI::GetJNIEnv();
                Env->DeleteGlobalRef(InClass);
            }
        }
    private:
        std::shared_ptr<_jclass> ClassHolder;
    };
} // namespace android
NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJAVACLASS_HPP
