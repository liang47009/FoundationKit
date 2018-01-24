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

NS_FK_BEGIN


//java class:android/net/NetworkInfo
//java enum:android.net.NetworkInfo$State
class AndroidJavaObject
{
public:

    AndroidJavaObject()
    : _object(nullptr)
    {

    }
    AndroidJavaObject(AndroidJavaObject&& right)
    {
        _Assign_rv(std::forward<AndroidJavaObject>(right));
    }

    AndroidJavaObject(const AndroidJavaObject& other)
    : _object(other._object)
    {

    }

    explicit AndroidJavaObject(jobject jobj)
    {
        JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
        _object = std::shared_ptr<_jobject>(jniEnv->NewGlobalRef(jobj), jobjectDeleter);
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
        // Promote local references to global
        _object = std::shared_ptr<_jobject>(jniEnv->NewGlobalRef(object), jobjectDeleter);
        jniEnv->DeleteLocalRef(object);
    }

    AndroidJavaObject& operator=(AndroidJavaObject&& right)
    {
        AndroidJavaObject(std::move(right)).Swap(*this);
        return (*this);
    }
    AndroidJavaObject& operator=(const AndroidJavaObject& right)
    {
        AndroidJavaObject(right).Swap(*this);
        return (*this);
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (GetRawObject() != nullptr);
    }

    void Swap(AndroidJavaObject& right)
    {
        _object.swap(right._object);
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

    jclass GetRawClass()const
    {
        JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
        return jniEnv->GetObjectClass(GetRawObject());
    }

    jobject GetRawObject()const
    {
        return _object.get();
    }

    virtual ~AndroidJavaObject()
    {
        ANDROID_LOGD("*** refcount %ld", _object.use_count());
    }

    // don't call this.
    static void jobjectDeleter(jobject jobj)
    {
        ANDROID_LOGD("*** jobjectDeleter:%p", jobj);
        JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
        jniEnv->DeleteGlobalRef(jobj);
    }

private:
    void _Assign_rv(AndroidJavaObject&& right)
    {	// assign by moving _Right
        if (this != &right)
            Swap(right);
    }
protected:
    std::shared_ptr<_jobject> _object;
};


NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDJAVAOBJECT_HPP
