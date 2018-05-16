/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_DATETIME_HPP
#define FOUNDATIONKIT_DATETIME_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <iostream>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/TimeSpan.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "FoundationKit/Base/types.hpp"

NS_FK_BEGIN

/**
 * Enumerates the days of the week in 7-day calendars.
 */
enum EDayOfWeek
{
	Monday = 0,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

/**
 * Enumerates the months of the year in 12-month calendars.
 */
enum EMonthOfYear
{
	January = 1,
	February,
	March,
	April,
	May,
	June,
	July,
	August,
	September,
	October,
	November,
	December
};


/**
 * Implements a date and time.
 *
 * Values of this type represent dates and times between Midnight 00:00:00, January 1, 0001 and
 * Midnight 23:59:59.9999999, December 31, 9999 in the Gregorian calendar. Internally, the time
 * values are stored in ticks of 0.1 microseconds (= 100 nanoseconds) since January 1, 0001.
 *
 * To retrieve the current local date and time, use the DateTime.Now() method. To retrieve the
 * current UTC time, use the DateTime.UTCNow() method instead.
 *
 * This class also provides methods to convert dates and times from and to string representations,
 * calculate the number of days in a given month and year, check for leap years and determine the
 * time of day, day of week and month of year of a given date and time.
 *
 * The companion struct TimeSpan is provided for enabling date and time based arithmetic, such as
 * calculating the difference between two dates or adding a certain amount of time to a given date.
 *
 *
 * @see TimeSpan
 */
class DateTime final
{
public:
    // The date/time format defined in the ISO 8601 standard.
    //
    // Examples: 
    //   2005-01-01T12:00:00+01:00
    //   2005-01-01T11:00:00Z
    static const std::string FORMAT_ISO8601;

    // The date/time format defined in the ISO 8601 standard,
    // with fractional seconds.
    //
    // Examples: 
    //   2005-01-01T12:00:00.000+01:00
    //   2005-01-01T11:00:00.000Z
    static const std::string FORMAT_ISO8601_FRAC;

    // The date/time format defined in RFC 822 (obsoleted by RFC 1123).
    //
    // Examples: 
    //   Sat, 1 Jan 05 12:00:00 +0100
    //   Sat, 1 Jan 05 11:00:00 GMT
    static const std::string FORMAT_RFC822;

    // The date/time format defined in RFC 1123 (obsoletes RFC 822).
    //
    // Examples: 
    //   Sat, 1 Jan 2005 12:00:00 +0100
    //   Sat, 1 Jan 2005 11:00:00 GMT
    static const std::string FORMAT_RFC1123;

    // The date/time format defined in the HTTP specification (RFC 2616),
    // which is basically a variant of RFC 1036 with a zero-padded day field.
    //
    // Examples: 
    //   Sat, 01 Jan 2005 12:00:00 +0100
    //   Sat, 01 Jan 2005 11:00:00 GMT
    static const std::string FORMAT_HTTP;

    // The date/time format defined in RFC 850 (obsoleted by RFC 1036).
    //
    // Examples: 
    //   Saturday, 1-Jan-05 12:00:00 +0100
    //   Saturday, 1-Jan-05 11:00:00 GMT
    static const std::string FORMAT_RFC850;

    // The date/time format defined in RFC 1036 (obsoletes RFC 850).
    //
    // Examples: 
    //   Saturday, 1 Jan 05 12:00:00 +0100
    //   Saturday, 1 Jan 05 11:00:00 GMT
    static const std::string FORMAT_RFC1036;

    // The date/time format produced by the ANSI C asctime() function.
    //
    // Example: 
    //   Sat Jan  1 12:00:00 2005
    static const std::string FORMAT_ASCTIME;

    // A simple, sortable date/time format.
    //
    // Example:
    //   2005-01-01 12:00:00
    static const std::string FORMAT_SORTABLE;

