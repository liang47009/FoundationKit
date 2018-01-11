/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"

#if PLATFORM_WINDOWS

#else
#include <time.h>
#include <sys/time.h>
#endif

NS_FK_BEGIN

Time::TimeDate Time::GetTime()
{
    TimeDate   td;
#if PLATFORM_WINDOWS
    SYSTEMTIME st;
    // localtime_s
    GetLocalTime(&st);
    td.Year         = st.wYear;
    td.Month        = st.wMonth;
    td.DayOfWeek    = st.wDayOfWeek;
    td.Day          = st.wDay;
    td.Hour         = st.wHour;
    td.Minute       = st.wMinute;
    td.Second       = st.wSecond;
    td.Milliseconds = st.wMilliseconds;
#else
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);
    // convert it to local time
    struct tm localTime;
    localtime_r(&tmVal.tv_sec, &localTime);
    // pull out data/time
    td.Year         = localTime.tm_year + 1900;
    td.Month        = localTime.tm_mon + 1;
    td.DayOfWeek    = localTime.tm_wday;
    td.Day          = localTime.tm_mday;
    td.Hour         = localTime.tm_hour;
    td.Minute       = localTime.tm_min;
    td.Second       = localTime.tm_sec;
    td.Milliseconds = tmVal.tv_usec / 1000;
#endif
    return td;
}

//Time::TimeDate Time::GetTime(time_t timeValue)
//{
//    tm* tmValue = localtime(&timeValue);
//}


Time::TimeDate Time::GetUTCTime()
{
    TimeDate   td;
#if PLATFORM_WINDOWS
    SYSTEMTIME st;
    //gmtime_s
    ::GetSystemTime(&st);
    td.Year         = st.wYear;
    td.Month        = st.wMonth;
    td.DayOfWeek    = st.wDayOfWeek;
    td.Day          = st.wDay;
    td.Hour         = st.wHour;
    td.Minute       = st.wMinute;
    td.Second       = st.wSecond;
    td.Milliseconds = st.wMilliseconds;
#else
    // query for calendar time
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);
    // convert it to UTC
    struct tm utcTime;
    gmtime_r(&tmVal.tv_sec, &utcTime);
    // pull out data/time
    td.Year         = utcTime.tm_year + 1900;
    td.Month        = utcTime.tm_mon + 1;
    td.DayOfWeek    = utcTime.tm_wday;
    td.Day          = utcTime.tm_mday;
    td.Hour         = utcTime.tm_hour;
    td.Minute       = utcTime.tm_min;
    td.Second       = utcTime.tm_sec;
    td.Milliseconds = tmVal.tv_usec / 1000;
#endif
    return td;
}

uint64 Time::GetUnixTimeStamp()
{
    uint64 time = GetUTCUnixTimeStamp();
    time += TimeZone::GetUTCOffset().GetTotalSeconds();
    time += TimeZone::GetDSTOffset().GetTotalSeconds();
    return time;
}

uint64 Time::GetUTCUnixTimeStamp()
{
#if PLATFORM_WINDOWS
    FILETIME ft;
    uint64 time = 0;
    GetSystemTimeAsFileTime(&ft);
    time |= ft.dwHighDateTime;
    time <<= 32;
    time |= ft.dwLowDateTime;
    time -= 116444736000000000LL;
    time /= TicksPerMillisecond;
    // or
    //uint64 time = 0;
    //GetSystemTimeAsFileTime((FILETIME*)&time);
    //time -= 116444736000000000LL;
    //time /= TicksPerMillisecond;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64 time = tv.tv_usec;
    time /= 1000;
    time += (tv.tv_sec * 1000);
#endif
    return time / 1000; // Convert to seconds.
}

uint64 Time::GetCurrentTimeNanosFromSystem()
{
    uint64 clocktime = 0;
#if PLATFORM_WINDOWS
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);
    clocktime = ((static_cast<int64>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime) - 116444736000000000LL;
    clocktime *= 100;
    // or
    //clocktime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - std::chrono::system_clock::from_time_t(0)).count();
#else
    timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);
    clocktime = ts.tv_sec * 1000000000LL + ts.tv_nsec;
    // or
    //timeval tv;
    //gettimeofday(&tv, NULL);
    //clocktime = tv.tv_sec * 1000000000LL + tv.tv_usec * 1000;
#endif
    // Other implement see https://github.com/abseil/abseil-cpp/tree/0271cd35577599fa99b59202da17d3136956e4c0/absl/time/internal
    return clocktime;
}


struct tm *localtime_t(const time_t* _Time, struct tm* _Tm)
{
#if PLATFORM_WINDOWS
    localtime_s(_Tm, _Time);
    return _Tm;
#else
    return localtime_r(_Time, _Tm);
#endif
}

struct tm *gmtime_t(const time_t* _Time, struct tm* _Tm)
{
#if PLATFORM_WINDOWS
    gmtime_s(_Tm, _Time);
    return _Tm;
#else
    return gmtime_r(_Time, _Tm);
#endif
}

NS_FK_END










