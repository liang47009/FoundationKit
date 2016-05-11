/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#include "Time.h"
NS_FK_BEGIN

	Time::Time()
		: m_TimeDuration()
	{

	}

	Time::Time(const Time & t)
		: m_TimeDuration(t.m_TimeDuration)
	{

	}

	Time::Time(Time && t)
		: m_TimeDuration(t.m_TimeDuration)
	{

	}

    Time::Time(const std::chrono::high_resolution_clock::time_point& timePoint)
        : m_TimeDuration(timePoint.time_since_epoch())
    {

    }

	Time::~Time()
	{

	}

	Time & Time::operator=(const Time & t)
	{
		m_TimeDuration = t.m_TimeDuration;
		return * this;
	}

	Time & Time::operator=(Time && t)
	{
		m_TimeDuration = t.m_TimeDuration;
		return * this;
	}

	Time & Time::operator+=(const Time & t)
	{
		m_TimeDuration += t.m_TimeDuration;
		return * this;
	}

	Time & Time::operator-=(const Time & t)
	{
		m_TimeDuration -= t.m_TimeDuration;
		return * this;
	}

	Time & Time::operator*=(const std::chrono::high_resolution_clock::duration::rep & t)
	{
		m_TimeDuration *= t;
		return * this;
	}

	Time & Time::operator/=(const std::chrono::high_resolution_clock::duration::rep & t)
	{
		m_TimeDuration /= t;
		return * this;
	}

	Time & Time::operator%=(const std::chrono::high_resolution_clock::duration::rep & t)
	{
		m_TimeDuration %= t;
		return * this;
	}

	Time & Time::operator%=(const Time & t)
	{
		m_TimeDuration %= t.m_TimeDuration;
		return * this;
	}

	Time Time::operator+(const Time & t) const
	{
		Time time(*this);
		time += t;
		return time;
	}

	Time Time::operator-(const Time & t) const
	{
		Time time(*this);
		time -= t;
		return time;
	}

	Time & Time::operator++()
	{
		++m_TimeDuration;
		return * this;
	}

	Time Time::operator++(int32)
	{
		Time time(*this);
		time.m_TimeDuration++;
		return time;
	}

	Time & Time::operator--()
	{
		--m_TimeDuration;
		return * this;
	}

	Time Time::operator--(int32)
	{
		Time time(*this);
		time.m_TimeDuration--;
		return time;
	}

	float64 Time::getHours() const
	{
        return static_cast<float64>(std::chrono::duration_cast<std::chrono::hours> (m_TimeDuration).count());
	}

	float64 Time::getMinutes() const
	{
        return static_cast<float64>(std::chrono::duration_cast<std::chrono::minutes> (m_TimeDuration).count());
	}

	float64 Time::getSeconds() const
	{
        return static_cast<float64>(std::chrono::duration_cast<std::chrono::seconds> (m_TimeDuration).count());
	}

	float64 Time::getMilliSeconds() const
	{
        return static_cast<float64>(std::chrono::duration_cast<std::chrono::milliseconds> (m_TimeDuration).count());
	}

	float64 Time::getMicroSeconds() const
	{
        return static_cast<float64>(std::chrono::duration_cast<std::chrono::microseconds> (m_TimeDuration).count());
	}

	float64 Time::getNanoSeconds() const
	{
        return static_cast<float64>(std::chrono::duration_cast<std::chrono::nanoseconds> (m_TimeDuration).count());
	}

    Time Time::now()
    {
        return Time(std::chrono::high_resolution_clock::now());
    }

NS_FK_END




