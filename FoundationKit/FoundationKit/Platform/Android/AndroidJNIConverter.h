/****************************************************************************
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

C++ 类型转换为JNI类型，支持的基础类型有：
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
char*
std::string

支持的数组类型有：
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

JNI 类型转换为C++类型，支持全部JNI类型
-------------------------------------------------------------------------
类型对于关系
Java类型    JNI类型          C++类型          
boolean     jboolean        bool             
byte        jbyte           char             
char        jchar           unsigned char    
short       jshort          short/unsigned short
int         jint            int/unsigned int
long        jlong           long/unsigned long/long long
float       jfloat          float
double      jdouble         double
Object      jobject         jobject
Class       jclass          jclass
String      jstring         std::string/ const char*
Object[]    jobjectArray    std::vector<jobject>
boolean[]   jbooleanArray   std::vector<bool>
byte[]      jbyteArray      std::vector<char>
char[]      jcharArray      std::vector<unsigned char>
short[]     jshortArray     std::vector<short>
int[]       jintArray       std::vector<int>
long[]      jlongArray      std::vector<long>
float[]     jfloatArray     std::vector<float>
double[]    jdoubleArray    std::vector<double>
****************************************************************************/

#pragma once

#include <string>
#include <jni.h>
#include "AndroidJNIHelper.h"

NS_FK_BEGIN

namespace Android
{
    // this type convert for std::vector<std::string>
    class _jstringArray : public _jarray {};
    typedef _jstringArray*  jstringArray;