    // names used by formatter and parser
    // English names of week days (Monday, Tuesday, ..., Sunday).
    static const std::string WEEKDAY_NAMES[7];

    // English names of months (January, February, ..., December).	
    static const std::string MONTH_NAMES[12];
    
public:

	/** Default constructor*/
    DateTime();

	/**
	 * Creates and initializes a new instance with the specified number of ticks.
	 *
	 * @param inTicks The ticks representing the date and time.
	 */
    explicit DateTime(int64 inTicks, ETimeKind Kind = ETimeKind::Local);

	/**
	 * Creates and initializes a new instance with the specified year, month, day, hour, minute, second and millisecond.
	 *
	 * @param year The year.
	 * @param month The month.
	 * @param day The day.
	 * @param hour The hour (optional).
	 * @param minute The minute (optional).
	 * @param second The second (optional).
	 * @param millisecond The millisecond (optional).
     * @param Kind One of the enumeration values that indicates whether ticks specifies a local
     *             time, Coordinated Universal Time (UTC), or neither.
	 */
	DateTime( int32 year, int32 month, int32 day, int32 hour = 0, int32 minute = 0, int32 second = 0, int32 millisecond = 0, ETimeKind Kind = ETimeKind::Local);

public:

	/**
	 * Returns result of adding the given time span to this date.
	 *
	 * @return A date whose value is the sum of this date and the given time span.
	 * @see TimeSpan
	 */
	DateTime operator+( const TimeSpan& other) const
	{
        return DateTime(Ticks + other.GetTicks(),GetTimeKind());
	}

	/**
	 * Adds the given time span to this date.
	 *
	 * @return This date.
	 * @see TimeSpan
	 */
    DateTime& operator+=(const TimeSpan& other)
	{
        Ticks += other.GetTicks();
		return *this;
	}

	/**
	 * Returns time span between this date and the given date.
	 *
	 * @return A time span whose value is the difference of this date and the given date.
	 * @see TimeSpan
	 */
    TimeSpan operator-(const DateTime& other) const
	{
        return TimeSpan(Ticks - other.GetTicks());
	}

	/**
	 * Returns result of subtracting the given time span from this date.
	 *
	 * @return A date whose value is the difference of this date and the given time span.
	 * @see TimeSpan
	 */
    DateTime operator-(const TimeSpan& other) const
	{
		return DateTime(Ticks - other.GetTicks(), GetTimeKind());
	}

	/**
	 * Subtracts the given time span from this date.
	 *
	 * @return This date.
	 * @see TimeSpan
	 */
    DateTime& operator-=(const TimeSpan& other)
	{
		Ticks -= other.GetTicks();
		return *this;
	}

	/**
	 * Compares this date with the given date for equality.
	 *
	 * @param other The date to compare with.
	 * @return true if the dates are equal, false otherwise.
	 */
    bool operator==(const DateTime& other) const
	{
        return (Ticks == other.Ticks);
	}

	/**
	 * Compares this date with the given date for inequality.
	 *
	 * @param other The date to compare with.
	 * @return true if the dates are not equal, false otherwise.
	 */
	bool operator!=( const DateTime& other ) const
	{
		return (Ticks != other.Ticks);
	}

	/**
	 * Checks whether this date is greater than the given date.
	 *
	 * @param other The date to compare with.
	 * @return true if this date is greater, false otherwise.
	 */
	bool operator>( const DateTime& other ) const
	{
		return (Ticks > other.Ticks);
	}

	/**
	 * Checks whether this date is greater than or equal to the date span.
	 *
	 * @param other The date to compare with.
	 * @return true if this date is greater or equal, false otherwise.
	 */
	bool operator>=( const DateTime& other ) const
	{
		return (Ticks >= other.Ticks);
	}

	/**
	 * Checks whether this date is less than the given date.
	 *
	 * @param other The date to compare with.
	 * @return true if this date is less, false otherwise.
	 */
	bool operator<( const DateTime& other ) const
	{
		return (Ticks < other.Ticks);
	}

