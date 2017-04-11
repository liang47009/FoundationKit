/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_RATIO_H
#define FOUNDATIONKIT_RATIO_H
#include <stdint.h>

namespace std
{
    // CLASS TEMPLATE _Abs
    template<intmax_t _Val>
    struct _Abs
    {   // computes absolute value of _Val
        static const intmax_t value = _Val < 0 ? -_Val : _Val;
    };

    // CLASS TEMPLATE _Safe_mult
    template<intmax_t _Ax,
    intmax_t _Bx,
    bool _Good>
    struct _Safe_multX
    {	// computes _Ax * _Bx without overflow
        static const intmax_t value = _Ax * _Bx;
    };

    template<intmax_t _Ax,
    intmax_t _Bx>
    struct _Safe_multX<_Ax, _Bx, false>
    {	// _Ax * _Bx would overflow
        //static_assert(_Always_false<_Safe_multX>::value,"integer arithmetic overflow");
    };

    #ifndef INTMAX_MAX
    #define INTMAX_MAX   9223372036854775807i64
    #endif

    template<intmax_t _Ax,
    intmax_t _Bx>
    struct _Safe_mult
    {	// computes _Ax * _Bx, forbids overflow
        static const intmax_t value = _Safe_multX<_Ax, _Bx,
            (_Abs<_Ax>::value <= INTMAX_MAX / _Abs<_Bx>::value)>::value;
    };

    template<intmax_t _Ax>
    struct _Safe_mult<_Ax, 0>
    {	// computes _Ax * 0, avoids division by 0
        static const intmax_t value = 0;
    };

    // CLASS TEMPLATE _Sign_of
    template<intmax_t _Val>
    struct _Sign_of
    {   // computes sign of _Val
        static const intmax_t value = _Val < 0 ? -1 : 1;
    };

    // CLASS TEMPLATE _Gcd
    template<intmax_t _Ax,
    intmax_t _Bx>
    struct _GcdX
    {   // computes greatest common divisor of _Ax and _Bx
        static const intmax_t value = _GcdX<_Bx, _Ax % _Bx>::value;
    };

    template<intmax_t _Ax>
    struct _GcdX<_Ax, 0>
    {   // computes greatest common divisor of _Ax and 0
        static const intmax_t value = _Ax;
    };

    template<intmax_t _Ax,
    intmax_t _Bx>
    struct _Gcd
    {   // computes greatest common divisor of abs(_Ax) and abs(_Bx)
        static const intmax_t value =
            _GcdX<_Abs<_Ax>::value, _Abs<_Bx>::value>::value;
    };

    template<>
    struct _Gcd<0, 0>
    {   // avoids division by 0 in ratio_less
        static const intmax_t value = 1;	// contrary to mathematical convention
    };

    // CLASS TEMPLATE ratio
    template<intmax_t _Nx,
    intmax_t _Dx = 1>
    struct ratio
    {   // holds the ratio of _Nx to _Dx

        //static_assert(_Dx != 0, "zero denominator");
        //static_assert(-INTMAX_MAX <= _Nx, "numerator too negative");
        //static_assert(-INTMAX_MAX <= _Dx, "denominator too negative");

        static const intmax_t num = _Sign_of<_Nx>::value * _Sign_of<_Dx>::value* _Abs<_Nx>::value / _Gcd<_Nx, _Dx>::value;

        static const intmax_t den = _Abs<_Dx>::value / _Gcd<_Nx, _Dx>::value;

        typedef ratio<num, den> type;

        static double _as_double() { return double(_Nx) / double(_Dx);}
    };


    typedef ratio<1, 1000000000000000000LL> atto;
    typedef ratio<1, 1000000000000000LL> femto;
    typedef ratio<1, 1000000000000LL> pico;
    typedef ratio<1, 1000000000> nano;
    typedef ratio<1, 1000000> micro;
    typedef ratio<1, 1000> milli;
    typedef ratio<1, 100> centi;
    typedef ratio<1, 10> deci;
    typedef ratio<10, 1> deca;
    typedef ratio<100, 1> hecto;
    typedef ratio<1000, 1> kilo;
    typedef ratio<1000000, 1> mega;
    typedef ratio<1000000000, 1> giga;
    typedef ratio<1000000000000LL, 1> tera;
    typedef ratio<1000000000000000LL, 1> peta;
    typedef ratio<1000000000000000000LL, 1> exa;


    // ALIAS TEMPLATE ratio_multiply
    template<class _R1,
    class _R2>
    struct _Ratio_multiply
    {	// multiply two ratios
        //static_assert(_Are_ratios<_R1, _R2>::value, "ratio_multiply<R1, R2> requires R1 and R2 to be ratio<>s.");

        static const intmax_t _N1 = _R1::num;
        static const intmax_t _D1 = _R1::den;
        static const intmax_t _N2 = _R2::num;
        static const intmax_t _D2 = _R2::den;

        static const intmax_t _Gx = _Gcd<_N1, _D2>::value;
        static const intmax_t _Gy = _Gcd<_N2, _D1>::value;

        // typename ratio<>::type is unnecessary here
        typedef ratio<
            _Safe_mult<_N1 / _Gx, _N2 / _Gy>::value,
            _Safe_mult<_D1 / _Gy, _D2 / _Gx>::value
        > type;
    };

    template<class _R1,class _R2>
    class ratio_multiply:public _Ratio_multiply<_R1, _R2>::type{};

    // ALIAS TEMPLATE ratio_divide
    template<class _R1,
    class _R2>
    struct _Ratio_divide
    {	// divide two ratios
        //static_assert(_Are_ratios<_R1, _R2>::value,"ratio_divide<R1, R2> requires R1 and R2 to be ratio<>s.");

        static const intmax_t _N2 = _R2::num;
        static const intmax_t _D2 = _R2::den;

        typedef ratio_multiply<_R1, ratio<_D2, _N2> > type;
    };

    template<class _R1, class _R2>
    class ratio_divide: public _Ratio_divide<_R1, _R2>::type{};


} //namespace std

#endif // FOUNDATIONKIT_RATIO_H



