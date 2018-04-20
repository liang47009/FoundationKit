/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVACORE_HPP
#define FOUNDATIONKIT_ANDROIDJAVACORE_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID
#include <algorithm>
#include "AndroidJNIOperation.hpp"
#include "AndroidLexicalCast.hpp"
NS_FK_BEGIN

namespace android 
{

    //deduces the signature of a JNI method according to the variadic params and the return type
    template <typename T, typename... Args>
    const char * GetJNISignature(Args...)
    {
        return CombineFixedChars <
            FixedChars<'('>,  //left parenthesis
            typename CPPTypeCast<typename std::decay<Args>::type>::Signature...,      //params signature
            FixedChars<')'>,  //right parenthesis
            typename CPPTypeCast<typename std::decay<T>::type>::Signature,            //return type signature
            FixedChars < '\0' >
        >::Result::value();
    }

    /**
    static std::string ParseOneSig(size_t& index, const std::string& subSigs)
    {
        std::string oneSig;
        char ch = subSigs[index];
        if (ch == '[')
        {
            index += 1;
            oneSig = '[';
            oneSig += ParseOneSig(index, subSigs);
        }
        else if (ch == 'L')
        {
            size_t nPos = subSigs.find(';', index);
            if (nPos == std::string::npos)
            {
                ASSERT_IF(nPos == std::string::npos, "Invalid signature.");
            }
            oneSig = subSigs.substr(index, nPos - index + 1);
            index = nPos + 1;
        }
        else
        {
            oneSig = ch;
            index += 1;

        }
        return oneSig;
    };
    static  std::vector<std::string> ParseMethodSignature(const std::string& sigs)
    {
        std::vector<std::string> SigList;
        size_t index = 0;
        while (index < sigs.size())
        {
            SigList.push_back(ParseOneSig(index, sigs));
        }
        return SigList;
    }
    */

   /** generic call to static method
    *  The generic parameter determines the field type.
    *  @param[in] className  The java class name example: "com/example/foundationkitunittest/MainActivity"
    *                        or "com.example.foundationkitunittest.MainActivity"
    *  @param[in] methodName The name of the method
    *  @param[in] args       Generic arguments
    *  @return   T type object.
    */
    template<typename T = void, typename... Args>
    T CallWithSig(jobject instance, const std::string & methodName, const std::string&  methodSignature, Args&&... args)
    {
        JNIEnv* env = AndroidJNI::GetJNIEnv();
        jclass  clazz = env->GetObjectClass(instance);
        JavaClassMethod javaMethod = AndroidJNI::GetClassMethod(clazz, methodName.c_str(), methodSignature.c_str());
        env->DeleteLocalRef(clazz);
        auto result = Op<typename CPPTypeCast<T>::JNIType>::CallMethod(instance, javaMethod.method, CPPTypeCast<typename std::decay<Args>::type>::Converter::convert(args)...);
        return android::lexical_cast<typename std::decay<T>::type, decltype(result)>(result);
    }

   /** Generic call to instance method
    *  The generic parameter determines the field type.
    *  @param[in] instance   The java class instance.
    *  @param[in] methodName The name of the method
    *  @param[in] args       Generic arguments
    *  @return   T type object.
    */
    template<typename T = void, typename... Args>
    T Call(jobject instance, const std::string & methodName, Args&&... args)
    {
        std::string methodSignature = GetJNISignature<T, Args...>(args...);
        return CallWithSig<T>(instance, methodName, methodSignature, std::forward<Args>(args)...);
    }


    template<typename T = void, typename... Args>
    T CallStaticWithSig(const std::string & className, const std::string & methodName, const std::string&  methodSignature, Args&&... args)
    {
        JavaClassMethod javaMethod = AndroidJNI::GetClassMethod(className.c_str(), methodName.c_str(), methodSignature.c_str(), true);
        auto result = Op<typename CPPTypeCast<T>::JNIType>::CallStaticMethod(javaMethod.clazz, javaMethod.method, CPPTypeCast<typename std::decay<Args>::type>::Converter::convert(args)...);
        return android::lexical_cast<typename std::decay<T>::type, decltype(result)>(result);
    }

    template<typename T = void, typename... Args>
    T CallStaticWithSig(jclass clazz, const std::string & methodName, const std::string&  methodSignature, Args&&... args)
    {
        JavaClassMethod javaMethod = AndroidJNI::GetClassMethod(clazz, methodName.c_str(), methodSignature.c_str(), true);
        auto result = Op<typename CPPTypeCast<T>::JNIType>::CallStaticMethod(javaMethod.clazz, javaMethod.method, CPPTypeCast<typename std::decay<Args>::type>::Converter::convert(args)...);
        return android::lexical_cast<typename std::decay<T>::type, decltype(result)>(result);
    }

