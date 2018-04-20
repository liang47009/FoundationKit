/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDLEXICALCAST_HPP
#define FOUNDATIONKIT_ANDROIDLEXICALCAST_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <type_traits>
#include <string>
#include <vector>
#include <jni.h>
#include "AndroidJNI.hpp"
#include "AndroidJNIOperation.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Base/identity_type.hpp"
NS_FK_BEGIN
namespace android 
{

    /**
        bool                <--> jboolean
        char                <--> jbyte
        wchar_t             <--> jchar
        short               <--> jshort
        int                 <--> jint
        long long           <--> jlong
        float               <--> jfloat
        double              <--> jdouble
        bool                <--> java/lang/Boolean
        char                <--> java/lang/Byte
        wchar_t             <--> java/lang/Character
        short               <--> java/lang/Short
        int                 <--> java/lang/Integer
        long long           <--> java/lang/Long
        float               <--> java/lang/Float
        double              <--> java/lang/Double
        std::string         <--> jstring
        std::vector<bool>       <--> jbooleanArray
        std::vector<char>,      <--> jbyteArray
        std::vector<wchar_t>,   <--> jcharArray
        std::vector<short>,     <--> jshortArray
        std::vector<int>,       <--> jintArray
        std::vector<long long>, <--> jlongArray 
        std::vector<float>,     <--> jfloatArray
        std::vector<double>,    <--> jdoubleArray
        std::vector<std::string><--> jobjectArray
        std::vector<jobject>,   <--> jobjectArray
        */


    template<typename To, typename From>
    struct converter
    {
        static To convert(From from)
        {
            return static_cast<To>(from);
        }
    };

    template<typename From>
    struct converter<void, From>
    {
        static void convert(From from)
        {
        }
    };
        
    template<typename JT>
    JT PrimitiveTypeConvert(jobject from, const std::string& InClassName, const std::string& InMethodName, const std::string& InSignature)
    {
        JNIEnv *env = AndroidJNI::GetJNIEnv();
        jclass booleanClass = env->FindClass(InClassName.c_str());
        ASSERT_IF(!(env->IsInstanceOf(from, booleanClass)), "Type mismatch.");
        jmethodID methodID = env->GetMethodID(booleanClass, InMethodName.c_str(), InSignature.c_str());
        env->DeleteLocalRef(booleanClass);
        return Op<JT>::CallMethod(from, methodID);
    }

#define JNI_TO_CPP_PRIMITIVE_CONVERTER(RT,JT,CLASS, METHOD, SIG)                                    \
    template<>                                                                                  \
    struct converter<RT, JT >                                                                   \
    {                                                                                           \
        static RT convert(jobject from)                                                         \
        {                                                                                       \
            return static_cast<RT>(PrimitiveTypeConvert<JT>(from, CLASS, METHOD, SIG));         \
        }                                                                                       \
        static RT convert(JT from)                                                              \
        {                                                                                       \
            return static_cast<RT>(from);                                                       \
        }                                                                                       \
    };

    JNI_TO_CPP_PRIMITIVE_CONVERTER(bool,      jboolean, "java/lang/Boolean",   "booleanValue", "()Z");
    JNI_TO_CPP_PRIMITIVE_CONVERTER(char,      jbyte,    "java/lang/Byte",      "byteValue",    "()B");
    JNI_TO_CPP_PRIMITIVE_CONVERTER(wchar_t,   jchar,    "java/lang/Character", "charValue",    "()C");
    JNI_TO_CPP_PRIMITIVE_CONVERTER(short,     jshort,   "java/lang/Short",     "shortValue",   "()S");
    JNI_TO_CPP_PRIMITIVE_CONVERTER(int,       jint,     "java/lang/Integer",   "intValue",     "()I");
    JNI_TO_CPP_PRIMITIVE_CONVERTER(int64,     jlong,    "java/lang/Long",      "longValue",    "()J");
    JNI_TO_CPP_PRIMITIVE_CONVERTER(float,     jfloat,   "java/lang/Float",     "floatValue",   "()F");
    JNI_TO_CPP_PRIMITIVE_CONVERTER(double,    jdouble,  "java/lang/Double",    "doubleValue",  "()D");
#undef  JNI_TO_CPP_PRIMITIVE_CONVERTER
    template<>
    struct converter<bool,    jobject> :public converter<bool,    jboolean> {};
    template<>
    struct converter<char,    jobject> :public converter<char,    jbyte> {};
    template<>
    struct converter<wchar_t, jobject> :public converter<wchar_t, jchar> {};
    template<>
    struct converter<short,   jobject> :public converter<short,   jshort> {};
    template<>
    struct converter<int,     jobject> :public converter<int,     jint> {};
    template<>
    struct converter<int64,   jobject> :public converter<int64,   jlong> {};
    template<>
    struct converter<float,   jobject> :public converter<float,   jfloat> {};
    template<>
    struct converter<double,  jobject> :public converter<double,  jdouble> {};

