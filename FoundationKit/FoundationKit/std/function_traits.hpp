/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCTION_TRAITS_HPP
#define FOUNDATIONKIT_FUNCTION_TRAITS_HPP

#pragma once
#include <tuple>
#include <functional>
#include <type_traits>

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
 *       printType<function_traits<std::function<int(int)>>::function_type>();
 *       printType<function_traits<std::function<int(int)>>::args<0>::type>();
 *       printType<function_traits<decltype(fn)>::function_type>();
 *       printType<function_traits<decltype(free_function)>::function_type>();
 *       printType<function_traits<decltype(castfunc)>::function_type>();
 *       printType<function_traits<AA>::function_type>();
 *       using T = decltype(&AA::f);
 *       printType<T>();
 *       printType<function_traits<decltype(&AA::f)>::function_type>();
 *   }
 * @endcode
 */

/*
* 1. function type							==>	Ret(Args...)
* 2. function pointer						==>	Ret(*)(Args...)
* 3. function reference						==>	Ret(&)(Args...)
* 4. pointer to non-static member function	==> Ret(T::*)(Args...)
* 5. function object and functor		    ==> &T::operator()
* 6. function with generic operator call	==> template <typeanme ... Args> &T::operator()
*/
template <typename T>
struct function_traits_impl;

template<typename T>
    struct function_traits : function_traits_impl<typename std::remove_cv<typename std::remove_reference<T>::type>::type>
{
};

template<typename Ret, typename... Args>
struct function_traits_impl<Ret(Args...)>
{
public:
    typedef  Ret function_type(Args...);
    typedef  Ret(*pointer)(Args...);
    using stl_function_type = std::function < function_type > ;
    using args_raw_tuple_type = std::tuple < Args... > ;
    using args_tuple_type = std::tuple < typename std::remove_cv<typename std::remove_reference<Args>::type >::type... > ;
    using return_type = Ret;
    using arity = std::integral_constant < unsigned, sizeof...(Args) > ;

    template<size_t I>
    struct args
    {
        static_assert(I < arity::value, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, args_raw_tuple_type>::type;
    };
};

// function pointer
template<typename Ret, typename... Args>
struct function_traits_impl<Ret(*)(Args...)> : function_traits<Ret(Args...)>{};

// std::function
template <typename Ret, typename... Args>
struct function_traits_impl<std::function<Ret(Args...)>> : function_traits_impl<Ret(Args...)>{};

//member function
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits_impl<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits_impl<ReturnType(Args...)>{};

// pointer of non-static member function
FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)

// functor
template<typename Callable>
struct function_traits_impl : function_traits_impl<decltype(&Callable::operator())> {};

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

#endif // FOUNDATIONKIT_FUNCTION_TRAITS_HPP


