
#include "AndroidJNIConverter.h"
#include "AndroidJavaObject.h"

NS_FK_BEGIN

namespace Android
{

    AndroidJavaObject::AndroidJavaObject()
        : _classID(nullptr)
        , _object(nullptr)
        , _shared_ObjPtr(nullptr)
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
        : AndroidJavaObject()
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
            jclass classPtr = env->GetObjectClass(jobj);
            _classID = (jclass)env->NewGlobalRef(classPtr);
            env->DeleteLocalRef(classPtr);
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
        FK_SAFE_RELEASE_NULL(_shared_ObjPtr);
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
        FK_SAFE_RELEASE(_shared_ObjPtr);
        _classID        = std::move(right._classID);
        _object         = std::move(right._object);
        _shared_ObjPtr  = std::move(right._shared_ObjPtr);
        right._classID        = nullptr;
        right._object         = nullptr;
        right._shared_ObjPtr  = nullptr;  


    }

    void AndroidJavaObject::swap(AndroidJavaObject& other)
    {
        std::swap(_classID,       other._classID);
        std::swap(_object,        other._object);
        std::swap(_shared_ObjPtr, other._shared_ObjPtr);
    }

    void AndroidJavaObject::copy(AndroidJavaObject& other)
    {
        FK_SAFE_RELEASE(_shared_ObjPtr);
        _classID       = other._classID;
        _object        = other._object;
        _shared_ObjPtr = other._shared_ObjPtr;
        FK_SAFE_RETAIN(_shared_ObjPtr);
    }


    jclass AndroidJavaObject::getRawClass()const
    {
        return _classID;
    }

    jobject AndroidJavaObject::getRawObject()const
    {
        return _object;
    }

    void AndroidJavaObject::createRefCountedBase()
    {
        jobject for_lambda_object = _object;
        jclass  for_lambda_class  = _classID;
        auto delFun = [for_lambda_object, for_lambda_class]()
        {
            if(for_lambda_object)
            {
                JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
                env->DeleteGlobalRef(for_lambda_object);
            }

            if(for_lambda_class)
            {
                JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
                env->DeleteGlobalRef(for_lambda_class);
            }


        };
        _shared_ObjPtr = new RefCounted<decltype(delFun)>(delFun);
    }


} //namespace Andorid


NS_FK_END