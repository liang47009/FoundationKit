#ifndef LOSEMYMIND_TIMERMANAGER_H
#define LOSEMYMIND_TIMERMANAGER_H


#include <vector>
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Base/ElapsedTimer.h"
#include "FoundationKit/Base/Timer.h"

NS_FK_BEGIN

#define REPEAT_FOREVER (UINT_MAX - 1)


enum class ETimerStatus : uint8
{
    PAUSED,
    ACTIVE,
};

struct ElapsedEventArgs
{
    DateTime signalTime;
    int64    timerId;
};

typedef std::function<void(ElapsedEventArgs*)>   TimerElapsedEvent;

struct TimerData
{
    // Gets or sets a uint32 indicating whether the Timer should raise the elapsed event repeat count.
    uint32                 repeat;

    // Gets or sets the interval, expressed in milliseconds, at which to raise the elapsed event.
    int64                  interval;
    TimerElapsedEvent      elapsed;
    ETimerStatus           state;
    ElapsedTimer           timer;
    int32                  idIndex;
};

class TimerManager
{
public:
    TimerManager();
    ~TimerManager();

    int64 addTimer(const TimerElapsedEvent& callback, int64 interval, uint32 repeat, bool paused);

    void removeTimer(int32 timerId);

    void pauseTimer(int32 timerId);

    void resumeTimer(int32 timerId);

    bool isTimerActive(int32 timerId);

    bool isTimerPaused(int32 timerId);

    bool timerExists(int32 timerId);

    int64 getTimerElapsed(int32 timerId);

    int64 getTimerInterval(int32 timerId);

    void update(float deltaTime);

protected:
    TimerData const* findTimer(int32 timerId, size_t& outIndex);
    size_t findTimerInList(const std::vector<TimerData>& searchArray, int32 const& timerId);

private:
    std::vector<TimerData> _activeTimerList;
    std::vector<TimerData> _pausedTimerList;
    std::vector<TimerData> _swapTimerList;
    std::vector<int64>     _willRemoveTimers;
    static int32           _nextValidId;
};

NS_FK_END


#endif // LOSEMYMIND_TIMERMANAGER_H

