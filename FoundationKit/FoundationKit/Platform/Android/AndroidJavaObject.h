#pragma once

/** 
   调用call 和callStatic函数的返回类型支持列表：
    jobject 
    std::vector<jobject> 
    std::vector<bool>
    std::vector<unsigned char>
    std::vector<char>
    std::vector<short> 
    std::vector<int>
    std::vector<long>
    std::vector<float> 
    std::vector<double>
    std::string
    void
    bool
    unsigned char
    char
    short
    int
    long
    float
    double

    参数支持列表：
    指针
    jobject
    unsigned char
    unsigned short
    unsigned int
    unsigned long
    unsigned long long
    bool
    char
    short
    int
    long
    long long
    float
    double
    const char* 
    std::string
    std::vector<std::string>
    std::vector<jobject>
    std::vector<bool>
    std::vector<unsigned char>
    std::vector<char>
    std::vector<short>
    std::vector<int> 
    std::vector<long>
    std::vector<float> 
    std::vector<double>

    其中std::vector 代表了数组。
 */

#include <jni.h>
#include <string>
#include <algorithm>
#include <cassert>
#include "AndroidJNIHelper.h"
#include "AndroidJNICaller.h"
#include "AndroidFunctional.h"

NS_FK_BEGIN

/**
* AndroidJavaObject is application representation of a generic instance of java.lang.Object.
* @note It can be used as type-less interface to an instance of any Java class.
* Example usage:
* @code
*    auto jo = new AndroidJavaObject("java.lang.String", "some string");
*    auto jo1 = new  AndroidJavaObject("java.lang.String");
* @endcode
*/

namespace Android
{

class AndroidJavaObject
{
public:

    AndroidJavaObject();
    AndroidJavaObject(AndroidJavaObject&& right);
    AndroidJavaObject(const AndroidJavaObject& other);

    AndroidJavaObject(jobject jobj);