    template<>
    struct converter<std::string, jstring>
    {
        static std::string convert(jstring from)
        {
            return AndroidJNI::jstring2string(from);
        }
        static std::string convert(jobject from)
        {
            JNIEnv *env = AndroidJNI::GetJNIEnv();
            jclass jstringClass = env->FindClass("java/lang/String");
            ASSERT_IF(!(env->IsInstanceOf(from, jstringClass)), "Type mismatch.");
            env->DeleteLocalRef(jstringClass);
            return convert((jstring)from);
        }
    };
    template<>
    struct converter<std::string, jobject> :public converter<std::string, jstring> {};

#define CPP_TO_JNI_PRIMITIVE_CONVERTER(RT,CT,CLASS,SIG)                                             \
    template<>                                                                                  \
    struct converter<RT, CT >                                                                   \
    {                                                                                           \
        static RT convert(CT from)                                                              \
        {                                                                                       \
            JNIEnv* env = AndroidJNI::GetJNIEnv();                                              \
            JavaClassMethod method = AndroidJNI::GetClassMethod(CLASS, "<init>", SIG);          \
            jobject object = env->NewObject(method.clazz, method.method, from);                 \
            jobject globalObject = env->NewGlobalRef(object);                                   \
            env->DeleteLocalRef(object);                                                        \
            return globalObject;                                                                \
        }                                                                                       \
    };

    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, bool,      "java/lang/Boolean",   "(Z)V");
    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, char,      "java/lang/Byte",      "(B)V");
    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, wchar_t,   "java/lang/Character", "(C)V");
    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, short,     "java/lang/Short",     "(S)V");
    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, int,       "java/lang/Integer",   "(I)V");
    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, int64,     "java/lang/Long",      "(J)V");
    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, float,     "java/lang/Float",     "(F)V");
    CPP_TO_JNI_PRIMITIVE_CONVERTER(jobject, double,    "java/lang/Double",    "(D)V");
#undef  CPP_TO_JNI_PRIMITIVE_CONVERTER


    template<>
    struct converter<jstring, std::string>
    {
        static jstring convert(const std::string& from)
        {
            return AndroidJNI::string2jstring(from);
        }
    };
    template <>
    struct converter<jstring,const char* > : public converter<jstring, std::string> {};

#define JNI_TO_CPP_ARRAY_CONVERTER(RT,JTA,JT)                       \
    template<>                                                  \
    struct converter<RT, JTA>                                   \
    {                                                           \
        static RT convert(JTA from)                             \
        {                                                       \
            RT result;                                          \
            if(!from) return result;                            \
            JT* elems = Op<JT>::GetArrayElements(from);         \
            JNIEnv *env = AndroidJNI::GetJNIEnv();              \
            jint length = env->GetArrayLength(from);            \
            result.resize(length);                              \
            memcpy(result.data(), elems, sizeof(JT)*length);    \
            Op<JT>::ReleaseArrayElements(from, elems, 0);       \
            return result;                                      \
        }                                                       \
    };

    JNI_TO_CPP_ARRAY_CONVERTER(std::vector<char>,      jbyteArray,   jbyte);
    JNI_TO_CPP_ARRAY_CONVERTER(std::vector<wchar_t>,   jcharArray,   jchar);
    JNI_TO_CPP_ARRAY_CONVERTER(std::vector<short>,     jshortArray,  jshort);
    JNI_TO_CPP_ARRAY_CONVERTER(std::vector<int>,       jintArray,    jint);
    JNI_TO_CPP_ARRAY_CONVERTER(std::vector<int64>,     jlongArray,   jlong);
    JNI_TO_CPP_ARRAY_CONVERTER(std::vector<float>,     jfloatArray,  jfloat);
    JNI_TO_CPP_ARRAY_CONVERTER(std::vector<double>,    jdoubleArray, jdouble);

