/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_MAX_INTEGER_OF_HPP
#define FOUNDATIONKIT_MAX_INTEGER_OF_HPP

#include <utility>
#include "FoundationKit/GenericPlatformMacros.hpp"
NS_FK_BEGIN
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

NS_FK_END
#endif // FOUNDATIONKIT_MAX_INTEGER_OF_HPP
