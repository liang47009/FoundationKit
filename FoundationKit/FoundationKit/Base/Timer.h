/****************************************************************************
Copyright (c) 2016 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#ifndef LOSEMYMIND_TIMER_H
#define LOSEMYMIND_TIMER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <thread>
#include <atomic>
#include <memory>
#include "FoundationKit/FoundationMacros.h"
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Base/DateTime.h"

NS_FK_BEGIN

// Provides data for the Timer::onTimedEvent event.
struct TimedEventArgs
{
    int      timerId;
    int      deltaTime;
    DateTime signalTime;
};
typedef std::function<void(const TimedEventArgs&)>  TimedEvent;

struct TimerOption
{
    bool  enable;
    int   interval;         //expressed in milliseconds
    float scale;
    int   maximumDeltaTime; //expressed in milliseconds
    int   repeatCount;
    TimedEvent onTimedEvent;

    /**
     * Creates and initializes a new TimerOption.
     *
     * @param e Sets a value indicating whether the Timer should raise the timed event.
     * @param i Sets the interval, expressed in milliseconds, at which to raise the timed event.
     * @param s The scale at which the time is passing. This can be used for slow motion effects.
     * @param m The maximum time a frame can take, expressed in milliseconds.
     * @param r Sets a value indicating the Timer invoke timed event to count, default Timer::repeat_forever
     * @param callback Occurs when the specified timer interval has elapsed and the timer is enabled.
     */
    TimerOption(bool e, int i, float s, int m, int r, const TimedEvent& callback = nullptr)
        : enable(e)
        , interval(i)
        , scale(s)
        , maximumDeltaTime(m)
        , repeatCount(r)
        , onTimedEvent(callback)
    {}

    TimerOption() :TimerOption(false, 1000, 1.0f, 100, -1, nullptr){}
};

// Generates an event after a set interval,with some option to generate recurring events.
// Expressed in milliseconds.
class Timer final : public std::enable_shared_from_this<Timer>
{
public:
    typedef std::shared_ptr<Timer>   pointer;
    static const int repeat_forever = -1;
    static const int involid_id = -1;

    // Occurs when the specified timer interval has elapsed and the timer is enabled.
    TimedEvent onTimedEvent;
public:
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    Timer();
    explicit Timer(int interval);
    Timer(const TimerOption& other);
    Timer(Timer&& other);
    ~Timer();
    Timer& operator=(Timer&& other);

    static Timer::pointer create();
    static Timer::pointer create(int interval);
    static Timer::pointer create(const TimerOption& option);

    void setOption(const TimerOption& option);

    // Sets a value indicating whether the Timer should raise the timed event.
    Timer& setEnabled(bool value);

    // Sets the interval, expressed in milliseconds, at which to raise the timed event.
    Timer& setInterval(int value);

    // The scale at which the time is passing. This can be used for slow motion effects.
    Timer&  setTimeScale(float value);

    // The maximum time a frame can take, expressed in milliseconds.
    Timer&  setMaximumDeltaTime(int value);

    // Sets a value indicating the Timer invoke timed event to count, default Timer::repeat_forever
    Timer&  setRepeatCount(int value);


    // Gets a value indicating whether the Timer should raise the timed event.
    bool getEnabled()const;

    // Gets the interval, expressed in milliseconds, at which to raise the timed event.
    int  getInterval()const;

    // The scale at which the time is passing. This can be used for slow motion effects.
    float getTimeScale()const;

    // The maximum time a frame can take, expressed in milliseconds.
    int   getMaximumDeltaTime()const;

    // The time in milliseconds it took to complete the last frame (Read Only).
    int   getDeltaTime()const;

    // The total number of frames that have passed (Read Only).
    int   getFrameCount()const;

    int   get_id()const;

    //expressed in milliseconds
    void update(int deltaTime);
    void start();
    void stop();
    void startInThread();
private:
    void move(Timer&& other);
    void reset();
    void internalUpdate(int deltaTime);
private:
    std::atomic_bool _enable;
    std::atomic_int  _interval;         // expressed in milliseconds
    float            _timeScale;
    std::atomic_int  _repeatCount;
    std::atomic_int  _maximumDeltaTime; // expressed in milliseconds,default to 100 seconds.
    std::atomic_int  _deltaTime;        // expressed in milliseconds
    std::atomic_int  _frameCount;
    std::atomic_int  _elapsedTime;      // expressed in milliseconds
    int              _myid;
    static int       _nextValidId;
    std::thread      _loopThread;
    bool             _useThread;
};

inline bool operator <(const Timer& _Left, const Timer& _Right)
{
    return _Left.getInterval() < _Right.getInterval();
}

NS_FK_END

#endif // LOSEMYMIND_TIMER_H

