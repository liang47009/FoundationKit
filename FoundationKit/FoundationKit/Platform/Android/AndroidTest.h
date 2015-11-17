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

             LOGD("===== ajo5 ref count:%d", ajo5._rep->getReferenceCount());


             AndroidJavaObject  ajo6 = ajo5;
             AndroidJavaObject  ajo7 = ajo6;
             AndroidJavaObject  ajo8 = ajo7;
             AndroidJavaObject  ajo9 = ajo8;
             AndroidJavaObject  ajo10 = ajo9;

             LOGD("===== javaString ref count:%d", javaString->_rep->getReferenceCount());
             LOGD("===== ajo10 ref count:%d", ajo10._rep->getReferenceCount());
             LOGD("==== AndroidJavaObject::raw jobject 000: %p", ajo10.getRawObject());
             delete javaString;
             LOGD("===== ajo10 ref count:%d", ajo10._rep->getReferenceCount());

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

            LOGD("===== ajo12 ref count:%d", ajo12._rep->getReferenceCount());

            last = std::move(ajo12);
        }

            vecbuf = last.call< std::vector<unsigned char> >("getBytes");
            buf = &(vecbuf.front());
            LOGD("===== buf 222 :%s", buf);

            LOGD("===== last ref count:%d", last._rep->getReferenceCount());
    }
} //namespace Andorid

NS_FK_END









