#include "FoundationKit/Foundation/Time.hpp"

#if TARGET_PLATFORM == PLATFORM_WINDOWS

#elif (TARGET_PLATFORM == PLATFORM_ANDROID) ||(TARGET_PLATFORM == PLATFORM_IOS) ||((TARGET_PLATFORM == PLATFORM_MAC))
#include <time.h>
#include <sys/time.h>
#endif

NS_FK_BEGIN


uint64 Time::GetTimeStamp()
{
#if TARGET_PLATFORM == PLATFORM_WINDOWS
    FILETIME filetime;
    uint64 time = 0;
    GetSystemTimeAsFileTime(&filetime);
    time |= filetime.dwHighDateTime;
    time <<= 32;
    time |= filetime.dwLowDateTime;
    time /= 10;
    time -= 11644473600000000Ui64;
    time /= 1000;
    return time;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64 time = tv.tv_usec;
    time /= 1000;
    time += (tv.tv_sec * 1000);
    return time;
#endif
}


Time::TimeDate Time::GetSystemTime()
{
    TimeDate   td;
#if TARGET_PLATFORM == PLATFORM_WINDOWS
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
#elif (TARGET_PLATFORM == PLATFORM_ANDROID) ||(TARGET_PLATFORM == PLATFORM_IOS) ||((TARGET_PLATFORM == PLATFORM_MAC))
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


Time::TimeDate Time::GetUTCTime()
{
    TimeDate   td;
#if TARGET_PLATFORM == PLATFORM_WINDOWS
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
#elif (TARGET_PLATFORM == PLATFORM_ANDROID) ||(TARGET_PLATFORM == PLATFORM_IOS) ||((TARGET_PLATFORM == PLATFORM_MAC))
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


NS_FK_END
