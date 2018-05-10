/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCTION_CACHE_HPP
#define FOUNDATIONKIT_FUNCTION_CACHE_HPP
#include "FoundationKit/GenericPlatformMacros.hpp"
#include <map>
#include <functional>
#include <utility>
NS_FK_BEGIN
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
*         return (n < 2) ? n : function_cache(hasCache)(n - 1) + function_cache(hasCache)(n - 2);
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

namespace detail 
{
    // function_cache helper method.
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

}
template <typename R, typename...  Args>  
std::function<R(Args...)> function_cache(R(*func)(Args...), bool needClear = false)
{  
    using fun_type = std::function<R(Args...)>;  
    static std::map<decltype(func), fun_type> functor_map;

    if (needClear)  
        return functor_map[func] = detail::function_cache(func);

    if (functor_map.find(func) == functor_map.end())  
        functor_map[func] = detail::function_cache(func);

    return functor_map[func];  
} 

NS_FK_END


#endif // FOUNDATIONKIT_FUNCTION_CACHE_HPP
