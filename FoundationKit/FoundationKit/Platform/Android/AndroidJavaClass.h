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

#include <algorithm>
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
        move(std::forward<AndroidJavaClass>(right));
    }
    AndroidJavaClass(const AndroidJavaClass& other): AndroidJavaClass()
    {
        copy(const_cast<AndroidJavaClass&>(other));
    }

    /** Construct an AndroidJavaClass based on the name of the java class.
     *  This essentially means locate the class type, allocate an object and run the specified constructor.
     *  @param[in] className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
     */

    AndroidJavaClass(std::string className)
    {
        std::replace(className.begin(), className.end(), '.', '/');
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        if (!env) 
        {
            LOGE("Failed to get evn");
            return;
        }
        jclass classPtr = AndroidJNIHelper::getInstance()->getClassID(className.c_str());
        if(classPtr)
        {
            _classID = (jclass)env->NewGlobalRef(classPtr);
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

    virtual ~AndroidJavaClass(){ _classID = nullptr;};

    AndroidJavaClass& operator=(AndroidJavaClass&& right)
    {
        move(std::forward<AndroidJavaClass>(right));
        return *this;
    }
    
    AndroidJavaClass& operator=(const AndroidJavaClass& other)
    {
        copy(const_cast<AndroidJavaClass&>(other));
        return *this;
    }

    /** 
    void move(AndroidJavaObject&& right) = delete;
    void swap(AndroidJavaObject& right)= delete;
    void copy(AndroidJavaObject& other) = delete;

    void move(AndroidJavaClass&& right)
    {
        FK_SAFE_RELEASE(_shared_ObjPtr);
        _classID        = std::move(right._classID);
        _shared_ObjPtr  = std::move(right._shared_ObjPtr);
        right._classID        = nullptr;
        right._shared_ObjPtr  = nullptr; 
    }
    void copy(AndroidJavaClass& other)
    {
        FK_SAFE_RELEASE(_shared_ObjPtr);
        _classID       = other.getRawClass();
        _shared_ObjPtr = other._shared_ObjPtr;
        FK_SAFE_RETAIN(_shared_ObjPtr);
    }

    */

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
};

} //namespace Andorid

NS_FK_END

