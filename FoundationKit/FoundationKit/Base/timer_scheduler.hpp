#ifndef LOSEMYMIND_TIMER_SCHEDULER_H
#define LOSEMYMIND_TIMER_SCHEDULER_H


#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "FoundationKit/Base/Timer.h"

NS_FK_BEGIN

struct TimerDelegate
{
    /// milliseconds
    float  interval;
    float  timeScale;
    Timer  currentTimer;
};

NS_FK_END










#endif // LOSEMYMIND_TIMER_SCHEDULER_H
