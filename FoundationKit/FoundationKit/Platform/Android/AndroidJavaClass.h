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

#include "AndroidJavaObject.h"

NS_FK_BEGIN

/**
* AndroidJavaClass is application representation of a generic instance of java.lang.Class.
* @note It can be used as type-less interface to an instance of any Java class.
* Example usage:
* @code
*    auto jc = new  AndroidJavaClass("java.lang.String");
* @endcode
*/

namespace Android
{

class AndroidJavaClass : public AndroidJavaObject
{
public:

    AndroidJavaClass():AndroidJavaObject(){};
    AndroidJavaClass(AndroidJavaClass&& right) : AndroidJavaClass()
    {
        _classID  = std::move(right._classID);
    }
    AndroidJavaClass(const AndroidJavaClass& other): AndroidJavaClass()
    {
        _classID = other.getRawClass();
    }

    /** Construct an AndroidJavaClass based on the name of the java class.
     *  This essentially means locate the class type, allocate an object and run the specified constructor.
     *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
     */

    AndroidJavaClass(std::string className)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        if (!env) 
        {
            LOGE("Failed to get evn");
            return;
        }
        _classID = AndroidJNIHelper::getInstance()->getClassID(className.c_str());
        if (!_classID)
        {
            LOGE("Failed to find class %s", className.c_str());
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }

    virtual ~AndroidJavaClass(){ _classID = nullptr;};

    AndroidJavaClass& operator=(AndroidJavaClass&& right)
    {
        _classID  = std::move(right._classID);
        return *this;
    }
    AndroidJavaClass& operator=(const AndroidJavaClass& other)
    {
        _classID = other.getRawClass();
        return *this;
    }

    void move(AndroidJavaObject&& right) = delete;
    void swap(AndroidJavaObject& right)= delete;
    void copy(AndroidJavaObject& other) = delete;

     /** Calls a Java method on an object (non-static).
     *  To call a method with return type 'void', use the regular version.
     *  @param[in]  methodName Specifies which method to call.
     *  @param[in]  args       An array of parameters passed to the method.
     */
    template<typename T = void, typename... Args>
    T call(std::string methodName, Args... args) = delete;

    /** Get the value of a field in an object (non-static).
     *  The generic parameter determines the field type.
     *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
     *  @return   _RT object.
     */
    template<typename T>
    T get(std::string fieldName) = delete;

    /** Set the value of a field in an object (non-static).
     *  The generic parameter determines the field type.
     *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
     *  @param[in] fieldValueThe value to assign to the field. It has to match the field type.
     */
    template<typename T>
    void set(std::string fieldName, T fieldValue) = delete;


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
     *  @return   _RT object.
     */
    template<typename T>
    T getStatic(std::string fieldName)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        const char * signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > > ::Result::value();
        jfieldID fid = env->GetStaticFieldID(_classID, fieldName.c_str(), signature);
        return JNICaller<T>::getFieldStatic(env, getRawObject(), fid);
    }

    /** Set the value of a static field in an object type.
     *  The generic parameter determines the field type.
     *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
     *  @param[in] fieldValueThe value to assign to the field. It has to match the field type.
     */
    template<typename T>
    void setStatic(std::string fieldName, T fieldValue)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        const char * signature = Concatenate<typename CPPToJNIConverter<T>::JNIType, CompileTimeString < '\0' > >::Result::value();
        jfieldID fid = env->GetStaticFieldID(_classID, fieldName.c_str(), signature);
        JNICaller<T>::setFieldStatic(env, getRawObject(), fid, CPPToJNIConverter<T>::convert(fieldValue));
    }
};

} //namespace Andorid

NS_FK_END

