/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef _SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

#include <windows.h>
#include <psapi.h>

#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN (-1)
#define RUSAGE_BOTH (-2)
#define RUSAGE_THREAD 1 

struct rusage {
    struct timeval ru_utime; /* user CPU time used */
    struct timeval ru_stime; /* system CPU time used */
    long   ru_maxrss;        /* maximum resident set size */
    long   ru_ixrss;         /* integral shared memory size */
    long   ru_idrss;         /* integral unshared data size */
    long   ru_isrss;         /* integral unshared stack size */
    long   ru_minflt;        /* page reclaims (soft page faults) */
    long   ru_majflt;        /* page faults (hard page faults) */
    long   ru_nswap;         /* swaps */
    long   ru_inblock;       /* block input operations */
    long   ru_oublock;       /* block output operations */
    long   ru_msgsnd;        /* IPC messages sent */
    long   ru_msgrcv;        /* IPC messages received */
    long   ru_nsignals;      /* signals received */
    long   ru_nvcsw;         /* voluntary context switches */
    long   ru_nivcsw;        /* involuntary context switches */
};


static void usage_to_timeval(FILETIME *ft, struct timeval *tv)
{
    ULARGE_INTEGER time;
    time.LowPart = ft->dwLowDateTime;
    time.HighPart = ft->dwHighDateTime;

    tv->tv_sec = static_cast<long>(time.QuadPart / 10000000);
    tv->tv_usec = (time.QuadPart % 10000000) / 10;
}

int getrusage(int who, rusage *usage)
{
    FILETIME creation_time, exit_time, kernel_time, user_time;
    PROCESS_MEMORY_COUNTERS pmc;

    memset(usage, 0, sizeof(struct rusage));

    if (who == RUSAGE_SELF)
    {
        if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
            &kernel_time, &user_time))
        {
            return -1;
        }

        if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        {
            return -1;
        }

        usage_to_timeval(&kernel_time, &usage->ru_stime);
        usage_to_timeval(&user_time, &usage->ru_utime);
        usage->ru_majflt = pmc.PageFaultCount;
        usage->ru_maxrss = pmc.PeakWorkingSetSize / 1024;
        return 0;
    }
    else if (who == RUSAGE_THREAD)
    {
        if (!GetThreadTimes(GetCurrentThread(), &creation_time, &exit_time,
            &kernel_time, &user_time))
        {
            return -1;
        }
        usage_to_timeval(&kernel_time, &usage->ru_stime);
        usage_to_timeval(&user_time, &usage->ru_utime);
        return 0;
    }
    else
    {
        return -1;
    }
}


#endif /* _SYS_RESOURCE_H_ */


