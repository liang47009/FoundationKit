#ifndef FOUNDATIONKIT_UNIQUE_ID_HPP
#define FOUNDATIONKIT_UNIQUE_ID_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/Types.hpp"
#include <atomic>
#if TARGET_PLATFORM == PLATFORM_WINDOWS
#include <time.h>
#else
#include <sys/time.h>
#endif

NS_FK_BEGIN
// https://github.com/beyondfengyu/SnowFlake/blob/master/SnowFlake.java
class unique_id
{
public:
	unique_id() 
	{
		sequence = 0;
		lasttimestamp = 0;
	}
	static uint64 START_TIMESTAMP;
	static const uint64 WORKERID_BIT = 5;
	static const uint64 CUSTOMID_BIT = 5;
	static const uint64 SEQUENCE_BIT = 12;

	static const uint64 MAX_SEQUENCE = -1L ^ (-1L << SEQUENCE_BIT);
	static const uint64 MAX_CUSTOMID = -1L ^ (-1L << CUSTOMID_BIT);
	static const uint64 MAX_WORKERID = -1L ^ (-1L << WORKERID_BIT);

	static const uint64 CUSTOMID_LEFT = SEQUENCE_BIT;
	static const uint64 WORKERID_LEFT = SEQUENCE_BIT + CUSTOMID_BIT;
	static const uint64 TIMESTMP_LEFT = SEQUENCE_BIT + CUSTOMID_BIT + WORKERID_BIT;

	uint64 generate(uint64 workerId, uint64 customId)
	{
		uint64 nowtimestamp = gettimestamp();
		if (nowtimestamp < lasttimestamp)
			throw std::runtime_error("Clock moved backwards, refusing to generate id.");
		if (nowtimestamp == lasttimestamp)
		{
			sequence = (++sequence) & MAX_SEQUENCE;
			if (sequence == 0L)
				nowtimestamp = waitnexttimestamp();
		}
		else
		{
			sequence = 0L;
		}

		lasttimestamp = nowtimestamp;
		return (nowtimestamp - START_TIMESTAMP) << TIMESTMP_LEFT
			| workerId << WORKERID_LEFT
			| customId << CUSTOMID_LEFT
			| sequence;
	}

	uint64 generate()
	{
		return generate(rand(), rand());
	}


protected:
	std::atomic_uint64_t sequence;
	std::atomic_uint64_t lasttimestamp;

	uint64 gettimestamp()
	{
#if TARGET_PLATFORM == PLATFORM_WINDOWS
		FILETIME filetime;
		uint64 time = 0;
		GetSystemTimeAsFileTime(&filetime);
		time |= filetime.dwHighDateTime;
		time <<= 32;
		time |= filetime.dwLowDateTime;
		time /= 10;
		time -= 11644473600000000Ui64;
		time /= 1000;
		return time;
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		uint64 time = tv.tv_usec;
		time /= 1000;
		time += (tv.tv_sec * 1000);
		return time;
#endif
	}

	uint64 waitnexttimestamp()
	{
		uint64 nowtimestamp = gettimestamp();
		while (nowtimestamp <= lasttimestamp)
		{
			nowtimestamp = gettimestamp();
		}
		return nowtimestamp;
	}
};


uint64 unique_id::START_TIMESTAMP = 0;

NS_FK_END
#endif // END OF FOUNDATIONKIT_UNIQUE_ID_HPP