#undef JNI_TO_CPP_ARRAY_CONVERTER

    template<>
    struct converter<std::vector<bool>, jbooleanArray>
    {
        static std::vector<bool> convert(jbooleanArray from)
        {
            std::vector<bool> result;
            jboolean* elems = Op<jboolean>::GetArrayElements(from);
            JNIEnv *env = AndroidJNI::GetJNIEnv();
            jint length = env->GetArrayLength(from);
            for (int i = 0; i < length; i++)
            {
                result.push_back(elems[i]);
            }
            Op<jboolean>::ReleaseArrayElements(from, elems, 0);
            return result;
        }
    };

    template<>
    struct converter<std::vector<std::string>, jobjectArray>
    {
        static std::vector<std::string> convert(jobjectArray from)
        {
            JNIEnv *env = AndroidJNI::GetJNIEnv();
            std::vector<std::string> result;
            if (!from) return result;
            jint length = env->GetArrayLength(from);
            for (int i = 0; i < length; i++)
            {
                jobject valueJObject = env->GetObjectArrayElement(from, i);
                result.push_back(AndroidJNI::jstring2string((jstring)valueJObject));
                env->DeleteLocalRef(valueJObject);
            }
            return result;
        }
    };

    template<>
    struct converter<std::vector<jobject>, jobjectArray>
    {
        static std::vector<jobject> convert(jobjectArray from)
        {
            JNIEnv *env = AndroidJNI::GetJNIEnv();
            std::vector<jobject> result;
            if (!from) return result;
            jint length = env->GetArrayLength(from);
            for (int i = 0; i < length; i++)
            {
                jobject valueJObject = env->GetObjectArrayElement(from, i);
                result.push_back(env->NewGlobalRef(valueJObject));
                env->DeleteLocalRef(valueJObject);
            }
            return result;
        }
    };


#define CPP_TO_JNI_ARRAY_CONVERTER(JTA,CTA,JT)                                             \
    template<>                                                                         \
    struct converter<JTA, CTA>                                                         \
    {                                                                                  \
        static JTA convert(const CTA& from)                                            \
        {                                                                              \
            JTA jniArray =Op<JT>::NewArray(from.size());                               \
            Op<JT>::SetArrayRegion(jniArray, 0, from.size(), (JT*)from.data());  \
            return jniArray;                                                           \
        }                                                                              \
    };
    CPP_TO_JNI_ARRAY_CONVERTER(jbyteArray,   std::vector<char>,      jbyte);
    CPP_TO_JNI_ARRAY_CONVERTER(jcharArray,   std::vector<wchar_t>,   jchar);
    CPP_TO_JNI_ARRAY_CONVERTER(jshortArray,  std::vector<short>,     jshort);
    CPP_TO_JNI_ARRAY_CONVERTER(jintArray,    std::vector<int>,       jint);
    CPP_TO_JNI_ARRAY_CONVERTER(jlongArray,   std::vector<int64>,     jlong);
    CPP_TO_JNI_ARRAY_CONVERTER(jfloatArray,  std::vector<float>,     jfloat);
    CPP_TO_JNI_ARRAY_CONVERTER(jdoubleArray, std::vector<double>,    jdouble);
#undef  CPP_TO_JNI_ARRAY_CONVERTER

    template<>
    struct converter<jbooleanArray, std::vector<bool>>
    {
        static jbooleanArray convert(const std::vector<bool>& from)
        {
            std::vector<unsigned char> boolArray;
            for (auto b : from)
            {
                boolArray.push_back(b);
            }
            JNIEnv *env = AndroidJNI::GetJNIEnv();
            jbooleanArray jniArray = env->NewBooleanArray(boolArray.size());
            env->SetBooleanArrayRegion(jniArray, 0, boolArray.size(), (const jboolean*)&boolArray[0]);
            return jniArray;
        }
    };

    template<>
    struct converter<jobjectArray,std::vector<std::string>>
    {
        static jobjectArray convert(const std::vector<std::string>& from)
        {
            JNIEnv *env = AndroidJNI::GetJNIEnv();
            jclass classId = AndroidJNI::FindJavaClass("java/lang/String");
            jint arraySize = from.size();
            jobjectArray resultArray = env->NewObjectArray(arraySize, classId, 0);
            for (int i = 0; i < arraySize; i++)
            {
                jstring jstr = AndroidJNI::string2jstring(from[i]);
                env->SetObjectArrayElement(resultArray, i, jstr);
            }
            env->DeleteLocalRef(classId);
            AndroidJNI::CheckException();
            return resultArray;
        }
    };

    template<>
    struct converter<jobjectArray, std::vector<jobject>>
    {
        static jobjectArray convert(const std::vector<jobject>& from)
        {
            JNIEnv *env = AndroidJNI::GetJNIEnv();
            jclass classId = env->FindClass("java/lang/Object");
            jint arraySize = from.size();
            jobjectArray resultArray = env->NewObjectArray(arraySize, classId, 0);
            for (int i = 0; i < arraySize; i++)
            {
                env->SetObjectArrayElement(resultArray, i, from[i]);
            }
            env->DeleteLocalRef(classId);
            AndroidJNI::CheckException();
            return resultArray;
        }
    };

    template<typename T>
    struct CPPTypeCast
    {
        using JNIType   = jobject ;
        using Signature = FixedChars< 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'O', 'b', 'j', 'e', 'c', 't', ';' >;
        using Converter = converter<jobject, T> ;
    };

    template<>
    struct CPPTypeCast<void>
    {
        using JNIType   = jvoid;
        using Signature = FixedChars< 'V' >;
        using Converter = converter<void, jvoid>;
    };