	/**
	 * Checks whether this date is less than or equal to the given date.
	 *
	 * @param other The date to compare with.
	 * @return true if this date is less or equal, false otherwise.
	 */
	bool operator<=( const DateTime& other ) const
	{
		return (Ticks <= other.Ticks);
	}

public:
    /**
     * Gets a value that indicates whether the time represented by this instance is
     * based on local time, Coordinated Universal Time (UTC), or neither.
     */
    ETimeKind GetTimeKind()const
    {
        return TimeKind;
    }

	/**
	 * Gets the date part of this date.
	 *
	 * The time part is truncated and becomes 00:00:00.000.
	 *
	 * @return A DateTime object containing the date.
	 */
	DateTime GetDate() const
	{
		return DateTime(Ticks - (Ticks % Time::TicksPerDay),GetTimeKind());
	}

	/**
	 * Gets the date components of this date.
	 *
	 * @param outYear  Will contain the year.
	 * @param outMonth Will contain the number of the month (1-12).
	 * @param outDay   Will contain the number of the day (1-31).
	 */
	void GetDate( int32& outYear, int32& outMonth, int32& outDay ) const;


    /**
     * Gets this date's year part.
     *
     * @return The year.
     * @see GetDay, GetHour, GetHour12, GetMillisecond, GetMinute, GetMonth, GetSecond
     */
    int32 GetYear() const;

    /**
     * Gets this date's the month part (1 to 12).
     *
     * @return The month.
     * @see GetDay, GetHour, GetHour12, GetMillisecond, GetMinute, GetSecond, GetYear
     */
    int32 GetMonth() const;

    /**
     * Gets the date's month of the year (January to December).
     *
     * @return Month of year.
     * @see GetDayOfWeek, GetDayOfYear, GetTimeOfDay
     */
    EMonthOfYear GetMonthOfYear() const
    {
        return static_cast<EMonthOfYear>(GetMonth());
    }

	/**
	 * Gets this date's day part (1 to 31).
	 *
	 * @return Day of the month.
	 * @see GetHour, GetHour12, GetMillisecond, GetMinute, GetMonth,GetSecond, GetYear
	 */
	int32 GetDay() const;

	/**
	 * Calculates this date's day of the week (Sunday - Saturday).
	 *
	 * @return The week day.
	 * @see GetDayOfYear, GetMonthOfYear, GetTimeOfDay
	 */
	EDayOfWeek GetDayOfWeek() const;

	/**
	 * Gets this date's day of the year.
	 *
	 * @return The day of year.
	 * @see GetDayOfWeek, GetMonthOfYear, GetTimeOfDay
	 */
	int32 GetDayOfYear() const;

    /**
     * Returns the Julian Day for this date.
     *
     * The Julian Day is the number of days since the inception of the Julian calendar at noon on
     * Monday, January 1, 4713 B.C.E. The minimum Julian Day that can be represented in DateTime is
     * 1721425.5, which corresponds to Monday, January 1, 0001 in the Gregorian calendar.
     *
     * @return Julian Day.
     * @see FromJulianDay, GetModifiedJulianDay
     */
    double GetJulianDay() const
    {
        return (double)(1721425.5 + Ticks / Time::TicksPerDay);
    }

    /**
     * Returns the Modified Julian day.
     *
     * The Modified Julian Day is calculated by subtracting 2400000.5, which corresponds to midnight UTC on
     * November 17, 1858 in the Gregorian calendar.
     *
     * @return Modified Julian Day
     * @see GetJulianDay
     */
    double GetModifiedJulianDay() const
    {
        return (GetJulianDay() - 2400000.5);
    }

	/**
	 * Gets this date's hour part in 24-hour clock format (0 to 23).
	 *
	 * @return The hour.
	 * @see GetDay, GetDayOfWeek, GetDayOfYear, GetHour12, GetMillisecond, GetMinute, GetMonth, GetSecond, GetYear
	 */
	int32 GetHour() const
	{
		return (int32)((Ticks / Time::TicksPerHour) % 24);
	}

