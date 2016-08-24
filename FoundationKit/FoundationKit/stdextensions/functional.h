//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   xlFunction.h
//    Author:      Streamlet
//    Create Time: 2011-01-09
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __XLFUNCTION_H_83D86772_25B2_4625_8A4F_0C5EAAF1E93C_INCLUDED__
#define __XLFUNCTION_H_83D86772_25B2_4625_8A4F_0C5EAAF1E93C_INCLUDED__

#include "xlDef.h"

#ifdef ENABLE_XL_LIBRARY

#include "xlQIPtr.h"
#include "xlMacros.h"
#include "FoundationKit/stdextensions/Loki/Typelist.h"

using namespace xl;
using namespace Loki::TL;

namespace std
{
#define XL_FUNCTION_DEFINE_MAX  17

#define XL_FUNCTION_TYPENAME_DECLARE_PATTERN(n)     typename T##n
#define XL_FUNCTION_TYPENAME_DECLARE(n)             XL_REPZ(XL_FUNCTION_TYPENAME_DECLARE_PATTERN, n, XL_COMMA)

#define XL_FUNCTION_TYPENAME_LIST_PATTERN(n)        T##n
#define XL_FUNCTION_TYPENAME_LIST(n)                XL_REPZ(XL_FUNCTION_TYPENAME_LIST_PATTERN, n, XL_COMMA)

#define XL_FUNCTION_TYPENAME_VARIABLE_PATTERN(n)    T##n v##n
#define XL_FUNCTION_TYPENAME_VARIABLE(n)            XL_REPZ(XL_FUNCTION_TYPENAME_VARIABLE_PATTERN, n, XL_COMMA)

#define XL_FUNCTION_VARIABLE_LIST_PATTERN(n)        v##n
#define XL_FUNCTION_VARIABLE_LIST(n)                XL_REPZ(XL_FUNCTION_VARIABLE_LIST_PATTERN, n, XL_COMMA)

    template <typename Signature>
    struct FunctionRefTraits
    {
        typedef Signature RefType;
    };
        
    template <typename RetType>
    struct FunctionRefTraits<RetType ()>
    {
        typedef RetType (&RefType)();
    };

#define XL_FUNCTION_GLOBALFUNCTIONTRAITS_PATTERN(n)                     \
                                                                        \
    template <typename RetType, XL_FUNCTION_TYPENAME_DECLARE(n)>        \
    struct FunctionRefTraits<RetType (XL_FUNCTION_TYPENAME_LIST(n))>    \
    {                                                                   \
        typedef RetType (&RefType)(XL_FUNCTION_TYPENAME_LIST(n));       \
    };                                                                  \

#define XL_FUNCTION_GLOBALFUNCTIONTRAITS(n)  XL_REPY(XL_FUNCTION_GLOBALFUNCTIONTRAITS_PATTERN, n, XL_NIL)

    XL_FUNCTION_GLOBALFUNCTIONTRAITS(XL_FUNCTION_DEFINE_MAX)

    template <typename R, typename TL>
    class FunctionBase;

    template <typename R>
    class FunctionBase<R, TYPELIST_0()>
    {
    public:
        virtual FunctionBase *Clone() const = 0;
        virtual R Invoke()                  = 0;
        virtual ~FunctionBase() {}
    };

#define XL_FUNCTION_FUNCTORBASE_PATTERN(n)                                                          \
                                                                                                    \
    template <typename R, XL_FUNCTION_TYPENAME_DECLARE(n)>                                          \
    class FunctionBase<R, XL_EVAL(XL_CONN(TYPELIST_, XL_INC(n)), XL_FUNCTION_TYPENAME_LIST(n))>  \
    {                                                                                               \
    public:                                                                                         \
        virtual FunctionBase *Clone() const             = 0;                                        \
        virtual R Invoke(XL_FUNCTION_TYPENAME_LIST(n))  = 0;                                        \
        virtual ~FunctionBase() {}                                                                  \
    };                                                                                              \

#define XL_FUNCTION_FUNCTORBASE(n)  XL_REPY(XL_FUNCTION_FUNCTORBASE_PATTERN, n, XL_NIL)