   /** generic call to static method
    *  The generic parameter determines the field type.
    *  @param[in] className  The java class name example: "com/example/foundationkitunittest/MainActivity"
    *                        or "com.example.foundationkitunittest.MainActivity"
    *  @param[in] methodName The name of the method
    *  @param[in] args       Generic arguments
    *  @return   T type object.
    */
    template<typename T = void, typename... Args>
    T CallStatic(const std::string & className, const std::string & methodName, Args&&... args)
    {
        std::string methodSignature = GetJNISignature<T, Args...>(args...);
        return CallStaticWithSig<T>(className, methodName, methodSignature, std::forward<Args>(args)...);
    }

   /** generic call to static method
    *  The generic parameter determines the field type.
    *  @param[in] jclass  java class instance
    *  @param[in] methodName The name of the method
    *  @param[in] args       Generic arguments
    *  @return   T type object.
    */
    template<typename T = void, typename... Args>
    T CallStatic(jclass clazz, const std::string & methodName, Args&&... args)
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
            return T();
        }
        JNIEnv* jniEnv = AndroidJNI::GetJNIEnv();
        jclass clazz = jniEnv->GetObjectClass(instance);
        std::string signature = sig;
        if (signature.empty())
        {
            signature = CombineFixedChars<typename CPPTypeCast<typename std::decay<T>::type>::Signature, FixedChars < '\0' > > ::Result::value();
        }
        jfieldID fid = jniEnv->GetFieldID(clazz, fieldName.c_str(), signature.c_str());
        jniEnv->DeleteLocalRef(clazz);
        auto result = Op<typename CPPTypeCast<typename std::decay<T>::type>::JNIType>::GetField(instance, fid);
        return android::lexical_cast<typename std::decay<T>::type, decltype(result)>(result);
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
    T GetStaticField(jclass clazz, const std::string & fieldName, std::string sig = "")
    {
        if (std::is_same<T, jobject>::value && sig.empty())
        {
            ANDROID_LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
            return T();
        }
        JNIEnv* jniEnv = AndroidJNI::GetJNIEnv();
        std::string signature = sig;
        if (signature.empty())
            signature = CombineFixedChars<typename CPPTypeCast<typename std::decay<T>::type>::Signature, FixedChars < '\0' > > ::Result::value();
        else
            std::replace(signature.begin(), signature.end(), '.', '/');
        jfieldID fid = jniEnv->GetStaticFieldID(clazz, fieldName.c_str(), signature.c_str());
        auto result = Op<typename CPPTypeCast<typename std::decay<T>::type>::JNIType>::GetStaticField(clazz, fid);
        return android::lexical_cast<typename std::decay<T>::type, decltype(result)>(result);
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
    T GetStaticField(const std::string & className, const std::string & fieldName, std::string sig = "")
    {
        jclass clazz = AndroidJNI::FindJavaClass(className.c_str());
        return GetStaticField<T>(clazz, fieldName, sig);
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
            return T();
        }
        JNIEnv* jniEnv = AndroidJNI::GetJNIEnv();
        jclass clazz = jniEnv->GetObjectClass(instance);
        std::string signature = sig;
        if (signature.empty())
        {
            signature = CombineFixedChars<typename CPPTypeCast<typename std::decay<T>::type>::Signature, FixedChars < '\0' > > ::Result::value();
        }
        jfieldID fid = jniEnv->GetFieldID(clazz, fieldName.c_str(), signature.c_str());
        jniEnv->DeleteLocalRef(clazz);
        Op<typename CPPTypeCast<typename std::decay<T>::type>::JNIType>::SetField(instance, fid, CPPTypeCast<typename std::decay<T>::type>::Converter::convert(fieldValue));
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
    void SetStaticField(jclass clazz, const std::string& fieldName, T fieldValue, std::string sig = "")
    {
        if (std::is_same<T, jobject>::value && sig.empty())
        {
            ANDROID_LOGD("Failed to get field value, return type is jobject, must be set the field signature.");
            return T();
        }
        JNIEnv* jniEnv = AndroidJNI::GetJNIEnv();
        std::string signature = sig;
        if (signature.empty())
        {
            signature = CombineFixedChars<typename CPPTypeCast<typename std::decay<T>::type>::Signature, FixedChars < '\0' > > ::Result::value();
        }
        jfieldID fid = jniEnv->GetStaticFieldID(clazz, fieldName.c_str(), signature.c_str());
        Op<typename CPPTypeCast<typename std::decay<T>::type>::JNIType>::SetStaticField(clazz, fid, CPPTypeCast<typename std::decay<T>::type>::Converter::convert(fieldValue));
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
    void SetStaticField(const std::string & className, const std::string& fieldName, T fieldValue, std::string sig = "")
    {
        jclass clazz = AndroidJNI::FindJavaClass(className.c_str());
        SetStaticField<T>(clazz, fieldName, fieldValue, sig);
    }
} //namespace android

NS_FK_END
#endif //#if PLATFORM_ANDROID

#endif // END OF FOUNDATIONKIT_ANDROIDJAVACORE_HPP
