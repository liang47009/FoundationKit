/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVAOBJECT_HPP
#define FOUNDATIONKIT_ANDROIDJAVAOBJECT_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <memory>
#include "AndroidJNI.hpp"
#include "AndroidJavaCore.hpp"

NS_FK_BEGIN

//java class:android/net/NetworkInfo
//java enum:android.net.NetworkInfo$State
namespace android
{
    class AndroidJavaObject
    {
    public:
        typedef std::shared_ptr<_jobject> shared_jobject;
        AndroidJavaObject()
            : ObjectHolder(nullptr)
        {

        }

        AndroidJavaObject(const AndroidJavaObject& InOther)
            : ObjectHolder(InOther.ObjectHolder)
        {

        }

        AndroidJavaObject(AndroidJavaObject&& InOther)
        {
            ObjectHolder = std::move(InOther.ObjectHolder);
        }

        AndroidJavaObject(jobject InObject)
        {
            ObjectHolder = NewGlobalReference(InObject);
        }

       /** Construct an android::AndroidJavaObject based on the name of the java class.
        *  This essentially means locate the class type, allocate an object and run the specified constructor.
        *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
        *  @param[in] args      An array of parameters passed to the constructor.
        */
        template<typename... Args>
        AndroidJavaObject(const std::string& InClassName, Args... InArgs)
        {
            JNIEnv* Env = AndroidJNI::GetJNIEnv();
            std::string methodSignature = GetJNISignature<void, Args...>(InArgs...);
            JavaClassMethod method = AndroidJNI::GetClassMethod(InClassName.c_str(), "<init>", methodSignature.c_str());
            jobject object = Env->NewObject(method.clazz, method.method, CPPTypeCast<typename std::decay<Args>::type>::Converter::convert(InArgs)...);
            ANDROID_CHECKF(object, "*** Create %s failed.", InClassName.c_str());
            ObjectHolder = NewGlobalReference(object);
            Env->DeleteLocalRef(object);
        }

        AndroidJavaObject& operator= (const AndroidJavaObject& InOther)
        {
            ObjectHolder = InOther.ObjectHolder;
            return *(this);
        }

        AndroidJavaObject& operator= (AndroidJavaObject&& InOther)
        {
            ObjectHolder = std::move(InOther.ObjectHolder);
            return *(this);
        }

        AndroidJavaObject& operator= (jobject InObject)
        {
            ObjectHolder = NewGlobalReference(InObject);
            return *(this);
        }

        virtual bool operator== (const AndroidJavaObject& InOther)
        {
            //JNIEnv* Env = AndroidJNI::GetJNIEnv();
            //return Env->IsSameObject(ObjectHolder.get(), InOther.ObjectHolder.get()) == JNI_TRUE;
            return ObjectHolder.get() == InOther.ObjectHolder.get();
        }

        virtual bool operator== (const AndroidJavaObject& InOther) const
        {
            //JNIEnv* Env = AndroidJNI::GetJNIEnv();
            //return Env->IsSameObject(ObjectHolder.get(), InOther.ObjectHolder.get()) == JNI_TRUE;
            return ObjectHolder.get() == InOther.ObjectHolder.get();
        }

        virtual bool operator!= (const AndroidJavaObject& InOther)
        {
            return !(*this == InOther);
        }

        virtual bool operator!= (const AndroidJavaObject& InOther) const
        {
            return !(*this == InOther);
        }

        inline operator jobject()
        {
            return ObjectHolder.get();
        }

        explicit operator bool() const// _NOEXCEPT
        {
            return (ObjectHolder != nullptr);
        }

        jobject Get()
        {
            return ObjectHolder.get();
        }

        jclass Class()
        {
            JNIEnv* jniEnv = AndroidJNI::GetJNIEnv();
            return jniEnv->GetObjectClass(*this);
        }

        template<typename T = void, typename... Args>
        T CallWithSig(const std::string& methodName, const std::string& sig, Args&&... args)
        {
            std::string rightSig(sig);
            std::replace(rightSig.begin(), rightSig.end(), '.', '/');
            return android::CallWithSig<T>(*this, methodName, rightSig, std::forward<Args>(args)...);
        }

        template<typename T = void, typename... Args>
        T Call(std::string methodName, Args&&... args)
        {
            return android::Call<T>(*this, methodName, std::forward<Args>(args)...);
        }

        template<typename T>
        void Set(std::string fieldName, T fieldValue, std::string sig = "")
        {
            android::SetField<T>(*this, fieldName, fieldValue, sig);
        }

        template<typename T>
        T Get(std::string fieldName, std::string sig = "")
        {
            return android::GetField<T>(*this, fieldName, sig);
        }

        template<typename T = void, typename... Args>
        T CallStaticWithSig(const std::string& methodName, const std::string& sig, Args&&... args)
        {
            std::string rightSig(sig);
            std::replace(rightSig.begin(), rightSig.end(), '.', '/');
            return android::CallStaticWithSig<T>(this->Class(), methodName, rightSig, std::forward<Args>(args)...);
        }

        template<typename T = void, typename... Args>
        T CallStatic(std::string methodName, Args&&... args)
        {
            return android::CallStatic<T>(this->Class(), methodName, std::forward<Args>(args)...);
        }

        template<typename T>
        void SetStatic(std::string fieldName, T fieldValue, std::string sig = "")
        {
            android::SetStaticField<T>(this->Class(), fieldName, fieldValue, sig);
        }

        template<typename T>
        T GetStatic(std::string fieldName, std::string sig = "")
        {
            return android::GetStaticField<T>(this->Class(), fieldName, sig);
        }

    private:
        static shared_jobject NewGlobalReference(jobject InObject)
        {
            if (InObject)
            {
                JNIEnv* Env = AndroidJNI::GetJNIEnv();
                return shared_jobject(Env->NewGlobalRef(InObject), DeleteGlobalReference);
            }
            return shared_jobject();
        }

        static void DeleteGlobalReference(jobject InObject)
        {
            if (InObject)
            {
                JNIEnv* Env = AndroidJNI::GetJNIEnv();
                Env->DeleteGlobalRef(InObject);
            }
        }
    protected:
        shared_jobject ObjectHolder;
    };
} // namespace android

NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJAVAOBJECT_HPP
