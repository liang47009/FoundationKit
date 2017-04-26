/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_CHRONO_H
#define FOUNDATIONKIT_CHRONO_H
#include "ratio.h"
#include "stdcommon.h"
namespace std
{

/// Minimal implementation of the @c chrono namespace.
/// The @c chrono namespace provides types for specifying time intervals.
namespace chrono 
{
    // CLASS TEMPLATE treat_as_floating_point
    template<class _Rep>
    struct treat_as_floating_point
        : is_floating_point<_Rep>
    {	// tests for floating-point type
    };

    // CLASS TEMPLATE duration_values
    template<class _Rep>
    struct duration_values
    {	// gets arithmetic properties of a type
        static _Rep zero()
        {	// get zero value
            return (_Rep(0));
        }

        static _Rep (min)()
        {	// get smallest value
            return ((numeric_limits<_Rep>::min)());
        }

        static _Rep (max)()
        {	// get largest value
            return ((numeric_limits<_Rep>::max)());
        }
    };

    // CLASS TEMPLATE _Is_ratio
    template<class _Ty>
    struct _Is_ratio
    {	// test for ratio type
        static const bool value = false;
    };

    template<intmax_t _R1,
        intmax_t _R2>
    struct _Is_ratio<ratio<_R1, _R2> >
    {	// test for ratio type
        static const bool value = true;
    };

    // CLASS TEMPLATE duration
    template<class _Rep,
    class _Period = ratio<1> >
    class duration;

    template<class _Ty>
    struct _Is_duration
    {	// tests for duration
        static const bool value = false;
    };

    template<class _To,
    class _Rep,
    class _Period> inline
        typename enable_if<_Is_duration<_To>::value,_To>::type
        duration_cast(const duration<_Rep, _Period>& _Dur)
    {
        typedef ratio_divide<_Period, typename _To::period> _CF;
        typedef typename _To::rep _ToRep;
        typedef _ToRep _CR;
        if (_CF::num == 1 && _CF::den == 1)
            return (_To(static_cast<_ToRep>(_Dur.count())));
        else if (_CF::num != 1 && _CF::den == 1)
            return (_To(static_cast<_ToRep>(
            static_cast<_CR>(_Dur.count()) * static_cast<_CR>(_CF::num))));
        else if (_CF::num == 1 && _CF::den != 1)
            return (_To(static_cast<_ToRep>(
            static_cast<_CR>(_Dur.count()) / static_cast<_CR>(_CF::den))));
        else
            return (_To(static_cast<_ToRep>(
            static_cast<_CR>(_Dur.count()) * static_cast<_CR>(_CF::num)
            / static_cast<_CR>(_CF::den))));
    }

    template<class _Rep,
    class _Period>
    class duration
    {	// represents a time duration
    public:
        typedef duration<_Rep, _Period> _Myt;
        typedef _Rep rep;
        typedef _Period period;

        duration()
            : _MyRep()
        {	// check asserts
            //static_assert(_Is_ratio<_Period>::value,"period must be an instance of std::ratio");
            //static_assert(0 < _Period::num,"duration negative or zero");
        }

        template<class _Rep2>
            explicit duration(const _Rep2& _Val)
            : _MyRep(static_cast<_Rep>(_Val))
        {	// construct from representation
            //static_assert(_Is_ratio<_Period>::value,"period not an instance of std::ratio");
            //static_assert(0 < _Period::num,"duration negative or zero");
        }

        template<class _Rep2,
        class _Period2>
            duration(const duration<_Rep2, _Period2>& _Dur)
            : _MyRep(duration_cast<_Myt>(_Dur).count())
        {	// construct from a duration
            //typedef ratio_divide<_Period2, _Period> _Checked_type;
            //static_assert(_Is_ratio<_Period>::value, "period not an instance of std::ratio");
            //static_assert(0 < _Period::num, "duration negative or zero");
        }

        _Rep count() const
        {	// get stored rep
            return (_MyRep);
        }

        _Myt operator+() const
        {	// get value
            return (*this);
        }

        _Myt operator-() const
        {	// get negated value
            return (_Myt(0 - _MyRep));
        }

        _Myt& operator++()
        {	// increment rep
            ++_MyRep;
            return (*this);
        }

        _Myt operator++(int)
        {	// postincrement rep
            return (_Myt(_MyRep++));
        }

        _Myt& operator--()
        {	// decrement rep
            --_MyRep;
            return (*this);
        }

        _Myt operator--(int)
        {	// postdecrement rep
            return (_Myt(_MyRep--));
        }

        _Myt& operator+=(const _Myt& _Right)
        {	// add _Right to rep
            _MyRep += _Right._MyRep;
            return (*this);
        }

        _Myt& operator-=(const _Myt& _Right)
        {	// subtract _Right from rep
            _MyRep -= _Right._MyRep;
            return (*this);
        }

        _Myt& operator*=(const _Rep& _Right)
        {	// multiply rep by _Right
            _MyRep *= _Right;
            return (*this);
        }

        _Myt& operator/=(const _Rep& _Right)
        {	// divide rep by _Right
            _MyRep /= _Right;
            return (*this);
        }

        _Myt& operator%=(const _Rep& _Right)
        {	// modulus rep by _Right
            _MyRep %= _Right;
            return (*this);
        }

        _Myt& operator%=(const _Myt& _Right)
        {	// modulus rep by _Right
            _MyRep %= _Right.count();
            return (*this);
        }

        static _Myt zero()
        {	// get zero value
            return (_Myt(duration_values<_Rep>::zero()));
        }

