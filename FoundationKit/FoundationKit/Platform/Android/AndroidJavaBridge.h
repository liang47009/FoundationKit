#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <cstdarg>
#include <vector>
#include "FoundationKit/Foundation/Singleton.h"
#include "AndroidJNIHelper.h"
#include "AndroidJavaObject.h"

NS_FK_BEGIN

/**
 * 调用函数时的参数类型必须和注册时的完全一样，否则将调用不到，
 * 不存在隐式转换。
 * @code
 *    bool testInvokeHolder000(int a, int b, float f, std::string str){return true;}
 *    InvokeBase* pInvoke = new InvokeHolder<decltype(testInvokeHolder000)>(testInvokeHolder000);
 *    bool value = pInvoke->invoke<bool>(10, 6, 10.f, sig);
 *    delete pInvoke;
 *
 *    void testInvokeHolder111(int a, int b, float f){}
 *    InvokeBase* pInvoke1 = new InvokeHolder<decltype(testInvokeHolder111)>(testInvokeHolder111);
 *    pInvoke1->invoke(10, 6, 10.f);
 *    delete pInvoke1;
 * @endcode
 */
class InvokeBase
{
public:
    InvokeBase(){}
    virtual~InvokeBase(){}

//    template<typename R=void, typename... Args>
//    R invoke(Args... args)
//    {
//        auto obj = dynamic_cast<InvokeHolder<R(Args...)>*>(this);
//        if (obj)
//        {
//            return obj->_fun(std::forward<Args>(args)...);
//        }
//        return R();
//    }
};

template<typename Fun>
class InvokeHolder : public InvokeBase
{
public:
    InvokeHolder(Fun & value)
        : _fun(value)
    {
    }

    InvokeHolder(Fun&& value)
        : _fun(static_cast<Fun&&>(value))
    {
    }

    std::function<Fun> _fun;
};

namespace InvokeHelper
{
    
    template<typename R=void, typename... Args>
    R invoke(InvokeBase* invoker, Args... args)
    {
        auto obj = dynamic_cast<InvokeHolder<R(Args...)>*>(invoker);
        if (obj)
        {
            return obj->_fun(std::forward<Args>(args)...);
        }
        return R();
    }
}



namespace Android
{

    typedef std::vector<AndroidJavaObject> NativeArguments;

    class AndroidJavaBridge : public Singleton < AndroidJavaBridge >
    {
        friend class Singleton < AndroidJavaBridge >;
    public:
        ~AndroidJavaBridge();

        template<typename Ft>
        bool registerMethod(std::string methodName, Ft & value)
        {
            auto iter = _methodMap.find(methodName);
            if (iter == _methodMap.end())
            {
                InvokeBase* pInvoke = new InvokeHolder<Ft>(value);
                _methodMap.insert(std::make_pair(methodName, pInvoke));
                return true;
            }
            LOGD("Method is exist of name %s", methodName.c_str());
            return false;
        	
        }
        
        template<typename R=void, typename... Args>
        R invoke(std::string methodName, Args... args)
        {
            auto iter = _methodMap.find(methodName);
            if (iter != _methodMap.end())
            {
                return InvokeHelper::invoke<R>(iter->second, std::forward<Args>(args)...);
            }
            LOGD("Method is not exist of name %s", methodName.c_str());
            return R();
        }
    protected:
        AndroidJavaBridge();
    private:
    	std::unordered_map<std::string, InvokeBase*>   _methodMap;
    };

} //namespace Android

NS_FK_END
