/****************************************************************************
 Copyright (c) 2015 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/
#ifndef LOSEMYMIND_UTILITY_H
#define LOSEMYMIND_UTILITY_H


#pragma once
#include <map>
#include <memory>
#include <utility>
#include <unordered_map>
#include "FoundationKit/Base/Types.h"
USING_NS_FK;

/*-----------------------------------------------------------------------------
Standard templates.
-----------------------------------------------------------------------------*/
namespace std{

/**
 * stl is not implement disable_if, so we implement it and add to std namespace.
 * written more simply as
 */
template < bool _Test, class _Ty = void > using disable_if = std::enable_if < !_Test, _Ty > ;

/** 
 * this can detect inherited member functions!
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
 *   bool bHave0 = has_member_applicationDidFinishLaunching<AppDelegate>::value; // true
 *   bool bHave1 = has_member_update<Scheduler,std::string>::value;   // false
 *   bool bHave2 = has_member_update<Scheduler,float>::value;  // true

 *   template<typename T,typename... Args>
 *    typename std::enable_if<has_member_update<T,decltype(Args)...>::value>::type testFun()
 *     {
 *          std::cout<<"has update function" << std::endl;
 *     } 
 *
 *    template<typename T,typename... Args>
 *    typename std::enable_if<!has_member_update<T,decltype(Args)...>::value>::type testFun()
 *     {
 *          std::cout<<"has no update function" << std::endl;
 *     } 
 *     testFun<Scheduler, int>();   // output: has no update function 
 *     testFun<Scheduler, float>(); // has update function
 * @endcode
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

/**
 * @code
 * struct MyStruct
 * {
 *     int a;
 *     double b;
 * };
 * memory_aligned<MyStruct>::type pMyStruct;
 * new(&pMyStruct)MyStruct();
 * MyStruct *pRawMyStruct = reinterpret_cast<MyStruct*>(&pMyStruct);
 * pRawMyStruct-> a = pRawMyStruct->b *10;
 * pRawMyStruct->~MyStruct();
 * @endcode
 */
template<typename T>
struct memory_aligned
{
    using type = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;
};

/**
 * Aligns a value to the nearest higher multiple of 'Alignment', which must be a power of two.
 *
 * @param Ptr			Value to align
 * @param Alignment		Alignment, must be a power of two
 * @return				Aligned value
 */
template <typename T>
inline constexpr T align(const T Ptr, int32_t Alignment)
{
    return (T)(((PTRINT)Ptr + Alignment - 1) & ~(Alignment - 1));
}
/**
 * Checks if a pointer is aligned to the specified alignment.
 * @param Ptr - The pointer to check.
 * @return true if the pointer is aligned, false otherwise.
 */
static FORCEINLINE bool isAligned(const volatile void* Ptr, const uint32_t Alignment)
{
    return !(UPTRINT(Ptr) & (Alignment - 1));
}

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

template<typename T>
struct contains_of_type<T> : std::false_type{};

template<typename T, typename H, typename... R>
struct contains_of_type<T, H, R...> : std::conditional < std::is_same<T, H>::value,
    std::true_type, contains_of_type < T, R... > > ::type{};

// static_assert(always_false<T>::value, "Not allowed");
template <class _Ty>
struct always_false : std::false_type{};

// ============= function cache implement ==================
// sugar helper method.
template <typename R, typename... Args>  
std::function<R(Args...)> function_cache(R(*func) (Args...))  
{  
    auto result_map = std::make_shared<std::map<std::tuple<Args...>, R>>();  
      
    return ([=](Args... args)
    {  
        std::tuple<Args...> t(args...);  
        if (result_map->find(t) == result_map->end())  
            (*result_map)[t] = func(args...);  
        return (*result_map)[t];  
    });  
} 

/**
 * sugar the function and cache.
 * @code
 *     size_t noCache(size_t n)
 *     {
 *        return (n < 2) ? n : noCache(n - 1) + noCache(n - 2);
 *     }
 *
 *     size_t hasCache(size_t n)
 *     {
 *         return (n < 2) ? n : sugar(hasCache)(n - 1) + sugar(hasCache)(n - 2);
 *     }
 *
 *     void testFunctionCache()
 *     {
 *         Timer t1;
 *         auto v1 = noCache(45);
 *         LOGD("=========== noCache value:%d run time:%lld", v1, t1.elapsed_seconds());
 *         Timer t2;
 *         auto v2 = hasCache(45);
 *         LOGD("=========== hasCache value:%d run time:%lld", v2, t2.elapsed_seconds());
 *     }
 * @endcode
 */
template <typename R, typename...  Args>  
std::function<R(Args...)> sugar(R(*func)(Args...), bool needClear = false)  
{  
    using function_type = std::function<R(Args...)>;  
    static std::unordered_map<decltype(func), function_type> functor_map;  

    if (needClear)  
        return functor_map[func] = function_cache(func);  

    if (functor_map.find(func) == functor_map.end())  
        functor_map[func] = function_cache(func);  

    return functor_map[func];  
} 

// ============= function cache implement end ==================

} // namespace std
#endif // LOSEMYMIND_UTILITY_H







