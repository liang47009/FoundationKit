
#include "AndroidJNIConverter.h"
#include "AndroidJavaObject.h"

NS_FK_BEGIN

namespace Android
{

    AndroidJavaObject::AndroidJavaObject()
        : _classID(nullptr)
        , _object(nullptr)
        , _rep(nullptr)
    {

    }

    AndroidJavaObject::AndroidJavaObject(AndroidJavaObject&& right)
    :AndroidJavaObject()
    {
        move(std::forward<AndroidJavaObject>(right));
    }

    AndroidJavaObject::AndroidJavaObject(const AndroidJavaObject& other)
    :AndroidJavaObject()
    {
        copy(const_cast<AndroidJavaObject&>(other));
    }


    AndroidJavaObject::AndroidJavaObject(jobject jobj)
    :AndroidJavaObject()
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        if (!env) 
        {
            LOGE("Failed to get evn");
            return;
        }

        if (jobj)
        {
            _object = env->NewGlobalRef(jobj);
            createRefCountedBase();
        }
        else
        {
            LOGE("Failed to create AndroidJavaObject");
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }


    AndroidJavaObject::~AndroidJavaObject()
    {
        FK_SAFE_RELEASE_NULL(_rep);
    }

    AndroidJavaObject& AndroidJavaObject::operator=(AndroidJavaObject&& right)
    {
        move(std::forward<AndroidJavaObject>(right));
        return *this;
    }
    AndroidJavaObject& AndroidJavaObject::operator=(const AndroidJavaObject& other) 
    {
        copy(const_cast<AndroidJavaObject&>(other));
        return *this;
    }

    void AndroidJavaObject::move(AndroidJavaObject&& right)
    {
        FK_SAFE_RELEASE(_rep);
        _classID  = std::move(right._classID);
        _object   = std::move(right._object);
        _rep      = std::move(right._rep);
        right._classID = nullptr;
        right._object  = nullptr;
        right._rep     = nullptr;  


    }

    void AndroidJavaObject::swap(AndroidJavaObject& other)
    {
        std::swap(_classID, other._classID);
        std::swap(_object,  other._object);
        std::swap(_rep,     other._rep);
    }

    void AndroidJavaObject::copy(AndroidJavaObject& other)
    {
        FK_SAFE_RELEASE(_rep);
        _classID  = other._classID;
        _object   = other._object;
        _rep      = other._rep;
        FK_SAFE_RETAIN(_rep);
    }


    jclass AndroidJavaObject::getRawClass()
    {
        return _classID;
    }

    jobject AndroidJavaObject::getRawObject()
    {
        return _object;
    }

    void AndroidJavaObject::createRefCountedBase()
    {
        jobject for_lambda = _object;
        auto delFun = [for_lambda]()
        {
            if(for_lambda)
            {
                JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
                env->DeleteGlobalRef(for_lambda);
            }

        };
        _rep = new RefCounted<decltype(delFun)>(delFun);
    }


} //namespace Andorid


NS_FK_END