	/**
	 * Gets this date's hour part in 12-hour clock format (1 to 12).
	 *
	 * @return The hour in AM/PM format.
	 * @see GetDay, GetHour, GetMillisecond, GetMinute, GetMonth, GetSecond, GetYear
	 */
	int32 GetHour12() const;

    /**
     * Gets this date's minute part (0 to 59).
     *
     * @return The minute.
     * @see GetDay, GetHour, GetHour12, GetMillisecond, GetMonth, GetSecond, GetYear
     */
    int32 GetMinute() const
    {
        return (int32)((Ticks / Time::TicksPerMinute) % 60);
    }

    /**
     * Gets this date's second part.
     *
     * @return The second.
     * @see GetDay, GetHour, GetHour12, GetMillisecond, GetMinute, GetMonth, GetYear
     */
    int32 GetSecond() const
    {
        return (int32)((Ticks / Time::TicksPerSecond) % 60);
    }

	/**
	 * Gets this date's millisecond part (0 to 999).
	 *
	 * @return The millisecond.
	 * @see GetDay, GetHour, GetHour12, GetMinute, GetMonth, GetSecond, GetYear
	 */
	int32 GetMillisecond() const
	{
		return (int32)((Ticks / Time::TicksPerMillisecond) % 1000);
	}

	/**
	 * Gets this date's representation as number of ticks.
	 *
	 * @return Number of ticks since midnight, January 1, 0001.
	 */
	int64 GetTicks() const
	{
		return Ticks;
	}

	/**
	 * Gets the time elapsed since midnight of this date.
	 *
	 * @see GetDayOfWeek, GetDayOfYear, GetMonthOfYear
	 */
	TimeSpan GetTimeOfDay() const
	{
		return TimeSpan(Ticks % Time::TicksPerDay);
	}

	/**
	 * Gets whether this date's time is in the afternoon.
	 *
	 * @return true if it is in the afternoon, false otherwise.
	 * @see IsMorning
	 */
	bool IsAfternoon() const
	{
		return (GetHour() >= 12);
	}

	/**
	 * Gets whether this date's time is in the morning.
	 *
	 * @return true if it is in the morning, false otherwise.
	 * @see IsAfternoon
	 */
	bool IsMorning() const
	{
		return (GetHour() < 12);
	}

    /**
     * Returns a value indicating whether the specified date and time is within a daylight
     * saving time period.
     *
     */
    bool IsDaylightSavingTime();

	/**
	 * Returns the string representation of this date using a default format.
	 *
	 * The returned string has the following format:
	 *		yyyy.mm.dd-hh.mm.ss
	 *
	 * @return String representation.
	 */
    std::string ToString() const;

	/**
	 * Returns the string representation of this date.
	 *
     * The format string is used as a template to format the date and
     * is copied character by character except for the following special characters,
     * which are replaced by the corresponding value.
     *
     * %w - abbreviated weekday (Mon, Tue, ...)
     * %b - abbreviated month (Jan, Feb, ...)
     * %B - full month (January, February, ...)
     * %d - zero-padded day of month (01 .. 31)
     * %e - day of month (1 .. 31)
     * %f - space-padded day of month ( 1 .. 31)
     * %m - zero-padded month (01 .. 12)
     * %n - month (1 .. 12)
     * %o - space-padded month ( 1 .. 12)
     * %y - year without century (70)
     * %Y - year with century (1970)
     * %H - hour (00 .. 23)
     * %h - hour (00 .. 12)
     * %a - am/pm
     * %A - AM/PM
     * %M - minute (00 .. 59)
     * %S - second (00 .. 59)
     * %s - millisecond (000 .. 999)
     * %z - time zone differential in ISO 8601 format (Z or +NN.NN)
     * %Z - time zone differential in RFC format (GMT or +NNNN)
     * %% - percent sign
     * @param InFormat The format of the returned string.
     * @return String representation.
     */
	std::string ToString( const std::string& InFormat ) const;

