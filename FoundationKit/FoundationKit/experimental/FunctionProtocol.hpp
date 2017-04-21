/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCTIONPROTOCOL_HPP
#define FOUNDATIONKIT_FUNCTIONPROTOCOL_HPP
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/experimental/ArgumentUnpacker.hpp"
#include "FoundationKit/Foundation/ApplyTuple.hpp"
#include "FoundationKit/std/function_traits.hpp"
NS_FK_BEGIN

class FunctionProtocolBase
{
public:
    FunctionProtocolBase(){}
    virtual~FunctionProtocolBase(){}
    virtual void invoke(const ArgumentPacker& args_packer) = 0;
};


template<typename _Ft>
class FunctionProtocol : public FunctionProtocolBase
{
    using function_traits_t = std::function_traits < _Ft >;
    using function_type = typename function_traits_t::stl_function_type;
    using args_tuple_type = typename function_traits_t::args_tuple_type;

public:
    function_type _caller;
    args_tuple_type _args;

    FunctionProtocol(_Ft fun)
    {
        _caller = fun;
    }

    virtual void invoke(const ArgumentPacker& args_packer)
    {
        args_packer.unpack(_args);
        applyTuple(_caller, _args);
    }
};
NS_FK_END
#endif // FOUNDATIONKIT_FUNCTIONPROTOCOL_HPP