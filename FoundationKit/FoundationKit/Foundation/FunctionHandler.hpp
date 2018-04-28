/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCATIONLISTENER_HPP
#define FOUNDATIONKIT_FUNCATIONLISTENER_HPP

#include <type_traits>
#include <memory>
#include <utility>
#include <functional>
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Base/function_traits.hpp"
#include "FoundationKit/Base/apply.hpp"

NS_FK_BEGIN
typedef ValueList ArgumentList;

template<size_t N>
struct BuildTuple
{
	template<typename Tuple>
	static void Build(Tuple& tp, ArgumentList& args)
	{
		std::get<N - 1>(tp) = args[N - 1].As<typename std::decay<decltype(std::get<N - 1>(tp))>::type>();
		BuildTuple<N - 1>::Build(tp, args);
	}
};

template<>
struct BuildTuple<0>
{
	template<typename Tuple>
	static void Build(Tuple& tp, ArgumentList& args)
	{
		std::get<0>(tp) = args[0].As<typename std::decay<decltype(std::get<0>(tp))>::type>();
	}
};

template<typename Tuple>
inline void ApplyBuildTuple(Tuple& tp, const ArgumentList& args)
{
	BuildTuple<std::tuple_size<Tuple>::value>::Build(tp, const_cast<ArgumentList&>(args));
}

template<>
inline void ApplyBuildTuple(std::tuple<ArgumentList>& tp, const ArgumentList& args)
{
	std::get<0>(tp) = args;
}

class FunctionHandlerBase
{
public:
	FunctionHandlerBase(){}
	virtual~FunctionHandlerBase(){}
	virtual void Invoke(const ArgumentList& args) = 0;
};
typedef std::shared_ptr<FunctionHandlerBase>   FunctionHandlerPointer;

template<typename _Ft, size_t ArgsNum>
class FunctionHandler : public FunctionHandlerBase
{
	using function_traits_t = function_traits < _Ft >;
	using stl_function_type = typename function_traits_t::stl_function_type;
	using args_tuple_type   = typename function_traits_t::args_tuple_type;

public:
	stl_function_type   Func;
	args_tuple_type     ArgsTuple;

	FunctionHandler(const stl_function_type& fun)
	{
        Func = fun;
	}

	virtual void Invoke(const ArgumentList& args)
	{
		ApplyBuildTuple(ArgsTuple, args);
		apply(Func, ArgsTuple);
	}
};

template<typename _Ft>
class FunctionHandler<_Ft, 0> : public FunctionHandlerBase
{
	using function_traits_t = function_traits < _Ft >;
	using stl_function_type = typename function_traits_t::stl_function_type;
public:
	stl_function_type   Func;

	FunctionHandler(const stl_function_type& fun)
	{
        Func = fun;
	}

	virtual void Invoke(const ArgumentList& /*args*/)
	{
        Func();
	}
};

namespace PlaceHolderDetail
{
	template<size_t _Index>
	struct PlaceHolderMaker{};
	template<>
	struct PlaceHolderMaker<0>{ static decltype(std::placeholders::_1) Get(){ return std::placeholders::_1; } };
	template<>
	struct PlaceHolderMaker<1>{ static decltype(std::placeholders::_2) Get(){ return std::placeholders::_2; } };
	template<>
	struct PlaceHolderMaker<2>{ static decltype(std::placeholders::_3) Get(){ return std::placeholders::_3; } };
	template<>
	struct PlaceHolderMaker<3>{ static decltype(std::placeholders::_4) Get(){ return std::placeholders::_4; } };
	template<>
	struct PlaceHolderMaker<4>{ static decltype(std::placeholders::_5) Get(){ return std::placeholders::_5; } };
	template<>
	struct PlaceHolderMaker<5>{ static decltype(std::placeholders::_6) Get(){ return std::placeholders::_6; } };
	template<>
	struct PlaceHolderMaker<6>{ static decltype(std::placeholders::_7) Get(){ return std::placeholders::_7; } };
	template<>
	struct PlaceHolderMaker<7>{ static decltype(std::placeholders::_8) Get(){ return std::placeholders::_8; } };
	template<>
	struct PlaceHolderMaker<8>{ static decltype(std::placeholders::_9) Get(){ return std::placeholders::_9; } };
	template<>
	struct PlaceHolderMaker<9>{ static decltype(std::placeholders::_10) Get(){ return std::placeholders::_10; } };
}


namespace detail
{
    template<typename _Ft, typename _Ty, std::size_t... index >
    FunctionHandlerPointer BindFunctionHandlerImpl(_Ft fun, _Ty* object, std::index_sequence<index...>)
    {
        const size_t arityvalue = function_traits < _Ft >::arity::value;
        std::shared_ptr<FunctionHandler<_Ft, arityvalue> > pSelector(new FunctionHandler<_Ft, arityvalue >(std::bind(fun, object, PlaceHolderDetail::PlaceHolderMaker<index>::Get()...)));
        return pSelector;
    }

    template<typename _Ft, std::size_t... index >
    FunctionHandlerPointer BindFunctionHandlerImpl(_Ft fun, std::index_sequence<index...>)
    {
        const size_t arityvalue = function_traits < _Ft >::arity::value;
        std::shared_ptr<FunctionHandler<_Ft, arityvalue> > pSelector(new FunctionHandler<_Ft, arityvalue>(std::bind(fun, PlaceHolderDetail::PlaceHolderMaker<index>::Get()...)));
        return pSelector;
    }


    template <typename T>
    void BuildArgumentList(ArgumentList& al, const T &t)
    {
        al.emplace_back(t);
    }


    template <typename T, typename...Args>
    void BuildArgumentList(ArgumentList& al, const T &t, const Args&... args)
    {
        al.emplace_back(t);
        BuildArgumentList(al, args...);
    }
} // namespace detail


template<typename _Ft, typename _Ty>
FunctionHandlerPointer BindFunctionHandler(_Ft fun, _Ty* object)
{
    const size_t arityvalue = function_traits < _Ft >::arity::value;
    return detail::BindFunctionHandlerImpl(fun, object, std::make_index_sequence<arityvalue>{});
    // or
    //typedef typename std::make_index_sequence<arityvalue> Indices;
    //return detail::BindFunctionHandlerImpl(fun, object, Indices());
}

template<typename _Ft>
FunctionHandlerPointer BindFunctionHandler(_Ft fun)
{
    const size_t arityvalue = function_traits < _Ft >::arity::value;
    return detail::BindFunctionHandlerImpl(fun, std::make_index_sequence<arityvalue>{});
    // or
    //typedef typename std::make_index_sequence<arityvalue> Indices;
    //return detail::BindFunctionHandlerImpl(fun, Indices());
}

template<typename... Args>
void InvokeFunctionHandler(FunctionHandlerPointer Handler, Args... args)
{
    ArgumentList al;
    detail::BuildArgumentList(al, std::forward<Args>(args)...);
    Handler->Invoke(al);
}


NS_FK_END

#endif // FOUNDATIONKIT_FUNCATIONLISTENER_HPP


