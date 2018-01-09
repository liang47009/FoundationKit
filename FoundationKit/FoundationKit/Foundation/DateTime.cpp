/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#include <vector>
#include <cstdlib>
#include <cassert>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"

NS_FK_BEGIN

/** Returns the system time. */
void LocalTimeForDate(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    Time::TimeDate td = Time::GetTime();
    year              = td.Year;
    month             = td.Month;
    dayOfWeek         = td.DayOfWeek;
    day               = td.Day;
    hour              = td.Hour;
    min               = td.Minute;
    sec               = td.Second;
    msec              = td.Milliseconds;
}

/** Returns the UTC time. */
void UTCTimeForDate(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    Time::TimeDate td = Time::GetUTCTime();
    year              = td.Year;
    month             = td.Month;
    dayOfWeek         = td.DayOfWeek;
    day               = td.Day;
    hour              = td.Hour;
    min               = td.Minute;
    sec               = td.Second;
    msec              = td.Milliseconds;
}

const uint64 MinTicks              = 0;
const uint64 MaxTicks              = 3652059 * Time::TicksPerDay - 1;
/* DateTime constants
 *****************************************************************************/

const int32 DateTime::DaysPerMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int32 DateTime::DaysToMonth[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

/* DateTime structors
 *****************************************************************************/

DateTime::DateTime()
    :Ticks(0)
{
}

DateTime::DateTime(int64 inTicks, ETimeKind Kind)
    : Ticks(inTicks)
    , TimeKind(Kind)
{
    //ASSERT_IF((inTicks < MinTicks || inTicks > MaxTicks), "inTicks must be between MinTicks and MaxTicks");
}

DateTime::DateTime( int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond, ETimeKind Kind)
    : TimeKind(Kind)
{
    ASSERT_IF(!Validate(year, month, day, hour, minute, second, millisecond), "Input date time is invalid.");
	int32 totalDays = 0;

    if ((month > 2) && IsLeapYear(year))
	{
        ++totalDays;
	}

    year--;											// the current year is not a full year yet
    month--;										// the current month is not a full month yet

    totalDays += year * 365;
    totalDays += year / 4;							// leap year day every four years...
    totalDays -= year / 100;						// ...except every 100 years...
    totalDays += year / 400;						// ...but also every 400 years
    totalDays += DaysToMonth[month];				// days in this year up to last month
    totalDays += day - 1;							// days in this month minus today

    Ticks = totalDays   * Time::TicksPerDay
          + hour        * Time::TicksPerHour
          + minute      * Time::TicksPerMinute
          + second      * Time::TicksPerSecond
          + millisecond * Time::TicksPerMillisecond;
}


void DateTime::GetDate( int32& outYear, int32& outMonth, int32& outDay ) const
{
	int32 i, j, k, l, n;
    l = Math::FloorToInt(static_cast<float>(GetJulianDay() + 0.5)) + 68569;
	n = 4 * l / 146097;
	l = l - (146097 * n + 3) / 4;
	i = 4000 * (l + 1) / 1461001;
	l = l - 1461 * i / 4 + 31;
	j = 80 * l / 2447;
	k = l - 2447 * j / 80;
	l = j / 11;
	j = j + 2 - 12 * l;
	i = 100 * (n - 49) + i + l;

	outYear = i;
	outMonth = j;
	outDay = k;
}

int32 DateTime::GetYear() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    return year;
}

int32 DateTime::GetMonth() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    return month;
}

int32 DateTime::GetDay() const
{
	int32 year, month, day;
    GetDate(year, month, day);
    return day;
}


EDayOfWeek DateTime::GetDayOfWeek() const
{
	// January 1, 0001 was a Monday
	return static_cast<EDayOfWeek>((Ticks / Time::TicksPerDay) % 7);
}


int32 DateTime::GetDayOfYear() const
{
    int32 year, month, day;
    GetDate(year, month, day);
    for (int32 currentMonth = 1; currentMonth < month; ++currentMonth)
	{
        day += DaysInMonth(year, currentMonth);
	}
    return day;
}

int32 DateTime::GetHour12() const
{
	int32 hour = GetHour();
    if (hour < 1)
	{
		return 12;
	}
    if (hour > 12)
	{
        return (hour - 12);
	}
    return hour;
}

std::string DateTime::ToISO8601() const
{
	return ToString("%Y-%m-%dT%H:%M:%S.%sZ");
}


std::string DateTime::ToString() const
{
	return ToString("%Y.%m.%d-%H.%M.%S");
}


