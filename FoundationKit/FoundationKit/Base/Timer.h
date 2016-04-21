
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

    template<typename Duration = std::chrono::milliseconds>
    int64_t elapsed()const
    {
        return duration_cast<Duration>(high_resolution_clock::now() - _begin).count();
    }

    // 纳秒
    int64_t elapsed_nano()const
    {
        return elapsed<std::chrono::nanoseconds>();
    }

    // 微秒
    int64_t elapsed_micro()const
    {
        return elapsed<std::chrono::microseconds>();
    }

    // 毫秒
    int64_t elapsed_mill()const
    {
        return elapsed<std::chrono::milliseconds>();
    }

    // 秒
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

    int (elapsed_day)()
    {
        return elapsed< duration<int, ratio<3600*24> >>();
    }
private:
    time_point<high_resolution_clock>   _begin;
};

NS_FK_END


