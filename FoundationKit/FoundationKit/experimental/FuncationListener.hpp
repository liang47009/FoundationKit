
#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/ApplyTuple.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include <type_traits>
NS_FK_BEGIN
typedef ValueList ArgumentList;

template<int N>
struct BuildTuple
{
    template<typename Tuple>
    static void Build(Tuple& tp, ArgumentList& args)
    {
        std::get<N - 1>(tp) = args[N - 1].As<std::decay<decltype(std::get<N - 1>(tp))>::type>();
        BuildTuple<N - 1>::Build(tp, args);
    }
};

template<>
struct BuildTuple<0>
{
    template<typename Tuple>
    static void Build(Tuple& tp, ArgumentList& args)
    {
        std::get<0>(tp) = args[0].As<std::decay<decltype(std::get<0>(tp))>::type>();
    }
};

template<typename Tuple>
inline void ApplyBuildTuple(Tuple& tp, const ArgumentList& args)
{
    BuildTuple<std::tuple_size<Tuple>::value>::Build(tp, const_cast<ArgumentList&>(args));
}


class FuncationListenerBase
{
public:
    FuncationListenerBase(){}
    virtual~FuncationListenerBase(){}
    virtual void Invoke(const ArgumentList& args) = 0;
};

template<typename _Ft>
class FuncationListener : public FuncationListenerBase
{
    using function_traits_t = std::function_traits < _Ft >;
    using function_type = typename function_traits_t::stl_function_type;
    using args_tuple_type = typename function_traits_t::args_tuple_type;

public:
    function_type   Invoker;
    args_tuple_type ArgsTuple;

    FuncationListener(_Ft fun)
    {
        Invoker = fun;
    }

    FuncationListener(const function_type& fun)
    {
        Invoker = fun;
    }

    virtual void Invoke(const ArgumentList& args)
    {
        ApplyBuildTuple(ArgsTuple, args);
        applyTuple(Invoker, ArgsTuple);
    }
};

NS_FK_END

