/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_APPLY_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_APPLY_HPP

#include <tuple>
#include <utility>
#include <type_traits>
namespace std
{
    namespace detail 
    {
        template <class F, class Tuple, std::size_t... I>
        constexpr decltype(auto) apply_impl(F &&f, Tuple &&t, std::index_sequence<I...>)
        {
            //return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
            return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
        }
    }  // namespace detail

    template <class F, class Tuple>
    constexpr decltype(auto) apply(F &&f, Tuple &&t)
    {
        return detail::apply_impl(
            std::forward<F>(f), std::forward<Tuple>(t),
            std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
    }
}

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_APPLY_HPP
