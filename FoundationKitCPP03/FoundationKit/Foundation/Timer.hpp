/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_TIMER_HPP
#define FOUNDATIONKIT_TIMER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <memory>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Base/types.hpp"

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

    TimerOption()
        : enable(false)
        , interval(1000)
        , scale(1.0f)
        , maximumDeltaTime(100)
        , repeatCount(-1)
        , onTimedEvent(nullptr)
    {}
};

// Generates an event after a set interval,with some option to generate recurring events.
// Expressed in milliseconds.
class Timer final
{
    Timer(const Timer&);
    Timer& operator=(const Timer&);
public:
    typedef std::shared_ptr<Timer>   pointer;
    static const int repeat_forever = -1;
    static const int involid_id = -1;

    // Occurs when the specified timer interval has elapsed and the timer is enabled.
    TimedEvent onTimedEvent;
public:


    Timer();
    explicit Timer(int interval);
    Timer(const TimerOption& other);
    ~Timer();

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

    //expressed in seconds
    void update(float deltaTime);
    void start();
    void stop();
    void startInThread();
private:
    void init(const TimerOption& option);
    void reset();
    void internalUpdate(int deltaTime);
    void threadLoop();
    static void threadFun(void* pThis);
private:
    std::atomic<bool>  _enable;
    std::atomic<int>   _interval;         // expressed in milliseconds
    float              _timeScale;
    std::atomic<int>   _repeatCount;
    std::atomic<int>   _maximumDeltaTime; // expressed in milliseconds,default to 100 seconds.
    std::atomic<int>   _deltaTime;        // expressed in milliseconds
    std::atomic<int>   _frameCount;
    std::atomic<int>   _elapsedTime;      // expressed in milliseconds
    int                _myid;
    static int         _nextValidId;
    std::thread*       _loopThread;
    bool               _useThread;
};

inline bool operator <(const Timer& _Left, const Timer& _Right)
{
    return _Left.getInterval() < _Right.getInterval();
}

NS_FK_END

#endif // FOUNDATIONKIT_TIMER_HPP
