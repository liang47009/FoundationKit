/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_TIME_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_TIME_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
NS_FK_BEGIN


enum class ETimeKind
{
    Unspecified = 0,
    Utc = 1,
    Local = 2,
};

class Time
{

public:
    /** The number of timespan ticks per microsecond. */
    static const int64 TicksPerMicrosecond = 10;
    /** The number of timespan ticks per millisecond. */
    static const int64 TicksPerMillisecond = 10'000;
    /** The number of timespan ticks per second. */
    static const int64 TicksPerSecond      = 10'000'000;
    /** The number of timespan ticks per minute. */
    static const int64 TicksPerMinute      = 600'000'000;
    /** The number of timespan ticks per hour. */
    static const int64 TicksPerHour        = 36'000'000'000;
    /** The number of timespan ticks per day. */
    static const int64 TicksPerDay         = 864'000'000'000;
    /** The number of timespan ticks per week. */
    static const int64 TicksPerWeek        = 6'048'000'000'000;

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

    /**
     * Get the local date and time on this computer
     * The information is local system format.
     */
    static TimeDate GetTime();

    /**
     * Get the UTC date and time on this computer
     * The information is in Coordinated Universal Time (UTC) format.
     */
    static TimeDate GetUTCTime();

    /**
     * Get unix timestamp, unit seconds
     * The information is local system format.
     */
    static uint64   GetUnixTimeStamp();

    /**
     * Get unix timestamp, unit seconds.
     * The information is in Coordinated Universal Time (UTC) format.
     */
    static uint64   GetUTCUnixTimeStamp();

    /**
     * Get the local timestamp, unit nanoseconds,But not always accurate
     * It can be very well support for microseconds of precision.
     * The information is in Coordinated Universal Time (UTC) format.
     */
    static uint64   GetSystemRealTime();

};

/* [TSF] Thread safe functions */
struct tm *localtime_t(const time_t* _Time, struct tm* _Tm);
struct tm *gmtime_t(const time_t* _Time, struct tm* _Tm);
NS_FK_END

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_TIME_HPP
