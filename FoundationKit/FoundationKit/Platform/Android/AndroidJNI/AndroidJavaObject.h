/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_ANDROIDJAVAOBJECT_H
#define LOSEMYMIND_ANDROIDJAVAOBJECT_H

#include <memory>
#include "AndroidJNI.h"
#include "AndroidFoundation.h"

namespace AndroidNode{

class AndroidJavaObject
{
public:

    AndroidJavaObject()
    {

    }
    AndroidJavaObject(AndroidJavaObject&& right)
    {
        _Assign_rv(std::forward<AndroidJavaObject>(right));
    }

    AndroidJavaObject(const AndroidJavaObject& other)
    {
        _object = other._object;
    }

    AndroidJavaObject(jobject jobj)
    {
        JNIEnv* jniEnv = AndroidJNI::getJavaEnv();
        _object = std::shared_ptr<_jobject>(jniEnv->NewGlobalRef(jobj), jobjectDeleter);
    }

    /** Construct an AndroidJavaObject based on the name of the java class.
    *  This essentially means locate the class type, allocate an object and run the specified constructor.
    *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
    *  @param[in] args      An array of parameters passed to the constructor.
    */
    template<typename... Args>
    AndroidJavaObject(std::string className, Args... args)
    {
        JNIEnv* jniEnv = AndroidJNI::getJavaEnv();
        std::string methodSignature = getJNISignature<void, Args...>(args...);
        JavaClassMethod method = AndroidJNI::getClassMethod(className.c_str(), "<init>", methodSignature.c_str());
        jobject object = jniEnv->NewObject(method.clazz, method.method, CPPToJNI<Args>::convert(args)...);
        ANDROID_CHECKF(object, "*** Create %s failed.", className.c_str());
        // Promote local references to global
        _object = std::shared_ptr<_jobject>(jniEnv->NewGlobalRef(object), jobjectDeleter);
        jniEnv->DeleteLocalRef(object);
    }

    AndroidJavaObject& operator=(AndroidJavaObject&& right)
    {
        AndroidJavaObject(std::move(right)).swap(*this);
        return (*this);
    }
    AndroidJavaObject& operator=(const AndroidJavaObject& right)
    {
        AndroidJavaObject(right).swap(*this);
        return (*this);
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (getRawObject() != nullptr);
    }

    void swap(AndroidJavaObject& right)
    {
        _object.swap(right._object);
    }

    template<typename T = void, typename... Args>
    T call(std::string methodName, Args... args)
    {
        return AndroidNode::call<T>(getRawObject(), methodName, std::forward<Args>(args)...);
    }

    template<typename T>
    void set(std::string fieldName, T fieldValue, std::string sig = "")
    {
        AndroidNode::setField<T>(getRawObject(), fieldName, fieldValue, sig);
    }

    template<typename T>
    T get(std::string fieldName, std::string sig = "")
    {
        return AndroidNode::getField<T>(getRawObject(), fieldName, sig);
    }

    template<typename T = void, typename... Args>
    T callStatic(std::string methodName, Args... args)
    {
        return AndroidNode::callStatic<T>(getRawClass(), methodName, std::forward<Args>(args)...);
    }

    template<typename T>
    void setStatic(std::string fieldName, T fieldValue, std::string sig = "")
    {
        AndroidNode::setFieldStatic<T>(getRawClass(), fieldName, fieldValue, sig);
    }

    template<typename T>
    T getStatic(std::string fieldName, std::string sig = "")
    {
        return AndroidNode::getFieldStatic<T>(getRawClass(), fieldName, sig);
    }

    jclass getRawClass()const
    {
        JNIEnv* jniEnv = AndroidJNI::getJavaEnv();
        return jniEnv->GetObjectClass(getRawObject());
    }

    jobject getRawObject()const
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
        JNIEnv* jniEnv = AndroidJNI::getJavaEnv();
        jniEnv->DeleteGlobalRef(jobj);
    }

private:
    void _Assign_rv(AndroidJavaObject&& right)
    {	// assign by moving _Right
        if (this != &right)
            swap(right);
    }
protected:
    std::shared_ptr<_jobject> _object;
};


} // namespace AndroidNode
#endif // LOSEMYMIND_ANDROIDJAVAOBJECT_H












