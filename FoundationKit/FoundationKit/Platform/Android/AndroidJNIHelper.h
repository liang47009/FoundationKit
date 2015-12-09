#pragma once

#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include <android/asset_manager.h>
#include "FoundationKit/Foundation/Singleton.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidJNIHelper", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidJNIHelper", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"AndroidJNIHelper", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,"AndroidJNIHelper", __VA_ARGS__))

NS_FK_BEGIN

namespace Android
{

    typedef struct JniMethodInfo_
    {
        JNIEnv *    env;
        jclass      classID;
        jmethodID   methodID;
    } JniMethodInfo;

    /**
     * Helper interface for JNI interaction.
     * @note Initialization AndroidJNIHelper,
     *       On JNI_OnLoad call AndroidJNIHelper::setJavaVM to set java virtual machine.
     *       On android main activity OnCreate method call AndroidJNIHelper::setClassLoaderActivity
     *       to get java class loader.
     */
    class AndroidJNIHelper : public Singleton < AndroidJNIHelper >
    {
    public:
        friend class Singleton < AndroidJNIHelper > ;

        /** Set JavaVM object, called this method on JNI_OnLoad
         *  @param[in]  javaVM Java virtual machine.
         */
        void setJavaVM(JavaVM *javaVM);

        /** Get Java virtual machine.
        *  @return  javaVM object.
        */
        JavaVM* getJavaVM();

        /** Set JavaVM object, called this method when android main activity onCreate method.
         *  @param[in]  activityInstance Android activity object.
         *  @return     if get class loader successful, return true, else return false.
         */
        bool setClassLoaderActivity(jobject activityInstance);

        /** Get JNIEnv.
         *  @return  JNIEnv object.
         */
        JNIEnv* getEnv();

        /** Get java class id.
         *  @param[in]  className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
         *  @return  jclass object.
         */
        jclass getClassID(const char *className);

        /** Convert jstring to std::string
         *  @param[in]  jstr jstring object
         *  @return  std::string object.
         */
        std::string jstring2string(jstring jstr);

        jstring string2jstring(std::string str);

        /**
         *  Get the method info for a static method of a java class.
         *  @param [in]  methodinfo   JniMethodInfo
         *  @param [in]  className    className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
         *  @param [in]  methodName   Specifies which method to call.
         *  @param [in]  paramCode    Parameter signature(e.g. "Ljava.lang.String;")
         *  @return Successful return true else false.
         */
        bool getStaticMethodInfo(JniMethodInfo &methodinfo,
            const char *className,
            const char *methodName,
            const char *signature);

        /**
         *  Get the method info for a static method of a java class.
         *  @param [in]  methodinfo   JniMethodInfo
         *  @param [in]  classID      From JNIEnv::FindClass or AndroidJNIHelper::getClassID.
         *  @param [in]  methodName   Specifies which method to call.
         *  @param [in]  paramCode    Parameter signature(e.g. "Ljava.lang.String;")
         *  @return Successful return true else false.
         */
        bool getStaticMethodInfo(JniMethodInfo &methodinfo,
            jclass      classID,
            const char *methodName,
            const char *signature);

        /**
         *  Returns the method info(JniMethodInfo) for an instance (nonstatic) method of a java class or interface.
         *  @param [in]  methodinfo   JniMethodInfo
         *  @param [in]  className    className Specifies the Java class name (e.g. "java.lang.String" or "java/lang/String")
         *  @param [in]  methodName   Specifies which method to call.
         *  @param [in]  paramCode    Parameter signature(e.g. "Ljava.lang.String;")
         *  @return Successful return true else false.
         */
        bool getMethodInfo(JniMethodInfo &methodinfo,
            const char *className,
            const char *methodName,
            const char *signature);

        /**
         *  Returns the method info(JniMethodInfo) for an instance (nonstatic) method of a java class or interface.
         *  @param [in]  methodinfo   JniMethodInfo
         *  @param [in]  classID      From JNIEnv::FindClass or AndroidJNIHelper::getClassID.
         *  @param [in]  methodName   Specifies which method to call.
         *  @param [in]  paramCode    Parameter signature(e.g. "Ljava.lang.String;")
         *  @return Successful return true else false.
         */
        bool getMethodInfo(JniMethodInfo &methodinfo,
            jclass      classID,
            const char *methodName,
            const char *signature);

        void checkException();

        AAssetManager* getAAssetManager();
        jobject        getActivityInstance();

        AndroidJNIHelper();
        ~AndroidJNIHelper();
    protected:
        JNIEnv* cacheJNIEnv(JavaVM* jvm);
        bool    getDefaultClassLoader(JniMethodInfo &methodinfo,
            const char *className,
            const char *methodName,
            const char *paramCode);
    private:
        JavaVM*   _psJavaVM;
        jmethodID _loadMethod;
        jobject   _classloader;
        jobject   _activityInstance;
    };

} //namespace Android

NS_FK_END

#define JNIHELPER FoundationKit::AndroidJNIHelper::getInstance() 






