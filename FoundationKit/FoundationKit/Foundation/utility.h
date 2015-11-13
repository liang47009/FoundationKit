/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/

#pragma once
#include "FoundationKit/FoundationKitDefines.h"
#include <utility>

NS_FK_BEGIN

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
};\


/**
 * Get max integer
 * @code
 *   int ret = MaxIntegerOf<10, 1, 20, 30,100, 20, 11>::value;
 * @endcode
 * the ret is 100
 */
template<size_t N, size_t... R>
struct MaxIntegerOf;

template<size_t N>
struct MaxIntegerOf<N> : std::integral_constant < size_t, N > {};

template<size_t N0, size_t N1, size_t... R>
struct MaxIntegerOf<N0, N1, R...> :std::integral_constant < size_t,
    N0 >= N1 ?
    MaxIntegerOf<N0, R...>::value :
    MaxIntegerOf<N1, R...>::value > {};

/** 
 * Get memory aligned size
 * @code
 *   size_t len = MaxAligned<int, std::string, double, char[4], long long, long long int>::value;
 * @endcode
 * len is 8
 */
template<typename... Args>
struct MaxAligned : std::integral_constant < int, MaxIntegerOf<std::alignment_of<Args>::value...>::value > {};


/**
 * Whether it contains a type
 * @code
 *   bool contain = ContainsOfType<int, std::string, int, double, float, char*, char>::value;
 *   bool notcontain = ContainsOfType<int, std::string, double, float, char*, char>::value;
 * @endcode
 * contain is true
 * notcontain is false
 */
template<typename T, typename... LIST>
struct ContainsOfType;

template<typename T, typename H, typename... R>
struct ContainsOfType<T, H, R...> : std::conditional < std::is_same<T, H>::value,
    std::true_type, ContainsOfType < T, R... > > ::type{};

template<typename T>
struct ContainsOfType<T> : std::false_type{};

NS_FK_END


