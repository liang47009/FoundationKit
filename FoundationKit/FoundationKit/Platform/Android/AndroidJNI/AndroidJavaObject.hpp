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
#include "AndroidFoundation.hpp"
#include "AndroidJavaClass.hpp"

NS_FK_BEGIN


//java class:android/net/NetworkInfo
//java enum:android.net.NetworkInfo$State
class AndroidJavaObject
{
public:

    AndroidJavaObject()
    : JavaObject(nullptr)
    , JavaClass(nullptr)
    {

    }
    AndroidJavaObject(AndroidJavaObject&& right)
    {
        JavaObject = std::move(right.JavaObject);
        JavaClass = std::move(right.JavaClass);
    }

    AndroidJavaObject(const AndroidJavaObject& other)
    : JavaObject(other.JavaObject)
    , JavaClass(other.JavaClass)
    {

    }

    explicit AndroidJavaObject(jobject InJavaObject)
    {
        if (InJavaObject)
        {
            JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
            JavaObject = InJavaObject;
            jclass ObjectClass = jniEnv->GetObjectClass(InJavaObject);
            JavaClass = ObjectClass;
            jniEnv->DeleteLocalRef(ObjectClass);
        }
        else
        {
            JavaObject = nullptr;
            JavaClass = nullptr;
        }
    }

    /** Construct an AndroidJavaObject based on the name of the java class.
    *  This essentially means locate the class type, allocate an object and run the specified constructor.
    *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
    *  @param[in] args      An array of parameters passed to the constructor.
    */
    template<typename... Args>
    AndroidJavaObject(const std::string& className, Args... args)
    {
        JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
        std::string methodSignature = AndroidFoundation::GetJNISignature<void, Args...>(args...);
        JavaClassMethod method = AndroidJNI::GetClassMethod(className.c_str(), "<init>", methodSignature.c_str());
        jobject object = jniEnv->NewObject(method.clazz, method.method, AndroidFoundation::CPPToJNI<typename std::decay<Args>::type>::convert(args)...);
        ANDROID_CHECKF(object, "*** Create %s failed.", className.c_str());
        jclass ObjectClass = jniEnv->GetObjectClass(object);
        JavaObject = object;
        JavaClass = ObjectClass;
        jniEnv->DeleteLocalRef(object);
        jniEnv->DeleteLocalRef(ObjectClass);
    }

    AndroidJavaObject& operator=(AndroidJavaObject&& right)
    {
        JavaObject = std::move(right.JavaObject);
        JavaClass = std::move(right.JavaClass);
        return (*this);
    }

    AndroidJavaObject& operator=(const AndroidJavaObject& right)
    {
        JavaObject = right.JavaObject;
        JavaClass = right.JavaClass;
        return (*this);
    }

    AndroidJavaObject& operator= (jobject InJavaObject)
    {
        if (InJavaObject)
        {
            JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
            JavaObject = InJavaObject;
            jclass ObjectClass = jniEnv->GetObjectClass(InJavaObject);
            JavaClass = ObjectClass;
            jniEnv->DeleteLocalRef(ObjectClass);
        }
        else
        {
            JavaObject = nullptr;
            JavaClass = nullptr;
        }
        return *(this);
    }

    inline operator jobject()
    {
        return JavaObject;
    }

    inline operator jclass()
    {
        return (jclass)JavaClass.Get();
    }

    inline operator AndroidJavaClass()
    {
        return AndroidJavaClass(JavaClass);
    }

    inline operator bool() const// _NOEXCEPT
    {
        return (GetRawObject() != nullptr);
    }

    template<typename T = void, typename... Args>
    T CallWithSig(const std::string& methodName, const std::string& sig, Args&&... args)
    {
      std::string rightSig(sig);
      std::replace(rightSig.begin(), rightSig.end(), '.', '/');
      return AndroidFoundation::CallWithSig<T>(GetRawObject(), methodName, rightSig, std::forward<Args>(args)...);
    }

    template<typename T = void, typename... Args>
    T Call(std::string methodName, Args&&... args)
    {
        return AndroidFoundation::Call<T>(GetRawObject(), methodName, std::forward<Args>(args)...);
    }

    template<typename T>
    void Set(std::string fieldName, T fieldValue, std::string sig = "")
    {
        AndroidFoundation::SetField<T>(GetRawObject(), fieldName, fieldValue, sig);
    }

    template<typename T>
    T Get(std::string fieldName, std::string sig = "")
    {
        return AndroidFoundation::GetField<T>(GetRawObject(), fieldName, sig);
    }

    template<typename T = void, typename... Args>
    T CallStaticWithSig(const std::string& methodName, const std::string& sig, Args&&... args)
    {
        std::string rightSig(sig);
        std::replace(rightSig.begin(), rightSig.end(), '.', '/');
        return AndroidFoundation::CallStaticWithSig<T>(GetRawClass(), methodName, rightSig, std::forward<Args>(args)...);
    }

    template<typename T = void, typename... Args>
    T CallStatic(std::string methodName, Args&&... args)
    {
        return AndroidFoundation::CallStatic<T>(GetRawClass(), methodName, std::forward<Args>(args)...);
    }

    template<typename T>
    void SetStatic(std::string fieldName, T fieldValue, std::string sig = "")
    {
        AndroidFoundation::SetStaticField<T>(GetRawClass(), fieldName, fieldValue, sig);
    }

    template<typename T>
    T GetStatic(std::string fieldName, std::string sig = "")
    {
        return AndroidFoundation::GetStaticField<T>(GetRawClass(), fieldName, sig);
    }

    jobject GetRawObject()const
    {
        return JavaObject;
    }

    jclass GetRawClass()const
    {
        return (jclass)JavaClass.Get();
    }

    virtual ~AndroidJavaObject()
    {
        ANDROID_LOGD("*** refcount %ld", _object.use_count());
    }
protected:
    GlobalJavaObjectRef JavaObject;
    GlobalJavaObjectRef JavaClass;
};


NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJAVAOBJECT_HPP
