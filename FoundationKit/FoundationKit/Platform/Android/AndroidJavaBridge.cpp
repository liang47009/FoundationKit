
#include "AndroidJavaBridge.h"
NS_FK_BEGIN

namespace Android
{

AndroidJavaBridge::AndroidJavaBridge()
{
}

AndroidJavaBridge::~AndroidJavaBridge()
{
}

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
        jobject obj, jstring funName, jstring argSig, ...)
    {
        std::string strFunName = AndroidJNIHelper::getInstance()->jstring2string(funName);
        std::string strArgSig  = AndroidJNIHelper::getInstance()->jstring2string(argSig);
        size_t count = strArgSig.size();

        LOGD("======== strFunName:%s", strFunName.c_str());
        LOGD("======== strArgSig:%s", strArgSig.c_str());
        LOGD("======== count:%d", count);


        size_t argIndex = 0;
        va_list args;
        va_start(args, count);
        while (argIndex < count)
        {
            char argType = strArgSig[argIndex];
            switch (argType)
            {
            case 'z':  
                break;    
            case 'b':  
                break;
            case 'c': 
                break;
            case 's':
                break;
            case 'i':
                break;
            case 'j':
                break;
            case 'f':
                break;
            case 'd':
                break;
            case 'l':
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
        va_end(args);
    }
}


} //namespace Android

NS_FK_END