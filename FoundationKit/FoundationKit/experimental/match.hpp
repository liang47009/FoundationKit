
#pragma once
#include "FoundationKit/std/function_traits.hpp"

#if (__cplusplus && (__cplusplus > 201103L)) || (_MSC_VER && _MSC_VER > 1800)

// forward declarations
template<class T, class Case, class ...Cases>
decltype(auto) match(T&& value, const Case& _case, const Cases&... cases);
template<class T, class Case>
decltype(auto) match(T&& value, const Case& _case);

namespace details {
    template<class T, class Case, class ...OtherCases>
    decltype(auto) match_call(const Case& _case, T&& value, std::true_type, const OtherCases&... other)
    {
        return _case(std::forward<T>(value));
    }

    template<class T, class Case, class ...OtherCases>
    decltype(auto) match_call(const Case& _case, T&& value, std::false_type, const OtherCases&... other)
    {
        return match(std::forward<T>(value), other...);
    }
}

template<class T, class Case, class ...Cases>
decltype(auto) match(T&& value, const Case& _case, const Cases&... cases)
{
    //using args = typename function_traits<Case>::args_tuple;
    //using arg = std::tuple_element_t<0, args>;
    using arg = std::function_traits<Case>::args<0>::type;
    using match = std::is_same < std::decay_t<arg>, std::decay_t<T> > ;
    return details::match_call(_case, std::forward<T>(value), match{}, cases...);
}


// the last one is default
template<class T, class Case>
decltype(auto) match(T&& value, const Case& _case)
{
    return _case(std::forward<T>(value));
}

#endif // #if (__cplusplus && (__cplusplus > 201103L))


#if ENABLE_TEST_MATCH

template<class T>
decltype(auto) matchTestInternal(T&& value) 
{

    auto fun0 = [](std::string value)
    {
        std::cout << "This is string "; return value + " Hi!";
    };

    auto fun1 = [](int i)
    {
        std::cout << "This is int ";	 return i * 100;
    };

    auto fun2 = [](const char* v)
    {
        std::cout << "This is const char* ";	 return "NoN";
    };


    auto defaultCase = [](auto a)
    {
        std::cout << "This is default "; return 0;
    };

    return match(value, fun0, fun1, fun2, defaultCase);
}


void matchTest()
{
    std::cout << matchTestInternal(200) << std::endl;
    std::cout << matchTestInternal("RR") << std::endl;          // because const char*
    std::cout << matchTestInternal(std::string{ "ARR" }) << std::endl;
    std::cout << matchTestInternal(100.f) << std::endl;
}


#endif