    XL_FUNCTION_FUNCTORBASE(XL_FUNCTION_DEFINE_MAX)

    template <typename R, typename P, typename F>
    class FunctionHandler : public FunctionBase<R, P>
    {
    private:
        typedef R ReturnType;
        typedef P ParamList;
        typedef F FunctionType;

    public:
        FunctionHandler(const FunctionType &fnFunction)
            : m_fnFunction(fnFunction)
        {

        }

    private:
        FunctionType m_fnFunction;

    public:
        FunctionBase *Clone() const
        {
            return new FunctionHandler(m_fnFunction);
        }

        ReturnType Invoke()
        {
            return m_fnFunction();
        }

#define XL_FUNCTION_FUNCTION_INVOKE_PATTERN(n)                          \
                                                                        \
typedef typename TypeAtNonStrict<ParamList, n, Loki::NullType>::Result  \
                XL_FUNCTION_TYPENAME_LIST_PATTERN(n);                   \
                                                                        \
        ReturnType Invoke(XL_FUNCTION_TYPENAME_VARIABLE(n))             \
        {                                                               \
            return m_fnFunction(XL_FUNCTION_VARIABLE_LIST(n));          \
        }                                                               \

#define XL_FUNCTION_FUNCTION_INVOKE(n)  XL_REPY(XL_FUNCTION_FUNCTION_INVOKE_PATTERN, n, XL_NIL)

        XL_FUNCTION_FUNCTION_INVOKE(XL_FUNCTION_DEFINE_MAX)
    };

    template <typename R, typename P, typename T, typename F>
    class MemberFunctionHandler : public FunctionBase<R, P>
    {
    private:
        typedef R ReturnType;
        typedef P ParamList;
        typedef T ClassPtrType;
        typedef F MemberFunctionType;

    public:
        MemberFunctionHandler(const ClassPtrType &pObject, const MemberFunctionType &fnFunction)
            : m_pObject(pObject), m_fnFunction(fnFunction)
        {

        }

    private:
        ClassPtrType        m_pObject;
        MemberFunctionType  m_fnFunction;

    public:
        FunctionBase *Clone() const
        {
            return new MemberFunctionHandler(m_pObject, m_fnFunction);
        }

        ReturnType Invoke()
        {
            return (m_pObject->*m_fnFunction)();
        }

#define XL_FUNCTION_MEMBERFUNCTION_INVOKE_PATTERN(n)                            \
                                                                                \
   typedef typename TypeAtNonStrict<ParamList, n, Loki::NullType>::Result       \
                XL_FUNCTION_TYPENAME_LIST_PATTERN(n);                           \
                                                                                \
        ReturnType Invoke(XL_FUNCTION_TYPENAME_VARIABLE(n))                     \
        {                                                                       \
            return (m_pObject->*m_fnFunction)(XL_FUNCTION_VARIABLE_LIST(n));    \
        }                                                                       \

#define XL_FUNCTION_MEMBERFUNCTION_INVOKE(n)  XL_REPY(XL_FUNCTION_MEMBERFUNCTION_INVOKE_PATTERN, n, XL_NIL)

        XL_FUNCTION_MEMBERFUNCTION_INVOKE(XL_FUNCTION_DEFINE_MAX)
    };

    template <typename S>
    class function;

