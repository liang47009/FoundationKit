/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCTIONBOX_HPP
#define FOUNDATIONKIT_FUNCTIONBOX_HPP

#include <type_traits>
#include <memory>
#include <utility>
#include <functional>
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Base/function_traits.hpp"
#include "FoundationKit/Base/apply.hpp"

NS_FK_BEGIN

template<size_t N>
struct BuildTuple
{
	template<typename Tuple>
	static void Build(Tuple& tp, ValueList& args)
	{
		std::get<N - 1>(tp) = args[N - 1].As<typename std::decay<decltype(std::get<N - 1>(tp))>::type>();
		BuildTuple<N - 1>::Build(tp, args);
	}
};

template<>
struct BuildTuple<0>
{
	template<typename Tuple>
	static void Build(Tuple& tp, ValueList& args)
	{
		std::get<0>(tp) = args[0].As<typename std::decay<decltype(std::get<0>(tp))>::type>();
	}
};

template<typename Tuple>
inline void ApplyBuildTuple(Tuple& tp, const ValueList& args)
{
	BuildTuple<std::tuple_size<Tuple>::value>::Build(tp, const_cast<ValueList&>(args));
}

template<>
inline void ApplyBuildTuple(std::tuple<ValueList>& tp, const ValueList& args)
{
	std::get<0>(tp) = args;
}

class FunctionBoxBase
{
public:
    FunctionBoxBase(){}
	virtual~FunctionBoxBase(){}
	virtual Value Invoke(const ValueList& args) = 0;
};
typedef std::shared_ptr<FunctionBoxBase>  FunctionBoxPointer;

template<typename _Ft, size_t ArgsNum>
class FunctionBox : public FunctionBoxBase
{
	using function_traits_t  = function_traits < _Ft >;
	using function_type      = typename function_traits_t::function_type;
	using argument_meta_type = typename function_traits_t::argument_meta_type;

public:
    function_type       Func;
    argument_meta_type  ArgsTuple;

    FunctionBox(const function_type& fun)
	{
        Func = fun;
	}

	virtual Value Invoke(const ValueList& args)
	{
        using is_void = std::is_same<typename function_traits_t::return_type, void>;
        return InvokeSwitch<typename function_traits_t::return_type>(is_void{}, args);
	}
protected:
    template <typename _Ty, typename = typename std::enable_if< !std::is_same<_Ty, void>::value, _Ty>::type >
    Value InvokeSwitch(std::false_type, const ValueList& args)
    {
        ApplyBuildTuple(ArgsTuple, args);
        return apply(Func, ArgsTuple);
    }

    template <typename _Ty, typename = typename std::enable_if< std::is_same<_Ty, void>::value, _Ty>::type >
    Value InvokeSwitch(std::true_type, const ValueList& args)
    {
        ApplyBuildTuple(ArgsTuple, args);
        apply(Func, ArgsTuple);
        return Value();
    }
};

template<typename _Ft>
class FunctionBox<_Ft, 0> : public FunctionBoxBase
{
	using function_traits_t = function_traits < _Ft >;
	using function_type     = typename function_traits_t::function_type;
public:
    function_type   Func;

    FunctionBox(const function_type& fun)
	{
        Func = fun;
	}

	virtual Value Invoke(const ValueList& /*args*/)
	{
        using is_void = std::is_same<typename function_traits_t::return_type, void>;
        return InvokeSwitch<typename function_traits_t::return_type >(is_void{});
	}
protected:
    template <typename _Ty, typename = typename std::enable_if< !std::is_same<_Ty, void>::value, _Ty>::type >
    Value InvokeSwitch(std::false_type)
    {
        return Func();
    }

    template <typename _Ty, typename = typename std::enable_if< std::is_same<_Ty, void>::value, _Ty>::type >
    Value InvokeSwitch(std::true_type)
    {
        Func();
        return Value();
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
    FunctionBoxPointer BindFunctionBoxImpl(_Ft fun, _Ty* object, std::index_sequence<index...>)
    {
        const size_t arityvalue = function_traits < _Ft >::arity::value;
        std::shared_ptr<FunctionBox<_Ft, arityvalue> > pSelector(new FunctionBox<_Ft, arityvalue >(std::bind(fun, object, PlaceHolderDetail::PlaceHolderMaker<index>::Get()...)));
        return pSelector;
    }

    template<typename _Ft, std::size_t... index >
    FunctionBoxPointer BindFunctionBoxImpl(_Ft fun, std::index_sequence<index...>)
    {
        const size_t arityvalue = function_traits < _Ft >::arity::value;
        std::shared_ptr<FunctionBox<_Ft, arityvalue> > pSelector(new FunctionBox<_Ft, arityvalue>(std::bind(fun, PlaceHolderDetail::PlaceHolderMaker<index>::Get()...)));
        return pSelector;
    }


    template <typename T>
    void BuildArgumentList(ValueList& al, const T &t)
    {
        al.emplace_back(t);
    }


    template <typename T, typename...Args>
    void BuildArgumentList(ValueList& al, const T &t, const Args&... args)
    {
        al.emplace_back(t);
        BuildArgumentList(al, args...);
    }
} // namespace detail


template<typename _Ft, typename _Ty>
FunctionBoxPointer BindFunctionBox(_Ft fun, _Ty* object)
{
    return detail::BindFunctionBoxImpl(fun, object, std::make_index_sequence<function_traits < _Ft >::arity::value>{});
}

template<typename _Ft>
FunctionBoxPointer BindFunctionBox(_Ft fun)
{
    return detail::BindFunctionBoxImpl(fun, std::make_index_sequence<function_traits < _Ft >::arity::value>{});
}

template<typename... Args>
void InvokeFunctionBox(FunctionBoxPointer Handler, Args... args)
{
    ValueList al;
    detail::BuildArgumentList(al, std::forward<Args>(args)...);
    Handler->Invoke(al);
}

NS_FK_END

#endif // END OF FOUNDATIONKIT_FUNCTIONBOX_HPP
