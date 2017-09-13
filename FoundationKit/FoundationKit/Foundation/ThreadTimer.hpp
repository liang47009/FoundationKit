

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <thread>
#include <atomic>
#include <memory>
#include <future>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Base/types.hpp"

NS_FK_BEGIN




// Generates an event after a set interval,with some option to generate recurring events.
// Expressed in milliseconds.
class ThreadTimer final : public std::enable_shared_from_this<ThreadTimer>
{
    static int         _nextValidId;
    ThreadTimer();
public:
    typedef std::shared_ptr<ThreadTimer>   Pointer;
    typedef std::function<void(const ThreadTimer::Pointer&)>  TimedEvent;
    static const long repeat_forever = -1;
    static const long involid_id     = -1;

    // Occurs when the specified timer interval has elapsed and the timer is enabled.
    TimedEvent onTimedEvent;
public:
    ThreadTimer(const ThreadTimer&) = delete;
    ThreadTimer& operator=(const ThreadTimer&) = delete;

    ThreadTimer(ThreadTimer&& other);
    ~ThreadTimer();
    ThreadTimer& operator=(ThreadTimer&& other);

    static ThreadTimer::Pointer Create();

    // Sets the interval, expressed in milliseconds, at which to raise the timed event.
    ThreadTimer&  SetInterval(long milliseconds);

    // Sets a value indicating the Timer invoke timed event to count, default Timer::repeat_forever
    ThreadTimer&  SetRepeatCount(long value);

    // The scale at which the time is passing. This can be used for slow motion effects.
    ThreadTimer&  SetTimeScale(float value);

    ThreadTimer&  SetUserData(void* userData);

    ThreadTimer&  SetTimedEvent(const TimedEvent& callback);

    // Gets the interval, expressed in milliseconds, at which to raise the timed event.
    long    GetInterval()const;

    // The time in milliseconds it took to complete the last frame (Read Only).
    long    GetDeltaTime()const;

    // The total number of frames that have passed (Read Only).
    long    GetFrameCount()const;

    long    GetId()const;

    // The scale at which the time is passing. This can be used for slow motion effects.
    float   GetTimeScale()const;

    void*   GetUserData()const;

    bool    Start();
    bool    Start(const TimedEvent& callback);
    bool    Start(const TimedEvent& callback, long milliseconds, long repeatCount = repeat_forever, float timeScale = 1.0f);
    void    Stop();
private:
    void    Move(ThreadTimer&& other);
    void    Reset();
    void    Run();
private:
    std::atomic_long   _interval;         // expressed in milliseconds
    std::atomic_long   _deltaTime;        // expressed in milliseconds
    std::atomic_long   _repeatCount;
    std::atomic_long   _frameCount;
    std::atomic_bool   _bRunning;
    ElapsedTimer       _deltaTimer;
    std::atomic<float> _timeScale;
    long               _myid;
    void*              _userData;
    std::future<void>  _future;

};

inline bool operator <(const ThreadTimer& _Left, const ThreadTimer& _Right)
{
    return _Left.GetInterval() < _Right.GetInterval();
}

inline bool operator ==(const ThreadTimer& _Left, const ThreadTimer& _Right)
{
    return _Left.GetId() == _Right.GetId();
}

inline bool operator ==(const ThreadTimer& _Left, int timerId)
{
    return _Left.GetId() == timerId;
}

inline bool operator ==(const ThreadTimer::Pointer& _Left, const ThreadTimer::Pointer& _Right)
{
    return _Left->GetId() == _Right->GetId();
}

inline bool operator ==(const ThreadTimer::Pointer& _Left, int timerId)
{
    return _Left->GetId() == timerId;
}

NS_FK_END

