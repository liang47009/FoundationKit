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
    static const int64 TicksPerMicrosecond = 10;                      //Represents the number of ticks in 1 microsecond.
    /** The number of timespan ticks per millisecond. */
    static const int64 TicksPerMillisecond = 10'000;                  //Represents the number of ticks in 1 millisecond.
    /** The number of timespan ticks per second. */
    static const int64 TicksPerSecond      = 10'000'000;              //Represents the number of ticks in 1 second.
    /** The number of timespan ticks per minute. */
    static const int64 TicksPerMinute      = 600'000'000;             //Represents the number of ticks in 1 minute.
    /** The number of timespan ticks per hour. */
    static const int64 TicksPerHour        = 36'000'000'000;          //Represents the number of ticks in 1 hour.
    /** The number of timespan ticks per day. */
    static const int64 TicksPerDay         = 864'000'000'000;         //Represents the number of ticks in 1 day.
    /** The number of timespan ticks per week. */
    static const int64 TicksPerWeek        = 6'048'000'000'000;       //Represents the number of ticks in 1 week.
    /** The number of timespan ticks from 0001.1.1 to 1970.1.1 */
    static const int64 UnixTimeEra         = 621'355'968'000'000'000; // == DateTime(1970, 1, 1).Ticks
    /** The number of nanoseconds per tick. */
    static const int64 NanosecondsPerTick  = 100;

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
    static int64   GetUnixTimeStamp();

    /**
     * Get unix timestamp, unit seconds.
     * The information is in Coordinated Universal Time (UTC) format.
     */
    static int64   GetUTCUnixTimeStamp();

    /**
     * Get current unix time(from 1970.1.1 to now), unit nanoseconds,But not always accurate
     * It can be very well support for microseconds of precision.
     * The information is in Coordinated Universal Time (UTC) format.
     */
    static int64   GetCurrentTimeNanosFromSystem();

};

/* [TSF] Thread safe functions */
struct tm *localtime_t(const time_t* _Time, struct tm* _Tm);
struct tm *gmtime_t(const time_t* _Time, struct tm* _Tm);
NS_FK_END

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_TIME_HPP