#define CPP_TYPE_CAST(JT,CT,SIG)               \
    template<>                                 \
    struct CPPTypeCast<CT>                     \
    {                                          \
        using JNIType   = JT;                    \
        using Signature = typename IDENTITY_TYPE((SIG)); \
        using Converter = converter<JT, CT>;             \
    };

    CPP_TYPE_CAST(jboolean,      bool,                     (FixedChars< 'Z' >));
    CPP_TYPE_CAST(jbyte,         char,                     (FixedChars< 'B' >));
    CPP_TYPE_CAST(jchar,         wchar_t,                  (FixedChars< 'C' >));
    CPP_TYPE_CAST(jshort,        short,                    (FixedChars< 'S' >));
    CPP_TYPE_CAST(jint,          int,                      (FixedChars< 'I' >));
    CPP_TYPE_CAST(jlong,         long long,                (FixedChars< 'J' >));
    CPP_TYPE_CAST(jfloat,        float,                    (FixedChars< 'F' >));
    CPP_TYPE_CAST(jdouble,       double,                   (FixedChars< 'D' >));
    CPP_TYPE_CAST(jstring,       std::string,              (FixedChars< 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' >));
    CPP_TYPE_CAST(jobject,       jobject,                  (FixedChars< 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'O', 'b', 'j', 'e', 'c', 't', ';' >));
    CPP_TYPE_CAST(jbooleanArray, std::vector<bool>,        (FixedChars< '[', 'Z'>));
    CPP_TYPE_CAST(jbyteArray,    std::vector<char>,        (FixedChars< '[', 'B'>));
    CPP_TYPE_CAST(jcharArray,    std::vector<wchar_t>,     (FixedChars< '[', 'C'>));
    CPP_TYPE_CAST(jshortArray,   std::vector<short>,       (FixedChars< '[', 'S'>));
    CPP_TYPE_CAST(jintArray,     std::vector<int>,         (FixedChars< '[', 'I'>));
    CPP_TYPE_CAST(jlongArray,    std::vector<int64>,       (FixedChars< '[', 'J'>));
    CPP_TYPE_CAST(jfloatArray,   std::vector<float>,       (FixedChars< '[', 'F'>));
    CPP_TYPE_CAST(jdoubleArray,  std::vector<double>,      (FixedChars< '[', 'D'>));
    CPP_TYPE_CAST(jobjectArray,  std::vector<std::string>, (FixedChars< '[', 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' >));
    CPP_TYPE_CAST(jobjectArray,  std::vector<jobject>,     (FixedChars< '[', 'L', 'j', 'a', 'v', 'a', '/', 'l', 'a', 'n', 'g', '/', 'S', 't', 'r', 'i', 'n', 'g', ';' >));
#undef  CPP_TYPE_CAST
    template<>
    struct CPPTypeCast<const char*> : public CPPTypeCast<std::string> {};


    template<typename To, typename From>
    typename std::enable_if<!std::is_same<To, From>::value, To>::type
        lexical_cast(const From& from)
    {
        return converter<To, typename std::decay<From>::type>::convert(from);
    }

    template<typename To, typename From>
    typename std::enable_if<std::is_same<To, From>::value, To>::type
        lexical_cast(const From& from)
    {
        return from;
    }


}// namespace android
NS_FK_END

#endif //#if PLATFORM_ANDROID

#endif // END OF FOUNDATIONKIT_ANDROIDLEXICALCAST_HPP