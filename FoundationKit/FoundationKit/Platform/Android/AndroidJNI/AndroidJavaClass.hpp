/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVACLASS_HPP
#define FOUNDATIONKIT_ANDROIDJAVACLASS_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include "AndroidJNI.hpp"
#include "AndroidFoundation.hpp"

NS_FK_BEGIN

class AndroidJavaClass
{
public:
    AndroidJavaClass() :_class(nullptr)
    {

    }

    AndroidJavaClass(AndroidJavaClass&& right)
    {
        _Assign_rv(std::forward<AndroidJavaClass>(right));
    }

    AndroidJavaClass(const AndroidJavaClass& other)
    {
        _class = other._class;
    }

    AndroidJavaClass(jclass jclazz)
    {
        _class = jclazz;
    }

   /** Construct an AndroidJavaClass based on the name of the java class.
    *  This essentially means locate the class type, allocate an object and run the specified constructor.
    *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
    */
    AndroidJavaClass(const std::string& className)
    {
        _class = AndroidJNI::FindJavaClass(className.c_str());
    }

    AndroidJavaClass& operator=(AndroidJavaClass&& right)
    {
        AndroidJavaClass(std::move(right)).Swap(*this);
        return (*this);
    }

    AndroidJavaClass& operator=(const AndroidJavaClass& right)
    {
        AndroidJavaClass(right).Swap(*this);
        return (*this);
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (GetRawClass() != nullptr);
    }

    void Swap(AndroidJavaClass& right)
    {
        std::swap(_class, right._class);
    }

    template<typename T = void, typename... Args>
    T CallStaticWithSig(const std::string& methodName, const std::string& sig, Args... args)
    {
        return AndroidFoundation::CallStaticWithSig<T>(_class, methodName, sig, std::forward<Args>(args)...);
    }

    template<typename T = void, typename... Args>
    T CallStatic(std::string methodName, Args... args)
    {
        return AndroidFoundation::CallStatic<T>(_class, methodName, std::forward<Args>(args)...);
    }

    template<typename T>
    void SetStatic(std::string fieldName, T fieldValue, std::string sig = "")
    {
        AndroidFoundation::SetStaticField<T>(_class, fieldName, fieldValue, sig);
    }

    template<typename T>
    T GetStatic(std::string fieldName, std::string sig = "")
    {
        return AndroidFoundation::GetStaticField<T>(_class, fieldName, sig);
    }

    jclass GetRawClass()const
    {
        return _class;
    }

private:
    void _Assign_rv(AndroidJavaClass&& right)
    {	// assign by moving _Right
        if (this != &right)
            Swap(right);
    }
protected:
    jclass _class;
};

NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJAVACLASS_HPP