    /** Construct an AndroidJavaObject based on the name of the java class.
     *  This essentially means locate the class type, allocate an object and run the specified constructor.
     *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
     *  @param[in] args      An array of parameters passed to the constructor.
     */
    template<typename... Args>
    AndroidJavaObject(std::string className, Args... args)
        : AndroidJavaObject()
    {
        std::replace(className.begin(), className.end(), '.', '/');
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        if (!env) 
        {
            LOGE("Failed to get evn");
            return;
        }

        jclass classPtr = AndroidJNIHelper::getInstance()->getClassID(className.c_str());
        if (classPtr)
        {
            std::string methodSignature = getJNISignature<void, Args...>(args...);
            //LOGD("==== AndroidJavaObject::method signature:%s", methodSignature.c_str());
            jmethodID methodID = env->GetMethodID(classPtr, "<init>", methodSignature.c_str());
            if (!methodID) 
            {
                LOGE("Failed to find method id of %s", "<init>");
                env->ExceptionDescribe();
                env->ExceptionClear();
                return;
            }
            jobject objPtr = env->NewObject(classPtr, methodID, CPPToJNIConverter<Args>::convert(args)...);
            _object = env->NewGlobalRef(objPtr);
            _classID = (jclass)env->NewGlobalRef(classPtr);
            env->DeleteLocalRef(objPtr);
            env->DeleteLocalRef(classPtr);
            createRefCountedBase();
        }
        else
        {
            LOGE("Failed to find class %s", className.c_str());
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        
    }

    virtual ~AndroidJavaObject();

    AndroidJavaObject& operator=(AndroidJavaObject&& right);
    AndroidJavaObject& operator=(const AndroidJavaObject& other);

    void move(AndroidJavaObject&& right);
    void swap(AndroidJavaObject& right);
    void copy(AndroidJavaObject& other);


    /** Calls a Java method on an object (non-static).
     *  To call a method with return type 'void', use the regular version.
     *  @param[in]  methodName Specifies which method to call.
     *  @param[in]  args       An array of parameters passed to the method.
     */
    template<typename T = void, typename... Args>
    T call(std::string methodName, Args... args)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::string methodSignature = getJNISignature<T, Args...>(args...);
        //LOGD("==== AndroidJavaObject::method signature:%s", methodSignature.c_str());
        jmethodID methodID = env->GetMethodID(_classID, methodName.c_str(), getJNISignature<T, Args...>(args...));
        if (!methodID) {
            LOGE("Failed to find method id of %s", methodName.c_str());
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        return JNICaller<T, decltype(CPPToJNIConverter<Args>::convert(args))...>::call(env, getRawObject(), methodID, CPPToJNIConverter<Args>::convert(args)...);
    }

    /** Get the value of a field in an object (non-static).
     *  The generic parameter determines the field type.
     *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
     *  @param[in] sig The field signature,for example: "com/example/foundationkitunittest/MainActivity" 
     *                  or "com.example.foundationkitunittest.MainActivity".
     *                  if return type is not jobject, needn't set sig value, default is "".
     *  @return   T object.
     */
    template<typename T>
    T get(std::string fieldName, std::string sig = "")
    {
        if (std::is_same<T, jobject>::value && sig.empty())
        {
            LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
            assert(false);
        }
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::string signature = sig;
        if (signature.empty())
        {
            signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > > ::Result::value();
        }
        jfieldID fid = env->GetFieldID(_classID, fieldName.c_str(), signature.c_str());
        return JNICaller<T>::getField(env, getRawObject(), fid);
    }

    /** Set the value of a field in an object (non-static).
     *  The generic parameter determines the field type.
     *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
     *  @param[in] fieldValueThe value to assign to the field. It has to match the field type.
     *  @param[in] sig The field signature,for example: "com/example/foundationkitunittest/MainActivity" 
     *                  or "com.example.foundationkitunittest.MainActivity".
     *                  if return type is not jobject, needn't set sig value, default is "".
     */
    template<typename T>
    void set(std::string fieldName, T fieldValue, std::string sig = "")
    {
        if (std::is_same<T, jobject>::value && sig.empty())
        {
            LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
            assert(false);
        }
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::string signature = sig;
        if (signature.empty())
        {
            signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > > ::Result::value();
        }
        jfieldID fid = env->GetFieldID(_classID, fieldName.c_str(), signature.c_str());
        JNICaller<T>::setField(env, getRawObject(), fid, CPPToJNIConverter<T>::convert(fieldValue));
    }


    /** Call a static Java method on a class.
     *  To call a static method with return type 'void', use the regular version.
     *  @param[in]  methodName Specifies which method to call.
     *  @param[in]  args       An array of parameters passed to the method.
     *  @return     _RT object.
     */
    template<typename T = void, typename... Args>
    T callStatic(std::string methodName, Args... args)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jmethodID methodID = env->GetStaticMethodID(_classID, methodName.c_str(), getJNISignature<T, Args...>(args...));
        if (!methodID) {
            LOGE("Failed to find method id of %s", methodName.c_str());
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        return JNICaller<T, decltype(CPPToJNIConverter<Args>::convert(args))...>::call(env, _classID, methodID, CPPToJNIConverter<Args>::convert(args)...);
    }

    /** Get the value of a static field in an object type.
     *  The generic parameter determines the field type.
     *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
     *  @param[in] sig The field signature,for example: "com/example/foundationkitunittest/MainActivity" 
     *                  or "com.example.foundationkitunittest.MainActivity".
     *                  if return type is not jobject, needn't set sig value, default is "".
     *  @return   _RT object.
     */
    template<typename T>
    T getStatic(std::string fieldName, std::string sig = "")
    {
        if (std::is_same<T, jobject>::value && sig.empty())
        {
            LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
            assert(false);
        }
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::string signature = sig;
        if (signature.empty())
        {
            signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > > ::Result::value();
        }
        jfieldID fid = env->GetStaticFieldID(_classID, fieldName.c_str(), signature.c_str());
        return JNICaller<T>::getFieldStatic(env, getRawClass(), fid);
    }

    /** Set the value of a static field in an object type.
     *  The generic parameter determines the field type.
     *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
     *  @param[in] sig The field signature,for example: "com/example/foundationkitunittest/MainActivity" 
     *                  or "com.example.foundationkitunittest.MainActivity".
     *                  if return type is not jobject, needn't set sig value, default is "".
     *  @param[in] fieldValueThe value to assign to the field. It has to match the field type.
     */
    template<typename T>
    void setStatic(std::string fieldName, T fieldValue, std::string sig = "")
    {
        if (std::is_same<T, jobject>::value && sig.empty())
        {
            LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
            assert(false);
        }
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::string signature = sig;
        if (signature.empty())
        {
            signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > > ::Result::value();
        }
        jfieldID fid = env->GetStaticFieldID(_classID, fieldName.c_str(), signature.c_str());
        JNICaller<T>::setFieldStatic(env, getRawClass(), fid, CPPToJNIConverter<T>::convert(fieldValue));
    }

    jclass getRawClass()const;
    jobject getRawObject()const;

protected:
    void createRefCountedBase();
    jclass                 _classID;
    jobject                _object;
public : 
    RefCountedBase*        _shared_ObjPtr;
};

} //namespace Andorid

NS_FK_END