        static _Myt (min)()
        {	// get minimum value
            return (_Myt((duration_values<_Rep>::min)()));
        }
        static _Myt (max)()
        {	// get maximum value
            return (_Myt((duration_values<_Rep>::max)()));
        }

    private:
        _Rep _MyRep;	// the stored rep
    };

    template<class _Rep1,
    class _Period1,
    class _Period2>
    class duration<duration<_Rep1, _Period1>, _Period2>
    {	// undefined: duration type as first template argument
        duration()
        {	// check asserts
            //static_assert(_Always_false<_Rep1>::value,"duration can't have duration as first template argument");
        }
    };

    // CLASS TEMPLATE _Is_duration
    template<class _Rep,
    class _Period>
    struct _Is_duration<duration<_Rep, _Period> >
    {	// tests for duration
        static const bool value = true;
    };

    typedef duration<long long, nano> nanoseconds;
    typedef duration<long long, micro> microseconds;
    typedef duration<long long, milli> milliseconds;
    typedef duration<long long> seconds;
    typedef duration<int, ratio<60> > minutes;
    typedef duration<int, ratio<3600> > hours;

    template<class _Clock, class _Duration = typename _Clock::duration>
    class time_point
    {	// represents a point in time
    public:
        typedef _Clock clock;
        typedef _Duration duration;
        typedef typename _Duration::rep rep;
        typedef typename _Duration::period period;

        time_point()
            : _MyDur(_Duration::zero())
        {	// check asserts
            //static_assert(_Is_duration<_Duration>::value, "duration must be an instance of std::duration");
        }

        explicit time_point(const _Duration& _Other)
            : _MyDur(_Other)
        {	// construct from a duration
        }

        template<class _Duration2>
            time_point(const time_point<_Clock, _Duration2>& _Tp)
            : _MyDur(_Tp.time_since_epoch())
        {	// construct from another duration
        }

        _Duration time_since_epoch() const
        {	// get duration from epoch
            return (_MyDur);
        }

        time_point& operator+=(const _Duration& _Dur)
        {	// increment by duration
            _MyDur += _Dur;
            return (*this);
        }

        time_point& operator-=(const _Duration& _Dur)
        {	// decrement by duration
            _MyDur -= _Dur;
            return (*this);
        }

        static time_point (min)()
        {	// get minimum time point
            return (time_point((_Duration::min)()));
        }
        static time_point (max)()
        {	// get maximum time point
            return (time_point((_Duration::max)()));
        }

    private:
        _Duration _MyDur;	// duration since the epoch
    };

    template<class _Rep1, class _Period1, class _Rep2, class _Period2> 
    inline typename duration<_Rep1, _Period1> operator+( const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right)
    {	// add two durations
        typedef typename duration<_Rep1, _Period1> _CD;
        return (_CD(_Left) += _Right);
    }

    template<class _Rep1, class _Period1, class _Rep2, class _Period2> 
    inline typename duration<_Rep1, _Period1> operator-( const duration<_Rep1, _Period1>& _Left, const duration<_Rep2, _Period2>& _Right)
    {	// subtract two durations
        typedef typename duration<_Rep1, _Period1> _CD;
        return (_CD(_Left) -= _Right);
    }

    template<class _Clock, class _Duration>
    inline time_point<_Clock,_Duration> operator+( const time_point<_Clock, _Duration>& _Left, const time_point<_Clock, _Duration>& _Right)
    {	// add time_point to duration
        return time_point<_Clock,_Duration>(_Right.time_since_epoch() + _Left.time_since_epoch());
    }

    template<class _Clock, class _Duration> 
    inline time_point<_Clock,_Duration> operator-( const time_point<_Clock, _Duration>& _Left, const time_point<_Clock, _Duration>& _Right)
    {	// subtract duration from time_point
        return time_point<_Clock,_Duration>(_Left.time_since_epoch() - _Right.time_since_epoch());
    }


    #define _XTIME_NSECS_PER_TICK	100
    #define _XTIME_TICKS_PER_TIME_T	(_LONGLONG)10000000
    #define _EPOCHFILETIME   (116444736000000000i64)
    inline _LONGLONG _Xtime_get_ticks()
    {
        _LONGLONG ft;
        GetSystemTimeAsFileTime((FILETIME*)&ft);
        return ft - _EPOCHFILETIME;
    }
    struct system_clock
    {	// wraps system clock
        typedef _LONGLONG rep;
        typedef ratio_multiply<ratio<_XTIME_NSECS_PER_TICK, 1>, nano> period;
        typedef chrono::duration<rep, period> duration;
        typedef chrono::time_point<system_clock> time_point;
        static const bool is_monotonic = false;	// retained
        static const bool is_steady = false;

        static time_point now() _NOEXCEPT
        {	// get current time
            return system_clock::time_point( system_clock::duration(_Xtime_get_ticks()));
        }

        // C conversions
        static time_t to_time_t(const time_point& _Time) _NOEXCEPT
        {	// convert to time_t
            return ((time_t)(_Time.time_since_epoch().count()
                / _XTIME_TICKS_PER_TIME_T));
        }

        static time_point from_time_t(time_t _Tm) _NOEXCEPT
        {	// convert from time_t
            return (time_point(duration(_Tm * _XTIME_TICKS_PER_TIME_T)));
        }
    };

    class steady_clock
        : public system_clock
    {	// wraps monotonic clock
    public:
        static const bool is_monotonic = true;	// retained
        static const bool is_steady = true;
    };

    typedef steady_clock monotonic_clock;	// retained
    typedef system_clock high_resolution_clock;

}//namespace chrono 
} //namespace std
#endif // FOUNDATIONKIT_CHRONO_H
