/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ELAPSEDTIMER_HPP
#define FOUNDATIONKIT_ELAPSEDTIMER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <chrono>
#include "FoundationKit/GenericPlatformMacros.hpp"

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

class ElapsedTimer final
{
public:
    typedef std::chrono::high_resolution_clock       clock_type;

    ElapsedTimer() : _begin(clock_type::now()){}
    long long Nanoseconds()const{ return Elapsed<std::chrono::nanoseconds>(); }
    long long Microseconds()const{ return Elapsed<std::chrono::microseconds>(); }
    long long Milliseconds()const{ return Elapsed<std::chrono::milliseconds>(); }
    long long Seconds()const{ return Elapsed<std::chrono::seconds>(); }
    long long Minutes()const{ return Elapsed<std::chrono::minutes>(); }
    long long Hours()const{ return Elapsed<std::chrono::hours>(); }
    long long Days(){ return Elapsed<std::chrono::day>(); }

    // double version
    double Nanosecondsf()const{ return Elapsed<std::chrono::nanoseconds_f>(); }
    // double version
    double Microsecondsf()const{ return Elapsed<std::chrono::microseconds_f>(); }
    // double version
    double Millisecondsf()const{ return Elapsed<std::chrono::milliseconds_f>(); }
    // double version
    double Secondsf()const{ return Elapsed<std::chrono::seconds_f>(); }
    // double version
    double Minutesf()const{ return Elapsed<std::chrono::minutes_f>(); }
    // double version
    double Hoursf()const{ return Elapsed<std::chrono::hours_f>(); }
    // double version
    double Daysf(){ return Elapsed<std::chrono::day_f>(); }

    void Reset(){ _begin = clock_type::now(); }
protected:
    template<typename Duration = std::chrono::milliseconds>
    typename Duration::rep Elapsed()const
    {
        return std::chrono::duration_cast<Duration>(clock_type::now() - _begin).count();
    }
private:
    clock_type::time_point _begin;
};

NS_FK_END

#endif // FOUNDATIONKIT_ELAPSEDTIMER_HPP
