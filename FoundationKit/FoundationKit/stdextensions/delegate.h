/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef FoundationKit_Delegate_H
#define FoundationKit_Delegate_H
#pragma once

#include "function_traits.hpp"
#include <vector>
#include "stddefines.hpp"
#include "utility.hpp"

_STD_BEGIN

template<typename _Fty>
class Delegate
{
    typedef std::function_traits<_Fty>                 CallbackTraits;
    typedef typename CallbackTraits::pointer           CallablePointer;
    typedef typename CallbackTraits::stl_function_type CallableType;
public:

    template<typename... Args>
    void operator()(Args... args)
    {
        for (auto& callable : _callables)
        {
            callable(std::forward<Args>(args)...);
        }
    }

    // for lambda
    Delegate& operator += (_Fty callable)
    {

        auto fun_ptr = std::to_function_pointer<_Fty>(callable);
        bool isExist = (std::find(_callables.begin(), _callables.end(), fun_ptr) != _callables.end());
        if (!isExist)
        {
            _callables.push_back(fun_ptr);
        }
        return *this;
    }

    // for lambda
    Delegate& operator -= (_Fty callable)
    {
        auto fun_ptr = std::to_function_pointer<_Fty>(callable);
        auto funIter = std::find(_callables.begin(), _callables.end(), fun_ptr);
        if (funIter != _callables.end())
        {
            _callables.erase(funIter);
        }
        return *this;
    }

private:
    std::vector<CallablePointer> _callables;

};

_STD_END

#endif //FoundationKit_Delegate_H

