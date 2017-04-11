/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FUNCTIONAL_H
#define FOUNDATIONKIT_FUNCTIONAL_H

#define LOKI_ENABLE_FUNCTION

#include <stdexcept>
#include "loki/Functor.h"
#include "loki/Sequence.h"

namespace std
{
    using namespace Loki;

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct function
    ///
    ///  \ingroup FunctorGroup
    ///  Allows a boost/TR1 like usage of Functor.
    /// 
    ///  \par Usage
    ///
    ///      - free functions: e.g.  \code function<int(int,int)> f(&freeFunction);
    ///                              \endcode
    ///      - member functions: e.g \code function<int()> f(&object,&ObjectType::memberFunction); 
    ///                              \endcode
    ///
    ///  see also test/function/FunctionTest.cpp (the modified test program from boost)
    ////////////////////////////////////////////////////////////////////////////////

    template<class R = void()>
    struct function;


    template<class R>
    struct function<R()> : public Functor<R>
    {
        typedef Functor<R> FBase;

        function() : FBase() {}

        function(const function& func) : FBase() 
        {
            if( !func.empty()) 
                FBase::operator=(func);
        }

        // test on emptiness
        template<class R2> 
        function(function<R2()> func) : FBase() 
        {
            if(!func.empty())
                FBase::operator=(func);
        }

        // clear  by '= 0'
        function(const int i) : FBase()
        { 
            if(i==0)
                FBase::clear();
            else
                throw std::runtime_error("Loki::function(const int i): i!=0");
        }

        template<class Func>
        function(Func func) : FBase(func) {}

        template<class Host, class Func>
        function(const Host& host, const Func& func) : FBase(host,func) {}

    };


    ////////////////////////////////////////////////////////////////////////////////
    // macros for the repetitions
    ////////////////////////////////////////////////////////////////////////////////

#define LOKI_FUNCTION_BODY                          \
    \
    function() : FBase() {}                     \
    \
    function(const function& func) : FBase()    \
    {                                           \
    if( !func.empty())                      \
    FBase::operator=(func);             \
    }                                           \
    \
    function(const int i) : FBase()             \
    {                                           \
    if(i==0)                                \
    FBase::clear();                     \
            else                                    \
            throw std::runtime_error(           \
            "Loki::function(const int i): i!=0");   \
    }                                           \
    \
    template<class Func>                        \
    function(Func func) : FBase(func) {}        \
    \
    template<class Host, class Func>            \
    function(const Host& host, const Func& func): FBase(host,func) {}


#define LOKI_FUNCTION_R2_CTOR_BODY          \
    \
    : FBase()                           \
    {                                   \
    if(!func.empty())               \
    FBase::operator=(func);     \
    }


    ////////////////////////////////////////////////////////////////////////////////
    // repetitions
    ////////////////////////////////////////////////////////////////////////////////

    template<>
    struct function<>
        : public Loki::Functor<>
    {
        typedef Functor<> FBase;

        template<class R2>
        function(function<R2()> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY // if compilation breaks here then 
            // function.h was not included before
            // Functor.h, check your include order
            // or define LOKI_ENABLE_FUNCTION 
    };

    template<class R,class P01>
    struct function<R(P01)> 
        : public Loki::Functor<R, Seq<P01> >
    {
        typedef Functor<R, Seq<P01> > FBase;

        template<class R2,class Q01>
        function(function<R2(Q01)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,class P01,class P02>
    struct function<R(P01,P02)> 
        : public Functor<R, Seq<P01,P02> >
    {
        typedef Functor<R, Seq<P01,P02> > FBase;

        template<class R2,class Q01, class Q02>
        function(function<R2(Q01,Q02)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,class P01,class P02, class P03>
    struct function<R(P01,P02,P03)> 
        : public Functor<R, Seq<P01,P02,P03> >
    {
        typedef Functor<R, Seq<P01,P02,P03> > FBase;

        template<class R2,class Q01, class Q02,class Q03>
        function(function<R2(Q01,Q02,Q03)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,class P01,class P02, class P03,class P04>
    struct function<R(P01,P02,P03,P04)> 
        : public Functor<R, Seq<P01,P02,P03,P04> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04> > FBase;

        template<class R2,class Q01,class Q02, class Q03,class Q04>
        function(function<R2(Q01,Q02,Q03,Q04)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,class P01,class P02, class P03,class P04,class P05>
    struct function<R(P01,P02,P03,P04,P05)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05> > FBase;

        template<class R2,class Q01,class Q02, class Q03,class Q04,class Q05>
        function(function<R2(Q01,Q02,Q03,Q04,Q05)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06>
    struct function<R(P01,P02,P03,P04,P05,P06)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06> > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07>
    struct function<R(P01,P02,P03,P04,P05,P06,P07)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07> > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08> > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08,class P09>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08,P09)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09    > > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08,class Q09>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08,Q09)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08,class P09,class P10>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08,P09,P10)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10> > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08,class Q09,class Q10>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08,Q09,Q10)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08,class P09,class P10,
    class P11>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11> >FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08,class Q09,class Q10,
        class Q11>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08,Q09,Q10,Q11)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08,class P09,class P10,
    class P11,class P12>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12> > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08,class Q09,class Q10,
        class Q11,class Q12>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08,Q09,Q10,Q11,Q12)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08,class P09,class P10,
    class P11,class P12, class P13>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13> > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08,class Q09,class Q10,
        class Q11,class Q12, class Q13>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08,Q09,Q10,Q11,Q12,Q13)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08,class P09,class P10,
    class P11,class P12, class P13,class P14>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13,P14)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13,P14> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13,P14> > FBase;
        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08,class Q09,class Q10,
        class Q11,class Q12, class Q13,class Q14>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08,Q09,Q10,Q11,Q12,Q13,Q14)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };

    template<class R,    class P01,class P02, class P03,class P04,class P05,
    class P06,class P07, class P08,class P09,class P10,
    class P11,class P12, class P13,class P14,class P15>
    struct function<R(P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13,P14,P15)> 
        : public Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13,P14,P15> >
    {
        typedef Functor<R, Seq<P01,P02,P03,P04,P05,P06,P07,P08,P09,P10,P11,P12,P13,P14,P15> > FBase;

        template<class R2,    class Q01,class Q02, class Q03,class Q04,class Q05,
        class Q06,class Q07, class Q08,class Q09,class Q10,
        class Q11,class Q12, class Q13,class Q14,class Q15>
            function(function<R2(Q01,Q02,Q03,Q04,Q05,Q06,Q07,Q08,Q09,Q10,Q11,Q12,Q13,Q14,Q15)> func) 
            LOKI_FUNCTION_R2_CTOR_BODY

            LOKI_FUNCTION_BODY
    };


} //namespace std
#endif // FOUNDATIONKIT_FUNCTIONAL_H
