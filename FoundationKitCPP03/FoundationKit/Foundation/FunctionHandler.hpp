/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCATIONLISTENER_HPP
#define FOUNDATIONKIT_FUNCATIONLISTENER_HPP


#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/std/stdheader.h"
#include <memory>
#include <functional>
NS_FK_BEGIN
typedef ValueList ArgumentList;
typedef std::function<void(const ArgumentList&)>   FunctionHandlerPointer;


template<typename _Ft, typename _Ty>
FunctionHandlerPointer BindFunctionHandler(_Ft fun, _Ty object)
{
    return std::function<void(const ArgumentList&)>(object, fun);
}

template<typename _Ft>
FunctionHandlerPointer BindFunctionHandler(_Ft fun)
{
    return std::function<void(const ArgumentList&)>(fun);
}

NS_FK_END

#endif // FOUNDATIONKIT_FUNCATIONLISTENER_HPP