	/**
	 * Returns this date as the number of seconds since the Unix Epoch (January 1st of 1970).
	 *
	 * @return Time of day.
	 * @see FromUnixTimestamp, ToTimestamp
	 */
    int64 ToUnixTimestamp() const
    {
        //return (Ticks - DateTime(1970, 1, 1).Ticks) / Time::TicksPerSecond;
        return (Ticks - Time::UnixTimeEra) / Time::TicksPerSecond;
    }

    /**
     * Returns this date as the number of seconds since the January 1st of 0001
     *
     * @return Time of day.
     * @see ToUnixTimestamp
     */
    int64 ToTimestamp() const
    {
        return Ticks / Time::TicksPerSecond;
    }

    /**
     * Converts the value of the current DateTime object to local time.
     * local time = UTC + UTC Offset + DST Offset
     * @return A local date time.
     *
     */
    DateTime ToLocalTime();

    /**
     * Converts the value of the current System.DateTime object to 
     * Coordinated Universal Time (UTC).
     * UTC time = local time - UTC Offset - DST Offset
     * @return A Coordinated Universal Time (UTC) time.
     */
    DateTime ToUTCTime();

    /**
     * Gets the hash for the specified date and time.
     *
     * @return Hash value.
     */
    size_t GetHash()
    {
        return std::hash<int64>()(this->Ticks);
    }
public:

	/**
	 * Gets the number of days in the year and month.
	 *
	 * @param year The year.
	 * @param month The month.
	 * @return The number of days
	 * @see DaysInYear
	 */
	static int32 DaysInMonth( int32 year, int32 month );

	/**
	 * Gets the number of days in the given year.
	 *
	 * @param year The year.
	 * @return The number of days.
	 * @see DaysInMonth
	 */
	static int32 DaysInYear( int32 year );

	/**
	 * Returns the proleptic Gregorian date for the given Julian Day.
	 *
	 * @param julianDay The Julian Day.
	 * @return Gregorian date and time.
	 * @see GetJulianDay
	 */
	static DateTime FromJulianDay( double julianDay, ETimeKind Kind = ETimeKind::Local)
	{
        return DateTime((int64)((julianDay - 1721425.5) * Time::TicksPerDay), Kind);
	}

	/**
	 * Returns the date from Unix time (seconds from midnight 1970-01-01)
	 *
     * @param InUnixTime Unix time (seconds from midnight 1970-01-01)
	 * @return Gregorian date and time.
	 * @see ToUnixTimestamp
	 */
	static DateTime FromUnixTimestamp( int64 InUnixTime, ETimeKind Kind = ETimeKind::Local)
	{
        //return DateTime(1970, 1, 1,0,0,0,0,Kind) + TimeSpan(InUnixTime * Time::TicksPerSecond);
        return DateTime(InUnixTime * Time::TicksPerSecond + Time::UnixTimeEra, Kind);
	}

	/**
	 * Checks whether the given year is a leap year.
	 *
	 * A leap year is a year containing one additional day in order to keep the calendar synchronized
	 * with the astronomical year. All years divisible by 4, but not divisible by 100 - except if they
	 * are also divisible by 400 - are leap years.
	 *
	 * @param year The year to check.
	 * @return true if the year is a leap year, false otherwise.
	 */
	static bool IsLeapYear( int32 year );

	/**
	 * Returns the maximum date value.
	 *
	 * The maximum date value is December 31, 9999, 23:59:59.9999999.
	 *
	 * @see MinValue
	 */
    static DateTime MaxValue();

	/**
	 * Returns the minimum date value.
	 *
	 * The minimum date value is January 1, 0001, 00:00:00.0.
	 *
	 * @see MaxValue
	 */
    static DateTime MinValue();

