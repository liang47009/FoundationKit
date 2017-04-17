/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include <vector>
#include <cassert>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/Timespan.hpp"

NS_FK_BEGIN

std::string Timespan::toString() const
{
	if (getDays() == 0)
	{
		return toString("%n%h:%m:%s.%f");
	}

	return toString("%n%d.%h:%m:%s.%f");
}


std::string Timespan::toString( const char* format ) const
{
	std::string result;

    while (*format != '\0')
	{
        if ((*format == '%') && (*++format != '\0'))
		{
            switch (*format)
			{
            case 'n': if (_ticks < 0) result += '-'; break;
            case 'N': result += (_ticks < 0) ? '-' : '+'; break;
            case 'd': result += StringUtils::format("%i",   Math::abs(getDays())); break;
            case 'h': result += StringUtils::format("%02i", Math::abs(getHours())); break;
            case 'm': result += StringUtils::format("%02i", Math::abs(getMinutes())); break;
            case 's': result += StringUtils::format("%02i", Math::abs(getSeconds())); break;
            case 'f': result += StringUtils::format("%03i", Math::abs(getMilliseconds())); break;
            case 'D': result += StringUtils::format("%f",   Math::abs(getTotalDays())); break;
            case 'H': result += StringUtils::format("%f",   Math::abs(getTotalHours())); break;
            case 'M': result += StringUtils::format("%f",   Math::abs(getTotalMinutes())); break;
            case 'S': result += StringUtils::format("%f",   Math::abs(getTotalSeconds())); break;
            case 'F': result += StringUtils::format("%f",   Math::abs(getTotalMilliseconds())); break;
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


/* Timespan static interface
 *****************************************************************************/

Timespan Timespan::fromDays( double days )
{
    ASSERTED((days >= minValue().getTotalDays()) && (days <= maxValue().getTotalDays()), "The days param invalid.");
    return Timespan(static_cast<int64>(days * ETimespan::TicksPerDay));
}

Timespan Timespan::fromHours( double hours )
{
    ASSERTED((hours >= minValue().getTotalHours()) && (hours <= maxValue().getTotalHours()), "The hours param invalid.");
    return Timespan(static_cast<int64>(hours * ETimespan::TicksPerHour));
}

Timespan Timespan::fromMinutes(double minutes)
{
    ASSERTED((minutes >= minValue().getTotalMinutes()) && (minutes <= maxValue().getTotalMinutes()), "The minutes param invalid.");
    return Timespan(static_cast<int64>(minutes * ETimespan::TicksPerMinute));
}

Timespan Timespan::fromSeconds(double seconds)
{
    ASSERTED((seconds >= minValue().getTotalSeconds()) && (seconds <= maxValue().getTotalSeconds()), "The seconds param invalid.");
    return Timespan(static_cast<int64>(seconds * ETimespan::TicksPerSecond));
}

Timespan Timespan::fromMilliseconds( double milliseconds )
{
    ASSERTED((milliseconds >= minValue().getTotalMilliseconds()) && (milliseconds <= maxValue().getTotalMilliseconds()), "The milliseconds param invalid.");
    return Timespan(static_cast<int64>(milliseconds * ETimespan::TicksPerMillisecond));
}


bool Timespan::parse( const std::string& timespanString, Timespan& outTimespan )
{
	// @todo gmp: implement stricter Timespan parsing; this implementation is too forgiving.
    std::string tokenString = timespanString;
    std::replace(tokenString.begin(), tokenString.end(), '.', ':');
    bool Negative = (tokenString[0] == '-');
    std::replace(tokenString.begin(), tokenString.end(), '-', ':');

	std::vector<std::string> tokens;
    StringUtils::split(tokenString, ':', tokens);


    if (tokens.size() == 4)
	{
        tokens.insert(tokens.begin(), "0");
	}

    if (tokens.size() == 5)
	{
        outTimespan.assign(std::atoi(tokens[0].c_str()), std::atoi(tokens[1].c_str()), std::atoi(tokens[2].c_str()), std::atoi(tokens[3].c_str()), std::atoi(tokens[4].c_str()));
		if (Negative)
		{
            outTimespan._ticks *= -1;
		}
		return true;
	}
	return false;
}


void Timespan::assign( int32 days, int32 hours, int32 minutes, int32 seconds, int32 milliseconds )
{
    int64 totalms = 1000 * (60 * 60 * 24 * (int64)days + 60 * 60 * (int64)hours + 60 * (int64)minutes + (int64)seconds) + (int64)milliseconds;
    ASSERTED((totalms >= minValue().getTotalMilliseconds()) && (totalms <= maxValue().getTotalMilliseconds()), "Total millisecond is invalid.");
	_ticks = totalms * ETimespan::TicksPerMillisecond;
}

NS_FK_END

