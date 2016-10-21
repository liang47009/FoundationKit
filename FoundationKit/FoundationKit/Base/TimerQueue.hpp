
#include <set>
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Base/Timer.h"

NS_FK_BEGIN

enum class ETimerStatus : uint8
{
    PAUSED,
    ACTIVE,
};

struct TimerOption
{
    bool  enable;
    int   interval;         //expressed in milliseconds
    float scale;
    int   maximumDeltaTime; //expressed in milliseconds
    int   repeatCount;
    TimerOption(bool e, int i, float s, int m, int r)
        : enable(e)
        , interval(i)
        , scale(s)
        , maximumDeltaTime(m)
        , repeatCount(r)
    {}

    TimerOption() :TimerOption(false, 1000, 1.0f, 100, Timer::repeat_forever){}
};

class TimerQueue
{
public:
    TimerQueue();
    ~TimerQueue();

    int32 enqueue(const TimerOption& timerOption);
    int32 insert();
    void  erase();
};


NS_FK_END


