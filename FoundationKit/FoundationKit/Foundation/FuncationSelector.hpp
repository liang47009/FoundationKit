/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCATIONLISTENER_HPP
#define FOUNDATIONKIT_FUNCATIONLISTENER_HPP


#include "FoundationKit/Foundation/Value.hpp"
#include "FoundationKit/Foundation/ApplyTuple.hpp"
#include "FoundationKit/std/function_traits.hpp"
#include <type_traits>
#include <memory>
#include <functional>
NS_FK_BEGIN
typedef ValueList ArgumentList;

template<size_t N>
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

class SelectorBase
{
public:
    SelectorBase(){}
    virtual~SelectorBase(){}
    virtual void Invoke(const ArgumentList& args) = 0;
};

template<typename _Ft>
class SelectorN : public SelectorBase
{
    using function_traits_t = std::function_traits < _Ft >;
    using stl_function_type = typename function_traits_t::stl_function_type;
    using args_tuple_type   = typename function_traits_t::args_tuple_type;

public:
    stl_function_type   Invoker;
    args_tuple_type     ArgsTuple;

    SelectorN(const stl_function_type& fun)
    {
        Invoker = fun;
    }

    virtual void Invoke(const ArgumentList& args)
    {
        ApplyBuildTuple(ArgsTuple, args);
        applyTuple(Invoker, ArgsTuple);
    }
};

template<typename _Ft>
class Selector0 : public SelectorBase
{
    using function_traits_t = std::function_traits < _Ft >;
    using stl_function_type = typename function_traits_t::stl_function_type;
public:
    stl_function_type   Invoker;

    Selector0(const stl_function_type& fun)
    {
        Invoker = fun;
    }

    virtual void Invoke(const ArgumentList& /*args*/)
    {
        Invoker();
    }
};


namespace SelectorDetail
{
    template<std::size_t... indices>
    struct IndexList
    {
        typedef IndexList<indices...> Type;
    };

    template<std::size_t count, std::size_t... indices>
    struct PlaceHolderIndexList : public PlaceHolderIndexList<count - 1, count - 1, indices...> {};

    // ends recursion - the complete (one from base) class, inheriting
    // directly from IndexList
    template<std::size_t... indices>
    struct PlaceHolderIndexList<0, indices...> : public IndexList<indices...>{};
#if TARGET_PLATFORM == PLATFORM_WINDOWS
    template<size_t _Index>
    inline std::_Ph<_Index + 1> GetPlaceHolder()
    {
        return std::_Ph<_Index + 1>();
    }
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    template<size_t _Index>
    inline std::_Placeholder<_Index + 1> GetPlaceHolder()
    {
        return std::_Placeholder<_Index + 1>();
    }
#endif
    /*
    template<size_t _Index>
    inline decltype(std::placeholders::_1) GetPlaceHolder(){ return std::placeholders::_1; }
    inline decltype(std::placeholders::_2) GetPlaceHolder<0>(){ return std::placeholders::_2; }
    template<3>
    inline decltype(std::placeholders::_3) GetPlaceHolder(){ return std::placeholders::_3; }
    template<4>
    inline decltype(std::placeholders::_4) GetPlaceHolder(){ return std::placeholders::_4; }
    template<5>
    inline decltype(std::placeholders::_5) GetPlaceHolder(){ return std::placeholders::_5; }
    template<6>
    inline decltype(std::placeholders::_6) GetPlaceHolder(){ return std::placeholders::_6; }
    template<7>
    inline decltype(std::placeholders::_7) GetPlaceHolder(){ return std::placeholders::_7; }
    template<8>
    inline decltype(std::placeholders::_8) GetPlaceHolder(){ return std::placeholders::_8; }
    template<9>
    inline decltype(std::placeholders::_9) GetPlaceHolder(){ return std::placeholders::_9; }
    template<10>
    inline decltype(std::placeholders::_10) GetPlaceHolder(){ return std::placeholders::_10; }
    template<11>
    inline decltype(std::placeholders::_11) GetPlaceHolder(){ return std::placeholders::_11; }
    template<12>
    inline decltype(std::placeholders::_12) GetPlaceHolder(){ return std::placeholders::_12; }
    template<13>
    inline decltype(std::placeholders::_13) GetPlaceHolder(){ return std::placeholders::_13; }
    template<14>
    inline decltype(std::placeholders::_14) GetPlaceHolder(){ return std::placeholders::_14; }
    template<15>
    inline decltype(std::placeholders::_15) GetPlaceHolder(){ return std::placeholders::_15; }
    template<16>
    inline decltype(std::placeholders::_16) GetPlaceHolder(){ return std::placeholders::_16; }
    template<17>
    inline decltype(std::placeholders::_17) GetPlaceHolder(){ return std::placeholders::_17; }
    template<18>
    inline decltype(std::placeholders::_18) GetPlaceHolder(){ return std::placeholders::_18; }
    template<19>
    inline decltype(std::placeholders::_19) GetPlaceHolder(){ return std::placeholders::_19; }
    template<20>
    inline decltype(std::placeholders::_20) GetPlaceHolder(){ return std::placeholders::_20; }
    */
}

