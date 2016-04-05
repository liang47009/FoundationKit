/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_ANDROIDJAVACLASS_H
#define LOSEMYMIND_ANDROIDJAVACLASS_H

#include "AndroidJNI.h"
#include "AndroidFoundation.h"

namespace AndroidNode{

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
        _class = AndroidJNI::findJavaClass(className.c_str());
    }

    AndroidJavaClass& operator=(AndroidJavaClass&& right)
    {
        AndroidJavaClass(std::move(right)).swap(*this);
        return (*this);
    }

    AndroidJavaClass& operator=(const AndroidJavaClass& right)
    {
        AndroidJavaClass(right).swap(*this);
        return (*this);
    }

    explicit operator bool() const// _NOEXCEPT
    {
        return (getRawClass() != nullptr);
    }

    void swap(AndroidJavaClass& right)
    {
        std::swap(_class, right._class);
    }

    template<typename T = void, typename... Args>
    T callStatic(std::string methodName, Args... args)
    {
        return AndroidNode::callStatic<T>(_class, methodName, std::forward<Args>(args)...);
    }

    template<typename T>
    void setStatic(std::string fieldName, T fieldValue, std::string sig = "")
    {
        AndroidNode::setFieldStatic<T>(_class, fieldName, fieldValue, sig);
    }

    template<typename T>
    T getStatic(std::string fieldName, std::string sig = "")
    {
        return AndroidNode::getFieldStatic<T>(_class, fieldName, sig);
    }

    jclass getRawClass()const
    {
        return _class;
    }

private:
    void _Assign_rv(AndroidJavaClass&& right)
    {	// assign by moving _Right
        if (this != &right)
            swap(right);
    }
protected:
    jclass _class;
};

} // namespace AndroidNode

#endif // LOSEMYMIND_ANDROIDJAVACLASS_H