    template <typename R>
    class function<R ()>
    {
    private:
		typedef typename MakeTypelist<>::Result ParamList;
		typedef function<R()> _Myt;                        

#define XL_FUCTION_IMPLEMENT_BODY()                                                                             \
                                                                                                                \
    private:                                                                                                    \
        typedef R                                   ReturnType;                                                 \
        typedef FunctionBase<ReturnType, ParamList> FunctionBaseType;                                           \
                                                                                                                \
    public:                                                                                                     \
        function(intptr_t isNull = NULL)                                                                        \
            : m_pFunctionBase(nullptr)                                                                          \
        {                                                                                                       \
                                                                                                                \
        }                                                                                                       \
                                                                                                                \
        function(const _Myt &that)                                                                              \
            : m_pFunctionBase(that.m_pFunctionBase)                                                             \
        {                                                                                                       \
                                                                                                                \
        }                                                                                                       \
                                                                                                                \
    public:                                                                                                     \
        template <typename F>                                                                                   \
        function(const F &fnFunction)                                                                           \
            : m_pFunctionBase(new FunctionHandler<ReturnType,                                                   \
                                                  ParamList,                                                    \
                                                  typename FunctionRefTraits<F>::RefType>(fnFunction))          \
        {                                                                                                       \
                                                                                                                \
        }                                                                                                       \
                                                                                                                \
        template <typename T, typename F>                                                                       \
        function(const T &pObject, const F &pMemberFunction)                                                    \
            : m_pFunctionBase(new MemberFunctionHandler<ReturnType, ParamList, T, F>(pObject, pMemberFunction)) \
        {                                                                                                       \
                                                                                                                \
        }                                                                                                       \
                                                                                                                \
    public:                                                                                                     \
	   _Myt &operator = (const _Myt &that)                                                                      \
		{                                                                                                       \
		    if (that.m_pFunctionBase != nullptr)                                                                \
		        this->m_pFunctionBase = that.m_pFunctionBase->Clone();                                          \
			else                                                                                                \
			    m_pFunctionBase = nullptr;                                                                      \
			return *this;                                                                                       \
		}                                                                                                       \
                                                                                                                \
		_Myt& operator=(int nullptr_t) { m_pFunctionBase = nullptr; }                                           \
	    operator bool() const { return (m_pFunctionBase != nullptr); }                                          \
                                                                                                                \
    private:                                                                                                    \
        QIPtr<FunctionBaseType> m_pFunctionBase;                                                                \

        XL_FUCTION_IMPLEMENT_BODY()

    public:
        ReturnType operator()()
        {
            return m_pFunctionBase->Invoke();
        }
    };

		// TEMPLATE NULL POINTER COMPARISONS
template<class _Fty>
	bool operator==(const function<_Fty>& _Other, int nullptr_t) 
	{	// compare to null pointer
	return (!_Other);
	}

template<class _Fty>
	bool operator==(int _Npc, const function<_Fty>& _Other)
	{	// compare to null pointer
	return (operator==(_Other, _Npc));
	}

template<class _Fty>
	bool operator!=(const function<_Fty>& _Other, int _Npc)
	{	// compare to null pointer
	return (!operator==(_Other, _Npc));
	}

template<class _Fty>
	bool operator!=(int _Npc, const function<_Fty>& _Other)
	{	// compare to null pointer
	return (!operator==(_Other, _Npc));
	}



#define XL_FUNCTION_IMPLEMENT_PATTERN(n)                                                \
                                                                                        \
    template <typename R, XL_FUNCTION_TYPENAME_DECLARE(n)>                              \
    class function<R (XL_FUNCTION_TYPENAME_LIST(n))>                                    \
    {                                                                                   \
    private:                                                                            \
	typedef typename MakeTypelist<XL_FUNCTION_TYPENAME_LIST(n)>::Result ParamList;      \
		typedef function<R (XL_FUNCTION_TYPENAME_LIST(n))> _Myt;                        \
                                                                                        \
        XL_FUCTION_IMPLEMENT_BODY()                                                     \
                                                                                        \
    public:                                                                             \
        ReturnType operator()(XL_FUNCTION_TYPENAME_VARIABLE(n))                         \
        {                                                                               \
            return m_pFunctionBase->Invoke(XL_FUNCTION_VARIABLE_LIST(n));               \
        }                                                                               \
    };                                                                                  \

#define XL_FUNCTION_IMPLEMENT(n)    XL_REPY(XL_FUNCTION_IMPLEMENT_PATTERN, n, XL_NIL)

    XL_FUNCTION_IMPLEMENT(XL_FUNCTION_DEFINE_MAX)

} // namespace std

#endif // #ifdef ENABLE_XL_LIBRARY

#endif // #ifndef __XLFUNCTION_H_83D86772_25B2_4625_8A4F_0C5EAAF1E93C_INCLUDED__
