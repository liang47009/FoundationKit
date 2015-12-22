#include <jni.h>
#include "FoundationKit/Foundation/FunctionCenter.hpp"
#include "AndroidJavaObject.h"
#include "AndroidJavaBridge.h"

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
        std::string strFunName = AndroidJNIHelper::getInstance()->jstring2string(funName);
        std::string strArgSig  = AndroidJNIHelper::getInstance()->jstring2string(argSig);
        size_t count = strArgSig.size();
        LOGD("======= nativeInvoke Method strFunName:%s", strFunName.c_str());
        LOGD("======= nativeInvoke Method strArgSig:%s", strArgSig.c_str());
        LOGD("======= nativeInvoke Method argcount:%d", count);
        std::vector<AndroidJavaObject>  arguments;
        jsize arrLen = env->GetArrayLength(args);
        for(size_t i = 0; i < arrLen; ++i)
        {
            arguments.emplace_back(env->GetObjectArrayElement(args, i));
        }
        FoundationKit::FunctionCenter::getInstance()->invoke(strFunName, arguments);


        /**
        size_t argIndex = 0;
        while (argIndex < count)
        {
            char argType = strArgSig[argIndex];
            switch (argType)
            {
            case 'z':
                bool val = arguments[argIndex].call<bool>("booleanValue");  
                break;    
            case 'b': 
                unsigned char val = arguments[argIndex].call<unsigned char>("byteValue");  
                break;
            case 'c': 
                char val = arguments[argIndex].call<char>("charValue");  
                break;
            case 's':
                short val = arguments[argIndex].call<short>("shortValue");
                break;
            case 'i':
                int val = arguments[argIndex].call<int>("intValue");
                break;
            case 'j':
                long val = arguments[argIndex].call<long>("longValue");
                break;
            case 'f':
                float val = arguments[argIndex].call<float>("floatValue");
                break;
            case 'd':
                double val = arguments[argIndex].call<double>("doubleValue");
                break;
            case 'l':
                jobject val = arguments[argIndex].getRawObject();
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
                break;
            default:
                LOGE("argument signature [%c] is error", argType);
                break;
            }
            ++argIndex;
        }
        */
    }
}//extern "C"