std::string DateTime::ToString( const char* format ) const
{
    std::string result;

    if (format != nullptr)
	{
        while (*format != '\0')
		{
            if ((*format == '%') && (*(++format) != '\0'))
			{
                switch (*format)
				{
                case 'a': result += IsMorning() ? "am" : "pm"; break;
                case 'A': result += IsMorning() ? "AM" : "PM"; break;
                case 'd': result += StringUtils::Format("%02i", GetDay()); break;
                case 'D': result += StringUtils::Format("%03i", GetDayOfYear()); break;
                case 'm': result += StringUtils::Format("%02i", GetMonth()); break;
                case 'y': result += StringUtils::Format("%02i", GetYear() % 100); break;
                case 'Y': result += StringUtils::Format("%04i", GetYear()); break;
                case 'h': result += StringUtils::Format("%02i", GetHour12()); break;
                case 'H': result += StringUtils::Format("%02i", GetHour()); break;
                case 'M': result += StringUtils::Format("%02i", GetMinute()); break;
                case 'S': result += StringUtils::Format("%02i", GetSecond()); break;
                case 's': result += StringUtils::Format("%03i", GetMillisecond()); break;
                default:		 result += *format;
				}
			}
			else
			{
                result += *format;
			}

			// move to the next one
            format++;
		}
	}

    return result;
}


/* DateTime static interface
 *****************************************************************************/

int32 DateTime::DaysInMonth( int32 year, int32 month )
{
    ASSERTED((month >= 1) && (month <= 12), "The month param is invaild.");
    if ((month == 2) && IsLeapYear(year))
	{
		return 29;
	}
    return DaysPerMonth[month];
}


int32 DateTime::DaysInYear( int32 year )
{
    if (IsLeapYear(year))
	{
		return 366;
	}
	return 365;
}


bool DateTime::IsLeapYear( int32 year )
{
    if ((year % 4) == 0)
	{
        return (((year % 100) != 0) || ((year % 400) == 0));
	}
	return false;
}

DateTime DateTime::MaxValue()
{
    return DateTime(MaxTicks);
}


DateTime DateTime::MinValue()
{
    return DateTime(MinTicks);
}

DateTime DateTime::Now()
{
	int32 year, month, day, dayOfWeek;
	int32 hour, minute, second, millisecond;
    LocalTime(year, month, dayOfWeek, day, hour, minute, second, millisecond);
    return DateTime(year, month, day, hour, minute, second, millisecond, ETimeKind::Local);
}

DateTime DateTime::UTCNow()
{
    int32 year, month, day, dayOfWeek;
    int32 hour, minute, second, millisecond;
    UTCTime(year, month, dayOfWeek, day, hour, minute, second, millisecond);
    return DateTime(year, month, day, hour, minute, second, millisecond, ETimeKind::Utc);
}

void DateTime::LocalTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    LocalTimeForDate(year, month, dayOfWeek, day, hour, min, sec, msec);
}

void DateTime::UTCTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec)
{
    UTCTimeForDate(year, month, dayOfWeek, day, hour, min, sec, msec);
}

bool DateTime::Parse( const std::string& dateTimeString, DateTime& outDateTime )
{
	// first replace -, : and . with space
    std::string fixedString = dateTimeString;
    std::replace(fixedString.begin(), fixedString.end(), '-', ' ');
    std::replace(fixedString.begin(), fixedString.end(), ':', ' ');
    std::replace(fixedString.begin(), fixedString.end(), '.', ' ');

	std::vector<std::string> tokens;

	// split up on a single delimiter
    StringUtils::Split(fixedString, ' ', tokens);
	// make sure it parsed it properly (within reason)
    if ((tokens.size() < 6) || (tokens.size() > 7))
	{
		return false;
	}

    const int32 year        = std::atoi(tokens[0].c_str());
    const int32 month       = std::atoi(tokens[1].c_str());
    const int32 day         = std::atoi(tokens[2].c_str());
    const int32 hour        = std::atoi(tokens[3].c_str());
    const int32 minute      = std::atoi(tokens[4].c_str());
    const int32 second      = std::atoi(tokens[5].c_str());
    const int32 millisecond = tokens.size() > 6 ? std::atoi(tokens[6].c_str()) : 0;

    if (!Validate(year, month, day, hour, minute, second, millisecond))
	{
		return false;
	}

	// convert the tokens to numbers
    outDateTime.Ticks = DateTime(year, month, day, hour, minute, second, millisecond).Ticks;

	return true;
}


