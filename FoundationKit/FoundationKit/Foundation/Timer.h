#pragma once
#include <chrono>
#include "FoundationKit/FoundationKitDefines.h"
NS_FK_BEGIN

using namespace std;
using namespace std::chrono;

class Timer
{
public:
    Timer(): _begin(high_resolution_clock::now()){}

    void reset()
    {
        _begin = high_resolution_clock::now();
    }

    template<typename Duration=milliseconds>
    int64_t elapsed()const
    {
        return duration_cast<Duration>(high_resolution_clock::now() - _begin).count();
    }

    //微秒
    int64_t elapsed_micro()const
    {
        return elapsed<std::chrono::microseconds>();
    }

    // 纳秒
    int64_t elapsed_nano()const
    {
        return elapsed<std::chrono::nanoseconds>();
    }

    //秒
    int64_t elapsed_seconds()const
    {
        return elapsed<std::chrono::seconds>();
    }

    // 分
    int64_t elapsed_minutes()const
    {
        return elapsed<std::chrono::minutes>();
    }

    // 时
    int64_t elapsed_hours()const
    {
        return elapsed<std::chrono::hours>();
    }
private:
    time_point<high_resolution_clock>   _begin;
};

NS_FK_END


