/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TIMESPAN_HPP
#define FOUNDATIONKIT_TIMESPAN_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Foundation/Time.hpp"

NS_FK_BEGIN

/**
 * Implements a time span.
 *
 * A time span is the difference between two dates and times. For example, the time span between
 * 12:00:00 January 1, 2000 and 18:00:00 January 2, 2000 is 30.0 hours. Time spans are measured in
 * positive or negative ticks depending on whether the difference is measured forward or backward.
 * Each tick has a resolution of 0.1 microseconds (= 100 nanoseconds).
 *
 * In conjunction with the companion class FDateTime, time spans can be used to perform date and time
 * based arithmetic, such as calculating the difference between two dates or adding a certain amount
 * of time to a given date.
 *
 * @see DateTime
 */
class TimeSpan final
{
public:

	/** Default constructor (no initialization). */
    TimeSpan() { }

	/**
	 * Creates and initializes a new time interval with the specified number of ticks.
	 *
	 * @param inTicks The number of ticks.
	 */
    explicit TimeSpan(int64 inTicks)
        : Ticks(inTicks)
	{ }

	/**
	 * Creates and initializes a new time interval with the specified number of hours, minutes and seconds.
	 *
	 * @param hours   The hours component.
	 * @param minutes The minutes component.
	 * @param seconds The seconds component.
	 */
    TimeSpan(int32 hours, int32 minutes, int32 seconds)
	{
        Assign(0, hours, minutes, seconds, 0);
	}

	/**
	 * Creates and initializes a new time interval with the specified number of days, hours, minutes and seconds.
	 *
	 * @param days    The days component.
	 * @param hours   The hours component.
	 * @param minutes The minutes component.
	 * @param seconds The seconds component.
	 */
    TimeSpan(int32 days, int32 hours, int32 minutes, int32 seconds)
	{
        Assign(days, hours, minutes, seconds, 0);
	}

	/**
	 * Creates and initializes a new time interval with the specified number of days, hours, minutes and seconds.
	 *
	 * @param days         The days component.
	 * @param hours        The hours component.
	 * @param minutes      The minutes component.
	 * @param seconds      The seconds component.
	 * @param milliseconds The milliseconds component.
	 */
    TimeSpan(int32 days, int32 hours, int32 minutes, int32 seconds, int32 milliseconds)
	{
        Assign(days, hours, minutes, seconds, milliseconds);
	}

public:

	/**
	 * Returns result of adding the given time span to this time span.
	 *
	 * @return A time span whose value is the sum of this time span and the given time span.
	 */
    TimeSpan operator+(const TimeSpan& other) const
	{
        return TimeSpan(Ticks + other.Ticks);
	}

	/**
	 * Adds the given time span to this time span.
	 *
	 * @return This time span.
	 */
    TimeSpan& operator+=(const TimeSpan& other)
	{
        Ticks += other.Ticks;
		return *this;
	}

	/**
	 * Returns the inverse of this time span.
	 *
	 * The value of this time span must be greater than TimeSpan::MinValue(), or else an overflow will occur.
	 *
	 * @return Inverse of this time span.
	 */
    TimeSpan operator-() const
	{
        return TimeSpan(-Ticks);
	}

	/**
	 * Returns the result of subtracting the given time span from this time span.
	 *
	 * @param other The time span to compare with.
	 * @return A time span whose value is the difference of this time span and the given time span.
	 */
    TimeSpan operator-(const TimeSpan& other) const
	{
        return TimeSpan(Ticks - other.Ticks);
	}

	/**
	 * Subtracts the given time span from this time span.
	 *
	 * @param other The time span to subtract.
	 * @return This time span.
	 */
    TimeSpan& operator-=(const TimeSpan& other)
	{
        Ticks -= other.Ticks;
		return *this;
	}

	/**
	 * Returns the result of multiplying the this time span with the given scalar.
	 *
	 * @param scalar The scalar to multiply with.
	 * @return A time span whose value is the product of this time span and the given scalar.
	 */
    TimeSpan operator*(float scalar) const
	{
        return TimeSpan(static_cast<int64>(Ticks * scalar));
	}

	/**
	 * Multiplies this time span with the given scalar.
	 *
	 * @param scalar The scalar to multiply with.
	 * @return This time span.
	 */
    TimeSpan& operator*=(float scalar)
	{
        Ticks = (int64)(Ticks * scalar);
		return *this;
	}

	/**
	 * Compares this time span with the given time span for equality.
	 *
	 * @param other The time span to compare with.
	 * @return true if the time spans are equal, false otherwise.
	 */
    bool operator==(const TimeSpan& other) const
	{
        return (Ticks == other.Ticks);
	}

