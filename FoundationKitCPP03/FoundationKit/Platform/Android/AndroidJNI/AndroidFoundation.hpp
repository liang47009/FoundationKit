/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDFOUNDATION_HPP
#define FOUNDATIONKIT_ANDROIDFOUNDATION_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if TARGET_PLATFORM == PLATFORM_ANDROID

#include <jni.h>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include "AndroidJNI.hpp"

NS_FK_BEGIN

namespace AndroidNode{

//Compile Time String
template <char... Chars> struct CompileTimeString
{
    static const char * value()
    {
        static const char _chararray[sizeof...(Chars)] = { Chars... };
        return _chararray;
    }
};

//CombineString 2 Strings
template <class L, class R> 
struct CombineStringImpl;

template <char... LC, char... RC>
struct CombineStringImpl < CompileTimeString<LC...>, CompileTimeString<RC...> >
{
    using Result = CompileTimeString < LC..., RC... >;
};

//CombineString N strings
template <typename ...C> 
struct CombineString;

template <typename C1> 
struct CombineString < C1 >
{
    using Result = C1;
};

template <typename C1, typename C2> 
struct CombineString < C1, C2 >
{
    using Result = typename CombineStringImpl<C1, C2>::Result;
};

template <typename C1, typename ...C> 
struct CombineString < C1, C... >
{
    using Result = typename CombineStringImpl<C1, typename CombineString<C...>::Result>::Result;
};

//=======================================================================
//========================== CPP Type To JNI Type =======================
//=======================================================================
// default template
template <typename T>
struct CPPToJNI
{
    //using JNIType = jobject;
    //using FunSig = CompileTimeString <''>;
    //inline static T convert(T obj){ return obj; };
};

//generic pointer implementation (using jlong type)
template<typename T>
struct CPPToJNI < T* >
{
    using JNIType = jlong;
    using FunSig = CompileTimeString < 'J' >;
    inline static jlong convert(T* obj) { return reinterpret_cast<jlong>(obj); }
};

// jobject
template<>
struct CPPToJNI <jobject>
{
    using JNIType = jobject;
    using FunSig = CompileTimeString < 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'O', 'b', 'j', 'e', 'c', 't', ';' >;
    inline static jobject convert(jobject obj) { return obj; }
};

// void, Use for return type
template<>
struct CPPToJNI < void >
{
    using JNIType = void;
    using FunSig = CompileTimeString < 'V' >;
    inline static void convert(void){}
};

// bool
template <>
struct CPPToJNI<bool>
{
    using JNIType = jboolean;
    using FunSig = CompileTimeString <'Z'>;
    inline static jboolean convert(bool val){ return val ? 1 : 0; };
};

// byte
template <>
struct CPPToJNI<char>
{
    using JNIType = jbyte;
    using FunSig = CompileTimeString < 'B' >;
    inline static jbyte convert(signed char val){ return static_cast<jbyte>(val); };
}; 
template <>
struct CPPToJNI<unsigned char> :public CPPToJNI<char>{};

//char
template <>
struct CPPToJNI<wchar_t>
{
    using JNIType = jchar;
    using FunSig = CompileTimeString < 'C' >;
    inline static jchar convert(unsigned short val){ return val; };
};

// short
template <>
struct CPPToJNI<short>
{
    using JNIType = jshort;
    using FunSig = CompileTimeString < 'S' >;
    inline static jshort convert(short val){ return val; };
};
template <>
struct CPPToJNI<unsigned short> :public CPPToJNI<short>{};

// int 
template <>
struct CPPToJNI<int>
{
    using JNIType = jint;
    using FunSig = CompileTimeString < 'I' >;
    inline static jint convert(short val){ return val; };
};
template <>
struct CPPToJNI<unsigned int> :public CPPToJNI<int>{};

// long
template <>
struct CPPToJNI<long long>
{
    using JNIType = jlong;
    using FunSig = CompileTimeString < 'J' >;
    inline static long long convert(long long val){ return val; };
};
template <>
struct CPPToJNI<long> :public CPPToJNI<long long>{};

template <>
struct CPPToJNI<unsigned long> :public CPPToJNI<long long>{};

// float
template <>
struct CPPToJNI<float>
{
    using JNIType = jfloat;
    using FunSig = CompileTimeString < 'F' >;
    inline static jfloat convert(float val){ return val; };
};

// double
template <>
struct CPPToJNI<double>
{
    using JNIType = jdouble;
    using FunSig = CompileTimeString < 'D' >;
    inline static jdouble convert(double val){ return val; };
};

template <>
struct CPPToJNI<std::string>
{
    using JNIType = jstring;
    using FunSig = CompileTimeString < 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' >;
    inline static jstring convert(const std::string& val){ return AndroidJNI::string2jstring(val); };
};

template <>
struct CPPToJNI<const char* > : public CPPToJNI<std::string>{};

// ============== Array implementation=========
template<>
struct CPPToJNI < std::vector<jobject> >
{
    using JNIType = jobjectArray;
    using FunSig = CompileTimeString < '[', 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'O', 'b', 'j', 'e', 'c', 't', ';' >;
    static jobjectArray convert(const std::vector<jobject> & obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass classId = env->FindClass("java/lang/Object");
        jint size = obj.size();
        jobjectArray joa = env->NewObjectArray(size, classId, 0);

        for (int i = 0; i < size; i++)
        {
            env->SetObjectArrayElement(joa, i, obj[i]);
        }
        env->DeleteLocalRef(classId);
        AndroidJNI::CheckJavaException();
        return joa;
    }
};


template<>
struct CPPToJNI < std::vector<bool> >
{
    using JNIType = jbooleanArray;
    using FunSig = CompileTimeString < '[', 'Z'>;
    static jbooleanArray convert(const std::vector<bool>& obj)
    {
        std::vector<unsigned char> boolArray;
        for (auto b : obj)
        {
            boolArray.push_back(b);
        }
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jbooleanArray jniArray = env->NewBooleanArray(boolArray.size());
        env->SetBooleanArrayRegion(jniArray, 0, boolArray.size(), (const jboolean*)&boolArray[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<char> >
{
    using JNIType = jbyteArray;
    using FunSig = CompileTimeString < '[', 'B' >;
    static jbyteArray convert(const std::vector<char>& obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jbyteArray jniArray = env->NewByteArray(obj.size());
        env->SetByteArrayRegion(jniArray, 0, obj.size(), (const jbyte*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<wchar_t> >
{
    using JNIType = jcharArray;
    using FunSig = CompileTimeString < '[', 'C' >;
    static jcharArray convert(const std::vector<wchar_t>& obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jcharArray jniArray = env->NewCharArray(obj.size());
        env->SetCharArrayRegion(jniArray, 0, obj.size(), (const jchar*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<short> >
{
    using JNIType = jshortArray;
    using FunSig = CompileTimeString < '[', 'S' >;
    static jshortArray convert(const std::vector<short>& obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jshortArray jniArray = env->NewShortArray(obj.size());
        env->SetShortArrayRegion(jniArray, 0, obj.size(), (const jshort*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<int> >
{
    using JNIType = jintArray;
    using FunSig = CompileTimeString < '[', 'I' >;
    static jintArray convert(const std::vector<int>& obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jintArray jniArray = env->NewIntArray(obj.size());
        env->SetIntArrayRegion(jniArray, 0, obj.size(), (const jint*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<long> >
{
    using JNIType = jlongArray;
    using FunSig = CompileTimeString < '[', 'J' >;
    static jlongArray convert(const std::vector<long>& obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jlongArray jniArray = env->NewLongArray(obj.size());
        env->SetLongArrayRegion(jniArray, 0, obj.size(), (const jlong*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<float> >
{
    using JNIType = jfloatArray;
    using FunSig = CompileTimeString < '[', 'F' >;
    static jfloatArray convert(const std::vector<float>& obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jfloatArray jniArray = env->NewFloatArray(obj.size());
        env->SetFloatArrayRegion(jniArray, 0, obj.size(), (const jfloat*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<double> >
{
    using JNIType = jdoubleArray;
    using FunSig = CompileTimeString < '[', 'D' >;
    static jdoubleArray convert(const std::vector<double>& obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jdoubleArray jniArray = env->NewDoubleArray(obj.size());
        env->SetDoubleArrayRegion(jniArray, 0, obj.size(), (const jdouble*)&obj[0]);
        return jniArray;
    }
};

template<>
struct CPPToJNI < std::vector<std::string> >
{
    using JNIType = jobjectArray;
    using FunSig = CompileTimeString < '[', 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' >;
    static jobjectArray convert(const std::vector<std::string> & obj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass classId = env->FindClass("java/lang/String");
        jint size = obj.size();
        jobjectArray joa = env->NewObjectArray(size, classId, 0);
        for (int i = 0; i < size; i++)
        {
            jstring jstr = AndroidJNI::string2jstring(obj[i]);
            env->SetObjectArrayElement(joa, i, jstr);
        }
        env->DeleteLocalRef(classId);
        AndroidJNI::CheckJavaException();
        return joa;
    }
};


//=======================================================================
//========================== JNI Type To CPP Type =======================
//=======================================================================
// default template
// T is C++ type,just return the obj.
template <typename T>
struct JNIToCPP
{
    using CPPType = T;
    inline static T convert(T obj){ return obj; }
};

template <>
struct JNIToCPP<jboolean>
{
    using CPPType = bool;
    inline static bool convert(jboolean obj){ return static_cast<bool>(obj); }
    static bool convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Boolean");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "booleanValue", "()Z");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallBooleanMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jbyte >
{
    using CPPType = char;
    inline static char convert(jbyte obj){ return static_cast<char>(obj); }
    static char convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Byte");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "byteValue", "()B");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallByteMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jchar >
{
    using CPPType = wchar_t;
    inline static wchar_t convert(jchar obj){ return static_cast<wchar_t>(obj); }
    static wchar_t convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Character");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "charValue", "()C");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallCharMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jshort >
{
    using CPPType = short;
    inline static short convert(jshort obj){ return static_cast<short>(obj); }
    static short convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Short");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "shortValue", "()S");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallShortMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jint >
{
    using CPPType = int;
    inline static int convert(jint obj){ return static_cast<int>(obj); }
    static int convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Integer");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "intValue", "()I");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallIntMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jlong >
{
    using CPPType = long;
    inline static long convert(jlong obj){ return static_cast<long>(obj); }
    static long convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Long");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "longValue", "()J");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallLongMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jfloat >
{
    using CPPType = float;
    inline static float convert(jfloat obj){ return static_cast<float>(obj); }
    static float convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Float");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "floatValue", "()F");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallFloatMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jdouble >
{
    using CPPType = double;
    inline static double convert(jdouble obj){ return static_cast<double>(obj); }
    static double convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/Double");
        assert(env->IsInstanceOf(jobj, instanceClass));
        jclass clazz = env->GetObjectClass(jobj);
        jmethodID methodID = env->GetMethodID(clazz, "doubleValue", "()D");
        env->DeleteLocalRef(instanceClass);
        env->DeleteLocalRef(clazz);
        return env->CallDoubleMethod(jobj, methodID);
    }
};

template <>
struct JNIToCPP < jstring >
{
    using CPPType = std::string;
    inline static std::string convert(jstring obj){ return AndroidJNI::jstring2string(obj); }
    static std::string convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        jclass instanceClass = env->FindClass("java/lang/String");
        assert(env->IsInstanceOf(jobj, instanceClass));
        env->DeleteLocalRef(instanceClass);
        return convert((jstring)jobj);
    }


};

// ============== Array implementation=========
template <>
struct JNIToCPP <jobjectArray >
{

    using CPPType = std::vector < jobject > ;
    static std::vector<jobject> convert(jobjectArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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

    static std::vector<jobject> convert(jobject jobj){ return convert((jobjectArray)jobj); }
};

template <>
struct JNIToCPP < jbooleanArray >
{
    using CPPType = std::vector < bool > ;
    static std::vector<bool> convert(jbooleanArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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
    static std::vector<bool> convert(jobject jobj){ return convert((jbooleanArray)jobj); }
};

template <>
struct JNIToCPP <jbyteArray>
{
    using CPPType = std::vector < char >;
    static std::vector<char> convert(jbyteArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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
    static std::vector<char> convert(jobject jobj){ return convert((jbyteArray)jobj); }
};

template <>
struct JNIToCPP < jcharArray >
{
    using CPPType = std::vector < wchar_t >;
    static std::vector<wchar_t> convert(jcharArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        std::vector<wchar_t> result;
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
    static std::vector<wchar_t> convert(jobject jobj){ return convert((jcharArray)jobj); }
};

template <>
struct JNIToCPP < jshortArray >
{
    using CPPType = std::vector < short >;
    static std::vector<short> convert(jshortArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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
    static std::vector<short> convert(jobject jobj){ return convert((jshortArray)jobj); }
};

template <>
struct JNIToCPP < jintArray >
{
    using CPPType = std::vector < int >;
    static std::vector<int> convert(jintArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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
    static std::vector<int> convert(jobject jobj){ return convert((jintArray)jobj); }
};

template <>
struct JNIToCPP < jlongArray >
{
    using CPPType = std::vector < long >;
    static std::vector<long> convert(jlongArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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
    static std::vector<long> convert(jobject jobj){ return convert((jlongArray)jobj); }
};

template <>
struct JNIToCPP < jfloatArray >
{
    using CPPType = std::vector < float >;
    static std::vector<float> convert(jfloatArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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
    static std::vector<float> convert(jobject jobj){ return convert((jfloatArray)jobj); }
};

template <>
struct JNIToCPP < jdoubleArray >
{
    using CPPType = std::vector < double >;
    static std::vector<double> convert(jdoubleArray jniArray)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
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

    static std::vector<double> convert(jobject jobj){ return convert((jdoubleArray)jobj); }
};


// this type convert for std::vector<std::string>
class _jstringArray : public _jarray {};
typedef _jstringArray*  jstringArray;
template <>
struct JNIToCPP < jstringArray >
{
    using CPPType = std::vector < std::string >;
    static std::vector<std::string> convert(jobject jobj)
    {
        JNIEnv *env = AndroidJNI::GetJavaEnv();
        std::vector<std::string> result;
        jobjectArray jniArray = (jobjectArray)jobj;
        if (jniArray)
        {
            jint length = env->GetArrayLength(jniArray);
            for (int i = 0; i < length; i++)
            {
                jobject valueJObject = env->GetObjectArrayElement(jniArray, i);
                result.push_back(AndroidJNI::jstring2string((jstring)valueJObject));
                env->DeleteLocalRef(valueJObject);
            }
        }
        return result;
    }
};

//=======================================================================
//============= Convert jobject to jobject array  ==============
//=======================================================================
//default template
template <typename T>
struct convertToArray
{
    inline static jobject convert(jobject obj){ return obj; }
};

template <>
struct convertToArray < std::vector<jobject> >
{
    static jobjectArray convert(jobject jobj)
    {
        return static_cast<jobjectArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<bool> >
{
    static jbooleanArray convert(jobject jobj)
    {
        return static_cast<jbooleanArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<char> >
{
    static jbyteArray convert(jobject jobj)
    {
        return static_cast<jbyteArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<unsigned char> >
{
    static jcharArray convert(jobject jobj)
    {
        return static_cast<jcharArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<short> >
{
    static jshortArray convert(jobject jobj)
    {
        return static_cast<jshortArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<int> >
{
    static jintArray convert(jobject jobj)
    {
        return static_cast<jintArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<long> >
{
    static jlongArray convert(jobject jobj)
    {
        return static_cast<jlongArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<float> >
{
    static jfloatArray convert(jobject jobj)
    {
        return static_cast<jfloatArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<double> >
{
    static jdoubleArray convert(jobject jobj)
    {
        return static_cast<jdoubleArray>(jobj);
    }
};

template <>
struct convertToArray < std::vector<std::string> >
{
    static jstringArray convert(jobject jobj)
    {
        return static_cast<jstringArray>(jobj);
    }
};

template <>
struct convertToArray < std::string >
{
    static jstring convert(jobject jobj)
    {
        return static_cast<jstring>(jobj);
    }
};

template <>
struct convertToArray < const char* > : public convertToArray < std::string > {};

//=======================================================================
//==========================      JNI CALLER      =======================
//=======================================================================


//default implementation (for jobject types)
template <typename T, typename... Args>
struct JNICaller 
{
    static T call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        auto obj = env->CallObjectMethod(instance, method, v...);
        T result = JNIToCPP<decltype(convertToArray<T>::convert(obj))>::convert(convertToArray<T>::convert(obj));
        if (obj)
            env->DeleteLocalRef(obj);
        return result;
    }

    static T getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        auto obj = env->GetObjectField(instance, fid);
        T result = JNIToCPP<decltype(convertToArray<T>::convert(obj))>::convert(convertToArray<T>::convert(obj));
        if (obj)
            env->DeleteLocalRef(obj);
        return result;
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, T fieldValue)
    {
        env->SetObjectField(instance, fid, fieldValue);
    }

    static T callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        auto obj = env->CallStaticObjectMethod(clazz, method, v...);
        T result = JNIToCPP<decltype(convertToArray<T>::convert(obj))>::convert(convertToArray<T>::convert(obj));
        if (obj)
            env->DeleteLocalRef(obj);
        return result;
    }
    static T getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        auto obj = env->GetStaticObjectField(clazz, fid);
        T result = JNIToCPP<decltype(convertToArray<T>::convert(obj))>::convert(convertToArray<T>::convert(obj));
        if (obj)
            env->DeleteLocalRef(obj);
        return result;
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, T fieldValue)
    {
        env->SetStaticObjectField(clazz, fid, fieldValue);
    }
};

//generic pointer implementation (using jlong types)

template <typename T, typename... Args>
struct JNICaller < T*, Args... >
{
    static T* call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return (T*)env->CallLongMethod(instance, method, v...);
    }

    static T* getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return (T*)env->GetLongField(instance, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jlong fieldValue)
    {
        env->SetLongField(instance, fid, fieldValue);
    }

    static T* callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return (T*)env->CallStaticLongMethod(clazz, method, v...);
    }

    static T* getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return (T*)env->GetStaticLongField(clazz, fid);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jlong fieldValue)
    {
        env->SetStaticLongField(clazz, fid, fieldValue);
    }
};


//void implementation
template <typename... Args>
struct JNICaller < void, Args... >
{
    static void call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        env->CallVoidMethod(instance, method, v...);
    }
    static void callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        env->CallStaticVoidMethod(clazz, method, v...);
    }
};

// jobject implementation
template <typename... Args>
struct JNICaller < jobject, Args... >
{
    static jobject call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallObjectMethod(instance, method, v...);
    }

    static jobject callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticObjectMethod(clazz, method, v...);
    }

    static jobject getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetObjectField(instance, fid);
    }

    static jobject getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticObjectField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jobject fieldValue)
    {
        env->SetObjectField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jobject fieldValue)
    {
        env->SetStaticObjectField(clazz, fid, fieldValue);
    }
};

//primitive types implementations
template <typename... Args>
struct JNICaller < bool, Args... >
{
    static bool call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallBooleanMethod(instance, method, v...);
    }

    static bool callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticBooleanMethod(clazz, method, v...);
    }

    static bool getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetBooleanField(instance, fid);
    }

    static bool getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticBooleanField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jboolean fieldValue)
    {
        env->SetBooleanField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jboolean fieldValue)
    {
        env->SetStaticBooleanField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller < char, Args... >
{

    static char call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallByteMethod(instance, method, v...);
    }

    static char callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticByteMethod(clazz, method, v...);
    }

    static char getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetByteField(instance, fid);
    }

    static char getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticByteField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jbyte fieldValue)
    {
        env->SetByteField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jbyte fieldValue)
    {
        env->SetStaticByteField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller < wchar_t, Args... >
{
    static unsigned char call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallCharMethod(instance, method, v...);
    }

    static unsigned char callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticCharMethod(clazz, method, v...);
    }

    static unsigned char getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetCharField(instance, fid);
    }

    static unsigned char getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticCharField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jchar fieldValue)
    {
        env->SetCharField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jchar fieldValue)
    {
        env->SetStaticCharField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller < short, Args... >
{
    static short call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallShortMethod(instance, method, v...);
    }

    static short callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticShortMethod(clazz, method, v...);
    }

    static short getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetShortField(instance, fid);
    }

    static short getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticShortField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jshort fieldValue)
    {
        env->SetShortField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jshort fieldValue)
    {
        env->SetStaticShortField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller < int, Args... >
{
    static int call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallIntMethod(instance, method, v...);
    }

    static int callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticIntMethod(clazz, method, v...);
    }

    static int getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetIntField(instance, fid);
    }

    static int getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticIntField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jint fieldValue)
    {
        env->SetIntField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jint fieldValue)
    {
        env->SetStaticIntField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller < long, Args... >
{
    static long call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallLongMethod(instance, method, v...);
    }

    static long callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticLongMethod(clazz, method, v...);
    }

    static long getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetLongField(instance, fid);
    }

    static long getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticLongField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jlong fieldValue)
    {
        env->SetLongField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jlong fieldValue)
    {
        env->SetStaticLongField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller < float, Args... >
{
    static float call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallFloatMethod(instance, method, v...);
    }

    static float callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticFloatMethod(clazz, method, v...);
    }

    static float getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetFloatField(instance, fid);
    }

    static float getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticFloatField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jfloat fieldValue)
    {
        env->SetFloatField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jfloat fieldValue)
    {
        env->SetStaticFloatField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller < double, Args... >
{
    static double call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        return env->CallDoubleMethod(instance, method, v...);
    }

    static double callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        return env->CallStaticDoubleMethod(clazz, method, v...);
    }

    static double getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        return env->GetDoubleField(instance, fid);
    }

    static double getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        return env->GetStaticDoubleField(clazz, fid);
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jdouble fieldValue)
    {
        env->SetDoubleField(instance, fid, fieldValue);
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jdouble fieldValue)
    {
        env->SetStaticDoubleField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller<std::string, Args...> {
    static std::string call(JNIEnv *env, jobject instance, jmethodID method, Args... v)
    {
        auto obj = env->CallObjectMethod(instance, method, v...);
        std::string result = JNIToCPP<jstring>::convert(obj);
        env->DeleteLocalRef(obj);
        return result;
    }

    static std::string getField(JNIEnv * env, jobject instance, jfieldID fid)
    {
        auto obj = env->GetObjectField(instance, fid);
        std::string result = JNIToCPP<jstring>::convert(obj);
        env->DeleteLocalRef(obj);
        return result;
    }

    static void setField(JNIEnv* env, jobject instance, jfieldID fid, jstring fieldValue)
    {
        env->SetObjectField(instance, fid, fieldValue);
    }

    static std::string callStatic(JNIEnv *env, jclass clazz, jmethodID method, Args... v)
    {
        auto obj = env->CallStaticObjectMethod(clazz, method, v...);
        std::string result = JNIToCPP<jstring>::convert(obj);
        env->DeleteLocalRef(obj);
        return result;
    }
    static std::string getFieldStatic(JNIEnv * env, jclass clazz, jfieldID fid)
    {
        auto obj = env->GetStaticObjectField(clazz, fid);
        std::string result = JNIToCPP<jstring>::convert(obj);
        env->DeleteLocalRef(obj);
        return result;
    }

    static void setFieldStatic(JNIEnv* env, jclass clazz, jfieldID fid, jstring fieldValue)
    {
        env->SetStaticObjectField(clazz, fid, fieldValue);
    }
};

template <typename... Args>
struct JNICaller<const char*, Args...> : public JNICaller<std::string, Args...>{};

//deduces the signature of a JNI method according to the variadic params and the return type
template <typename T, typename... Args>
const char * GetJNISignature(Args...)
{
    return CombineString < 
        CompileTimeString<'('>,  //left parenthesis
        typename CPPToJNI<typename std::decay<Args>::type>::FunSig...,      //params signature
        CompileTimeString<')'>,  //right parenthesis
        typename CPPToJNI<typename std::decay<T>::type>::FunSig,                //return type signature
        CompileTimeString < '\0' > 
    >::Result::value();
}

template<typename T = void, typename... Args>
T CallWithSig(jobject instance, const std::string & methodName, const std::string&  methodSignature, Args... args)
{
    JNIEnv* env = AndroidJNI::GetJavaEnv();
    jclass  clazz = env->GetObjectClass(instance);
    JavaClassMethod javaMethod = AndroidJNI::GetClassMethod(clazz, methodName.c_str(), methodSignature.c_str());
    env->DeleteLocalRef(clazz);
    return JNICaller<T, decltype(CPPToJNI<typename std::decay<Args>::type>::convert(args))...>::call(env, instance, javaMethod.method, CPPToJNI<typename std::decay<Args>::type>::convert(args)...);
}

/** Generic call to instance method
 *  The generic parameter determines the field type.
 *  @param[in] instance   The java class instance.
 *  @param[in] methodName The name of the method 
 *  @param[in] v          Generic arguments
 *  @return   T type object.
 */
template<typename T = void, typename... Args>
T Call(jobject instance, const std::string & methodName, Args... args)
{
    std::string methodSignature = GetJNISignature<T, Args...>(args...);
    return CallWithSig<T>(instance, methodName, methodSignature, std::forward<Args>(args)...);
}


template<typename T = void, typename... Args>
T CallStaticWithSig(const std::string & className, const std::string & methodName, const std::string&  methodSignature, Args... args)
{
    JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
    JavaClassMethod javaMethod = AndroidJNI::GetClassMethod(className.c_str(), methodName.c_str(), methodSignature.c_str(), true);
    return JNICaller<T, decltype(CPPToJNI<typename std::decay<Args>::type>::convert(args))...>::callStatic(jniEnv, javaMethod.clazz, javaMethod.method, CPPToJNI<typename std::decay<Args>::type>::convert(args)...);
}
/** generic call to static method
 *  The generic parameter determines the field type.
 *  @param[in] className  The java class name example: "com/example/foundationkitunittest/MainActivity"
 *                        or "com.example.foundationkitunittest.MainActivity"
 *  @param[in] methodName The name of the method
 *  @param[in] v          Generic arguments
 *  @return   T type object.
 */
template<typename T = void, typename... Args>
T CallStatic(const std::string & className, const std::string & methodName, Args... args)
{
    std::string methodSignature = GetJNISignature<T, Args...>(args...);
    return CallStaticWithSig<T>(className, methodName, methodSignature, std::forward<Args>(args)...);
}

template<typename T = void, typename... Args>
T CallStaticWithSig(jclass clazz, const std::string & methodName, const std::string&  methodSignature,Args... args)
{
    JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
    JavaClassMethod javaMethod = AndroidJNI::GetClassMethod(clazz, methodName.c_str(),methodSignature.c_str(), true);
    return JNICaller<T, decltype(CPPToJNI<typename std::decay<Args>::type>::convert(args))...>::callStatic(jniEnv, javaMethod.clazz, javaMethod.method, CPPToJNI<typename std::decay<Args>::type>::convert(args)...);
}
/** generic call to static method
 *  The generic parameter determines the field type.
 *  @param[in] jclass  java class instance
 *  @param[in] methodName The name of the method
 *  @param[in] v          Generic arguments
 *  @return   T type object.
 */
template<typename T = void, typename... Args>
T CallStatic(jclass clazz, const std::string & methodName, Args... args)
{
    std::string methodSignature = GetJNISignature<T, Args...>(args...);
    return CallStaticWithSig<T>(clazz, methodName, methodSignature, std::forward<Args>(args)...);
}

/** Get the value of a field in an object (non-static).
 *  The generic parameter determines the field type.
 *  @param[in] instance  The java object instance.
 *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
 *  @param[in] sig The field signature,for example: 
 *                 if the field type is String: String field = "";
 *                 the field sig is: Ljava/lang/String;
 *  @return    The field value.
 */
template<typename T>
T GetField(jobject instance, const std::string & fieldName, std::string sig = "")
{
    if (std::is_same<T, jobject>::value && sig.empty())
    {
        ANDROID_LOGD("Failed to get field value, return type is jobject, You must be set the field signature.");
        assert(false);
    }
    JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
    jclass clazz = jniEnv->GetObjectClass(instance);
    std::string signature = sig;
    if (signature.empty())
    {
        signature = CombineString<typename CPPToJNI<typename std::decay<T>::type>::FunSig, CompileTimeString < '\0' > > ::Result::value();
    }
    jfieldID fid = jniEnv->GetFieldID(clazz, fieldName.c_str(), signature.c_str());
    jniEnv->DeleteLocalRef(clazz);
    return JNICaller<typename std::decay<T>::type>::getField(jniEnv, instance, fid);
}

/** Get the value of a static field in an object type.
 *  The generic parameter determines the field type.
 *  @param[in] clazz     The java class.
 *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
 *  @param[in] sig The field signature,for example:
 *                 if the field type is String: String field = "";
 *                 the field sig is: Ljava/lang/String;
 *  @return   T type object.
 */
template<typename T>
T GetFieldStatic(jclass clazz, const std::string & fieldName, std::string sig = "")
{
    if (std::is_same<T, jobject>::value && sig.empty())
    {
        ANDROID_LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
        assert(false);
    }
    JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
    std::string signature = sig;
    if (signature.empty())
        signature = CombineString<typename CPPToJNI<typename std::decay<T>::type>::FunSig, CompileTimeString < '\0' > > ::Result::value();
    else
        std::replace(signature.begin(), signature.end(), '.', '/');
    jfieldID fid = jniEnv->GetStaticFieldID(clazz, fieldName.c_str(), signature.c_str());
    return JNICaller<typename std::decay<T>::type>::getFieldStatic(jniEnv, clazz, fid);
}

/** Get the value of a static field in an object type.
 *  The generic parameter determines the field type.
 *  @param[in] className The java class name example: "com/example/foundationkitunittest/MainActivity"
 *                        or "com.example.foundationkitunittest.MainActivity"
 *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
 *  @param[in] sig The field signature,for example:
 *                 if the field type is String: String field = "";
 *                 the field sig is: Ljava/lang/String;
 *  @return   T type object.
 */
template<typename T>
T GetFieldStatic(const std::string & className, const std::string & fieldName, std::string sig = "")
{
    jclass clazz = AndroidJNI::FindJavaClass(className.c_str());
    return GetFieldStatic<T>(clazz, fieldName, sig);
}

/** Set the value of a field in an object (non-static).
 *  The generic parameter determines the field type.
 *  @param[in] instance   The java object instance.
 *  @param[in] fieldName  The name of the field (e.g. int counter; would have fieldName = "counter")
 *  @param[in] fieldValue The value to assign to the field. It has to match the field type.
 *  @param[in] sig The field signature,for example:
 *                 if the field type is String: String field = "";
 *                 the field sig is: Ljava/lang/String;
 */
template<typename T>
void SetField(jobject instance, const std::string& fieldName, T fieldValue, std::string sig = "")
{
    if (std::is_same<T, jobject>::value && sig.empty())
    {
        ANDROID_LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
        assert(false);
    }
    JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
    jclass clazz = jniEnv->GetObjectClass(instance);
    std::string signature = sig;
    if (signature.empty())
    {
        signature = CombineString<typename CPPToJNI<typename std::decay<T>::type>::FunSig, CompileTimeString < '\0' > > ::Result::value();
    }
    jfieldID fid = jniEnv->GetFieldID(clazz, fieldName.c_str(), signature.c_str());
    jniEnv->DeleteLocalRef(clazz);
    JNICaller<typename std::decay<T>::type>::setField(jniEnv, instance, fid, CPPToJNI<typename std::decay<T>::type>::convert(fieldValue));
}

/** Set the value of a static field in an object type.
 *  The generic parameter determines the field type.
 *  @param[in] clazz     The java class.
 *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
 *  @param[in] fieldValueThe value to assign to the field. It has to match the field type.
 *  @param[in] sig The field signature,for example:
 *                 if the field type is String: String field = "";
 *                 the field sig is: Ljava/lang/String;
 */
template<typename T>
void SetFieldStatic(jclass clazz, const std::string& fieldName, T fieldValue, std::string sig = "")
{
    if (std::is_same<T, jobject>::value && sig.empty())
    {
        ANDROID_LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
        assert(false);
    }
    JNIEnv* jniEnv = AndroidJNI::GetJavaEnv();
    std::string signature = sig;
    if (signature.empty())
    {
        signature = CombineString<typename CPPToJNI<typename std::decay<T>::type>::FunSig, CompileTimeString < '\0' > > ::Result::value();
    }
    jfieldID fid = jniEnv->GetStaticFieldID(clazz, fieldName.c_str(), signature.c_str());
    JNICaller<typename std::decay<T>::type>::setFieldStatic(jniEnv, clazz, fid, CPPToJNI<typename std::decay<T>::type>::convert(fieldValue));
}

/** Set the value of a static field in an object type.
 *  The generic parameter determines the field type.
 *  @param[in] className The java class name example: "com/example/foundationkitunittest/MainActivity"
 *                        or "com.example.foundationkitunittest.MainActivity"
 *  @param[in] fieldName The name of the field (e.g. int counter; would have fieldName = "counter")
 *  @param[in] fieldValueThe value to assign to the field. It has to match the field type.
 *  @param[in] sig The field signature,for example:
 *                 if the field type is String: String field = "";
 *                 the field sig is: Ljava/lang/String;
 */
template<typename T>
void SetFieldStatic(const std::string & className, const std::string& fieldName, T fieldValue, std::string sig = "")
{
    jclass clazz = AndroidJNI::FindJavaClass(className.c_str());
    SetFieldStatic<T>(clazz, fieldName, fieldValue, sig);
}

} // namespace AndroidNode

NS_FK_END

#endif //#if TARGET_PLATFORM == PLATFORM_ANDROID

#endif // FOUNDATIONKIT_ANDROIDFOUNDATION_HPP
