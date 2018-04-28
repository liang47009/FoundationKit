#include "AndroidJNIBridge.hpp"
#if PLATFORM_ANDROID

#include <vector>
#include <string>
#include "AndroidJavaProxy.hpp"
#include "FoundationKit/Foundation/NotificationCenter.hpp"

USING_NS_FK;

extern "C"
{
    JNIEXPORT jobject JNICALL Java_com_losemymind_foundationkit_AndroidJNIBridge_InvokeProxy(JNIEnv* env, jobject thiz, jlong ptr, jclass clazz, jobject method, jobjectArray args)
    {
        jmethodID methodID = env->FromReflectedMethod(method);
        android::AndroidJavaProxy* proxy = (android::AndroidJavaProxy*)ptr;
        return proxy->__OnInvoke(clazz, methodID, args);
    }

    JNIEXPORT void JNICALL Java_com_losemymind_foundationkit_AndroidJNIBridge_DeleteProxy(JNIEnv* env, jobject thiz, jlong ptr)
    {
        delete (android::AndroidJavaProxy*)ptr;
    }

    JNIEXPORT void JNICALL Java_com_losemymind_foundationkit_AndroidJNIBridge_NativeSendMeeage(JNIEnv* env,
        jobject obj, jstring InMethodName, jstring InSignature, jobjectArray InArgs)
    {
        std::string InvokeMethodName      = AndroidJNI::jstring2string(InMethodName);
        std::string InvokeMethodSignature = AndroidJNI::jstring2string(InSignature);
        //ANDROID_LOGE("======= nativeInvoke InvokeMethodName:%s", InvokeMethodName.c_str());
        //ANDROID_LOGE("======= nativeInvoke InvokeMethodSignature:%s", InvokeMethodSignature.c_str());
        size_t ArgsCount = env->GetArrayLength(InArgs);
        ArgumentList Arglist;
        for (size_t argIndex = 0; argIndex < ArgsCount; ++argIndex)
        {
            char argType = InvokeMethodSignature[argIndex];
            switch (argType)
            {
            case 'Z':
                Arglist.emplace_back(android::lexical_cast<bool>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'B':
                Arglist.emplace_back(android::lexical_cast<char>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'C':
                Arglist.emplace_back(android::lexical_cast<wchar_t>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'S':
                Arglist.emplace_back(android::lexical_cast<short>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'I':
                Arglist.emplace_back(android::lexical_cast<int>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'J':
                Arglist.emplace_back(android::lexical_cast<int64>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'F':
                Arglist.emplace_back(android::lexical_cast<float>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'D':
                Arglist.emplace_back(android::lexical_cast<double>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'T':
                Arglist.emplace_back(android::lexical_cast<std::string>(env->GetObjectArrayElement(InArgs, argIndex)));
                break;
            case 'O':
                Arglist.emplace_back(android::lexical_cast<jobject>(env->GetObjectArrayElement(InArgs, argIndex)));
            default:
                ANDROID_LOGE("argument signature [%c] is error", argType);
                break;
            }
        }
        FoundationKit::NotificationCenter::DefaultCenter.Invoke(InvokeMethodName, Arglist);

    }
}//extern "C"


#endif //#if PLATFORM_ANDROID