    template<typename T>
    struct TypeTranslation{using type = T;}; // jobject/ jclass
    template<>
    struct TypeTranslation<jboolean>{ using type = bool;};
    template<>
    struct TypeTranslation<jbyte>{ using type = char;};
    template<>
    struct TypeTranslation<jchar>{ using type = unsigned char;};
    template<>
    struct TypeTranslation<jshort>{ using type = short;};
    template<>
    struct TypeTranslation<jint>{ using type = int;};
    template<>
    struct TypeTranslation<jlong>{ using type = long;};
    template<>
    struct TypeTranslation<jfloat>{ using type = float;};
    template<>
    struct TypeTranslation<jdouble>{ using type = double;};
    template<>
    struct TypeTranslation<jstring>{ using type = std::string;};
    template<>
    struct TypeTranslation<jobjectArray>{ using type = std::vector<jobject>;};
    template<>
    struct TypeTranslation<jbooleanArray>{ using type = std::vector<bool>;};
    template<>
    struct TypeTranslation<jbyteArray>{ using type = std::vector<char>;};
    template<>
    struct TypeTranslation<jcharArray>{ using type = std::vector<unsigned char>;};
    template<>
    struct TypeTranslation<jshortArray>{ using type = std::vector<short>;};
    template<>
    struct TypeTranslation<jintArray>{ using type = std::vector<int>;};
    template<>
    struct TypeTranslation<jlongArray>{ using type = std::vector<long>;};
    template<>
    struct TypeTranslation<jfloatArray>{ using type = std::vector<float>;};
    template<>
    struct TypeTranslation<jdoubleArray>{ using type = std::vector<double>;};



//Compile Time String
template <char... Cs> struct CompileTimeString
{
    static const char * value()
    {
        static const char a[sizeof...(Cs)] = { Cs... };
        return a;
    }
};

//Concatenate 2 Strings
template <class L, class R> struct Concatenate2;
template <char... LC, char... RC>
struct Concatenate2 < CompileTimeString<LC...>, CompileTimeString<RC...> >
{
    using Result = CompileTimeString < LC..., RC... > ;
};

//Concatenate N strings
template <typename ...C> struct Concatenate;

template <typename C1> struct Concatenate < C1 >
{
    using Result = C1;
};

template <typename C1, typename C2> struct Concatenate < C1, C2 >
{
    using Result = typename Concatenate2<C1, C2>::Result;
};

template <typename C1, typename ...C> struct Concatenate < C1, C... >
{
    using Result = typename Concatenate2<C1, typename Concatenate<C...>::Result>::Result;
};

// ======================= CPPToJNIConverter ======================
//default template
template <typename T>
struct CPPToJNIConverter
{
    //The type is not support.
    // using JNIType = CompileTimeString < '' >;
     inline static T convert(T obj){ return obj; };
    
};

//generic pointer implementation (using jlong type)
template<typename T>
struct CPPToJNIConverter < T* >
{
    using JNIType = CompileTimeString < 'J' > ;
    inline static jlong convert(T* obj) { return reinterpret_cast<jlong>(obj); }
};

template<>
struct CPPToJNIConverter <jobject>
{
    using JNIType = CompileTimeString < 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'O', 'b', 'j', 'e', 'c', 't', ';' > ;
    inline static jobject convert(jobject obj) { return obj; }
};


template<>
struct CPPToJNIConverter < void >
{
    using JNIType = CompileTimeString < 'V' > ;
    inline static void convert(void){}
};

template<>
struct CPPToJNIConverter < unsigned char >
{
    using JNIType = CompileTimeString < 'C' > ;
    inline static jchar convert(unsigned char value){ return static_cast<jchar>(value); }
};

template<>
struct CPPToJNIConverter < unsigned short >
{
    using JNIType = CompileTimeString < 'S' > ;
    inline static jshort convert(unsigned short value){ return static_cast<jshort>(value); }
};

template<>
struct CPPToJNIConverter < unsigned int >
{
    using JNIType = CompileTimeString < 'I' > ;
    inline static jint convert(unsigned int value){ return static_cast<jint>(value); }
};

template<>
struct CPPToJNIConverter < unsigned long >
{
    using JNIType = CompileTimeString < 'J' > ;
    inline static jlong convert(unsigned long value){ return static_cast<jlong>(value); }
};

template<>
struct CPPToJNIConverter < unsigned long long>
{
    using JNIType = CompileTimeString < 'J' > ;
    inline static jlong convert(unsigned long long value){ return static_cast<jlong>(value); }
};

template<>
struct CPPToJNIConverter < bool >
{
    using JNIType = CompileTimeString < 'Z' > ;
    inline static jboolean convert(bool value){ return static_cast<jboolean>(value); }
};

template<>
struct CPPToJNIConverter < char >
{
    using JNIType = CompileTimeString < 'B' > ;
    inline static jbyte convert(char value){ return static_cast<jbyte>(value); }
};

template<>
struct CPPToJNIConverter < short >
{
    using JNIType = CompileTimeString < 'S' > ;
    inline static jshort convert(short value){ return static_cast<jshort>(value); }
};


template<>
struct CPPToJNIConverter < int >
{
    using JNIType = CompileTimeString < 'I' > ;
    inline static jint convert(int value) { return static_cast<jint>(value); }
};

template<>
struct CPPToJNIConverter < long >
{
    using JNIType = CompileTimeString < 'J' > ;
    inline static jlong convert(long value) { return static_cast<jlong>(value); }
};

template<>
struct CPPToJNIConverter < long long >
{
    using JNIType = CompileTimeString < 'J' > ;
    inline static jlong convert(long long value) { return static_cast<jlong>(value); }
};

template<>
struct CPPToJNIConverter < float >
{
    using JNIType = CompileTimeString < 'F' > ;
    inline static jfloat convert(float value) { return static_cast<jfloat>(value); }
};

template<>
struct CPPToJNIConverter < double >
{
    using JNIType = CompileTimeString < 'D' > ;
    inline static jdouble convert(double value) { return static_cast<jdouble>(value); }
};

template<>
struct CPPToJNIConverter < const char* >
{
    using JNIType = CompileTimeString < 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' > ;
    inline static jstring convert(const char * obj) { return AndroidJNIHelper::getInstance()->string2jstring(obj); }
};

template<>
struct CPPToJNIConverter < std::string >
{
    using JNIType = CompileTimeString < 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' > ;
    inline static jstring convert(const std::string & obj) { return AndroidJNIHelper::getInstance()->string2jstring(obj); }
};


//object implementations
template<>
struct CPPToJNIConverter < std::vector<std::string> >
{
    using JNIType = CompileTimeString < '[', 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' > ;
    static jobjectArray convert(const std::vector<std::string> & obj)
    { 
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass classId = env->FindClass("java/lang/String");
        jint size = obj.size();
        jobjectArray joa = env->NewObjectArray(size, classId, 0);

        for (int i = 0; i < size; i++)
        {
            jstring jstr = AndroidJNIHelper::getInstance()->string2jstring(obj[i]);
            env->SetObjectArrayElement(joa, i, jstr);
        }
        env->DeleteLocalRef(classId);
        AndroidJNIHelper::getInstance()->checkException();
        return joa;
    }
};

template<>
struct CPPToJNIConverter < std::vector<jobject> >
{
    using JNIType = CompileTimeString < '[', 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'O', 'b', 'j', 'e', 'c', 't', ';' > ;
    static jobjectArray convert(const std::vector<jobject> & obj)
    { 
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass classId = env->FindClass("java/lang/Object");
        jint size = obj.size();
        jobjectArray joa = env->NewObjectArray(size, classId, 0);

        for (int i = 0; i < size; i++)
        {
            env->SetObjectArrayElement(joa, i, obj[i]);
        }
        env->DeleteLocalRef(classId);
        AndroidJNIHelper::getInstance()->checkException();
        return joa;
    }
};


template<>
struct CPPToJNIConverter < std::vector<bool> >
{
    using JNIType = CompileTimeString < '[', 'Z'> ;
    static jbooleanArray convert(const std::vector<bool>& obj)
    {
        std::vector<unsigned char> boolArray;
        for (auto b : obj)
        {
            boolArray.push_back(b);
        }
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jbooleanArray jniArray = env->NewBooleanArray(boolArray.size());
        env->SetBooleanArrayRegion(jniArray, 0, boolArray.size(), (const jboolean*)&boolArray[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNIConverter < std::vector<char> >
{
    using JNIType = CompileTimeString < '[', 'B' > ;
    static jbyteArray convert(const std::vector<char>& obj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jbyteArray jniArray = env->NewByteArray(obj.size());
        env->SetByteArrayRegion(jniArray, 0, obj.size(), (const jbyte*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNIConverter < std::vector<unsigned char> >
{
    using JNIType = CompileTimeString < '[', 'C' > ;
    static jcharArray convert(const std::vector<unsigned char>& obj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jcharArray jniArray = env->NewCharArray(obj.size());
        env->SetCharArrayRegion(jniArray, 0, obj.size(), (const jchar*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNIConverter < std::vector<short> >
{
    using JNIType = CompileTimeString < '[', 'S' > ;
    static jshortArray convert(const std::vector<short>& obj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jshortArray jniArray = env->NewShortArray(obj.size());
        env->SetShortArrayRegion(jniArray, 0, obj.size(), (const jshort*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNIConverter < std::vector<int> >
{
    using JNIType = CompileTimeString < '[', 'I' > ;
    static jintArray convert(const std::vector<int>& obj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jintArray jniArray = env->NewIntArray(obj.size());
        env->SetIntArrayRegion(jniArray, 0, obj.size(), (const jint*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNIConverter < std::vector<long> >
{
    using JNIType = CompileTimeString < '[', 'J' > ;
    static jlongArray convert(const std::vector<long>& obj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jlongArray jniArray = env->NewLongArray(obj.size());
        env->SetLongArrayRegion(jniArray, 0, obj.size(), (const jlong*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNIConverter < std::vector<float> >
{
    using JNIType = CompileTimeString < '[', 'F' > ;
    static jfloatArray convert(const std::vector<float>& obj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jfloatArray jniArray = env->NewFloatArray(obj.size());
        env->SetFloatArrayRegion(jniArray, 0, obj.size(), (const jfloat*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNIConverter < std::vector<double> >
{
    using JNIType = CompileTimeString < '[', 'D' > ;
    static jdoubleArray convert(const std::vector<double>& obj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jdoubleArray jniArray = env->NewDoubleArray(obj.size());
        env->SetDoubleArrayRegion(jniArray, 0, obj.size(), (const jdouble*)&obj[0]);
        return jniArray;
    }
};

// ======================= getJNISignature ======================
//deduces the signature of a JNI method according to the variadic params and the return type
template <typename T, typename... Args>
const char * getJNISignature(Args...)
{
    return Concatenate < CompileTimeString<'('>, //left parenthesis
        typename CPPToJNIConverter<Args>::JNIType..., //params signature
        CompileTimeString<')'>, //right parenthesis
        typename CPPToJNIConverter<T>::JNIType, //return type signature
        CompileTimeString < '\0' > >::Result::value();
}


// ======================= JNIToCPPConverter ======================

// default template
// T is C++ type,just return the obj.
template <typename T>
struct JNIToCPPConverter
{
    inline static T convert(T obj){ return obj; }
};

template <>
struct JNIToCPPConverter<jboolean>
{
    inline static bool convert(jboolean obj){ return static_cast<bool>(obj); }
    static bool convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Boolean");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "booleanValue", "()Z");
        return env->CallBooleanMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jbyte >
{
    inline static char convert(jbyte obj){ return static_cast<char>(obj); }
    static char convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Byte");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "byteValue", "()B");
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jchar >
{
    inline static unsigned char convert(jchar obj){ return static_cast<unsigned char>(obj); }
    static unsigned char convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Character");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "charValue", "()C");
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jshort >
{
    inline static short convert(jshort obj){ return static_cast<short>(obj); }
    static short convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Short");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "shortValue", "()S");
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jint >
{
    inline static int convert(jint obj){ return static_cast<int>(obj); }
    static int convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Integer");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "intValue", "()I");
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jlong >
{
    inline static long convert(jlong obj){ return static_cast<long>(obj); }
    static long convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Long");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "longValue", "()J");
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jfloat >
{
    inline static float convert(jfloat obj){ return static_cast<float>(obj); }
    static float convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Float");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "floatValue", "()F");
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jdouble >
{
    inline static double convert(jdouble obj){ return static_cast<double>(obj); }
    static double convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/Double");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "doubleValue", "()D");
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPPConverter < jstring >
{
    inline static std::string convert(jstring obj){ return AndroidJNIHelper::getInstance()->jstring2string(obj); }
    inline static std::string convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        jclass instanceClass = env->FindClass("java/lang/String");
        assert(env->IsInstanceOf(jobj, instanceClass));
        return convert((jstring)jobj);
    }


};

// class AndroidJavaObject;
// template <>
// struct JNIToCPPConverter < jobject >
// {
//    inline static AndroidJavaObject convert(jobject obj){ return AndroidJavaObject(obj); }
// };

template <>
struct JNIToCPPConverter < jobjectArray >
{
    static std::vector<jobject> convert(jobjectArray jniArray)
    { 
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<jobject> result;
        if (jniArray)
        {
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                jobject valueJObject = env->GetObjectArrayElement(jniArray, i);
                result.push_back(valueJObject);
                env->DeleteLocalRef(valueJObject);
            }
        }
        return result;
    }

    static std::vector<jobject> convert(jobject jobj){return convert((jobjectArray)jobj);}
};

template <>
struct JNIToCPPConverter < jbooleanArray >
{
    static std::vector<bool> convert(jbooleanArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<bool> result;
        if (jniArray)
        {
            jboolean* elems = env->GetBooleanArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseBooleanArrayElements(jniArray, elems, 0);
        }
        return result;
    }
    static std::vector<bool> convert(jobject jobj){return convert((jbooleanArray)jobj);}
};

template <>
struct JNIToCPPConverter <jbyteArray>
{
    static std::vector<char> convert(jbyteArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<char> result;
        if (jniArray)
        {
            jbyte* elems = env->GetByteArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseByteArrayElements(jniArray, elems, 0);
        }
        return result;
    }
    static std::vector<char> convert(jobject jobj){return convert((jbyteArray)jobj);}
};

template <>
struct JNIToCPPConverter < jcharArray >
{
    static std::vector<unsigned char> convert(jcharArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<unsigned char> result;
        if (jniArray)
        {
            jchar* elems = env->GetCharArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseCharArrayElements(jniArray, elems, 0);
        }
        return result;
    }
    static std::vector<unsigned char> convert(jobject jobj){return convert((jcharArray)jobj);}
};

template <>
struct JNIToCPPConverter < jshortArray >
{
    static std::vector<short> convert(jshortArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<short> result;
        if (jniArray)
        {
            jshort* elems = env->GetShortArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseShortArrayElements(jniArray, elems, 0);
        }
        return result;
    }
    static std::vector<short> convert(jobject jobj){return convert((jshortArray)jobj);}
};

template <>
struct JNIToCPPConverter < jintArray >
{
    static std::vector<int> convert(jintArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<int> result;
        if (jniArray)
        {
            jint* elems = env->GetIntArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseIntArrayElements(jniArray, elems, 0);
        }
        return result;
    }
    static std::vector<int> convert(jobject jobj){return convert((jintArray)jobj);}
};

template <>
struct JNIToCPPConverter < jlongArray >
{
    static std::vector<long> convert(jlongArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<long> result;
        if (jniArray)
        {
            jlong* elems = env->GetLongArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseLongArrayElements(jniArray, elems, 0);
        }
        return result;
    }
    static std::vector<long> convert(jobject jobj){return convert((jlongArray)jobj);}
};

template <>
struct JNIToCPPConverter < jfloatArray >
{
    static std::vector<float> convert(jfloatArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<float> result;
        if (jniArray)
        {
            jfloat* elems = env->GetFloatArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseFloatArrayElements(jniArray, elems, 0);
        }
        return result;
    }
    static std::vector<float> convert(jobject jobj){return convert((jfloatArray)jobj);}
};

template <>
struct JNIToCPPConverter < jdoubleArray >
{
    static std::vector<double> convert(jdoubleArray jniArray)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<double> result;
        if (jniArray)
        {
            jdouble* elems = env->GetDoubleArrayElements(jniArray, NULL);
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            env->ReleaseDoubleArrayElements(jniArray, elems, 0);
        }
        return result;
    }

    static std::vector<double> convert(jobject jobj){return convert((jdoubleArray)jobj);}
};


template <>
struct JNIToCPPConverter < jstringArray >
{
    static std::vector<std::string> convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNIHelper::getInstance()->getEnv();
        std::vector<std::string> result;
        jobjectArray jniArray = (jobjectArray)jobj;
        if (jniArray)
        {
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                jobject valueJObject = env->GetObjectArrayElement(jniArray, i);
                result.push_back(AndroidJNIHelper::getInstance()->jstring2string((jstring)valueJObject));
                env->DeleteLocalRef(valueJObject);
            }
        }
        return result;
    }
};


// ======================= JobjectToArrayConverter ======================

//default template
template <typename T>
struct JobjectToArrayConverter
{
    inline static jobject convert(jobject obj){ return obj; }
};

template <>
struct JobjectToArrayConverter < std::vector<jobject> >
{
    static jobjectArray convert(jobject jobj)
    {
        return static_cast<jobjectArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<bool> >
{
    static jbooleanArray convert(jobject jobj)
    {
        return static_cast<jbooleanArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<char> >
{
    static jbyteArray convert(jobject jobj)
    {
        return static_cast<jbyteArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<unsigned char> >
{
    static jcharArray convert(jobject jobj)
    {
        return static_cast<jcharArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<short> >
{
    static jshortArray convert(jobject jobj)
    {
        return static_cast<jshortArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<int> >
{
    static jintArray convert(jobject jobj)
    {
        return static_cast<jintArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<long> >
{
    static jlongArray convert(jobject jobj)
    {
        return static_cast<jlongArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<float> >
{
    static jfloatArray convert(jobject jobj)
    {
        return static_cast<jfloatArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<double> >
{
    static jdoubleArray convert(jobject jobj)
    {
        return static_cast<jdoubleArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::vector<std::string> >
{
    static jstringArray convert(jobject jobj)
    {
        return static_cast<jstringArray>(jobj);
    }
};

template <>
struct JobjectToArrayConverter < std::string >
{
    static jstring convert(jobject jobj)
    {
        return static_cast<jstring>(jobj);
    }
};

} // namespace Android

NS_FK_END