	/**
	 * Gets the local date and time on this computer.
	 *
	 * This method takes into account the local computer's time zone and daylight saving
	 * settings. For time zone independent time comparisons, and when comparing times
	 * between different computers, please use UtcNow() instead.
	 *
	 * @return Current date and time.
	 * @see Today, UTCNow
	 */
	static DateTime Now();

    /**
     * Gets the UTC date and time on this computer.
     *
     * This method returns the Coordinated Universal Time (UTC), which does not take the
     * local computer's time zone and daylight savings settings into account. It should be
     * used when comparing dates and times that should be independent of the user's locale.
     * To get the date and time in the current locale, use Now() instead.
     *
     * @return Current date and time.
     * @see Now
     */
    static DateTime UTCNow();

    /**
     * Gets the local date on this computer.
     *
     * The time component is set to 00:00:00
     *
     * @return Current date.
     * @see Now, UTCNow
     */
    static DateTime Today()
    {
        return Now().GetDate();
    }

    /**
     * Get the system date
     *
     * @return Date string
     */
    static std::string GetDateString();

    /**
     * Get the system time
     *
     * @return Time string
     */
    static std::string GetTimeString();

    /**
     * Returns a timestamp string built from the current date and time.
     * NOTE: Only one return value is valid at a time!
     *
     * @return timestamp string
     */
    static std::string GetTimestampString();

    /**
     * Get the local date and time on this computer
     */
    static void LocalTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec);

    /**
     * Get the UTC date and time on this computer
     */
    static void UTCTime(int32& year, int32& month, int32& dayOfWeek, int32& day, int32& hour, int32& min, int32& sec, int32& msec);

	/**
	 * Converts a string to a date and time.
	 *
	 * Currently, the string must be in the format written by either DateTime.ToString() or
	 * FTimeStamp.TimestampToFString(). Other formats are not supported at this time.
	 *
	 * @param dateTimeString The string to convert.
	 * @param outDateTime Will contain the parsed date and time.
	 * @return true if the string was converted successfully, false otherwise.
	 */
	static bool Parse( const std::string& dateTimeString, DateTime& outDateTime, ETimeKind Kind);

	/**
	 * Parses a date string in ISO-8601 format.
	 * 
	 * @param dateTimeString The string to be parsed
	 * @param outDateTime DateTime object (in UTC) corresponding to the input string (which may have been in any timezone).
	 * @return true if the string was converted successfully, false otherwise.
	 * @see Parse, ToISO8601
	 */
	static bool ParseISO8601( const std::string& dateTimeString, DateTime& outDateTime);

	/**
	 * Validates the given components of a date and time value.
	 *
	 * The allow ranges for the components are:
	 *		year: 1 - 9999
	 *		month: 1 - 12
	 *		day: 1 - daysInMonth(month)
	 *		hour: 0 - 23
	 *		minute: 0 - 59
	 *		second: 0 - 59
	 *		millisecond: 0 - 999
	 *
	 * @return true if the components are valid, false otherwise.
	 */
	static bool Validate( int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond );
protected:

    void TZDISO(std::string& OutputStr, int TimeZoneDifferential)const;
    void TZDRFC(std::string& OutputStr, int TimeZoneDifferential)const;
	/** Holds the days per month in a non-leap year. */
	static const int32 DaysPerMonth[];

	/** Holds the cumulative days per month in a non-leap year. */
	static const int32 DaysToMonth[];

private:

	/** Holds the ticks in 100 nanoseconds resolution since January 1, 0001 A.D. */
    int64     Ticks;
    ETimeKind TimeKind;
    friend std::ostream& operator<< (std::ostream& stream, const DateTime& datetime);
};

inline std::ostream& operator<< (std::ostream& stream, const DateTime& datetime)
{
    stream << datetime.ToString();
    return stream;
}

NS_FK_END


#endif // FOUNDATIONKIT_DATETIME_HPP
