#include <ctime>
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"

NS_FK_BEGIN

std::string FoundationKit::TimeZone::DisplayName()
{
    std::string result;
#if PLATFORM_WINDOWS
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    WCHAR* ptr = (dstFlag == TIME_ZONE_ID_DAYLIGHT) ? tzInfo.DaylightName : tzInfo.StandardName;
    result = StringUtils::wstring2UTF8string(ptr);
#else
    tzset();
    result = std::string(tzname[daylight ? 1 : 0]);
#endif
    return result;
}

std::string TimeZone::DaylightName()
{
    std::string result;
#if PLATFORM_WINDOWS
    TIME_ZONE_INFORMATION tzInfo;
    GetTimeZoneInformation(&tzInfo);
    WCHAR* ptr = tzInfo.DaylightName;
    result = StringUtils::wstring2UTF8string(ptr);
#else
    tzset();
    result = std::string(tzname[1]);
#endif
    return result;
}

std::string TimeZone::StandardName()
{
    std::string result;
#if PLATFORM_WINDOWS
    TIME_ZONE_INFORMATION tzInfo;
    GetTimeZoneInformation(&tzInfo);
    WCHAR* ptr = tzInfo.StandardName;
    result = StringUtils::wstring2UTF8string(ptr);
#else
    tzset();
    result = std::string(tzname[0]);
#endif
return result;
}

bool TimeZone::IsDaylightSavingTime(DateTime date)
{
    std::time_t time = date.ToUnixTimestamp();
    struct std::tm tms;
    localtime_t(&time,&tms);
    return tms.tm_isdst > 0;
}

TimeSpan TimeZone::GetUTCOffset()
{
#if PLATFORM_WINDOWS
    TIME_ZONE_INFORMATION tzInfo;
    GetTimeZoneInformation(&tzInfo);
    return TimeSpan(-tzInfo.Bias * Time::TicksPerMinute);
#else
//    std::time_t now = std::time(NULL);
//    struct std::tm t;
//    gmtime_r(&now, &t);
//    std::time_t utc = std::mktime(&t);
//    return TimeSpan((now - utc)*Time::TicksPerSecond);
    tzset();
    return TimeSpan(-timezone*Time::TicksPerSecond);
#endif
}

TimeSpan TimeZone::GetDSTOffset()
{
#if PLATFORM_WINDOWS
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation(&tzInfo);
    return TimeSpan((dstFlag == TIME_ZONE_ID_DAYLIGHT ? -tzInfo.DaylightBias : 0)*Time::TicksPerMinute);
#else
    std::time_t now = std::time(NULL);
    struct std::tm t;
    ASSERT_IF(!localtime_t(&now, &t), "Cannot get local time DST offset");
    return TimeSpan((t.tm_isdst == 1 ? 3600 : 0)*Time::TicksPerHour);
#endif
}

DateTime TimeZone::ToLocalTime(const DateTime& date)
{
    if (date.GetTimeKind() == ETimeKind::Local)
    {
        return date;
    }
    DateTime ResultDateTime = date;
    ResultDateTime += GetUTCOffset();
    ResultDateTime += GetDSTOffset();
    return ResultDateTime;
}

DateTime TimeZone::ToUniversalTime(const DateTime& date)
{
    if (date.GetTimeKind() == ETimeKind::Utc)
    {
        return date;
    }
    DateTime ResultDateTime(date.GetTicks(), ETimeKind::Utc);
    ResultDateTime -= GetUTCOffset();
    ResultDateTime -= GetDSTOffset();
    return ResultDateTime;
}


std::string TimeZone::ToDebugString()
{
    std::string result = StringUtils::Format("TimeZone[DisplayName:%s, DaylightName:%s, StandardName:%s, UTCOffset:%s, DSTOffset:%s]",
        DisplayName().c_str(), DaylightName().c_str(), StandardName().c_str(),
        GetUTCOffset().ToString().c_str(), GetDSTOffset().ToString().c_str());
    return result;
}



NS_FK_END

