/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TIMER_HPP
#define FOUNDATIONKIT_TIMER_HPP

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
class Timer final : public std::enable_shared_from_this<Timer>
{
    static int         _nextValidId;
    Timer();
public:
    typedef std::shared_ptr<Timer>   Pointer;
    typedef std::function<void(const Timer::Pointer&)>  TimedEvent;
    static const long repeat_forever = -1;
    static const long involid_id     = -1;

    // Occurs when the specified timer interval has elapsed and the timer is enabled.
    TimedEvent onTimedEvent;
public:
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    Timer(Timer&& other);
    ~Timer();
    Timer& operator=(Timer&& other);

    static Timer::Pointer Create();

    // Sets the interval, expressed in milliseconds, at which to raise the timed event.
    Timer&  SetInterval(long milliseconds);

    // Sets a value indicating the Timer invoke timed event to count, default Timer::repeat_forever
    Timer&  SetRepeatCount(long value);

    // The scale at which the time is passing. This can be used for slow motion effects.
    Timer&  SetTimeScale(float value);

    Timer&  SetUserData(void* userData);

    Timer&  SetTimedEvent(const TimedEvent& callback);

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
    void    Move(Timer&& other);
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

inline bool operator <(const Timer& _Left, const Timer& _Right)
{
    return _Left.GetInterval() < _Right.GetInterval();
}

inline bool operator ==(const Timer& _Left, const Timer& _Right)
{
    return _Left.GetId() == _Right.GetId();
}

inline bool operator ==(const Timer& _Left, int timerId)
{
    return _Left.GetId() == timerId;
}

inline bool operator ==(const Timer::Pointer& _Left, const Timer::Pointer& _Right)
{
    return _Left->GetId() == _Right->GetId();
}

inline bool operator ==(const Timer::Pointer& _Left, int timerId)
{
    return _Left->GetId() == timerId;
}

NS_FK_END

#endif // FOUNDATIONKIT_TIMER_HPP
