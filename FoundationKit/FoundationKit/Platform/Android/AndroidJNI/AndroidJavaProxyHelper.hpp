/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ANDROIDJAVAPROXYHELPER_HPP
#define FOUNDATIONKIT_ANDROIDJAVAPROXYHELPER_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#if PLATFORM_ANDROID

#include "FoundationKit/Base/function_traits.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJNI.hpp"
#include "FoundationKit/Platform/Android/AndroidJNI/AndroidJavaCore.hpp"
#include <jni.h>
NS_FK_BEGIN
namespace android
{

    class LazyJNIMethodBase
    {
    public:
        LazyJNIMethodBase() {}
        virtual ~LazyJNIMethodBase() {}
        virtual jmethodID GetMethodID(jclass InClass) = 0;
    };

    template<typename _Ft>
    class LazyJNIMethod : public LazyJNIMethodBase
    {
    public:
        LazyJNIMethod(const std::string& InMethodName) :MethodName(InMethodName) {}
        virtual~LazyJNIMethod() {}
        template <typename _Rt, typename Tuple, std::size_t... I>
        const char* ApplyGetMethodID(Tuple &&t, std::index_sequence<I...>)
        {
            return GetJNISignature<_Rt>(std::get<I>(std::forward<Tuple>(t))...);
        }

        virtual jmethodID GetMethodID(jclass InClass) override
        {
            using function_traits_t = function_traits < _Ft >;
            typename function_traits_t::argument_meta_type t;
            std::string MethodSig = ApplyGetMethodID<typename function_traits_t::return_type>(t, std::make_index_sequence<function_traits_t::arity::value>{});
            JavaClassMethod JNIMethod = AndroidJNI::GetClassMethod(InClass, MethodName.c_str(), MethodSig.c_str());
            return JNIMethod.method;
        }
    protected:
        std::string MethodName;
    };
    typedef std::shared_ptr<LazyJNIMethodBase>LazyJNIMethodPointer;



    template<size_t N>
    struct PaddingTuple
    {
        template<typename Tuple>
        static void Padding(Tuple& tp, jobjectArray& args, JNIEnv* env)
        {
            std::get<N - 1>(tp) = lexical_cast<typename std::decay<decltype(std::get<N - 1>(tp))>::type>(env->GetObjectArrayElement(args, N - 1));
            PaddingTuple<N - 1>::Padding(tp, args, env);
        }
    };

    template<>
    struct PaddingTuple<0>
    {
        template<typename Tuple>
        static void Padding(Tuple& tp, jobjectArray& args, JNIEnv* env)
        {
            std::get<0>(tp) = lexical_cast<typename std::decay<decltype(std::get<0>(tp))>::type>(env->GetObjectArrayElement(args, 0));
        }
    };

    template<typename Tuple>
    inline void ApplyPaddingTuple(Tuple& tp, const jobjectArray& args)
    {
        JNIEnv* env = AndroidJNI::GetJNIEnv();
        PaddingTuple<std::tuple_size<Tuple>::value>::Padding(tp, const_cast<jobjectArray&>(args), env);
    }


    class JNIInvokeHandlerBase
    {
    public:
        JNIInvokeHandlerBase() {}
        virtual~JNIInvokeHandlerBase() {}
        virtual jobject Invoke(const jobjectArray& args) = 0;
    };

    typedef std::shared_ptr<JNIInvokeHandlerBase>   JNIInvokeHandlerPointer;

    template<typename _Ft, size_t ArgsNum>
    class JNIInvokeHandler : public JNIInvokeHandlerBase
    {
        using function_traits_t  = function_traits < _Ft >;
        using function_type      = typename function_traits_t::function_type;
        using argument_meta_type = typename function_traits_t::argument_meta_type;

    public:
        function_type       Func;
        argument_meta_type  ArgsTuple;

        JNIInvokeHandler(const function_type& fun)
        {
            Func = fun;
        }

        virtual jobject Invoke(const jobjectArray& args)
        {
            using is_void =std::is_same<typename function_traits_t::return_type, void>;
            return InvokeSwitch<typename function_traits_t::return_type>(is_void{},args);
        }

    protected:
        template <typename _Ty, typename = typename std::enable_if< !std::is_same<_Ty, void>::value, _Ty>::type >
        jobject InvokeSwitch(std::false_type, const jobjectArray& args)
        {
            ApplyPaddingTuple(ArgsTuple, args);
            typename function_traits_t::return_type val = apply(Func, ArgsTuple);
            return lexical_cast<jobject>(val);
        }

        template <typename _Ty, typename = typename std::enable_if< std::is_same<_Ty, void>::value, _Ty>::type >
        jobject InvokeSwitch(std::true_type, const jobjectArray& args)
        {
            ApplyPaddingTuple(ArgsTuple, args);
            apply(Func, ArgsTuple);
            return nullptr;
        }
    };

    template<typename _Ft>
    class JNIInvokeHandler<_Ft, 0> : public FunctionHandlerBase
    {
        using function_traits_t = function_traits < _Ft >;
        using function_type     = typename function_traits_t::function_type;
    public:
        function_type   Func;

        JNIInvokeHandler(const function_type& fun)
        {
            Func = fun;
        }

        virtual jobject Invoke(const jobjectArray& /*args*/)
        {
            using is_void = std::is_same<typename function_traits_t::return_type, void>;
            return InvokeSwitch<typename function_traits_t::return_type >(is_void{});
        }
    protected:
        template <typename _Ty, typename = typename std::enable_if< !std::is_same<_Ty, void>::value , _Ty>::type >
        jobject InvokeSwitch(std::false_type)
        {
            typename function_traits_t::return_type val = Func();
            return lexical_cast<jobject>(val);
        }

        template <typename _Ty, typename = typename std::enable_if< std::is_same<_Ty, void>::value, _Ty>::type >
        jobject InvokeSwitch(std::true_type)
        {
            Func();
            return nullptr;
        }
    };

    template<typename _Ft, typename _Ty, std::size_t... index >
    JNIInvokeHandlerPointer BindJNIInvokeHandlerImpl(_Ft fun, _Ty* object, std::index_sequence<index...>)
    {
        const size_t arityvalue = function_traits < _Ft >::arity::value;
        std::shared_ptr<JNIInvokeHandler<_Ft, arityvalue> > pSelector(new JNIInvokeHandler<_Ft, arityvalue >(std::bind(fun, object, PlaceHolderDetail::PlaceHolderMaker<index>::Get()...)));
        return pSelector;
    }

    template<typename _Ft, typename _Ty>
    JNIInvokeHandlerPointer BindJNIInvokeHandler(_Ft fun, _Ty* object)
    {
        const size_t arityvalue = function_traits < _Ft >::arity::value;
        return BindJNIInvokeHandlerImpl(fun, object, std::make_index_sequence<arityvalue>{});
    }
}
NS_FK_END
#endif //#if PLATFORM_ANDROID

#endif // END OF FOUNDATIONKIT_ANDROIDJAVAPROXYHELPER_HPP