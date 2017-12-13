
#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include <jni.h>
#include "FoundationKit/Foundation/NotificationCenter.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "AndroidJNI/AndroidJNI.hpp"
#include "AndroidJavaBridge.hpp"

using namespace FoundationKit;
using namespace FoundationKit::Android;
extern "C"
{
    /** 
    jboolean      z   bool
    jbyte         b   byte
    jchar         c   char
    jshort        s   short
    jint          i   int
    jlong         j   long
    jfloat        f   float
    jdouble       d   double
    jobject       l   jobject
    jbooleanArray Z   std::vector<bool>
    jbyteArray    B   std::vector<byte>
    jcharArray    C   std::vector<char>
    jshortArray   S   std::vector<short>
    jintArray     I   std::vector<int>
    jlongArray    J   std::vector<long>
    jfloatArray   F   std::vector<float>
    jdoubleArray  D   std::vector<double>
    jobjectArray  L   std::vector<jobject>
    jstring       T   std::string
    */
    JNIEXPORT void JNICALL Java_com_losemymind_foundationkit_AndroidJavaBridge_nativeInvoke(JNIEnv* env, 
        jobject obj, jstring funName, jstring argSig, jobjectArray args)
    {
        std::string strFunName = AndroidJNI::jstring2string(funName);
        std::string strArgSig  = AndroidJNI::jstring2string(argSig);
        size_t count = strArgSig.size();
        ANDROID_LOGD("======= nativeInvoke Method strFunName:%s", strFunName.c_str());
        ANDROID_LOGD("======= nativeInvoke Method strArgSig:%s", strArgSig.c_str());
        ANDROID_LOGD("======= nativeInvoke Method argcount:%d", count);
        std::vector<AndroidJavaObject>  arguments;
        jsize arrLen = env->GetArrayLength(args);
        for(int i = 0; i < arrLen; ++i)
        {
            arguments.emplace_back(env->GetObjectArrayElement(args, i));
        }
        //FoundationKit::DelegateManager::GetInstance()->InvokeDelegate(strFunName, &arguments);
        ArgumentList arglist;
        size_t argIndex = 0;
        while (argIndex < count)
        {
            char argType = strArgSig[argIndex];
            switch (argType)
            {
            case 'z': 
                arglist.emplace_back(arguments[argIndex].Call<bool>("booleanValue"));
                break;    
            case 'b': 
                arglist.emplace_back(arguments[argIndex].Call<char>("byteValue"));
                break;
            case 'c': 
                arglist.emplace_back(arguments[argIndex].Call<char>("charValue"));
                break;
            case 's':
                arglist.emplace_back(arguments[argIndex].Call<short>("shortValue"));
                break;
            case 'i':
                arglist.emplace_back(arguments[argIndex].Call<int>("intValue"));
                break;
            case 'j':
                arglist.emplace_back((int64)arguments[argIndex].Call<long>("longValue"));
                break;
            case 'f':
                arglist.emplace_back(arguments[argIndex].Call<float>("floatValue"));
                break;
            case 'd':
                arglist.emplace_back(arguments[argIndex].Call<double>("doubleValue"));
                break;
            case 'l':
                arglist.emplace_back(arguments[argIndex].GetRawObject());
                break;
            case 'Z':
                break;
            case 'B':
                break;
            case 'C':
                break;
            case 'S':
                break;
            case 'I':
                break;
            case 'J':
                break;
            case 'F':
                break;
            case 'D':
                break;
            case 'L':
                break;
            case 'T':
            {
                jstring jstr = static_cast<jstring>(arguments[argIndex].GetRawObject());
                std::string str = AndroidJNI::jstring2string(jstr);
                arglist.emplace_back(str);
            }
                break;
            default:
                ANDROID_LOGE("argument signature [%c] is error", argType);
                break;
            }
            ++argIndex;
        }
        FoundationKit::NotificationCenter::GetInstance()->Invoke(strFunName, arglist);
    }
}//extern "C"

#endif //#if PLATFORM_ANDROID