	/**
	 * Compares this time span with the given time span for inequality.
	 *
	 * @param other The time span to compare with.
	 * @return true if the time spans are not equal, false otherwise.
	 */
    bool operator!=(const TimeSpan& other) const
	{
        return (Ticks != other.Ticks);
	}

	/**
	 * Checks whether this time span is greater than the given time span.
	 *
	 * @param other The time span to compare with.
	 * @return true if this time span is greater, false otherwise.
	 */
    bool operator>(const TimeSpan& other) const
	{
        return (Ticks > other.Ticks);
	}

	/**
	 * Checks whether this time span is greater than or equal to the given time span.
	 *
	 * @param other The time span to compare with.
	 * @return true if this time span is greater or equal, false otherwise.
	 */
    bool operator>=(const TimeSpan& other) const
	{
        return (Ticks >= other.Ticks);
	}

	/**
	 * Checks whether this time span is less than the given time span.
	 *
	 * @param other The time span to compare with.
	 * @return true if this time span is less, false otherwise.
	 */
    bool operator<(const TimeSpan& other) const
	{
        return (Ticks < other.Ticks);
	}

	/**
	 * Checks whether this time span is less than or equal to the given time span.
	 *
	 * @param other The time span to compare with.
	 * @return true if this time span is less or equal, false otherwise.
	 */
	bool operator<=( const TimeSpan& other ) const
	{
        return (Ticks <= other.Ticks);
	}

public:

	/**
	 * Returns a time span with the absolute value of this time span.
	 *
	 * This method may overflow the timespan if its value is equal to MinValue.
	 *
	 * @return Duration of this time span.
	 * @see minValue
	 */
	TimeSpan GetDuration()
	{
        return TimeSpan(Ticks >= 0 ? Ticks : -Ticks);
	}

   /**
    * Gets the days component of this time span.
    *
    * @return Days component.
    */
    int32 GetDays() const
    {
        return static_cast<int32>(Ticks / Time::TicksPerDay);
    }

	/**
	 * Gets the hours component of this time span.
	 *
	 * @return Hours component.
	 * @see getTotalHours
	 */
	int32 GetHours() const
	{
        return static_cast<int32>((Ticks / Time::TicksPerHour) % 24);
	}

   /**
    * Gets the minutes component of this time span.
    *
    * @return Minutes component.
    * @see getTotalMinutes
    */
    int32 GetMinutes() const
    {
        return static_cast<int32>((Ticks / Time::TicksPerMinute) % 60);
    }

   /**
    * Gets the seconds component of this time span.
    *
    * @return Seconds component.
    * @see getTotalSeconds
    */
    int32 GetSeconds() const
    {
        return static_cast<int32>((Ticks / Time::TicksPerSecond) % 60);
    }

	/**
	 * Gets the milliseconds component of this time span.
	 *
	 * @return Milliseconds component.
	 * @see getTotalMilliseconds
	 */
	int32 GetMilliseconds() const
	{
        return static_cast<int32>((Ticks / Time::TicksPerMillisecond) % 1000);
	}

	/**
	 * Gets the number of ticks represented by this time span.
	 *
	 * @return Number of ticks.
	 */
	int64 GetTicks() const
	{
		return Ticks;
	}

	/**
	 * Gets the total number of days represented by this time span.
	 *
	 * @return Number of days.
	 * @see getDays
	 */
	double GetTotalDays() const
	{
        return static_cast<double>(Ticks / Time::TicksPerDay);
	}

	/**
	 * Gets the total number of hours represented by this time span.
	 *
	 * @return Number of hours.
	 * @see getHours
	 */
	double GetTotalHours() const
	{
        return static_cast<double>(Ticks / Time::TicksPerHour);
	}

	/**
	 * Gets the total number of milliseconds represented by this time span.
	 *
	 * @return Number of milliseconds.
	 * @see getMilliseconds
	 */
	double GetTotalMilliseconds() const
	{
        return static_cast<double>(Ticks / Time::TicksPerMillisecond);
	}

	/**
	 * Gets the total number of minutes represented by this time span.
	 *
	 * @return Number of minutes.
	 * @see getMinutes
	 */
	double GetTotalMinutes() const
	{
        return static_cast<double>(Ticks / Time::TicksPerMinute);
	}

	/**
	 * Gets the total number of seconds represented by this time span.
	 *
	 * @return Number of seconds.
	 * @see getSeconds
	 */
	double GetTotalSeconds() const
	{
        return static_cast<double>(Ticks / Time::TicksPerSecond);
	}

	/**
	 * Returns the string representation of this time span using a default format.
	 *
	 * The returned string has the following format:
	 *		[-][d.]hh:mm:ss.fff
	 *
	 * @return String representation.
	 * @see parse
	 */
	std::string ToString() const;

