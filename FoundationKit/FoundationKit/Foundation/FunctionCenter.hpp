/****************************************************************************
Copyright (c) 2015 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef FoundationKit_FunctionCenter_H
#define FoundationKit_FunctionCenter_H

#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Foundation/Singleton.h"
#include "FoundationKit/Foundation/Logger.h"
#include "FoundationKit/std/function_traits.hpp"

NS_FK_BEGIN

class InvokeBase
{
public:
	InvokeBase(){}
	virtual~InvokeBase(){}
};

template<typename STL_FUNC>
class InvokeHolder : public InvokeBase
{
public:
	InvokeHolder(STL_FUNC & value)
		: _fun(value)
	{
	}

	InvokeHolder(STL_FUNC&& value)
		: _fun(std::forward<STL_FUNC>(value))
	{
	}

	STL_FUNC _fun;
};

/**
 *  @code
	 FunctionCenter::getInstance()->declare("Test1", [&](int arg)
	 {
	 int tmp = arg;
	 tmp += 1;
	 });
	 FunctionCenter::getInstance()->declare("Test2", [&](int arg)
	 {
	 int tmp = arg;
	 tmp += 1;
	 });
	 FunctionCenter::getInstance()->declare("Test3", Test3);
	 std::function<void()>  background = std::bind(&AppDelegate::applicationDidEnterBackground, this);
	 FunctionCenter::getInstance()->declare("Test4", background);
	 FunctionCenter::getInstance()->invoke("Test1", 100);
	 FunctionCenter::getInstance()->invoke("Test2", 1000);
	 int valTest3 = FunctionCenter::getInstance()->invoke<int>("Test3", 10000);
	 FunctionCenter::getInstance()->invoke("Test4");
	 FunctionCenter::getInstance()->remove("Test1", [&](int arg){});
	 FunctionCenter::getInstance()->remove("Test2", [&](int arg){});
	 FunctionCenter::getInstance()->remove("Test3", Test3);
	 FunctionCenter::getInstance()->remove("Test4", background);
	@endcode
 */
class FunctionCenter:public Singleton<FunctionCenter>
{
	friend Singleton < FunctionCenter > ;
	FunctionCenter()
	{
	}
public:
	typedef std::lock_guard<std::mutex>   GuardLock;
	
	~FunctionCenter()
	{
		for (auto& iter : _methodMap)
		{
			delete iter.second;
		}
	}

	template<typename Function>
	bool declare(std::string methodName, Function func)
	{
		GuardLock guard(_methodLock);
		auto iter = _methodMap.find(methodName);
		if (iter == _methodMap.end())
		{
			using stl_fun_type = typename std::function_traits<Function>::stl_function_type;
			auto stl_fun = std::to_function<Function>(func);
			InvokeBase* pInvoke = new InvokeHolder<stl_fun_type>(func);
			_methodMap.insert(std::make_pair(methodName, pInvoke));
			return true;
		}
		LOG_INFO("Method is exist of name %s", methodName.c_str());
		return false;
	}

	template<typename Function>
	void remove(std::string methodName, Function func)
	{
		GuardLock guard(_methodLock);
		auto iter = _methodMap.find(methodName);
		if (iter != _methodMap.end())
		{
			using stl_fun_type = typename std::function_traits<Function>::stl_function_type;
			InvokeBase* invoker = dynamic_cast<InvokeHolder<stl_fun_type>*>(iter->second);
			if (invoker)
			{
				delete invoker;
				_methodMap.erase(iter->first);
			}
			else
			{
				LOG_INFO("Function sig error %s", methodName.c_str());
			}
		}
	}

	template<typename R = void, typename... Args>
	R invoke(std::string methodName, Args... args)
	{
		_methodLock.lock();
		InvokeBase* invokerBase = nullptr;
		auto iter = _methodMap.find(methodName);
		if (iter == _methodMap.end())
		{
			LOG_INFO("Function is not exist of name %s", methodName.c_str());
			_methodLock.unlock();
			return R();
			
		}
		invokerBase = iter->second;
		_methodLock.unlock();
		auto invoker = dynamic_cast<InvokeHolder<std::function<R(Args...)>>*>(invokerBase);
		if (invoker)
			return invoker->_fun(std::forward<Args>(args)...);
		else
			LOG_INFO("Function sig error %s", methodName.c_str());
		return R();
	}

private:
	std::unordered_map<std::string, InvokeBase*>   _methodMap;
	std::mutex                                     _methodLock;
};

NS_FK_END

#endif // FoundationKit_FunctionCenter_H


