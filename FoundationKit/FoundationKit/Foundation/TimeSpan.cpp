/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include <vector>
#include <cassert>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"

NS_FK_BEGIN

std::string TimeSpan::ToString() const
{
	if (GetDays() == 0)
	{
		return ToString("%n%h:%m:%s.%f");
	}

	return ToString("%n%d.%h:%m:%s.%f");
}


std::string TimeSpan::ToString( const char* format ) const
{
	std::string result;

    while (*format != '\0')
	{
        if ((*format == '%') && (*++format != '\0'))
		{
            switch (*format)
			{
            case 'n': if (Ticks < 0) result += '-'; break;
            case 'N': result += (Ticks < 0) ? '-' : '+'; break;
            case 'd': result += StringUtils::Format("%i",   Math::Abs(GetDays())); break;
            case 'h': result += StringUtils::Format("%02i", Math::Abs(GetHours())); break;
            case 'm': result += StringUtils::Format("%02i", Math::Abs(GetMinutes())); break;
            case 's': result += StringUtils::Format("%02i", Math::Abs(GetSeconds())); break;
            case 'f': result += StringUtils::Format("%03i", Math::Abs(GetMilliseconds())); break;
            case 'D': result += StringUtils::Format("%f",   Math::Abs(GetTotalDays())); break;
            case 'H': result += StringUtils::Format("%f",   Math::Abs(GetTotalHours())); break;
            case 'M': result += StringUtils::Format("%f",   Math::Abs(GetTotalMinutes())); break;
            case 'S': result += StringUtils::Format("%f",   Math::Abs(GetTotalSeconds())); break;
            case 'F': result += StringUtils::Format("%f",   Math::Abs(GetTotalMilliseconds())); break;
			default:
                result += *format;
			}
		}
		else
		{
            result += *format;
		}

        ++format;
	}

    return result;
}


/* TimeSpan static interface
 *****************************************************************************/

TimeSpan TimeSpan::FromDays( double days )
{
    ASSERTED((days >= MinValue().GetTotalDays()) && (days <= MaxValue().GetTotalDays()), "The days param invalid.");
    return TimeSpan(static_cast<int64>(days * Time::TicksPerDay));
}

TimeSpan TimeSpan::FromHours( double hours )
{
    ASSERTED((hours >= MinValue().GetTotalHours()) && (hours <= MaxValue().GetTotalHours()), "The hours param invalid.");
    return TimeSpan(static_cast<int64>(hours * Time::TicksPerHour));
}

TimeSpan TimeSpan::FromMinutes(double minutes)
{
    ASSERTED((minutes >= MinValue().GetTotalMinutes()) && (minutes <= MaxValue().GetTotalMinutes()), "The minutes param invalid.");
    return TimeSpan(static_cast<int64>(minutes * Time::TicksPerMinute));
}

TimeSpan TimeSpan::FromSeconds(double seconds)
{
    ASSERTED((seconds >= MinValue().GetTotalSeconds()) && (seconds <= MaxValue().GetTotalSeconds()), "The seconds param invalid.");
    return TimeSpan(static_cast<int64>(seconds * Time::TicksPerSecond));
}

TimeSpan TimeSpan::FromMilliseconds( double milliseconds )
{
    ASSERTED((milliseconds >= MinValue().GetTotalMilliseconds()) && (milliseconds <= MaxValue().GetTotalMilliseconds()), "The milliseconds param invalid.");
    return TimeSpan(static_cast<int64>(milliseconds * Time::TicksPerMillisecond));
}


bool TimeSpan::Parse( const std::string& timespanString, TimeSpan& outTimespan )
{
	// @todo gmp: implement stricter TimeSpan parsing; this implementation is too forgiving.
    std::string tokenString = timespanString;
    std::replace(tokenString.begin(), tokenString.end(), '.', ':');
    bool Negative = (tokenString[0] == '-');
    std::replace(tokenString.begin(), tokenString.end(), '-', ':');

	std::vector<std::string> tokens;
    StringUtils::Split(tokenString, ':', tokens);


    if (tokens.size() == 4)
	{
        tokens.insert(tokens.begin(), "0");
	}

    if (tokens.size() == 5)
	{
        outTimespan.Assign(std::atoi(tokens[0].c_str()), std::atoi(tokens[1].c_str()), std::atoi(tokens[2].c_str()), std::atoi(tokens[3].c_str()), std::atoi(tokens[4].c_str()));
		if (Negative)
		{
            outTimespan.Ticks *= -1;
		}
		return true;
	}
	return false;
}


void TimeSpan::Assign( int32 days, int32 hours, int32 minutes, int32 seconds, int32 milliseconds )
{
    int64 totalms = 1000 * (60 * 60 * 24 * (int64)days + 60 * 60 * (int64)hours + 60 * (int64)minutes + (int64)seconds) + (int64)milliseconds;
    ASSERTED((totalms >= MinValue().GetTotalMilliseconds()) && (totalms <= MaxValue().GetTotalMilliseconds()), "Total millisecond is invalid.");
	Ticks = totalms * Time::TicksPerMillisecond;
}

NS_FK_END

