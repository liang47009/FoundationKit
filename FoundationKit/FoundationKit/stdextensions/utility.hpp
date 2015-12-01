/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
#ifndef FoundationKit_utility_H
#define FoundationKit_utility_H

#pragma once
#include "FoundationKit/FoundationKitDefines.h"
#include <utility>
#include "stddefines.hpp"

_STD_BEGIN

/** 
 * check class has a member
 * @code
 *   struct AppDelegate
 *   {
 *      void applicationDidFinishLaunching(){}
 *   }
 *
 *   struct Scheduler
 *   {
 *      void update(float dt){}
 *   }
 *
 *   HAS_MEMBER(applicationDidFinishLaunching);
 *   HAS_MEMBER(update);
 *   bool bHave0 = has_member_applicationDidFinishLaunching<AppDelegate>::value;
 *   bool bHave1 = has_member_update<Scheduler,std::string>::value;
 *   bool bHave2 = has_member_update<Scheduler,float>::value;
 * @endcode
 *   bHave0 = true
 *   bHave1 = false
 *   bHave2 = true
 */
#define HAS_MEMBER(member)\
template<typename T, typename... Args>\
struct has_member_##member\
{\
private:\
		template<typename U> static auto check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type()); \
	template<typename U> static std::false_type check(...);\
public:\
	enum{value = std::is_same<decltype(check<T>(0)), std::true_type>::value};\
};

namespace detail {
    
    /**  this can detect inherited member functions!
     *   usage:
     *   bool b1 = has_function<TestCheckFun>::value;
     *   bool b1 = has_function<TestCheckFun, int>::value;
     */
    template<typename T, typename... Args>
    struct has_function
    {
        template<typename C, typename = decltype(std::declval<C>().destroyInstance(std::declval<Args>()...))>
        static std::true_type test(int);
        template<typename C> static std::false_type test(...);
        const static bool value = decltype(test<T>(0))::value;
    };
}//namespace detail



/**
 * Get max integer
 * @code
 *   int ret = max_integer_of<10, 1, 20, 30,100, 20, 11>::value;
 * @endcode
 * the ret is 100
 */
template<size_t N, size_t... R>
struct max_integer_of;

template<size_t N>
struct max_integer_of<N> : std::integral_constant < size_t, N >{};

template<size_t N0, size_t N1, size_t... R>
struct max_integer_of<N0, N1, R...> :std::integral_constant < size_t,
    N0 >= N1 ?
    max_integer_of<N0, R...>::value :
    max_integer_of<N1, R...>::value >{};

/** 
 * Get memory aligned size
 * @code
 *   size_t len = max_aligned<int, std::string, double, char[4], long long, long long int>::value;
 * @endcode
 * len is 8
 */
template<typename... Args>
struct max_aligned : std::integral_constant < int, max_integer_of<std::alignment_of<Args>::value...>::value > {};


/**
 * Whether it contains a type
 * @code
 *   bool contain = contains_of_type<int, std::string, int, double, float, char*, char>::value;
 *   bool notcontain = contains_of_type<int, std::string, double, float, char*, char>::value;
 * @endcode
 * contain is true
 * notcontain is false
 */
template<typename T, typename... LIST>
struct contains_of_type;

template<typename T, typename H, typename... R>
struct contains_of_type<T, H, R...> : std::conditional < std::is_same<T, H>::value,
    std::true_type, contains_of_type < T, R... > > ::type{};

template<typename T>
struct contains_of_type<T> : std::false_type{};


template <class _Ty>
struct always_false
{
    static const bool value = false;
};

_STD_END


#endif // FoundationKit_utility_H