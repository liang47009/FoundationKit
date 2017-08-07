/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_SCOPE_GUARD_HPP
#define FOUNDATIONKIT_SCOPE_GUARD_HPP
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/noncopyable.hpp"
NS_FK_BEGIN
template<typename F>
class scope_guard : noncopyable
{
public:
    scope_guard(const F& func) : _func(func) {}
    scope_guard(scope_guard&& other) : _func(std::move(other._func)){}
    ~scope_guard() { _func(); }
    scope_guard() = delete;
    scope_guard(scope_guard const&) = delete;
    scope_guard &operator =(scope_guard const&) = delete;
protected:
	F _func;
};

template< typename F > 
scope_guard< F >make_scope_guard(const F& f)
{
    return scope_guard< F >(f);
}

template< typename Lambda > 
scope_guard< Lambda >make_scope_exit(Lambda l)
{
    return scope_guard< Lambda >(std::move(l));
}

enum class scope_exit_placeholder {};

template< typename Lambda > 
scope_guard< Lambda > operator +(scope_exit_placeholder, Lambda&& l)
{
    return make_scope_exit< Lambda >(std::forward< Lambda >(l));
}

#define SCOPE_EXIT_CONCAT(x, y) TEXT_CAT(x, y)
#define SCOPE_EXIT auto SCOPE_EXIT_CONCAT(scope_exit_, __LINE__) = scope_exit_placeholder() + [&]()

NS_FK_END
#endif // FOUNDATIONKIT_SCOPE_GUARD_HPP



