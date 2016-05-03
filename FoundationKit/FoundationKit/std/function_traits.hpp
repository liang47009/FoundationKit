/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#pragma once
#include <tuple>
#include <functional>

namespace std{

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

//Ordinary functions
//Function pointer
//function/lambda
//Member functions
//Callable object

template<typename T>
struct function_traits;

//Ordinary functions
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
    //using function_type = Ret(Args...); //C++ 14 support
    //using signature = Ret(Args...); //C++ 14 support
    typedef  Ret function_type(Args...);
    typedef  Ret signature(Args...);
    typedef  Ret(*pointer)(Args...);
    using stl_function_type = std::function<function_type>;
    using args_tuple  = std::tuple < Args... > ;
    using return_type = Ret;
    using arity       = std::integral_constant < unsigned, sizeof...(Args) > ;

    template<size_t I>
    struct args
    {
        static_assert(I < arity::value, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, args_tuple>::type;
    };



};

//Function pointer
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

//Callable object
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

} // namespace std


