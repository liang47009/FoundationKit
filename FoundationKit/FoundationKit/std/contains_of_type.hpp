/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_CONTAINS_OF_TYPE_H
#define LOSEMYMIND_CONTAINS_OF_TYPE_H

#include <utility>
namespace std
{
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
struct contains_of_type<T, H, R...> 
    : std::conditional < std::is_same<T, H>::value, std::true_type, contains_of_type < T, R... > > ::type{};

} //namespace std

#endif // LOSEMYMIND_CONTAINS_OF_TYPE_H