	/**
	 * Converts this time span to its string representation.
	 *
	 * The following formatting codes are available:
	 *		%n - prints the minus sign (for negative time spans only)
	 *		%N - prints the minus or plus sign (always)
	 *		%d - prints the time span's days part
	 *		%h - prints the time span's hours part (0..23)
	 *		%m - prints the time span's minutes part (0..59)
	 *		%s - prints the time span's seconds part (0..59)
	 *		%f - prints the time span's milliseconds part (0..999)
	 *		%D - prints the total number of days (without minus sign)
	 *		%H - prints the total number of hours (without minus sign)
	 *		%M - prints the total number of minutes (without minus sign)
	 *		%S - prints the total number of seconds (without minus sign)
	 *		%F - prints the total number of milliseconds (without minus sign)
	 *
	 * @param format - The format of the returned string.
	 * @return String representation.
	 * @see parse
	 */
	 std::string ToString( const char* format ) const;

public:

	/**
	 * Creates a time span that represents the specified number of days.
	 *
	 * @param days The number of days.
	 * @return Time span.
	 * @see fromHours, fromMilliseconds, fromMinutes, fromSeconds
	 */
	static TimeSpan FromDays( double days );

	/**
	 * Creates a time span that represents the specified number of hours.
	 *
	 * @param hours The number of hours.
	 * @return Time span.
	 * @see fromDays, fromMilliseconds, fromMinutes, fromSeconds
	 */
	static TimeSpan FromHours( double hours );

   /**
    * Creates a time span that represents the specified number of minutes.
    *
    * @param minutes The number of minutes.
    * @return Time span.
    * @see fromDays, fromHours, fromMilliseconds, fromSeconds
    */
    static TimeSpan FromMinutes(double minutes);

   /**
    * Creates a time span that represents the specified number of seconds.
    *
    * @param seconds The number of seconds.
    * @return Time span.
    * @see fromDays, fromHours, fromMilliseconds, fromMinutes
    */
    static TimeSpan FromSeconds(double seconds);

	/**
	 * Creates a time span that represents the specified number of milliseconds.
	 *
	 * @param milliseconds The number of milliseconds.
	 * @return Time span.
	 * @see fromDays, fromHours, fromMinutes, fromSeconds
	 */
	static TimeSpan FromMilliseconds( double milliseconds );

	/**
	 * Returns the maximum time span value.
	 *
	 * The maximum time span value is slightly more than 10,675,199 days.
	 *
	 * @return Maximum time span.
	 * @see minValue,Zero
	 */
	static TimeSpan MaxValue()
	{
		return TimeSpan(9223372036854775807);
	}

	/**
	 * Returns the minimum time span value.
	 *
	 * The minimum time span value is slightly less than -10,675,199 days.
	 *
	 * @return Minimum time span.
	 * @see maxValue, zeroValue
	 */
	static TimeSpan MinValue()
	{
		return TimeSpan(-9223372036854775807 - 1);
	}

	/**
	 * Converts a string to a time span.
	 *
	 * Currently, the string must be in the format written by TimeSpan.ToString().
	 * Other formats are not supported at this time.
	 *
	 * @param timespanString The string to convert.
	 * @param outTimespan Will contain the parsed time span.
	 * @return true if the string was converted successfully, false otherwise.
	 * @see ToString
	 */
	static bool Parse( const std::string& timespanString, TimeSpan& outTimespan );

	/**
	 * Returns the zero time span value.
	 *
	 * The zero time span value can be used in comparison operations with other time spans.
	 *
	 * @return Zero time span.
	 * @see MaxValue, MinValue
	 */
	static TimeSpan Zero()
	{
		return TimeSpan(0);
	}

	/**
	 * Gets the hash for the specified time span.
	 *
	 * @return Hash value.
	 */
	size_t GetHash()
	{
        return std::hash<int64>()(this->Ticks);
	}

protected:

	/**
	 * Assigns the specified components to this time span.
	 *
	 * @param days         The days component.
	 * @param hours        The hours component.
	 * @param minutes      The minutes component.
	 * @param seconds      The seconds component.
	 * @param milliseconds The milliseconds component.
	 */
	void Assign( int32 days, int32 hours, int32 minutes, int32 seconds, int32 milliseconds );

private:

	/** Holds the time span in 100 nanoseconds resolution. */
	int64 Ticks;
};


/**
 * Pre-multiplies a time span with the given scalar.
 *
 * @param scalar The scalar to pre-multiply with.
 * @param timespan The time span to multiply.
 */
inline TimeSpan operator*( float scalar, const TimeSpan& timespan )
{
    return timespan.operator*(scalar);
}


NS_FK_END
#endif // FOUNDATIONKIT_TIMESPAN_HPP
