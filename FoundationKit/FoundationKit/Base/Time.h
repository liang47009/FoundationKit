/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_TIME_H
#define LOSEMYMIND_TIME_H

#pragma once
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"
#include <chrono>
NS_FK_BEGIN

class Time final
{
public:
    explicit Time(const Time & t);
	Time();
	Time(Time && t);
    Time(const std::chrono::high_resolution_clock::time_point& timePoint);

	~Time();
	
	Time & operator=(const Time & t);
	Time & operator=(Time && t);

	Time & operator+=(const Time & t);
	Time & operator-=(const Time & t);
	Time & operator*=(const std::chrono::high_resolution_clock::duration::rep & t);
	Time & operator/=(const std::chrono::high_resolution_clock::duration::rep & t);
	Time & operator%=(const std::chrono::high_resolution_clock::duration::rep & t);
	Time & operator%=(const Time & t);

	Time   operator+(const Time & t) const;
	Time   operator-(const Time & t) const;

	Time & operator++();
	Time   operator++(int32 t);
	Time & operator--();
	Time   operator--(int32 t);

    int32 getHours() const;
    int32 getMinutes() const;
    int32 getSeconds() const;
    int32 getMilliSeconds() const;
    int32 getMicroSeconds() const;
    int32 getNanoSeconds() const;

    static Time now();
private:
	std::chrono::high_resolution_clock::duration m_TimeDuration;

};

NS_FK_END



#endif // LOSEMYMIND_TIME_H
