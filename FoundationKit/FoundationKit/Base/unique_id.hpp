#ifndef FOUNDATIONKIT_UNIQUE_ID_HPP
#define FOUNDATIONKIT_UNIQUE_ID_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include <atomic>
#if PLATFORM_WINDOWS
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <mutex>

NS_FK_BEGIN
// https://github.com/beyondfengyu/SnowFlake/blob/master/SnowFlake.java
class unique_id
{
public:
    unique_id()
        : CustomId(0)
        , WorkerId(0)
        , StartTimeStamp(GetTimeStamp())
    {
        SequenceId = 0;
        LastTimeStamp = -1;
    }
    static const int64 WORKERID_BITS = 5;
    static const int64 CUSTOMID_BITS = 5;
    static const int64 SEQUENCE_BITS = 12;

    static const int64 MAX_CUSTOMID = -1L ^ (-1L << CUSTOMID_BITS);
    static const int64 MAX_WORKERID = -1L ^ (-1L << WORKERID_BITS);

    static const int64 WORKERID_SHIFT = SEQUENCE_BITS;
    static const int64 CUSTOMID_SHIFT = SEQUENCE_BITS + WORKERID_BITS;
    static const int64 TIMESTMP_SHIFT = SEQUENCE_BITS + WORKERID_BITS + CUSTOMID_BITS;

    static const uint64 SEQUENCE_MASK = -1L ^ (-1L << SEQUENCE_BITS);

    bool SetLogo(int64 workerid, int64 customid)
    {
        if (workerid > MAX_WORKERID || workerid < 0 || customid > MAX_CUSTOMID || customid < 0)
            return false;
        WorkerId = workerid;
        CustomId = customid;
        return true;
    }

    bool SetStartTimeStamp(int64 timestamp)
    {
        if (timestamp < 0)
            return false;
        StartTimeStamp = timestamp;
        return true;
    }

    int64 Generate()
	{
        std::lock_guard<std::mutex> lock(IdMutex);
        int64 nowtimestamp = GetTimeStamp();
        if (nowtimestamp < LastTimeStamp)
            throw std::runtime_error("Clock moved backwards, refusing to generate id.");
        if (nowtimestamp == LastTimeStamp)
        {
            SequenceId = (SequenceId + 1) & SEQUENCE_MASK;
            if (SequenceId == 0L)
                nowtimestamp = WaitNextTimeStamp(LastTimeStamp);
        }
        else
        {
            SequenceId = 0L;
        }

        LastTimeStamp = nowtimestamp;
        return (nowtimestamp - StartTimeStamp) << TIMESTMP_SHIFT
            | CustomId << CUSTOMID_SHIFT
            | WorkerId << WORKERID_SHIFT
            | SequenceId;
	}

protected:
    int64 CustomId;
    int64 WorkerId;
    int64 SequenceId;
    int64 StartTimeStamp;
    int64 LastTimeStamp;
    std::mutex IdMutex;

    int64 GetTimeStamp()
	{
#if PLATFORM_WINDOWS
		FILETIME filetime;
        int64 time = 0;
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
        int64 time = tv.tv_usec;
		time /= 1000;
		time += (tv.tv_sec * 1000);
		return time;
#endif
	}

    int64 WaitNextTimeStamp(int64 laststamp)
	{
        int64 nowtimestamp = GetTimeStamp();
        while (nowtimestamp <= laststamp)
		{
            nowtimestamp = GetTimeStamp();
		}
		return nowtimestamp;
	}
};

NS_FK_END
#endif // END OF FOUNDATIONKIT_UNIQUE_ID_HPP
