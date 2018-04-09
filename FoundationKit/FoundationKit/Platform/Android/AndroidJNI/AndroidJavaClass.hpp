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
#include "AndroidFoundation.hpp"

NS_FK_BEGIN

class AndroidJavaClass
{
public:
    AndroidJavaClass() :JavaClass(nullptr)
    {

    }

    AndroidJavaClass(AndroidJavaClass&& InOther)
    {
        JavaClass = std::move(InOther.JavaClass);
    }

    AndroidJavaClass(const AndroidJavaClass& InOther)
    {
        JavaClass = InOther.JavaClass;
    }

    AndroidJavaClass(jclass InJavaClass)
    {
        JavaClass = InJavaClass;
    }

    AndroidJavaClass(GlobalJavaObjectRef InJavaClassRef)
    {
        JavaClass = InJavaClassRef;
    }

   /** Construct an AndroidJavaClass based on the name of the java class.
    *  This essentially means locate the class type, allocate an object and run the specified constructor.
    *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
    */
    AndroidJavaClass(const std::string& className)
    {
        jclass ObjectClass = AndroidJNI::FindJavaClass(className.c_str());
        JavaClass = ObjectClass;
        JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
        jniEnv->DeleteLocalRef(ObjectClass);
    }

    AndroidJavaClass& operator=(AndroidJavaClass&& InOther)
    {
        JavaClass = std::move(InOther.JavaClass);
        return (*this);
    }

    AndroidJavaClass& operator=(const AndroidJavaClass& InOther)
    {
        JavaClass = InOther.JavaClass;
        return (*this);
    }

    AndroidJavaClass& operator=(const GlobalJavaObjectRef& InJavaClassRef)
    {
        JavaClass = InJavaClassRef;
        return (*this);
    }

    

    AndroidJavaClass& operator= (jclass InClazz)
    {
        JavaClass = InClazz;
        return *(this);
    }

    inline operator jclass()
    {
        return (jclass)JavaClass.Get();
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (GetRawClass() != nullptr);
    }

    template<typename T = void, typename... Args>
    T CallStaticWithSig(const std::string& methodName, const std::string& sig, Args&&... args)
    {
        return AndroidFoundation::CallStaticWithSig<T>(GetRawClass(), methodName, sig, std::forward<Args>(args)...);
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

    inline jclass GetRawClass()const
    {
        return (jclass)JavaClass.Get();
    }
protected:
    GlobalJavaObjectRef JavaClass;
};

NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJAVACLASS_HPP
