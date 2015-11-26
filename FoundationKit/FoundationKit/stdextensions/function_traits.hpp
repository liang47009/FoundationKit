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

template < typename T>
struct function_traits;

template < typename Ret, typename... Args>
struct function_traits < Ret(Args...) >
{
    using return_type = Ret;
    using args_tuple = std::tuple < Args... > ;
    //using signature = Ret(Args...);
    typedef Ret signature(Args...);
    enum{ arity = sizeof...(Args) };

    template < std::size_t N>
    struct arg
    {
        static_assert(N < arity, "no such parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};

// function pointer
template < typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : public function_traits < Ret(Args...) >{};

// std::function
template <  typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)> > : public function_traits < Ret(Args...) >{};

//member function pointer
template < typename T, typename Ret, typename... Args>
struct function_traits<Ret(T::*)(Args...)> : public function_traits < Ret(Args...) >{};

//const member function pointer
template < typename T, typename Ret, typename... Args>
struct function_traits<Ret(T::*)(Args...) const> : public function_traits < Ret(Args...) >{};

//volatile member function pointer
template < typename T, typename Ret, typename... Args>
struct function_traits<Ret(T::*)(Args...) volatile> : public function_traits < Ret(Args...) > {};

//const volatile member function pointer
template < typename T, typename Ret, typename... Args>
struct function_traits<Ret(T::*)(Args...) const volatile> : public function_traits < Ret(Args...) > {};

// member object pointer
template < typename T, typename Ret>
struct function_traits<Ret(T::*)> : public function_traits < Ret(T&) >{};

//callable object
template < typename T>
struct function_traits : public function_traits < decltype(&T::operator()) >{};

//
template < typename T>
struct function_traits<T&> : public function_traits < typename std::remove_reference<T>::type >{};

_STD_END