bool DateTime::ParseISO8601( const char* dateTimeString, DateTime& outDateTime )
{
	// DateOnly: YYYY-MM-DD
	// DateTime: YYYY-mm-ddTHH:MM:SS(.ssss)(Z|+th:tm|-th:tm)

    const char* dtPtr = dateTimeString;
	char* next = nullptr;

	int32 year   = 0, month = 0, day = 0;
	int32 hour   = 0, minute = 0, second = 0, millisecond = 0;
	int32 tzHour = 0, tzMinute = 0;

	// get date
    year = static_cast<int32>(std::strtol(dtPtr, &next, 10));

    if ((next <= dtPtr) || (*next == '\0'))
	{
		return false;
	}

    dtPtr = next + 1; // skip separator
    month = static_cast<int32>(std::strtol(dtPtr, &next, 10));

    if ((next <= dtPtr) || (*next == '\0'))
	{
		return false;
	}

    dtPtr = next + 1; // skip separator
    day = static_cast<int32>(std::strtol(dtPtr, &next, 10));

    if (next <= dtPtr)
	{
		return false;
	}

	// check whether this is date and time
    if (*next == 'T')
	{
        dtPtr = next + 1;

		// parse time
        hour = static_cast<int32>(std::strtol(dtPtr, &next, 10));

        if ((next <= dtPtr) || (*next == '\0'))
		{
			return false;
		}

        dtPtr = next + 1; // skip separator
        minute = static_cast<int32>(std::strtol(dtPtr, &next, 10));

        if ((next <= dtPtr) || (*next == '\0'))
		{
			return false;
		}

        dtPtr = next + 1; // skip separator
        second = static_cast<int32>(std::strtol(dtPtr, &next, 10));

        if (next <= dtPtr)
		{
			return false;
		}

		// check for milliseconds
        if (*next == '.')
		{
            dtPtr = next + 1;
            millisecond = static_cast<int32>(std::strtol(dtPtr, &next, 10));

			// should be no more than 3 digits
            if ((next <= dtPtr) || (next > dtPtr + 3))
			{
				return false;
			}

            for (PTRINT Digits = next - dtPtr; Digits < 3; ++Digits)
			{
				millisecond *= 10;
			}
		}

		// see if the timezone offset is included
        if (*next == '+' || *next == '-')
		{
			// include the separator since it's + or -
            dtPtr = next;

			// parse the timezone offset
            tzHour = static_cast<int32>(std::strtol(dtPtr, &next, 10));

            if ((next <= dtPtr) || (*next == '\0'))
			{
				return false;
			}

            dtPtr = next + 1; // skip separator
            tzMinute = static_cast<int32>(std::strtol(dtPtr, &next, 10));

            if (next <= dtPtr)
			{
				return false;
			}
		}
        else if ((*next != '\0') && (*next != 'Z'))
		{
			return false;
		}
	}
    else if (*next != '\0')
	{
		return false;
	}

	if (!Validate(year, month, day, hour, minute, second, millisecond))
	{
		return false;
	}

	DateTime Final(year, month, day, hour, minute, second, millisecond);

	// adjust for the timezone (bringing the DateTime into UTC)
	int32 TzOffsetMinutes = (tzHour < 0) ? tzHour * 60 - tzMinute : tzHour * 60 + tzMinute;
	Final -= TimeSpan(0, TzOffsetMinutes, 0);
	outDateTime = Final;

	return true;
}

bool DateTime::Validate( int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond )
{
    return (year >= 1) && (year <= 9999) &&
        (month       >= 1) && (month       <= 12) &&
        (day         >= 1) && (day         <= DaysInMonth(year, month)) &&
        (hour        >= 0) && (hour        <= 23) &&
        (minute      >= 0) && (minute      <= 59) &&
        (second      >= 0) && (second      <= 59) &&
        (millisecond >= 0) && (millisecond <= 999);
}

std::string DateTime::GetDateString()
{
    int32 Year;
    int32 Month;
    int32 DayOfWeek;
    int32 Day;
    int32 Hour;
    int32 Min;
    int32 Sec;
    int32 MSec;
    LocalTimeForDate(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);
    std::string dateStr = StringUtils::Format("%04d-%02d-%02d", Year,Month, Day);
    return dateStr;
}

std::string DateTime::GetTimeString()
{
    int32 Year;
    int32 Month;
    int32 DayOfWeek;
    int32 Day;
    int32 Hour;
    int32 Min;
    int32 Sec;
    int32 MSec;
    LocalTimeForDate(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);
    std::string timeStr = StringUtils::Format("%02d:%02d:%02d", Hour, Min, Sec);
    return timeStr;
}

std::string DateTime::GetTimestampString()
{
    std::string timestamp = GetDateString();
    timestamp += " ";
    timestamp += GetTimeString();
    return timestamp;
}



NS_FK_END

