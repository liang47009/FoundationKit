/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_TIME_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_TIME_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
NS_FK_BEGIN


enum ETimeKind
{
    Unspecified = 0,
    Utc = 1,
    Local = 2,
};

class Time
{

public:
    /** The number of timespan ticks per millisecond. */
    static const int64 TicksPerMillisecond = 10000;
    /** The number of timespan ticks per second. */
    static const int64 TicksPerSecond      = 10000000;
    /** The number of timespan ticks per minute. */
    static const int64 TicksPerMinute      = 600000000;
    /** The number of timespan ticks per hour. */
    static const int64 TicksPerHour        = 36000000000;
    /** The number of timespan ticks per day. */
    static const int64 TicksPerDay         = 864000000000;
    /** The number of timespan ticks per week. */
    static const int64 TicksPerWeek        = 6048000000000;

    struct TimeDate 
    {
        int32 Year;
        int32 Month;
        int32 DayOfWeek;
        int32 Day;
        int32 Hour;
        int32 Minute;
        int32 Second;
        int32 Milliseconds;
    };

    //double TimeValToSecond(timeval & tv)
    //{
    //    return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) / 1e6;
    //}

    static uint64   GetTimeStamp();

    /**
    * Get the local date and time on this computer
    */
    static TimeDate GetTime();
    //static TimeDate GetTime(time_t timeValue);

    /**
    * Get the UTC date and time on this computer
    */
    static TimeDate GetUTCTime();
    //static TimeDate GetUTCTime(time_t timeValue);

};


NS_FK_END

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_TIME_HPP
