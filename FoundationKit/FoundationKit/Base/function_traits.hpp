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
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN

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
*       FKLog("======= TestFunctionTraits =======");
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

// TEMPLATE CLASS function_traits_impl
template <typename T>
struct function_traits_impl;

// TEMPLATE CLASS function_traits
template<typename _Ft>
struct function_traits : function_traits_impl <std::remove_cv_t<std::remove_reference_t<_Ft>>>
{
};

template<typename _Rt, typename... Args>
struct function_traits_impl<_Rt(Args...)>
{
public:
    typedef  _Rt function_meta_type(Args...);
    typedef  _Rt(*pointer)(Args...);
    using function_type = std::function < function_meta_type > ;
    using argument_type = std::tuple < Args... > ;
    using argument_meta_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args> >... > ;
    using return_type = _Rt;
    using arity = std::integral_constant < unsigned, sizeof...(Args) > ;
    //static constexpr std::size_t arity = std::tuple_size<argument_type>::value;
    template<size_t I>
    struct argument
    {
        static_assert(I < arity::value, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, argument_type>::type;
    };
};

// function pointer
template<typename _Rt, typename... Args>
struct function_traits_impl<_Rt(*)(Args...)> : function_traits<_Rt(Args...)>{};

// std::function
template <typename _Rt, typename... Args>
struct function_traits_impl<std::function<_Rt(Args...)>> : function_traits_impl<_Rt(Args...)>{};

//member function
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits_impl<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits_impl<ReturnType(Args...)>{};

// pointer of non-static member function
FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)
#undef FUNCTION_TRAITS

// functor
template<typename _CallableType>
struct function_traits_impl : function_traits_impl<decltype(&_CallableType::operator())> {};

template <typename _Ft>
typename function_traits<_Ft>::function_type to_function(const _Ft& lambda)
{
    return static_cast<typename function_traits<_Ft>::function_type>(lambda);
}

template <typename _Ft>
typename function_traits<_Ft>::function_type to_function(_Ft&& lambda)
{
    return static_cast<typename function_traits<_Ft>::function_type>(std::forward<_Ft>(lambda));
}

template <typename _Ft>
typename function_traits<_Ft>::pointer to_function_pointer(const _Ft& lambda)
{
    return static_cast<typename function_traits<_Ft>::pointer>(lambda);
}

NS_FK_END

#endif // FOUNDATIONKIT_FUNCTION_TRAITS_HPP


