#pragma once
#include <chrono>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Time.h"
NS_FK_BEGIN

class Timer
{
public:
    Timer() : _begin(std::chrono::high_resolution_clock::now()){}

    void reset()
    {
        _begin = std::chrono::high_resolution_clock::now();
    }

    double nanoseconds()const
    {
        return elapsed<nanoseconds_f>();
    }

    double microseconds()const
    {
        return elapsed<microseconds_f>();
    }

    double milliseconds()const
    {
        return elapsed<milliseconds_f>();
    }

    double seconds()const
    {
        return elapsed<seconds_f>();
    }

    double minutes()const
    {
        return elapsed<minutes_f>();
    }

    double hours()const
    {
        return elapsed<hours_f>();
    }

    double days()
    {
        return elapsed< std::chrono::duration<double, std::ratio<3600 * 24> >>();
    }
protected:
    template<typename Duration = milliseconds_f>
    double elapsed()const
    {
        return std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now() - _begin).count();
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock>   _begin;
};

NS_FK_END