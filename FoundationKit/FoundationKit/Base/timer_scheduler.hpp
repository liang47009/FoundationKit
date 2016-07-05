
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










