#pragma once

#include "AndroidJNIHelper.h"
#include "AndroidJNIConverter.h"
#include "AndroidJNICaller.h"
#include "AndroidJavaObject.h"
#include "AndroidJavaClass.h"

NS_FK_BEGIN

/**
* AndroidJavaObject is application representation of a generic instance of java.lang.Object.
* @note It can be used as type-less interface to an instance of any Java class.
* Example usage:
* @code
*    auto jo = new AndroidJavaObject("java.lang.String", "some string");
*    auto jo1 = new  AndroidJavaObject("java.lang.String");
* @endcode
*/

#define EQUAL_PCHAR(a, b) assert(strcmp(a, b)==0)
#define EQUAL_STR(a,b)    assert(a.compare(b) == 0)
#define EQUAL_VAL(a, b)   assert(a == b)

namespace Android
{

    static void testJavaString()
    {

        LOGD("======================= TEST AndroidJavaClass===============");
        AndroidJavaClass  class4;

        {
            AndroidJavaClass class0("com.example.foundationkitunittest.MainActivity");
            LOGD("======= copy Constructor====");
            AndroidJavaClass class1 = class0;
            LOGD("class0 ref count should be is 2 : %d", class0._shared_ObjPtr->getReferenceCount());
            LOGD("class1 ref count should be is 2 : %d", class1._shared_ObjPtr->getReferenceCount());

            LOGD("======= move Constructor====");
            AndroidJavaClass class3 = std::move(class0);
            LOGD("class1 ref count should be is 2 : %d", class1._shared_ObjPtr->getReferenceCount());
            LOGD("class3 ref count should be is 2 : %d", class3._shared_ObjPtr->getReferenceCount());

            class4 = class3;
        }

        LOGD("class4 ref count should be is 1 : %d", class4._shared_ObjPtr->getReferenceCount());

        LOGD("============================================================");

        //const char* strBuf = "This is test java String.";
        //size_t length      = strlen(strBuf);

       std::string strBuf = "This is test java String.";
       size_t length      = strBuf.size();



        AndroidJavaObject* javaString = new AndroidJavaObject("java/lang/String", strBuf);


        
        int len = javaString->call<int>("length");
        LOGD("===== len:%d", len);
        EQUAL_VAL(length, len);

        std::vector<unsigned char> vecbuf = javaString->call< std::vector<unsigned char> >("getBytes");
        unsigned char* buf = &(vecbuf.front());
        LOGD("===== buf:%s", buf);
        EQUAL_PCHAR(strBuf, (const char*)buf);

        
        std::string strconcat = javaString->call<std::string>("concat", strBuf);
        LOGD("===== strconcat:%s", strconcat.c_str());

        vecbuf = javaString->call< std::vector<unsigned char> >("getBytes");
        buf = &(vecbuf.front());
        LOGD("===== buf:%s", buf);
        
        std::string temp = strBuf;
        temp.append(strBuf);
        EQUAL_PCHAR(temp, (const char*)buf);
        
        AndroidJavaObject last;
         {

             AndroidJavaObject  ajo1 = *javaString;
             AndroidJavaObject  ajo2 = *javaString;
             AndroidJavaObject  ajo3 = *javaString;
             AndroidJavaObject  ajo4 = *javaString;
             AndroidJavaObject  ajo5 = *javaString;

             LOGD("===== ajo5 ref count:%d", ajo5._shared_ObjPtr->getReferenceCount());


             AndroidJavaObject  ajo6 = ajo5;
             AndroidJavaObject  ajo7 = ajo6;
             AndroidJavaObject  ajo8 = ajo7;
             AndroidJavaObject  ajo9 = ajo8;
             AndroidJavaObject  ajo10 = ajo9;

             LOGD("===== javaString ref count:%d", javaString->_shared_ObjPtr->getReferenceCount());
             LOGD("===== ajo10 ref count:%d", ajo10._shared_ObjPtr->getReferenceCount());
             LOGD("==== AndroidJavaObject::raw jobject 000: %p", ajo10.getRawObject());
             delete javaString;
             LOGD("===== ajo10 ref count:%d", ajo10._shared_ObjPtr->getReferenceCount());

             LOGD("==== AndroidJavaObject::raw jobject 111 : %p", ajo10.getRawObject());


            vecbuf = ajo10.call< std::vector<unsigned char> >("getBytes");
            buf = &(vecbuf.front());
            LOGD("===== buf 000 :%s", buf);

            AndroidJavaObject  ajo11(std::move(ajo10));
            vecbuf = ajo11.call< std::vector<unsigned char> >("getBytes");
            buf = &(vecbuf.front());
            LOGD("===== buf 111 :%s", buf);

            AndroidJavaObject  ajo12(ajo11);
            vecbuf = ajo12.call< std::vector<unsigned char> >("getBytes");
            buf = &(vecbuf.front());
            LOGD("===== buf 222 :%s", buf);

            LOGD("===== ajo12 ref count:%d", ajo12._shared_ObjPtr->getReferenceCount());

            last = std::move(ajo12);
        }

        vecbuf = last.call< std::vector<unsigned char> >("getBytes");
        buf = &(vecbuf.front());
        LOGD("===== buf 222 :%s", buf);
        LOGD("===== last ref count:%d", last._shared_ObjPtr->getReferenceCount());
        AndroidJavaClass mainActivityClass("com.example.foundationkitunittest.MainActivity");
        //jobject activityobj = mainActivityClass.getStatic<jobject>("activity", "com.example.foundationkitunittest.MainActivity");
        jobject activityobj = mainActivityClass.getStatic<jobject>("activity", "com/example/foundationkitunittest/MainActivity");
        LOGD("===== activity pointer address:%p", activityobj);
        AndroidJavaObject mainActivity(activityobj);
        mainActivity.call("debugPrint",__LINE__, __FILE__, "TEST HAHAHAHH.");
        bool isEnable = mainActivity.get<bool>("isEnable");
        int  value    = mainActivity.get<int>("value");
        LOGD("===== mainActivity isEnable:%s", isEnable?"true":"false");
        LOGD("===== mainActivity  value:%d", value);
        mainActivity.set<bool>("isEnable", false);
        mainActivity.set<int>("value", 9999);

        isEnable = mainActivity.get<bool>("isEnable");
        value    = mainActivity.get<int>("value");
        LOGD("===== mainActivity after change isEnable:%s", isEnable?"true":"false");
        LOGD("===== mainActivity after change value:%d", value);
        std::string TAG = mainActivity.getStatic<std::string>("TAG");
        LOGD("===== mainActivity  TAG:%s", TAG.c_str());

        jobject  bundle    = mainActivity.get<jobject>("instanceState", "android.os.Bundle");
        LOGD("===== mainActivity instanceState address:%p", bundle);
        bundle    = mainActivity.get<jobject>("instanceState", "android/os/Bundle");
        LOGD("===== mainActivity instanceState address:%p", bundle);


        LOGD("======================= TEST ref count ===============");

        mainActivity = mainActivity;

        LOGD("===== mainActivity ref count should be 1 :%d", mainActivity._shared_ObjPtr->getReferenceCount());

        LOGD("===== mainActivity crash by get isEnable");
        // crash
        isEnable = mainActivity.get<bool>("isEnable");

        LOGD("===== mainActivity no crash.");
        mainActivity = std::move(mainActivity);
        LOGD("===== mainActivity crash by get isEnable");
        // crash
        isEnable = mainActivity.get<bool>("isEnable");
        LOGD("===== mainActivity ref count should be 1 :%d", mainActivity._shared_ObjPtr->getReferenceCount());
        LOGD("===== mainActivity no crash -- end");




    }
} //namespace Andorid

NS_FK_END









