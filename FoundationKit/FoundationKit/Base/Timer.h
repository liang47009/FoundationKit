/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_TIMER_H
#define LOSEMYMIND_TIMER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <chrono>
#include "FoundationKit/FoundationMacros.h"

namespace std
{
    namespace chrono
    {
        typedef duration<int,    std::ratio<3600 * 24> > day;
        typedef duration<double, std::nano> nanoseconds_f;
        typedef duration<double, std::micro> microseconds_f;
        typedef duration<double, std::milli> milliseconds_f;
        typedef duration<double> seconds_f;
        typedef duration<double, std::ratio<60> > minutes_f;
        typedef duration<double, std::ratio<3600> > hours_f;
        typedef duration<double, std::ratio<3600 * 24> > day_f;
    }
}

NS_FK_BEGIN

class Timer final
{
public:
    typedef std::chrono::high_resolution_clock       clock_type;

    Timer() : _begin(clock_type::now()){}
    long long nanoseconds()const{return elapsed<std::chrono::nanoseconds>();}
    long long microseconds()const{return elapsed<std::chrono::microseconds>();}
    long long milliseconds()const{return elapsed<std::chrono::milliseconds>();}
    long long seconds()const{return elapsed<std::chrono::seconds>();}
    int minutes()const{return elapsed<std::chrono::minutes>();}
    int hours()const{return elapsed<std::chrono::hours>();}
    int days(){ return elapsed<std::chrono::day>(); }

    // double version
    double nanosecondsf()const{ return elapsed<std::chrono::nanoseconds_f>(); }
    // double version
    double microsecondsf()const{ return elapsed<std::chrono::microseconds_f>(); }
    // double version
    double millisecondsf()const{ return elapsed<std::chrono::milliseconds_f>(); }
    // double version
    double secondsf()const{ return elapsed<std::chrono::seconds_f>(); }
    // double version
    double minutesf()const{ return elapsed<std::chrono::minutes_f>(); }
    // double version
    double hoursf()const{ return elapsed<std::chrono::hours_f>(); }
    // double version
    double daysf(){ return elapsed<std::chrono::day_f>(); }

    void reset(){ _begin = clock_type::now(); }

protected:
    template<typename Duration = std::chrono::milliseconds>
    typename Duration::rep elapsed()const
    {
        return std::chrono::duration_cast<Duration>(clock_type::now() - _begin).count();
    }
private:
    clock_type::time_point _begin;
};




NS_FK_END

#endif // LOSEMYMIND_TIMER_H