template<typename _Ft, typename _Ty, std::size_t... indices >
SelectorN<_Ft> MakeSelectorImpl(_Ft fun, _Ty object, SelectorDetail::IndexList<indices...>)
{
    return SelectorN<_Ft>(std::bind(fun, object, SelectorDetail::GetPlaceHolder<indices>()...));
}

template<typename _Ft, std::size_t... indices >
SelectorN<_Ft> MakeSelectorImpl(_Ft fun, SelectorDetail::IndexList<indices...>)
{
    return SelectorN<_Ft>(std::bind(fun, SelectorDetail::GetPlaceHolder<indices>()...));
}

template<typename _Ft, typename _Ty>
SelectorN<_Ft> MakeSelectorN(_Ft fun, _Ty object)
{
    using function_traits_t = std::function_traits < _Ft >;
    typedef typename SelectorDetail::PlaceHolderIndexList<function_traits_t::arity::value>::Type List;
    return MakeSelectorImpl(fun, object, List());
}

template<typename _Ft>
SelectorN<_Ft> MakeSelectorN(_Ft fun)
{
    using function_traits_t = std::function_traits < _Ft >;
    typedef typename SelectorDetail::PlaceHolderIndexList<function_traits_t::arity::value>::Type List;
    return MakeSelectorImpl(fun, List());
}

template<typename _Ft, typename _Ty>
Selector0<_Ft> MakeSelector0(_Ft fun, _Ty object)
{
    return Selector0<_Ft>(std::bind(fun,object));
}

template<typename _Ft>
Selector0<_Ft> MakeSelector0(_Ft fun)
{
    return Selector0<_Ft>(std::bind(fun));
}


typedef std::shared_ptr<SelectorBase>   SelectorPointer;

template<typename _Ft, typename _Ty, std::size_t... indices >
SelectorPointer CreateSelectorImpl(_Ft fun, _Ty object, SelectorDetail::IndexList<indices...>)
{
    std::shared_ptr<SelectorN<_Ft> > pSelector(new SelectorN<_Ft>(std::bind(fun, object, SelectorDetail::GetPlaceHolder<indices>()...)));
    return pSelector;
}

template<typename _Ft, std::size_t... indices >
SelectorPointer CreateSelectorImpl(_Ft fun, SelectorDetail::IndexList<indices...>)
{
    std::shared_ptr<SelectorN<_Ft> > pSelector(new SelectorN<_Ft>(std::bind(fun, SelectorDetail::GetPlaceHolder<indices>()...)));
    return pSelector;
}

template<typename _Ft, typename _Ty>
SelectorPointer CreateSelectorN(_Ft fun, _Ty object)
{
    using function_traits_t = std::function_traits < _Ft >;
    typedef typename SelectorDetail::PlaceHolderIndexList<function_traits_t::arity::value>::Type List;
    return CreateSelectorImpl(fun, object, List());
}

template<typename _Ft>
SelectorPointer CreateSelectorN(_Ft fun)
{
    using function_traits_t = std::function_traits < _Ft >;
    typedef typename SelectorDetail::PlaceHolderIndexList<function_traits_t::arity::value>::Type List;
    return CreateSelectorImpl(fun, List());
}

template<typename _Ft, typename _Ty>
SelectorPointer CreateSelector0(_Ft fun, _Ty object)
{
    std::shared_ptr<SelectorN<_Ft> > pSelector(new Selector0<_Ft>(std::bind(fun,object)));
    return pSelector;
}

template<typename _Ft>
SelectorPointer CreateSelector0(_Ft fun)
{
    std::shared_ptr<SelectorN<_Ft> > pSelector(new Selector0<_Ft>(std::bind(fun)));
    return pSelector;
}


NS_FK_END

#endif // FOUNDATIONKIT_FUNCATIONLISTENER_HPP


