#pragma once
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"
#include <chrono>
NS_FK_BEGIN

class Time final
{
public:
	Time();
	Time(const Time & t);
	Time(Time && t);
    Time(const std::chrono::system_clock::time_point& timePoint);

	~Time();
	
	Time & operator=(const Time & t);
	Time & operator=(Time && t);

	Time & operator+=(const Time & t);
	Time & operator-=(const Time & t);
	Time & operator*=(const std::chrono::system_clock::duration::rep & t);
	Time & operator/=(const std::chrono::system_clock::duration::rep & t);
	Time & operator%=(const std::chrono::system_clock::duration::rep & t);
	Time & operator%=(const Time & t);

	Time   operator+(const Time & t) const;
	Time   operator-(const Time & t) const;

	Time & operator++();
	Time   operator++(int32 t);
	Time & operator--();
	Time   operator--(int32 t);

	float64 getHours() const;
	float64 getMinutes() const;
	float64 getSeconds() const;
	float64 getMilliSeconds() const;
	float64 getMicroSeconds() const;
	float64 getNanoSeconds() const;

    static Time now();
private:
	std::chrono::system_clock::duration m_TimeDuration;

};

NS_FK_END



