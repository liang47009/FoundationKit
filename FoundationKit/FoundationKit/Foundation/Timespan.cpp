/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include <vector>
#include <cassert>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Math.hpp"
#include "FoundationKit/Foundation/Timespan.hpp"

NS_FK_BEGIN

std::string Timespan::ToString() const
{
	if (GetDays() == 0)
	{
		return ToString("%n%h:%m:%s.%f");
	}

	return ToString("%n%d.%h:%m:%s.%f");
}


std::string Timespan::ToString( const char* format ) const
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


/* Timespan static interface
 *****************************************************************************/

Timespan Timespan::FromDays( double days )
{
    ASSERTED((days >= MinValue().GetTotalDays()) && (days <= MaxValue().GetTotalDays()), "The days param invalid.");
    return Timespan(static_cast<int64>(days * ETimespan::TicksPerDay));
}

Timespan Timespan::FromHours( double hours )
{
    ASSERTED((hours >= MinValue().GetTotalHours()) && (hours <= MaxValue().GetTotalHours()), "The hours param invalid.");
    return Timespan(static_cast<int64>(hours * ETimespan::TicksPerHour));
}

Timespan Timespan::FromMinutes(double minutes)
{
    ASSERTED((minutes >= MinValue().GetTotalMinutes()) && (minutes <= MaxValue().GetTotalMinutes()), "The minutes param invalid.");
    return Timespan(static_cast<int64>(minutes * ETimespan::TicksPerMinute));
}

Timespan Timespan::FromSeconds(double seconds)
{
    ASSERTED((seconds >= MinValue().GetTotalSeconds()) && (seconds <= MaxValue().GetTotalSeconds()), "The seconds param invalid.");
    return Timespan(static_cast<int64>(seconds * ETimespan::TicksPerSecond));
}

Timespan Timespan::FromMilliseconds( double milliseconds )
{
    ASSERTED((milliseconds >= MinValue().GetTotalMilliseconds()) && (milliseconds <= MaxValue().GetTotalMilliseconds()), "The milliseconds param invalid.");
    return Timespan(static_cast<int64>(milliseconds * ETimespan::TicksPerMillisecond));
}


bool Timespan::Parse( const std::string& timespanString, Timespan& outTimespan )
{
	// @todo gmp: implement stricter Timespan parsing; this implementation is too forgiving.
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
            outTimespan._ticks *= -1;
		}
		return true;
	}
	return false;
}


void Timespan::Assign( int32 days, int32 hours, int32 minutes, int32 seconds, int32 milliseconds )
{
    int64 totalms = 1000 * (60 * 60 * 24 * (int64)days + 60 * 60 * (int64)hours + 60 * (int64)minutes + (int64)seconds) + (int64)milliseconds;
    ASSERTED((totalms >= MinValue().GetTotalMilliseconds()) && (totalms <= MaxValue().GetTotalMilliseconds()), "Total millisecond is invalid.");
	_ticks = totalms * ETimespan::TicksPerMillisecond;
}

NS_FK_END

