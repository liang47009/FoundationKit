/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <tuple>
#include <functional>
#include "stddefines.hpp"

_STD_BEGIN

/** 
 * 
 *
 * @code
 *   #include <iostream> // for test
 *   #include <string>
 *
 *   template<typename T>
 *   void printType()
 *   {
 *       std::cout << typeid(T).name() << std::endl;
 *   }
 *
 *   float(*castfunc)(std::string, int);
 *   float free_function(const std::string&a, int b)
 *   {
 *       return (float)a.size() / b;
 *   }
 *
 *   struct AA
 *   {
 *       int f(int a, int b) volatile{ return a + b; }
 *       int operator()(int)const { return 0; }
 *   };
 *
 *   void TestFunctionTraits()
 *   {
 *       LOG_INFO("======= TestFunctionTraits =======");
 *       std::function<int(int)> fn = [](int a){ return a; };
 *       printType<function_traits<std::function<int(int)>>::signature>();
 *       printType<function_traits<std::function<int(int)>>::arg<0>::type>();
 *       printType<function_traits<decltype(fn)>::signature>();
 *       printType<function_traits<decltype(free_function)>::signature>();
 *       printType<function_traits<decltype(castfunc)>::signature>();
 *       printType<function_traits<AA>::signature>();
 *       using T = decltype(&AA::f);
 *       printType<T>();
 *       printType<function_traits<decltype(&AA::f)>::signature>();
 *   }
 * @endcode

 */

//普通函数
//函数指针
//function/lambda
//成员函数
//函数对象

//转换为std::function和函数指针 
template<typename T>
struct function_traits;

//普通函数
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
public:
    enum { arity = sizeof...(Args) };
    typedef Ret function_type(Args...);
    typedef Ret return_type;
    using stl_function_type = std::function<function_type>;
    typedef Ret(*pointer)(Args...);

    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
    };
};

//函数指针
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)>{};

//std::function
template <typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)>{};

//member function
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{};\

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)

//函数对象
template<typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())>{};

template <typename Function>
typename function_traits<Function>::stl_function_type to_function(const Function& lambda)
{
    return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
}

template <typename Function>
typename function_traits<Function>::stl_function_type to_function(Function&& lambda)
{
    return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
}

template <typename Function>
typename function_traits<Function>::pointer to_function_pointer(const Function& lambda)
{
    return static_cast<typename function_traits<Function>::pointer>(lambda);
}
_STD_END


